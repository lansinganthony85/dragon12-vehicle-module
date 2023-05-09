#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

#ifndef H_eeprom
#define H_eeprom

/* EEPROM opcodes */
#define WREN    0x06                // set write enable latch
#define WRDI    0x04                // reset write enabl latch
#define READ    0x03                // read the memory code
#define WRITE   0x02                // write to memory code

/* SPI definitions */
/* We redefine here in case we wish
to change ports later one */
#define SPIBR   SPI2BR
#define SPICR1  SPI2CR1
#define SPICR2  SPI2CR2
#define SS_LO   SS2_LO
#define SS_HI   SS2_HI
#define SPIDR   SPI2DR
#define SPISR   SPI2SR
#define SS_BITMASK  0x40            // The SS2 port
#define SPI_CLOCKSPEED 0x10         // to set clockspeed to 12MHz
#define SPI_MASTER 0x50             // to set Dragon12 to masterwith CPHA and CPOL at 0

/* GLOBALS */
#define JUNK 0xFF                   // a junk value to send to the EEPROM when we want to read data
#define DATA_SIZE_MEMORY 0x0000     // the reserved memory address for the size of the data set

/* prototypes */
void eeprom_init(void);
void write_char(unsigned char);
void write_int(unsigned int);
uint8 read_char(void);
uint16 read_int(void);
void write_enable(void);
uint8 send_byte(unsigned char);
void set_mem_addr(unsigned int);
void write_disable(void);
void complete_write(void);
uint16 read_data_size(void);

#endif