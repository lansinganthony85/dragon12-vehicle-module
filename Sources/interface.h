/* common defines and macros */ 
#include <hidef.h>

/* derivative information */
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

/* interface to the assembly module */
#include "main_asm.h"

/* access the common constants */
#include "dragon.h"
#include "sound.h"

#ifndef H_interface
#define H_interface

/* GLOBAL CONSTANTS */
#define DATA_FREQ_MINIMUM   10
#define DATA_FREQ_MAXIMUM   60
#define DATA_FREQ_INCREMENT 5

/* TEXT FOR LCD */
#define WELCOME_LABEL   "WELCOME"
#define EXPLORE_LABEL   "EXPLORE"
#define DATA_LABEL      "GET DATA"
#define SELECTOR        '*'
#define BLANK_SPACE     ' '
#define DATA_FREQ_LABEL "DATA FREQUENCY: "
#define EXPLORING_LABEL "EXPLORING..."
#define PRESS_ENTER     "PRESS ENTER KEY"
#define DATA_READY      "DATA READY"
#define PRINTING_LABEL  "PRINTING DATA..."
#define DONE_LABEL      "DONE."
#define GOODBYE_MESS    "GOOD BYE"

/* TEXT LCD ADDRESSES */
#define EXPLORE_SELECT_ADDR 0x07
#define DATA_SELECT_ADDR 0x48

/* PORT M BITMASKS FOR BUTTONS */
#define ENTER_BITMASK 0x04                  // Connected to PM2
#define BUTTON1_BITMASK 0x01                // Connected to PM0
#define BUTTON2_BITMASK 0x02                // Connected to PM1

/* PROTOTYPES */
uint8 get_mode(void);
uint8 get_frequency(void);
void wait_for_enter_press(void);

#endif