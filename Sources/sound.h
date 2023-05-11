#include <hidef.h>
#include <mc9s12dg256.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"

#include "main_asm.h"
#include "dragon.h"

#ifndef H_sound
#define H_sound

/* PITCHES USED */
#define C 1434
#define E 1138
#define F 1074
#define G 957
#define A 853

/* NOTE LENGTHS */
#define EIGTH_NOTE 250
#define QUARTER_NOTE (EIGTH_NOTE * 2)
#define DOTTED_QUARTER_NOTE (QUARTER_NOTE + EIGTH_NOTE)
#define HALF_NOTE (QUARTER_NOTE * 2)

/* ARRAY SIZES FOR SONGS */
#define START_JINGLE_SIZE 3
#define EXPLORE_JINGLE_SIZE 6
#define END_JINGLE_SIZE 3

/* PROTOTYPES */
void start_jingle(void);
void sound_effect(void);
void explore_jingle(void);
void end_jingle(void);

#endif