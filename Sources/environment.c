/*
 * environment.c
 *
 *  DESCRIPTION
 *      This module provides the methods necessary to collect environmental data.
 *      Functions provided here can provide light level, temperature, and collision
 *      detection.
 */
 
#include "environment.h"

void environment_sensor_init(void)
{
    ad0_enable();                             // enable ATD0 for 10-bit
       
} /* light_sensor_init */

uint16 get_light_level(void)
{
    return ad0conv(LIGHT_CHANNEL);
} /* light_sensor_init */

uint16 get_temp(void)
{
    uint16 temp_value = ad0conv(TEMP_CHANNEL);
    temp_value >>= 1;
    return TEMP_CONV_FORMULA(temp_value);
}