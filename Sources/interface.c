/*
 *  interface.c
 *
 *  DESCRIPTION
 *      This module contains the functions for traversing a menu and getting
 *      user data for operating the vehicle.
 */
 
#include "interface.h"

/*
 *  NAME
 *      get_mode
 *
 *  DESCRIPTION
 *      This function loops over a menu to select between two modes
 *      of the vehicle: Explore Mode and Data Mode. When a choice is
 *      made, this function returns either TRUE or FALSE for where
 *      explore mode was selected.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      TRUE or FALSE
 */
uint8 get_mode(void)
{
    uint8 is_explore_mode = TRUE;
    
    /* wait until enter button pressed */
    while((PTM & ENTER_BITMASK) == ENTER_BITMASK)
    {
        /* if button 1 pressed, set is_explore_mode
            to TRUE */
        if((PTM & BUTTON1_BITMASK) == 0)
        {
            sound_effect();                         // sound confirms button 1 pressed
            is_explore_mode = TRUE;
            
            /* update the LCD screen */
            set_lcd_addr(EXPLORE_SELECT_ADDR);
            data8(SELECTOR);                        // show option was selected
            set_lcd_addr(DATA_SELECT_ADDR);
            data8(BLANK_SPACE);   
        } /* if */
        /* if button 2 pressed, set is_explore_mode
            to FALSE */
        if((PTM & BUTTON2_BITMASK) == 0)
        {    
            sound_effect();                         // sound confirms button 2 pressed
            is_explore_mode = FALSE;
            
            /* update the LCD screen */
            set_lcd_addr(DATA_SELECT_ADDR);
            data8(SELECTOR);                        // show option was selected
            set_lcd_addr(EXPLORE_SELECT_ADDR);
            data8(BLANK_SPACE);   
        } /* if */
    } /* while */
    sound_effect();                                 // sound confirms enter button pressed
    
    return is_explore_mode;    
} /* get_mode */

/*
 *  NAME
 *      get_frequency
 *
 *  DESCRIPTION
 *      This function loops over a menu to select frequency at which
 *      data is collected. The frequency is in seconds with 10 seconds
 *      being the minimum and 60 seconds being the maximum.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      uint8 data_freq
 */
uint8 get_frequency(void)
{
    uint8 data_freq = DATA_FREQ_MINIMUM;
    
    set_lcd_addr(LINE_1);
    type_lcd(DATA_FREQ_LABEL);
    set_lcd_addr(LINE_2);
    write_int_lcd(data_freq);
    
    /* wait until enter button is pressed */
    while((PTM & ENTER_BITMASK) == ENTER_BITMASK)
    {
        /* increment data_freq when button 1 is pressed but this only works
            as long as data_freq is less than DATA_FREQ_MAXIMUM */
        if((data_freq < DATA_FREQ_MAXIMUM) && (PTM & BUTTON1_BITMASK) == 0)
        {
            sound_effect();                                 // confirm button press
            data_freq += DATA_FREQ_INCREMENT;
            
            /* update LCD */
            set_lcd_addr(LINE_2);
            write_int_lcd(data_freq);   
        } /* if */
        /* decrement data_freq when button 2 is pressed but this only works
            as long as data_freq is more than DATA_FREQ_MINIMUM */
        if((data_freq > DATA_FREQ_MINIMUM) && (PTM & BUTTON2_BITMASK) == 0)
        {
            sound_effect();                                 // confirm button press
            data_freq -= DATA_FREQ_INCREMENT;
            
            /* update LCD */
            set_lcd_addr(LINE_2);
            write_int_lcd(data_freq);  
        } /* if */
            
    } /* while */   
    
    return data_freq;
} /* get_frequency */

/*
 *  NAME
 *      wait_for_enter_press
 *
 *  DESCRIPTION
 *      This function does nothing until the enter button is pressed.
 *      Then ends.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void wait_for_enter_press(void)
{
    while((PORTA & ENTER_BITMASK) == ENTER_BITMASK);   
} /* wait_for_enter_press */