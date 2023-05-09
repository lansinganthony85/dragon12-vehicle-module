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

/* DEFINITIONS*/
#define MOTOR_SPEED_MULTIPLIER             100.0
#define BATTERY_ADC_CHANNEL                3 // Port A bit 11


#define WELCOME_LABEL "WELCOME"
#define EXPLORE_LABEL "EXPLORE"
#define DATA_LABEL "GET DATA"
#define SELECTOR '*'
#define EXPLORE_SELECT_ADDR 0x07
#define DATA_SELECT_ADDR 0x48
#define ENTER_BITMASK 0x04                  // Connected to PM2
#define BUTTON1_BITMASK 0x02                // Connected to PM1
#define BUTTON2_BITMASK 0x04                // Connected to PM0

/* PITCHES */
#define c 2867
#define d 2554
#define e 2276
#define f 2148
#define g 1914
#define a 1705
#define b 1519
#define C 1434
#define D 1277
#define E 1138
#define F 1074
#define G 957
#define A 853
#define B 760
#define CC 717
#define DD 639


/* PROTOTYPES */
float abs_value(float input_val);
void start_jingle(void);
void sound_effect(void);
void explore_jingle(void);
void end_jingle(void);

/* GLOBALS */
uint8 g_done = FALSE;
int motor_left_speed = 0;
int motor_right_speed = 0;
uint16 g_pitch;

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
void interrupt 13 noise_maker()
{
    tone(g_pitch);
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
  
  char LR_decimal = 0.0;
  char FB_decimal = 0.0;

  //float motor_scalar = 1;
  char dip_switch_read = 0;
  char degrees_f_reading = 0;
  int ADC_battery_reading = 0;
  
  PLL_init();        // set system clock frequency to 24 MHz
  lcd_init();
  clear_lcd();
  seg7_disable();
  led_disable();
  SW_enable();
  //ad0_enable();
  ad1_enable();
  
  HILO2_init();
  HILO1_init(); 
  
  motor1_init();
  motor2_init();
  
  sound_init(); 
  eeprom_init();
  
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
         
        while(!g_done)
        {
            // Loops until button is pressed to exit explore mode
            true_false = is_collect_time(data_freq);
            if(true_false)
            {
                time_of_data = get_time();
                light = get_light_level();
                temperature = get_temp();
                data_log = make_data_log(time_of_data, temperature, light, 0);
                write_data(data_log);
                ms_delay(2000);

              
            } /* if */
            
            // Channel 1: left/right
            // Channel 2: forward/backward
            LR_decimal = get_high_time_decimal(1);
            FB_decimal = get_high_time_decimal(2);
            
            // Combine values from front/back and left/right inputs
            motor_left_speed = (FB_decimal + LR_decimal);
            motor_right_speed = (FB_decimal - LR_decimal);
            
            
            // Only print to LCD if dip SW1 is HIGH
            dip_switch_read = SW1_dip();
            if ((dip_switch_read & 0x40) == 0x00)
            {
              // Display un-scaled inputs on LCD
              set_lcd_addr(0x00);
              write_int_lcd(motor_left_speed);
              type_lcd("  ");
              write_int_lcd(motor_right_speed); 
            }
            

            scale_motor_speed();
            
            
            
            if ((dip_switch_read & 0x40) == 0x00)
            {
              // Display scaled control data on LCD
              set_lcd_addr(0x40);
              write_int_lcd(motor_left_speed);
              type_lcd("  ");
              write_int_lcd(motor_right_speed);
            }
            
            
            // Set motor speed
            set_motor_speed(1, motor_left_speed);
            set_motor_speed(2, motor_right_speed);

            ADC_battery_reading = ad1conv(BATTERY_ADC_CHANNEL);

            
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
} /* end_program */

void start_jingle(void)
{
    uint8 array_size = 3;
    uint8 index;
    uint16 note[] = 
    {
        C, E, F
    };
    uint8 length[] = 
    {
        250, 250, 500
    };
    
    for(index = 0; index < array_size; index++)
    {
        g_pitch = note[index];
        sound_on();
        ms_delay(length[index]);   
    }
    sound_off();
    _asm cli;
}

void sound_effect(void)
{
    g_pitch = C;
    sound_on();
    ms_delay(250);
    sound_off();
    _asm cli;
}

void explore_jingle(void)
{
    uint8 array_size = 6;
    uint8 index;
    uint16 note[] = 
    {
        C, E, C, F, G, A
    };
    uint8 length[] = 
    {
        750, 500, 500, 500, 500, 1000
    };
    
    for(index = 0; index < array_size; index++)
    {
        g_pitch = note[index];
        sound_on();
        ms_delay(length[index]);   
    }
    sound_off();
    _asm cli;   
}

void end_jingle(void)
{
    uint8 array_size = 3;
    uint8 index;
    uint16 note[] = 
    {
        F, E, C
    };
    uint8 length[] = 
    {
        250, 250, 500
    };
    
    for(index = 0; index < array_size; index++)
    {
        g_pitch = note[index];
        sound_on();
        ms_delay(length[index]);   
    }
    sound_off();
    _asm cli;   
}