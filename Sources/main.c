/* common defines and macros */ 
#include <hidef.h>

/* derivative information */
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

/* interface to the assembly module */
#include "main_asm.h"

/* access the common constants */
#include "dragon.h"

/* GLOBALS */
#define VRX_CHANNEL 3
#define VRY_CHANNEL 4
#define BAUDRATE 19200
#define TRVL 0x20
#define CLRP 0x28
#define SREV 0x30
#define SMAX 0x40
#define SSRR 0x48

/* ID1 is left side and ID2 is right side */
#define ID1 0x00
#define ID2 0x00

/* PROTOTYPES */
void trvl(uint8, sint16);
void clrp(uint8);
void srev(uint8);
void smax(uint8, uint16);
void ssrr(uint8, uint8);

void main(void) {
  /* VARIABLES */
  sint16 x, y;
  sint8 x_rate, y_rate;
  
  PLL_init();        // set system clock frequency to 24 MHz 
  
  /* Initialize Serial */
  SCI1_init(BAUDRATE);
  
  /* Initialize ADC */
  ad1_enable();
  
  /* Initialize LCD */
  lcd_init();
  clear_lcd();
  
 // srev(ID2);
  //smax(0x00, 30000);
  
  while (TRUE)
  {
    x = ad1conv(VRX_CHANNEL) - 512;
    y = ad1conv(VRY_CHANNEL) - 512;
    x_rate = x / 4;
    x_rate = (x_rate < 0) ? (x_rate * -1) : (x_rate);
    y_rate = y / 4;
    y_rate = (y_rate < 0) ? (y_rate * -1) : (y_rate);
    set_lcd_addr(LINE_1);
    write_int_lcd(x);
    set_lcd_addr(LINE_2);
    write_int_lcd(y);
    
    ssrr(ID2, x_rate);
        trvl(ID2, x);
        
    if (x < -10)
    {
        ssrr(ID2, x_rate);
        trvl(ID2, x);     
    }
    else if (x > 10)
    {
        ssrr(ID1, x_rate);
        trvl(ID1, x_rate);
    }
    else if (y < -10 || y > 10)
    {
        ssrr(0x00, y_rate);
        trvl(0x00, y);
    }
    else
    {
        trvl(0x00, 0);
    }
    
     
  } /* while */
} /* main */

void trvl(uint8 ID, sint16 positions)
{
    uint8 upper_byte, lower_byte;
    uint8 command = TRVL;
    command |= ID;
    upper_byte = positions >> 8;
    lower_byte = positions;
    
    outchar1(command);
    outchar1(upper_byte);
    outchar1(lower_byte);
} /* trvl */

void clrp(uint8 ID)
{
    uint8 command = CLRP;
    command |= ID;
    
    outchar1(command);    
} /* clrp */

void srev(uint8 ID)
{
    uint8 command = SREV;
    command |= ID;
    
    outchar1(command);    
} /* srev */

void smax(uint8 ID, uint16 maxspeed)
{
    uint8 upper_byte, lower_byte;
    uint8 command = SMAX;
    command |= ID;
    upper_byte = maxspeed >> 8;
    lower_byte = maxspeed;
    
    outchar1(command);
    outchar1(upper_byte);
    outchar1(lower_byte);
} /* smax */

void ssrr(uint8 ID, uint8 rate)
{
    uint8 command = SSRR;
    command |= ID;
    
    outchar1(command);
    outchar1(rate);
} /* ssrr */