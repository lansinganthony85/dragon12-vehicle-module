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
#define LIGHT_ADC_CHANNEL                  4
#define TEMP_ADC_CHANNEL                   5
#define BATTERY_ADC_CHANNEL                3 // Port A bit 11

#define ADC_TO_DEGREES_F(input)            32 + ((input/2) * 1.8);  


/* PROTOTYPES */
float abs_value(float input_val);

/* GLOBALS */
uint8 g_done = FALSE;
int motor_left_speed = 0;
int motor_right_speed = 0;

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
  uint8 true_false;
  
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
  
  
  eeprom_init();
  
  _asm cli                                  // enable interrupts
  
  /* set up Port H interrupts */
  PIFH = SETALL;                            // clear port h flags
  PPSH = CLEAR;
  PIEH |= SW5_BITMASK;
  
  true_false = is_explore_mode();
  if(true_false == TRUE)
  {
        clock_init();
        environment_sensor_init();
        
        
        while(!g_done)
        {
            // Loops until button is pressed to exit explore mode
            true_false = is_collect_time(30);
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
        output_init(9600);
        data_size = read_data_size();
        write_labels();
        
        for(index = 0; index < data_size; index++)
        {
            data_log = get_data();
            write_to_putty(data_log);
        } /* for */
  } /* else */
  
  set_lcd_addr(LINE_1);
  type_lcd("Program Stopped");
 
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