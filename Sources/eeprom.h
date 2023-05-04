#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

#ifndef H_eeprom
#define H_eeprom

void eeprom_init(void);
void write_char(unsigned char);
void write_int(unsigned int);
unsigned char read_char(void);
unsigned int read_int(void);
void write_enable(void);
unsigned char send_byte(unsigned char);
void set_mem_addr(unsigned int);
void write_disable(void);
void complete_write(void);
uint16 read_data_size(void);

#endif