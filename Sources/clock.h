#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

#ifndef H_clock
#define H_clock

typedef struct 
{
    uint8 hour;
    uint8 minute;
    uint8 second;   
} Clock;

void clock_init(void);
void increment_clock(void);
void interrupt 7 clock_timer(void);
uint8 is_collect_time(uint8);
Clock get_time(void);

#endif