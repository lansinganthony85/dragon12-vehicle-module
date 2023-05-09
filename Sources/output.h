#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"
#include "data.h"
#include "clock.h"

#ifndef H_output
#define H_output

#define FRONT_COLLISION "FRONT COLLISION DETECTED"
#define REAR_COLLISION "REAR COLLISION DETECTED"

void write_to_putty(g_collected_data);
void print_int (uint16);
uint8 get_num_length(uint16);
uint8 get_num_char(uint8 num);
void print_time(Clock);
void alt_print(char *);
void write_labels(void);
void output_init(uint16);

#endif