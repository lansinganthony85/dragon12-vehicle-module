/*
 *  clock.c
 *
 *  DESCRIPTION
 *      This module provides the functionality necessary to keep track of the time
 *      that the RC Exploration Vehicle is running. We use a struct to keep track
 *      of the current hour, minute, and second that the vehicle is running. This
 *      data is used to keep track of when data is collected and when collisions
 *      are detected. We utilize Real-Time Interrupts to keep track of when a 
 *      second goes by and this determines how much time has elapsed since the start
 *      of the operation.
 */
#include "clock.h"

/* GLOBALS */
Clock g_clock;                              // the global timer
uint16 g_total_seconds = 0;                 // the total seconds for the operation

/*
 *  NAME
 *      clock_init
 *
 *  DESCRIPTION
 *      set all values of the clock to zero and start RTI for an interrupt
 *      every 10.24 milliseconds.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void clock_init(void)
{
    g_clock.hour = 0;
    g_clock.minute = 0;
    g_clock.second = 0;
    RTI_init();    
} /* clock_init */

/*
 *  NAME
 *      increment_clock
 *
 *  DESCRIPTION
 *      Increments the clock by one second. We also increment the global
 *      second count by 1.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void increment_clock(void)
{
    g_clock.second++;                           // increment seconds
    g_total_seconds++;                          // increment global count
    
    /* to keep the clock structure, we make sure seconds
    and minutes don't go over 59. */
    if(g_clock.second >= TIME_COUNT_LIMIT)
    {
        g_clock.second = 0;
        g_clock.minute++;
        
        if(g_clock.minute >= TIME_COUNT_LIMIT)
        {
            g_clock.minute = 0;
            g_clock.hour++;
        } /* if */
    } /* if */
} /* increment_clock */

/*
 *  NAME
 *      clock_timer
 *
 *  DESCRIPTION
 *      This RTI sets the flag every 10.24 milliseconds. After 100 flags
 *      are set, we increment the clock.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void interrupt 7 clock_timer(void)
{
    static uint8 count = 0;                     // static to keep track over all interrupts
    
    /* increment clock by one second if 
    count reaches 100 */
    if(count >= ONE_SECOND)
    {
        increment_clock();
        count = 0;   
    } /* if */
    else count++;
    
    clear_RTI_flag();    
} /* clock_timer */

/*
 *  NAME
 *      is_collect_time
 *
 *  DESCRIPTION
 *      This functions returns true if it is time to collect data.
 *
 *  PARAMETERS
 *      uint8 when_to_take_data
 *
 *  RETURN
 *      TRUE or FALSE
 */
uint8 is_collect_time(uint8 when_to_take_data)
{
    return (g_total_seconds % when_to_take_data) == 0;
} /* is_collect_time */

/*
 *  NAME
 *      get_time
 *
 *  DESCRIPTION
 *      Returns this Clock.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      Clock g_clock
 */
Clock get_time(void)
{
    return g_clock;
} /* get_time */