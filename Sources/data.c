/*
 *  data.c
 *
 *  DESCRIPTION
 *      This module provides the methods needed to group data into packets that can
 *      then be written to and read from an eeprom. This functionality provides the
 *      structure necessary to understand the data that is stored on the eeprom.
 */
 
#include "data.h"

#define DATA_BITMASK 0x80

uint16 g_data_size = 0;
/*
uint8 is_explore_mode(void)
{
    uint8 porth_dip = SW1_dip();
    if((porth_dip & DATA_BITMASK) == DATA_BITMASK) return TRUE;
    else return FALSE;
}*/ /* explore_mode */

/*
 *  NAME
 *      write_data
 *
 *  DESCRIPTION
 *      This function writes the data from a struct onto the current memory addr
 *      to the eeprom.
 *
 *  PARAMETERS
 *      struct data_struct
 *
 *  RETURN
 *      NONE
 */
void write_data(g_collected_data data_struct)
{
    write_enable();
    
    write_char(data_struct.clock.hour);
    write_char(data_struct.clock.minute);
    write_char(data_struct.clock.second);
    write_int(data_struct.temperature);
    write_int(data_struct.light_level);
    write_char(data_struct.collision_detected);
    
    g_data_size++;
    
} /* write_data */

g_collected_data get_data(void)
{
    g_collected_data data_to_get;
    data_to_get.clock.hour = read_char();
    data_to_get.clock.minute = read_char();
    data_to_get.clock.second = read_char();
    data_to_get.temperature = read_int();
    data_to_get.light_level = read_int();
    data_to_get.collision_detected = read_char();
    
    return data_to_get;
} /* get_data */

g_collected_data make_data_log(Clock clock, uint16 temperature, uint16 light_level, uint8 collision_detected)
{
    g_collected_data data_log;
    data_log.clock = clock;
    data_log.temperature = temperature;
    data_log.light_level = light_level;
    data_log.collision_detected = collision_detected;
    
    return data_log;
}

uint16 get_data_size(void)
{
    return g_data_size;
}