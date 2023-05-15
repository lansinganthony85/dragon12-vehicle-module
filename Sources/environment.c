/*
 * environment.c
 *
 *  DESCRIPTION
 *      This module provides the methods necessary to collect environmental data.
 *      Functions provided here can provide light level, temperature, and collision
 *      detection. The light level and temperature are obtain from ATD0 on the 
 *      Dragon12 board.
 */
 
#include "environment.h"

/*
 *  NAME
 *      get_light_level
 *
 *  DESCRIPTION
 *      This function returns the value from the light sensor on ATD0.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      uint16
 */
uint16 get_light_level(void)
{
    return ad0conv(LIGHT_CHANNEL);
} /* light_sensor_init */

/*
 *  NAME
 *      get_temp
 *
 *  DESCRIPTION
 *      This function returns the temperature reading from ATD0.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      uint16
 */
uint16 get_temp(void)
{
    uint16 temp_value = ad0conv(TEMP_CHANNEL);      // get the base value from ATD0
    temp_value >>= 1;                               // divide by 2
    return TEMP_CONV_FORMULA(temp_value);           // return value in Fahrenheit
} /* get_temp */