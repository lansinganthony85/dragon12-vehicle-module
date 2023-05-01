
#ifndef H_data
#define H_data

void eeprom_init(void);
void write_char(unsigned char);
void write_int(unsigned int);
void write_data(struct g_collected_data);
unsigned char read_char(void);
void write_enable(void);
unsigned char send_byte(unsigned char);
void set_mem_addr(unsigned int);

#endif