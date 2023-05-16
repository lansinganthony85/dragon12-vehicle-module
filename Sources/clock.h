#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

#ifndef H_clock
#define H_clock

/* GLOBALS */
#define TIME_COUNT_LIMIT    60          // when seconds and minutes to roll over to zero
#define ONE_SECOND          100         // the number of RTIs before 1 second has passed

/* use a struct to handle clock-related data as an object */
typedef struct 
{
    uint8 hour;
    uint8 minute;
    uint8 second;   
} Clock;

/* PROTOTYPES */
void clock_init(void);
void increment_clock(void);
void interrupt 7 clock_timer(void);
uint8 is_collect_time(uint8);
Clock get_time(void);

#endif