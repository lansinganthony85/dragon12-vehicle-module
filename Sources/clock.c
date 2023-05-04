#include "clock.h"

#define TIME_COUNT_LIMIT 60

Clock g_clock;

void clock_init(void)
{
    g_clock.hour = 0;
    g_clock.minute = 0;
    g_clock.second = 0;
    RTI_init();    
} /* clock_init */

void increment_clock(void)
{
    g_clock.second++;
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

void interrupt 7 clock_timer(void)
{
    static uint8 count = 0;
    
    if(count >= 100)
    {
        increment_clock();
        count = 0;   
    } /* if */
    else count++;
    
    clear_RTI_flag();    
} /* clock_timer */

uint8 is_collect_time(uint8 when_to_take_data)
{
    return (g_clock.second % when_to_take_data) == 0;
} /* is_collect_time */

Clock get_time(void)
{
    return g_clock;
} /* get_time */