/* common defines and macros */ 
#include <hidef.h>

/* derivative information */
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

/* interface to the assembly module */
#include "main_asm.h"

/* access the common constants */
#include "dragon.h"
#include "data.h"

void main(void) {
  /* VARIABLES */
  uint8 letter;
  
  PLL_init();        // set system clock frequency to 24 MHz
  
  lcd_init();
  clear_lcd();
  set_lcd_addr(LINE_1);
  
  led_disable();
  seg7_disable();
  
  
  eeprom_init();
  
  write_char('Q');
  letter = read_char(); 
    
  
  data8(letter);
  
} /* main */