/* common defines and macros */ 
#include <hidef.h>

/* derivative information */
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

/* interface to the assembly module */
#include "main_asm.h"

/* access the common constants */
#include "dragon.h"

/* Files that contain all code and definitions to run vehicle */
#include "eeprom.h"                     // Functions for reading/writing to FRAM
#include "data.h"                       // Functions for bundling data
#include "environment.h"                // Functions for collecting data
#include "output.h"                     // Functions for outputting data to PuTTY
#include "clock.h"                      // Functions for keeping time
#include "rc_read.h"                    // include code to read RC PWM
#include "motor_control.h"
#include "battery_monitor.h"
#include "sound.h"                      // Functions for making sounds and music
#include "interface.h"                  // Functions and definitions for menu interface

/* DEFINITIONS*/
#define MOTOR_SPEED_MULTIPLIER  100.0
#define JUNK_VALUE              99      // This value is arbitrary. 99 is chosen as it is not a likely value
#define MENU_DELAY              2000
#define MINI_DELAY              50
#define BAUDRATE                9600


/* PROTOTYPES */
float abs_value(float input_val);
void interrupt 25 detect_switches(void);

/* GLOBALS */
uint8 g_done = FALSE;                   // For ending the program
int motor_left_speed = 0;
int motor_right_speed = 0;

char g_low_battery_20_percent = FALSE;
char g_low_battery_5_percent = FALSE;

/* INTERRUPTS */
// Read high times from PWM 1
void interrupt 9 handler1()
{
    HILOtimes1();
}
// Read high times from PWM 2
void interrupt 10 handler2()
{
    HILOtimes2();
}


