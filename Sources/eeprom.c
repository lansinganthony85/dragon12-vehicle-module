
/*
 *  NAME
 *      eeprom.c
 *
 *  AUTHORS
 *      Rowan Daly and Anthony Lansing
 *
 *  DESCRIPTION
 *      This collection of functions deal with the reading and writing
 *      of data related to the RC Exploration Vehicle. The EEPROM is connected to the
 *      Dragon 12 through SPI2 with the following Port P connections: SCK connected to
 *      PP7, MISO connected to PP4, MOSI connected to PP5, and CS connected to PP6.
 *      We interface with the EEPROM by sending opcodes to indicate the operations
 *      we wish to perform. The reading and writing of data always starts at 
 *      address 0x0002. Address 0x0000 is reserved for the size of the data set writting
 *      to the EEPROM.
 *
 */            

#include "eeprom.h"

uint16 g_read_addr = 0x0002;        // the current memory addr of the eeprom for reading
uint16 g_write_addr = 0x0002;       // the current memory addr of the eeprom for writing

/*
 *  DESCRIPTION
 *      This functions is similar to the SPI2_init subroutine with the difference
 *      that SS2 is left in high state to disable until data is ready to be read
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
    SPICR1 = SPI_MASTER;            // set Dragon12 as master with CPHA and CPOL to 0
    SPICR2 = CLEAR;                 // Disable MODF
    DDRP = SS_BITMASK;              // SS2 as an output port
    SS_HI();                        // disable slave for now       
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
 *      uint8 char_to_send
 *
 *  RETURN
 *      None
 */
void write_char(uint8 char_to_send)
{   
    write_enable();
    SS_LO();
    (void) send_byte(WRITE);            // send the command to eeprom
    set_mem_addr(g_write_addr);         // send the address
    (void) send_byte(char_to_send);     // return value not needed so void
    SS_HI();
    write_disable();
    
    g_write_addr++;                     // go to next memory address
           
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
    (void) send_byte(WRITE);            // send the command to the eeprom
    set_mem_addr(g_write_addr);         // send the memory address to write to
    (void)send_byte(int_to_send>>8);    // send upper byte
    (void)send_byte(int_to_send);       // send lower byte
    SS_HI();
    
    write_disable();
    
    g_write_addr += 2;                  // memory address incremented by 2 bytes
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
 *      uint8 char_to_return
 */
uint8 read_char(void)
{
    uint8 char_to_return;
    
    SS_LO();
    (void) send_byte(READ);                 // send the command to read
    set_mem_addr(g_read_addr);              // send the address we are reading from
    char_to_return = send_byte(JUNK);       // send junk value to just get the return value
    SS_HI();
    g_read_addr++;                          // increment to next memory address
    
    return char_to_return;
    
} /* read_char */

/*
 *  NAME
 *      read_int
 *
 *  DESCRIPTION
 *      This function reads an int from the current memory address of the eeprom
 *      and returns it to the calling function.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      uint16 int_to_return
 */
uint16 read_int(void)
{
    uint16 int_to_return;
    
    SS_LO();
    (void) send_byte(READ);                 // send the command to read
    set_mem_addr(g_read_addr);              // send the address we are reading from
    int_to_return = send_byte(JUNK);        // get the upper byte of the int
    int_to_return <<= 8;                    // shift the upper byte
    int_to_return &= 0xFF00;                // ensure that lower byte is clear
    int_to_return |= send_byte(JUNK);       // get lower byte
    SS_HI();
    
    g_read_addr += 2;                       // increment address by two bytes
    
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

/*
 *  NAME
 *      write_disable
 *
 *  DESCRIPTION
 *      This functions sends the WRDI opcode to the eeprom to disable writing.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      None
 */
void write_disable(void)
{
    SS_LO();
    (void) send_byte(WRDI);
    SS_HI();   
} /* write_disable */

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
    while (!(SPISR & (1<<5)));
    
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
    (void) send_byte(address>>8);           // send upper byte
    (void) send_byte(address);              // send lower byte
} /* set_mem_addr */

/*
 *  NAME
 *      complete_write
 *
 *  DESCRIPTION
 *      This function writes the size of the data set to memory address 0x0000
 *      of the eeprom.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      None
 */
void complete_write(void)
{
    uint16 data_size = get_data_size();     // get the current data size
    write_enable();
    SS_LO();
    (void) send_byte(WRITE);
    set_mem_addr(DATA_SIZE_MEMORY);         // set memory address to 0x0000
    (void)send_byte(data_size>>8);          // send upper byte
    (void)send_byte(data_size);             // send lower byte
    SS_HI();
    
    write_disable();   
} /* complete_write */

/*
 *  NAME
 *      read_data_size
 *
 *  DESCRIPTION
 *      This function reads the size of the data set at memory address 0x0000
 *      of the eeprom and returns.
 *
 *  PARAMETERS
 *      None
 *
 *  RETURN
 *      uint16 data_size
 */
uint16 read_data_size(void)
{
    uint16 data_size;
    
    SS_LO();
    (void) send_byte(READ);                 // send the code to READ
    set_mem_addr(DATA_SIZE_MEMORY);         // set memory address to 0x0000
    data_size = send_byte(JUNK);            // get the upper byte
    data_size <<= 8;                        // shift upper byte
    data_size &= 0xFF00;                    // make sure lower byte is cleared
    data_size |= send_byte(JUNK);           // get lower byte
    SS_HI();
    
    return data_size;    
} /* read_data_size */