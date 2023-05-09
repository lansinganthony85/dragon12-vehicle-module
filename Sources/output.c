/*
 *  output.c
 *
 *  DESCRIPTION
 *      This module contains functions necessary for outputting data to putty.
 */

#include "output.h"
 
#define LIGHT_LABEL "LIGHT LEVEL"
#define TEMP_LABEL "TEMPERATURE"
#define COLL_LABEL "COLLISION DETECTED"
#define TIME_LABEL "TIME"
#define DEGREE_SYMBOL 0xB0
#define NULL_CHAR '\0'
#define COLON ":"
#define NEW_LINE '\n'
#define RETURN_LINE '\r'
#define TAB '\t'
 
uint8 g_digits_array[] = 
{
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

void output_init(uint16 baudrate)
{
    SCI1_init(baudrate);
} /* output_init */

void write_labels(void)
{
    alt_print(TIME_LABEL);
    outchar1(TAB);
    outchar1(TAB);
    alt_print(LIGHT_LABEL);
    outchar1(TAB);
    outchar1(TAB);
    alt_print(TEMP_LABEL);
    outchar1(NEW_LINE);
    outchar1(RETURN_LINE);       
} /* write_labels */

void write_to_putty(g_collected_data data_log)
{
    print_time(data_log.clock);
    outchar1(TAB);
    outchar1(TAB);
    print_int(data_log.light_level);
    outchar1(TAB);
    outchar1(TAB);
    print_int(data_log.temperature);
    outchar1(DEGREE_SYMBOL);
    outchar1('F');
    outchar1(NEW_LINE);
    outchar1(RETURN_LINE);           
} /* write_to_putty */

void print_int (uint16 int_to_print)
{
    sint8 i;
    uint8 current_digit;
    uint8 length = get_num_length(int_to_print);
    char num_string[5];
    num_string[length] = '\0';
    
    for (i = (length - 1); i >= 0; i--)
    {
        current_digit =  int_to_print % 10;
        current_digit = get_num_char(current_digit);
        num_string[i] = current_digit;
        int_to_print /= 10;    
    }
    
    alt_print(num_string);
} /* int_to_string */

uint8 get_num_length(uint16 num)
{
    if (num >= 1000) return 4;
    if (num >= 100) return 3;
    if (num >= 10) return 2;
    return 1;
} /* get_num_length */

uint8 get_num_char(uint8 num)
{
    return g_digits_array[num];
} /* get_num_char */

void print_time(Clock clock)
{
    if(clock.hour < 10)
    {
        print_int(0);
        print_int(clock.hour);
    }
    else print_int(clock.hour);
    
    alt_print(COLON);
    
    if(clock.minute < 10)
    {
        print_int(0);
        print_int(clock.minute);
    }
    else print_int(clock.minute);
    
    alt_print(COLON);
    
    if(clock.second < 10)
    {
        print_int(0);
        print_int(clock.second);
    }
    else print_int(clock.second);
   
}

/*
 *      Name: alt_print function
 *      Description: This function sends a string to the terminal emulator one
 *          character at a time until the null character is reached.
 *      Input: A pointer to a c-string
 *      Output: The c-string onto the terminal emulator
 *      Return: none 
 */
void alt_print(char * string)
{
    uint8 index = 0;
    while((string[index]) != NULL_CHAR)
    {
        outchar1(string[index]);
        index++;
    }
} /* alt_print */