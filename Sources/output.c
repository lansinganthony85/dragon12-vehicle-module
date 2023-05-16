/*
 *  output.c
 *
 *  DESCRIPTION
 *      This module contains functions necessary for outputting data to PuTTY.
 */

#include "output.h"



/*
 *  NAME
 *      write_labels
 *
 *  DESCRIPTION
 *      This functions prints out the headers for the data table.
 *
 *  PARAMTERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void write_labels(void)
{
    alt_print(TIME_LABEL);
    outchar1(TAB);
    outchar1(TAB);
    alt_print(LIGHT_LABEL);
    outchar1(TAB);
    outchar1(TAB);
    alt_print(TEMP_LABEL);
    
    /* go to the next line */
    outchar1(NEW_LINE);
    outchar1(RETURN_LINE);       
} /* write_labels */

/*
 *  NAME
 *      write_to_putty
 *
 *  DESCRIPTION
 *      This functions goes through the values of a g_collected_data
 *      and displays on PuTTY.
 *
 *  PARAMTERS
 *      g_collected_data data_log
 *
 *  RETURN
 *      NONE
 */
void write_to_putty(g_collected_data data_log)
{
    print_time(data_log.clock);
    outchar1(TAB);
    outchar1(TAB);
    
    /* if there was a collision, then we do not display data.
        We simply display that there was a collision */
    if(data_log.front_collision_detected || data_log.rear_collision_detected)
    {
        /* print that there was a front collision
            if TRUE */
        if(data_log.front_collision_detected)
        {
            alt_print(FRONT_COLLISION);
        } /* if */
        /* else print that there was a rear collision */
        else
        {
            alt_print(REAR_COLLISION);
        } /* else */
    } /* if */
    else
    {
        /* print light level and temperature */
        print_int(data_log.light_level);
        outchar1(TAB);
        outchar1(TAB);
        print_int(data_log.temperature);
        outchar1(DEGREE_SYMBOL);
        outchar1('F');
    } /* else */
    
    /* got to next line */
    outchar1(NEW_LINE);
    outchar1(RETURN_LINE);           
} /* write_to_putty */

/*
 *  NAME
 *      print_int
 *
 *  DESCRIPTION
 *      This functions takes an integer, converts to a string,
 *      and displays on PuTTY.
 *
 *  PARAMTERS
 *      uint16 int_to_print
 *
 *  RETURN
 *      NONE
 */
void print_int (uint16 int_to_print)
{
    sint8 index;                                    // index of the string
    uint8 current_digit;
    uint8 length = get_num_length(int_to_print);    // get the length of the integer
    
    char num_string[NUM_MAX_LENGTH];
    num_string[length] = NULL_CHAR;                 // the final char should be the Null char
    
    /* work backwards to convert each digit in the integer
        to an ASCII char and add to the string */
    for (index = (length - 1); index >= 0; index--)
    {
        current_digit =  int_to_print % 10;         // take off the last digit
        current_digit = get_num_char(current_digit);// convert to ASCII
        num_string[index] = current_digit;
        int_to_print /= 10;                         // remove last digit    
    } /* for */
    
    alt_print(num_string);
} /* int_to_string */

/*
 *  NAME
 *      get_num_length
 *
 *  DESCRIPTION
 *      Return the number of digits in an integer.
 *
 *  PARAMTERS
 *      uint16 num
 *
 *  RETURN
 *      uint8
 */
uint8 get_num_length(uint16 num)
{
    if (num >= FOUR_DIGIT_NUM) return FOUR;
    if (num >= THREE_DIGIT_NUM) return THREE;
    if (num >= TWO_DIGIT_NUM) return TWO;
    return 1;
} /* get_num_length */

/*
 *  NAME
 *      get_num_char
 *
 *  DESCRIPTION
 *      This functions takes a uint8 and returns
 *      the ASCII char.
 *
 *  PARAMTERS
 *      uint8 num
 *
 *  RETURN
 *      uint8
 */
uint8 get_num_char(uint8 num)
{
    uint8 digits_array[] = 
    {
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE
    };
    
    return digits_array[num];
} /* get_num_char */

/*
 *  NAME
 *      print_time
 *
 *  DESCRIPTION
 *      This functions displays a clock to PuTTY.
 *
 *  PARAMTERS
 *      Clock clock
 *
 *  RETURN
 *      NONE
 */
void print_time(Clock clock)
{
    /* add an extra '0' to the beginning of the number
        if it is less than 10 */
    if(clock.hour < TWO_DIGIT_NUM)
    {
        outchar1(ZERO);
    } /* if */
    print_int(clock.hour);
    
    alt_print(COLON);
    
    /* add an extra '0' to the beginning of the number
        if it is less than 10 */
    if(clock.minute < TWO_DIGIT_NUM)
    {
        outchar1(ZERO);
    } /* if */
    print_int(clock.minute);
    
    alt_print(COLON);
    
    /* add an extra '0' to the beginning of the number
        if it is less than 10 */
    if(clock.second < TWO_DIGIT_NUM)
    {
        outchar1(ZERO);
    } /* if */
    print_int(clock.second);
   
} /* print_time */

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