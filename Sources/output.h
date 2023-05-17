/*
 *  AUTHORS
 *      Rowan Daly and Anthony Lansing
 */
 
#include <mc9s12dg256.h>

#include "main_asm.h"
#include "dragon.h"
#include "data.h"
#include "clock.h"

#ifndef H_output
#define H_output

/* LABELS FOR THE DISPLAY */ 
#define LIGHT_LABEL     "LIGHT LEVEL"
#define TEMP_LABEL      "TEMPERATURE"
#define COLL_LABEL      "COLLISION DETECTED"
#define TIME_LABEL      "TIME"
#define FRONT_COLLISION "FRONT COLLISION DETECTED"
#define REAR_COLLISION  "REAR COLLISION DETECTED"

/* SYMBOLS FOR FORMATTING */
#define DEGREE_SYMBOL   0xB0
#define FAHRENHEIT      'F'
#define NULL_CHAR       '\0'
#define COLON           ":"
#define NEW_LINE        '\n'
#define RETURN_LINE     '\r'
#define TAB             '\t'

/* NUMBERS FOR CONVERTING INTS TO ASCII chars*/
#define ZERO    0x30    
#define ONE     0x31
#define TWO     0x32
#define THREE   0x33
#define FOUR    0x34
#define FIVE    0x35
#define SIX     0x36
#define SEVEN   0x37
#define EIGHT   0x38
#define NINE    0x39

/* CONSTANTS FOR COMPARING LENGTHS OF NUMBERS */
#define FOUR_DIGIT_NUM      1000
#define THREE_DIGIT_NUM     100
#define TWO_DIGIT_NUM       10
#define NUM_MAX_LENGTH      5             // The max num of digits is 4 with one more space for null
#define LENGTH_FOUR         4
#define LENGTH_THREE        3
#define LENGTH_TWO          2
#define LENGTH_ONE          1

/* PROTOTYPES */
void write_to_putty(g_collected_data);
void print_int(uint16);
uint8 get_num_length(uint16);
uint8 get_num_char(uint8);
void print_time(Clock);
void alt_print(char *);
void write_labels(void);

#endif