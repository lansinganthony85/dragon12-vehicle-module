/* common defines and macros */ 
#include <hidef.h>

/* derivative information */
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

/* interface to the assembly module */
#include "main_asm.h"

/* access the common constants */
#include "dragon.h"
#include "eeprom.h"
#include "data.h"
#include "environment.h"
#include "output.h"
#include "clock.h"
//#include "csc202_lab_support.h"     /* include CSC202 Support */
#include "rc_read.h"                /* include code to read RC PWM */
#include "motor_control.h"
#include "battery_monitor.h"
#include "sound.h"

/* DEFINITIONS*/
#define MOTOR_SPEED_MULTIPLIER             100.0



#define WELCOME_LABEL "WELCOME"
#define EXPLORE_LABEL "EXPLORE"
#define DATA_LABEL "GET DATA"
#define SELECTOR '*'
#define EXPLORE_SELECT_ADDR 0x07
#define DATA_SELECT_ADDR 0x48
#define ENTER_BITMASK 0x04                  // Connected to PM2
#define BUTTON1_BITMASK 0x01                // Connected to PM1
#define BUTTON2_BITMASK 0x02                // Connected to PM0


/* PROTOTYPES */
float abs_value(float input_val);

/* GLOBALS */
uint8 g_done = FALSE;
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
  Clock time_of_data;
  g_collected_data data_log;
  uint16 light;
  uint16 temperature;
  uint16 data_size;
  uint8 index;
  uint8 true_false = FALSE;
  uint8 is_explore_mode = TRUE;
  uint8 data_freq = 10;                     //default value
  uint8 front_collision_logged = FALSE;
  uint8 rear_collision_logged = FALSE;
  
  char LR_decimal = 0.0;
  char FB_decimal = 0.0;

  //float motor_scalar = 1;
  char dip_switch_read = 0;
  char degrees_f_reading = 0;
  int ADC_battery_reading = 0;
  char battery_level = 100;
  
  PLL_init();        // set system clock frequency to 24 MHz
  lcd_init();
  clear_lcd();
  seg7_disable();
  led_enable();
  SW_enable();
  //ad0_enable();
  ad1_enable();
  
  HILO2_init();
  HILO1_init(); 
  
  motor1_init();
  motor2_init();
  
  noise_init();                             // set up ch5 and ch7 for pulse train through speaker 
  eeprom_init();                            // prepare FRAM for SPI
  
  _asm cli                                  // enable interrupts
  
  /* set up Port H interrupts */
  PIFH = SETALL;                            // clear port h flags
  PPSH = CLEAR;
  PIEH |= SW5_BITMASK;
  
  type_lcd(WELCOME_LABEL);
  start_jingle();
  ms_delay(2000);
  
  clear_lcd();
  set_lcd_addr(LINE_1);
  type_lcd(EXPLORE_LABEL);
  data8(SELECTOR);
  set_lcd_addr(LINE_2);
  type_lcd(DATA_LABEL);
  
  DDRA = 0x00;
  
  // ---- CODE FOR SETUP MENU OPTIONS ----
  while((PTM & ENTER_BITMASK) == ENTER_BITMASK)
  {
        if((PTM & BUTTON1_BITMASK) == 0)
        {
            sound_effect();
            is_explore_mode = TRUE;
            set_lcd_addr(EXPLORE_SELECT_ADDR);
            data8(SELECTOR);
            set_lcd_addr(DATA_SELECT_ADDR);
            data8(' ');   
        } /* if */
        if((PTM & BUTTON2_BITMASK) == 0)
        {    
            sound_effect();
            is_explore_mode = FALSE;
            set_lcd_addr(DATA_SELECT_ADDR);
            data8(SELECTOR);
            set_lcd_addr(EXPLORE_SELECT_ADDR);
            data8(' ');   
        } /* if */
  } /* while */
  sound_effect();
  ms_delay(50);
  clear_lcd();
  
  if(is_explore_mode)
  {        
        // ---- CODE FOR DATA FREQUENCY MENU ----
        
        set_lcd_addr(LINE_1);
        type_lcd("DATA FREQUENCY: ");
        set_lcd_addr(LINE_2);
        write_int_lcd(data_freq);
        while((PTM & ENTER_BITMASK) == ENTER_BITMASK)
        {
            if((data_freq < 60) && (PTM & BUTTON1_BITMASK) == 0)
            {
                sound_effect();
                data_freq += 5;
                set_lcd_addr(LINE_2);
                write_int_lcd(data_freq);   
            } /* if */
            if((data_freq > 10) && (PTM & BUTTON2_BITMASK) == 0)
            {
                sound_effect();
                data_freq -= 5;
                set_lcd_addr(LINE_2);
                write_int_lcd(data_freq);  
            } /* if */
            
            ms_delay(50);
         } /* while */
        
        explore_jingle();
        clear_lcd();
        set_lcd_addr(LINE_1);
        type_lcd("EXPLORING...");
        
        clock_init();
        environment_sensor_init();
        

        // ---- CODE FOR WHEN ROBOT IS RUNNING ----        
        while(!g_done)
        {
            // Only run when true_false is set true by RTI handler
            true_false = is_collect_time(data_freq);
            if(true_false)
            {
                time_of_data = get_time();
                light = get_light_level();
                temperature = get_temp();
                data_log = make_data_log(time_of_data, temperature, light, FALSE, FALSE);
                write_data(data_log);

              
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
                leds_on(TRUE);
                
                if(!front_collision_logged)
                {
                    time_of_data = get_time();
                    data_log = make_data_log(time_of_data, 0, 0, TRUE, FALSE);
                    write_data(data_log);
                    front_collision_logged = TRUE;
                }
            }
            else
            {
                leds_off();
                front_collision_logged = FALSE;   
            }
            
            // If ultrasonic detects object behind robot, stop
            // backwards movement and log timestamp
            if((PTH & SW3_BITMASK) == 0)
            {
                motor_left_speed = (motor_left_speed < 0) ? 0 : motor_left_speed;
                motor_right_speed = (motor_right_speed < 0) ? 0 : motor_right_speed;
                leds_on(TRUE);
                
                if(!rear_collision_logged)
                {
                    time_of_data = get_time();
                    data_log = make_data_log(time_of_data, 0, 0, FALSE, TRUE);
                    write_data(data_log);
                    rear_collision_logged = TRUE;
                }
            }
            else
            {
                leds_off();
                rear_collision_logged = FALSE;   
            }
            
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
        
        complete_write();      
               
        
  } /* if */
  else
  {        
        set_lcd_addr(LINE_1);
        type_lcd("PRESS ENTER FOR");
        set_lcd_addr(LINE_2);
        type_lcd("DATA");
        while((PORTA & ENTER_BITMASK) == ENTER_BITMASK);
        sound_effect();
        clear_lcd();
        set_lcd_addr(LINE_1);
        type_lcd("PRINTING DATA...");
        
        output_init(9600);
        data_size = read_data_size();
        write_labels();        
        
        for(index = 0; index < data_size; index++)
        {
            data_log = get_data();
            write_to_putty(data_log);
        } /* for */
        
        clear_lcd();
        set_lcd_addr(LINE_1);
        type_lcd("DONE.");
        while(!g_done);
        
  } /* else */
  
  clear_lcd();
  set_lcd_addr(LINE_1);
  type_lcd("Good Bye");
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
}


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