/*
 *  AUTHORS
 *      Rowan Daly and Anthony Lansing
 */
 
#include "main_asm.h"
#include "dragon.h"

#ifndef H_environment
#define H_environment

/* GLOBALS */
#define LIGHT_CHANNEL 4                     // light sensor is on channel 4
#define TEMP_CHANNEL 5                      // The channel that temp sensor connects to

/* MACRO FOR CALCULATING FAHRENHEIT FROM CELSIUS */
#define TEMP_CONV_FORMULA(value_from_adc) (value_from_adc * (float)9/5) + 32;

/* PROTOTYPES */
uint16 get_light_level(void);
uint16 get_temp(void);

#endif