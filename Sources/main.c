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

/* PROTOTYPES */

/* GLOBALS */
uint8 g_done = FALSE;

/* INTERRUPTS */

void main(void) {
  /* VARIABLES */
  Clock time_of_data;
  g_collected_data data_log;
  uint16 light;
  uint16 temperature;
  uint16 data_size;
  uint8 index;
  uint8 true_false;
  
  PLL_init();        // set system clock frequency to 24 MHz
  lcd_init();
  clear_lcd();
  seg7_disable();
  led_disable();
  
 // SW_enable();
  
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