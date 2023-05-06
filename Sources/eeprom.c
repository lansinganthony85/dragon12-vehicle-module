
/*
 *  NAME
 *      eeprom.c
 *
 *  DESCRIPTION
 *      This collection of functions deal with the collecting, saving, and displaying
 *      of data related to the Dragon12 Remote Vehicle. The EEPROM is connected to the
 *      Dragon 12 through SPI1 with the following port connections: 
 *
 */            

#include "eeprom.h"

/* EEPROM opcodes */
#define WREN    0x06        // set write enable latch
#define WRDI    0x04        // reset write enabl latch
#define READ    0x03        // read the memory code
#define WRITE   0x02        // write to memory code

/* SPI definitions */
#define SPIBR   SPI2BR
#define SPICR1  SPI2CR1
#define SPICR2  SPI2CR2
#define SS_LO   SS2_LO
#define SS_HI   SS2_HI
#define SPIDR   SPI2DR
#define SPISR   SPI2SR
#define SS_BITMASK  0x40
#define SPI_CLOCKSPEED 0x10
#define SPI_MASTER 0x50

/* GLOBALS */
#define JUNK 0xFF

uint16 g_data_size_addr = 0x0000;
uint16 g_read_addr = 0x0002;      // the current memory addr of the eeprom for reading
uint16 g_write_addr = 0x0002;     // the current memory addr of the eeprom for writing

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
    SPIBR = SPI_CLOCKSPEED;         // Divide 24MHz by 2
    SPICR1 = SPI_MASTER;         // set Dragon12 as master with CPHA and CPOL to 0
    SPICR2 = CLEAR;        // Disable MODF
    DDRP = SS_BITMASK;            // SS2 as an output port
    SS_HI();               // disable slave for now       
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
    write_enable();
    SS_LO();
    (void) send_byte(WRITE);
    set_mem_addr(g_write_addr);
    (void) send_byte(char_to_send);     // return value not needed so void
    SS_HI();
    write_disable();
    
    g_write_addr++;
           
} /* write_char */

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
    write_enable();
    SS_LO();
    (void) send_byte(WRITE);
    set_mem_addr(g_write_addr);
    (void)send_byte(int_to_send>>8);    // send upper byte
    (void)send_byte(int_to_send);       // send lower byte
    SS_HI();
    
    write_disable();
    
    g_write_addr += 2;
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
    uint8 char_to_return;
    
    SS_LO();
    (void) send_byte(READ);
    set_mem_addr(g_read_addr);
    char_to_return = send_byte(JUNK);
    SS_HI();
    g_read_addr++;
    
    return char_to_return;
    
} /* read_char */

uint16 read_int(void)
{
    uint16 int_to_return;
    
    SS_LO();
    (void) send_byte(READ);
    set_mem_addr(g_read_addr);
    int_to_return = send_byte(JUNK);
    int_to_return <<= 8;
    int_to_return &= 0xFF00;
    int_to_return |= send_byte(JUNK);
    SS_HI();
    
    g_read_addr += 2;
    
    return int_to_return;
} /* read_int */

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
    SS_LO();
    (void) send_byte(WREN);
    SS_HI();
} /* write_enable */

void write_disable(void)
{
    SS_LO();
    (void) send_byte(WRDI);
    SS_HI();   
}

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
    while (!(SPI2SR & (1<<5)));
    
    SPIDR = byte_to_send;
    
    /* wait for transfer to complete */
    while (!(SPISR & (1<<7)));
    
    return SPIDR;
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
    (void) send_byte(address);     // send lower byte
} /* set_mem_addr */

void complete_write(void)
{
    uint16 data_size = get_data_size();
    write_enable();
    SS_LO();
    (void) send_byte(WRITE);
    set_mem_addr(g_data_size_addr);
    (void)send_byte(data_size>>8);    // send upper byte
    (void)send_byte(data_size);       // send lower byte
    SS_HI();
    
    write_disable();   
}

uint16 read_data_size(void)
{
    uint16 data_size;
    
    SS_LO();
    (void) send_byte(READ);
    set_mem_addr(g_data_size_addr);
    data_size = send_byte(JUNK);
    data_size <<= 8;
    data_size &= 0xFF00;
    data_size |= send_byte(JUNK);
    SS_HI();
    
    return data_size;    
}