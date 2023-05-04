#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"
#include "eeprom.h"
#include "clock.h"

#ifndef H_data
#define H_data

/* the following struct is used to store the data that is collected by the vehicle.
    The struct serves as the structure of the eeprom so that we can read and write
    to it without worrying about the different sizes of the datatypes. Since data
    is all in a struct, everything written and read to/from eeprom will be the same
    size.
*/
typedef struct   
{
    Clock clock;
    uint16 temperature;              // 2 bytes size
    uint16 light_level;             // 2 bytes size             
    uint8 collision_detected;       // 1 byte size
} g_collected_data; // total size is 8 bytes

void write_data(g_collected_data);
g_collected_data get_data(void);
g_collected_data make_data_log(Clock, uint16, uint16, uint8);
uint8 explore_mode(void);
uint16 get_data_size(void);

#endif