void main(void) {
  /* VARIABLES */
  Clock time_of_data;                   // To keep track of current time
  uint8 old_time_of_data = JUNK_VALUE;  // To prevent repetitive data collection
  g_collected_data data_log;            // To bundle data
  uint16 light;                         // The light level
  uint16 temperature;
  uint8 time_to_collect = FALSE;        // To check when to collect environmental data
  uint8 is_explore_mode;                // To check user choice
  uint8 data_freq;                      // How often to collect data
  uint8 front_collision_logged = FALSE; // So not to repetively log collisions
  uint8 rear_collision_logged = FALSE;
  
  char LR_decimal = 0.0;
  char FB_decimal = 0.0;

  //float motor_scalar = 1;
  //char dip_switch_read = 0;
  //char degrees_f_reading = 0;
  //int ADC_battery_reading = 0;
  char battery_level = 100;
  
  /* Initialize Dragon12 */
  PLL_init();                           // set system clock frequency to 24 MHz
  lcd_init();
  clear_lcd();
  seg7_disable();
  led_enable();
  SW_enable();
  ad1_enable();
  
  HILO2_init();
  HILO1_init(); 
  
  motor1_init();
  motor2_init();
  
  noise_init();                         // set up ch5 and ch7 for pulse train through speaker 
  eeprom_init();                        // prepare FRAM for SPI
  
  /* set up Port H interrupts */
  PIFH = SETALL;                        // clear port h flags
  PPSH = CLEAR;
  PIEH |= SW5_BITMASK;
  _asm cli                              // enable interrupts
  
  /* Display Welcome and starter music */
  type_lcd(WELCOME_LABEL);
  start_jingle();
  ms_delay(MENU_DELAY);                 // Slight delay for aesthetics
  
  // ---- CODE FOR SETUP MENU OPTIONS ----
  clear_lcd();
  set_lcd_addr(LINE_1);
  type_lcd(EXPLORE_LABEL);
  data8(SELECTOR);                      // Cursor for selections
  set_lcd_addr(LINE_2);
  type_lcd(DATA_LABEL);
  
  is_explore_mode = get_mode();         // Get choice
  ms_delay(MINI_DELAY);                 // Slight delay for aesthetics
  clear_lcd();
  
  if(is_explore_mode)
  {        
        // ---- CODE FOR DATA FREQUENCY MENU ----
        data_freq = get_frequency();    // Get choice
        ms_delay(MINI_DELAY);           // Slight delay for aesthetics
        clear_lcd();
        
        // ---- Initialize Explore Mode ----
        explore_jingle();
        set_lcd_addr(LINE_1);
        type_lcd(EXPLORING_LABEL);
        
        clock_init();                   // Clock begins running here
        ad0_enable();                   // Initialize for collecting environmental data        

        // ---- CODE FOR WHEN ROBOT IS RUNNING ----        
        while(!g_done)
        {
            // Only run when time_to_collect is set to TRUE
            time_to_collect = is_collect_time(data_freq);
            if(time_to_collect)
            {
                time_of_data = get_time();
                
                /* Prevent data from being collected multiple times in the same
                    time slot */
                if(!(time_of_data.second == old_time_of_data))
                {
                    light = get_light_level();
                    temperature = get_temp();
                    data_log = make_data_log(time_of_data, temperature, light, FALSE, FALSE);
                    write_data(data_log);
                    old_time_of_data = time_of_data.second;         // update old_time_data to prevent reuse    
                } /* if */
                  
            } /* if */
            
            
            // Channel 1: left/right
            // Channel 2: forward/backward
            LR_decimal = get_high_time_decimal(1);
            FB_decimal = get_high_time_decimal(2);
            
            // Combine values from front/back and left/right inputs
            motor_left_speed = (FB_decimal + LR_decimal);
            motor_right_speed = (FB_decimal - LR_decimal);
            

            scale_motor_speed();
            
            
            // If ultrasonic detects object in front of robot, stop
            // forward movement and log timestamp
            if((PTH & SW4_BITMASK) == 0)
            {
                // Set motor values to zero if they're positive
                motor_left_speed = (motor_left_speed > 0) ? 0 : motor_left_speed;
                motor_right_speed = (motor_right_speed > 0) ? 0 : motor_right_speed;
                sound_effect();
                
                /* Only log a collision once */
                if(!front_collision_logged)
                {
                    time_of_data = get_time();
                    data_log = make_data_log(time_of_data, 0, 0, TRUE, FALSE);
                    write_data(data_log);
                    front_collision_logged = TRUE;              // This is set true so it can't be logged again
                } /* if */
            } /* if */
            else
            {
                front_collision_logged = FALSE;                 // Collision is over so can start logging again   
            } /* else */
            
            // If ultrasonic detects object behind robot, stop
            // backwards movement and log timestamp
            if((PTH & SW3_BITMASK) == 0)
            {
                motor_left_speed = (motor_left_speed < 0) ? 0 : motor_left_speed;
                motor_right_speed = (motor_right_speed < 0) ? 0 : motor_right_speed;
                sound_effect();
                
                /* Only log a collision once */
                if(!rear_collision_logged)
                {
                    time_of_data = get_time();
                    data_log = make_data_log(time_of_data, 0, 0, FALSE, TRUE);
                    write_data(data_log);
                    rear_collision_logged = TRUE;               // This is set true so it can't be logged again 
                } /* if */
            } /* if */
            else
            {
                rear_collision_logged = FALSE;                  // Collision is over so can start logging again   
            } /* else */
            
            // Check battery level when not in motion
            if ((motor_left_speed == 0) && (motor_right_speed == 0))
            {
              battery_level = read_battery_level();
              led_battery_indicator(battery_level);
            }
            
            // Make sound if battery is under 20%
            if ((battery_level <= 20) && (!g_low_battery_20_percent))
            {
              sound_effect();
              g_low_battery_20_percent = TRUE; 
            }
            // If battery is below 5%, set a flag to disable motion
            if ((battery_level <= 5) && (!g_low_battery_5_percent))
            {
              sound_effect();
              sound_effect();
              sound_effect();
              g_low_battery_5_percent = TRUE; 
            }
            
            // Set motor speed
            set_motor_speed(1, motor_left_speed);
            set_motor_speed(2, motor_right_speed);

            
        } /* while */
        
        complete_write();                       // Can finish writing data      
               
        
  } /* if */
  else
  { 
        uint8 index;                            // For incrementing
        uint16 data_size;                       // To get data size for outputting to PuTTY

        /* Display Wait Screen for when ready to print */
        set_lcd_addr(LINE_1);
        type_lcd(DATA_READY);
        set_lcd_addr(LINE_2);
        type_lcd(PRESS_ENTER);
        wait_for_enter_press();
        sound_effect();
        
        /* Display that printing is being executed */
        clear_lcd();
        set_lcd_addr(LINE_1);
        type_lcd(PRINTING_LABEL);
        
        /* Initialize SCI1 and print out data */
        SCI1_init(BAUDRATE);
        data_size = read_data_size();
        write_labels();        
        
        for(index = 0; index < data_size; index++)
        {
            data_log = get_data();
            write_to_putty(data_log);
        } /* for */
        
        /* printing done so let user know */
        clear_lcd();
        set_lcd_addr(LINE_1);
        type_lcd(DONE_LABEL);
        set_lcd_addr(LINE_2);
        type_lcd(PRESS_SW5);
        while(!g_done);
        
  } /* else */
  
  /* End Program */
  clear_lcd();
  set_lcd_addr(LINE_1);
  type_lcd(GOODBYE_MESS);
  end_jingle();

} /* main */



// Returns the absolute value of the input
// INPUTS:
//         (float) input_val: a float
// OUTPUTS:
//         (float) the absolute value of the input
float abs_value(float input_val)
{
  // If input is negative, multiply by -1 and return.
  // Otherwise return the input.
  if (input_val < 0.0) 
  {
    return (-1.0*input_val);
  }
  else
  {
    return input_val;
  }
} /* abs_value */


/*
 *  DESCRIPTION
 *      ISR routine that checks whether SW5 was pressed.
 *      If SW5 pressed, then the flag is set for code to be handled
 *      in main where the program is ended.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      None
 */
void interrupt 25 detect_switches(void)
{
    uint8 clear_bits = 0;
    uint8 pifh_state = PIFH;                    // Grab the flag that was set
    
    /* confirm that SW5 was set and set flag to true */
    if((pifh_state & SW5_BITMASK) == SW5_BITMASK)
    {
        g_done = TRUE;
        clear_bits |= SW5_BITMASK;
        
    } /* if */
    
    /* clear the flag */
    PIFH = clear_bits;   
} /* detect_switches */