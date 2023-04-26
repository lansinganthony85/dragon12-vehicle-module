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
#define WREN 6
#define WRITE 2
#define READ 3

/* PROTOTYPES */
//void eeprom_init(void);
uint8 spi_transfer(uint8 data);

void main(void) {
  /* VARIABLES */
  uint8 letter;
  
  PLL_init();        // set system clock frequency to 24 MHz
  
  lcd_init();
  clear_lcd();
  set_lcd_addr(LINE_1);
  
  led_disable();
  seg7_disable();
  
  SPI1_init();
  
  SS1_LO();
  spi_transfer(WREN);
  SS1_HI();
  
  SS1_LO();
  spi_transfer(WRITE);
  spi_transfer(0);
  spi_transfer(0);
  spi_transfer('A');
  SS1_HI();
  
  SS1_LO();
  spi_transfer(READ);  
  spi_transfer(0);
  spi_transfer(0);
  letter = spi_transfer(0xFF);
  SS1_HI(); 
  
  data8(letter);
  
} /* main */

uint8 spi_transfer(uint8 data)
{

  SPI1DR = data;                    // Start the transmission

  while (!(SPI1SR & (1<<7)))     // Wait the end of the transmission

  {

  };

  return SPI1DR;                    // return the received byte
}