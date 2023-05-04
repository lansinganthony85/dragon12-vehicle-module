//  Dragon.h

#ifndef _DRAGON_H_
#define _DRAGON_H_

/* TRUE/FALSE values */
#define TRUE    1
#define FALSE   0

#define CLEAR 0x00
#define SETALL 0xFF
#define BIT1 0x01

/* pushbutton bitmasks */
#define PORTH_SW5_BITMASK   0x01
#define PORTH_SW4_BITMASK   0x02
#define PORTH_SW3_BITMASK   0x04
#define PORTH_SW2_BITMASK   0x08

/* 7seg displays */
#define DIG0    0x03
#define DIG1    0x02
#define DIG2    0x01
#define DIG3    0x00

/* 7-seg display bitmasks */
#define DIG0_BITMASK 0x07
#define DIG1_BITMASK 0x0B
#define DIG2_BITMASK 0x0D
#define DIG3_BITMASK 0x0E

/* SW bitmasks */
#define SW2_BITMASK 0x08
#define SW3_BITMASK 0x04
#define SW4_BITMASK 0x02
#define SW5_BITMASK 0x01

/* Constants for LCD */
#define MAX_CHAR_PER_LINE 16
#define MAX_CHAR_ON_SCREEN 32
#define LINE_1      0x00
#define LINE_1_LAST 0x0F
#define LINE_2      0x40
#define LINE_2_LAST 0x4F 

/* common data types */
typedef     signed              char    sint8;
typedef     unsigned            char    uint8;
typedef     signed              int     sint16;
typedef     unsigned            int     uint16;
typedef     signed      long    int     sint32;
typedef     unsigned    long    int     uint32;




#endif