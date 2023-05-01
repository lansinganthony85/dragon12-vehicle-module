
/*
 *  NAME
 *      data.c
 *
 *  DESCRIPTION
 *      This collection of functions deal with the collecting, saving, and displaying
 *      of data related to the Dragon12 Remote Vehicle. The EEPROM is connected to the
 *      Dragon 12 through SPI1 with the following port connections: 
 *
 */            

#include "data.h"
#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

/* EEPROM opcodes */
#define WREN    0x06        // set write enable latch
#define WRDI    0x04        // reset write enabl latch
#define RDSR    0x05        // read the status register
#define WRSR    0x01        // write to the status register
#define READ    0x03        // read the memory code
#define WRITE   0x02        // write to memory code
#define RDID    0x9F        // read device ID

/* GLOBALS */
uint16 g_read_addr = 0x0000;      // the current memory addr of the eeprom
uint16 g_write_addr = 0x0000;
uint8 g_data_struct_size = 10;
uint16 g_data_size = 0;

/* the following struct is used to store the data that is collected by the vehicle.
    The struct serves as the structure of the eeprom so that we can read and write
    to it without worrying about the different sizes of the datatypes. Since data
    is all in a struct, everything written and read to/from eeprom will be the same
    size.
*/
typedef struct   
{
    uint8 time_hr;                  // 1 byte size
    uint8 time_min;                 // 1 byte size
    uint8 time_sec;                 // 1 byte size
    uint16 temperature;              // 2 bytes size
    uint16 light_level;             // 2 bytes size             
    uint16 humidity_level;           // 2 bytes size
    uint8 collision_detected;       // 1 byte size
} g_collected_data; // total size is 10 bytes


/*
 *  DESCRIPTION
 *      This functions is similar to the SPI1_init subroutine with the difference
 *      that SS1 is left in high state to disable until data is ready to be read
 *      or written to the eeprom.
 *
 *  PARAMETERS
 *      None
 * 
 *  RETURN
 *      None
 */
void eeprom_init(void)
{
    SPI1BR = 0x10;         // Divide 24MHz by 2
    SPI1CR1 = 0x50;         // set Dragon12 as master with CPHA and CPOL to 0
    SPI1CR2 = CLEAR;        // Disable MODF
    DDRP = 0x08;            // SS1 as an output port
    SS1_HI();               // disable slave for now       
} /* eeprom_init */

/*
 *  NAME
 *      char_byte
 *
 *  DESCRIPTION
 *      This function takes an unsigned char and writes to the current memory address
 *      on the eeprom.
 *
 *  PARAMETERS
 *      unsigned char char_to_send
 *
 *  RETURN
 *      None
 */
void write_char(uint8 char_to_send)
{    
    //SS1_LO();
    (void) send_byte(char_to_send);     // return value not needed so void
    //SS1_HI();       
} /* write_byte */

/*
 *  NAME
 *      write_int
 *
 *  DESCRIPTION
 *      This function takes an unsigned int and writes it to the current memory addr
 *      on the eeprom.
 *
 *  PARAMETERS
 *      unsigned int int_to_send
 *
 *  RETURN
 *      NONE
 */
void write_int(uint16 int_to_send)
{
    //SS1_LO();
    (void)send_byte(int_to_send>>8);    // send upper byte    (void)send_byte(int_to_send);       // send lower byte    //SS1_HI();
} /* write_int */

/*
 *  NAME
 *      read_char
 *
 *  DESCRIPTION
 *      This function reads a char from the current memory address of the eeprom
 *      and returns it to the calling function.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      unsigned char
 */
unsigned char read_char(void)
{
    uint8 read_byte = 0;
        
    SS1_LO();
    (void) send_byte(READ);
    set_mem_addr(g_read_addr);
    read_byte = send_byte(read_byte);
    SS1_HI();
    
    g_read_addr++;
    
    return read_byte;
} /* read_byte */

/*
 *  NAME
 *      write_enable
 *
 *  DESCRIPTION
 *      This functions sends the WREN opcode to the eeprom to enable writing.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      None
 */
void write_enable(void)
{
    SS1_LO();
    (void) send_byte(WREN);
    SS1_HI();
} /* write_enable */

/*
 *  NAME
 *      write_data
 *
 *  DESCRIPTION
 *      This function writes the data from a struct onto the current memory addr
 *      to the eeprom.
 *
 *  PARAMETERS
 *      struct data_struct
 *
 *  RETURN
 *      NONE
 */
void write_data(g_collected_data data_struct)
{
    write_enable();
    
    SS1_LO();
    set_mem_addr(g_write_addr);
    write_char(data_struct.time_hr);
    write_char(data_struct.time_min);
    write_char(data_struct.time_sec);
    write_int(data_struct.temperature);
    write_int(data_struct.light_level);
    write_int(data_struct.humidity_level);
    write_char(data_struct.collision_detected);
    SS1_HI();
    
    g_write_addr += g_data_struct_size;    
} /* write_data */

/*
 *  NAME
 *      send_byte
 *
 *  DESCRIPTION
 *      This functions takes an unsigned char and sends it to the eeprom. Once transfer
 *      is complete, the value transfered from the eeprom is returned.
 *
 *  PARAMTERS
 *      unsigned char byte_to_send
 *
 *  RETURN
 *      unsigned char in SPI1DR
 */
uint8 send_byte(uint8 byte_to_send)
{
    /* wait for SPTEF */
    while (!(SPI1SR & (1<<5)));
    
    SPI1DR = byte_to_send;
    
    /* wait for transfer to complete */
    while (!(SPI1SR & (1<<7)));
    
    return SPI1DR;
} /* send_byte */

/*
 *  NAME
 *      set_mem_addr
 *
 *  DESCRIPTION
 *      This function send the current available memory address to the eeprom to
 *      select the memory address for reading or writing.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      None
 */
void set_mem_addr(uint16 address)
{
    (void) send_byte(address>>8);        // send upper byte
    (void) send_byte(address);           // send lower byte
} /* set_mem_addr */