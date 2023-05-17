
/*
 *  NAME
 *      sound.c
 *
 *  DESCRIPTION
 *      This collection of functions for using the timer on the Dragon12 to produce
 *      various sound effects and music. We use the already provided functions from
 *      main.asm in order to accomplish this.
 *
 */

#include "sound.h"



/* GLOBAL */
uint16 g_pitch;

/*
 *  NAME
 *      noise_maker
 *
 *  DESCRIPTION
 *      This function takes the pitch and sends it to the tone assembly routine.
 *      There channels 5 and 7 are used to create a pulse of length g_pitch
 *      through the on-board speaker. When the interrupt is turned on, the pulse
 *      repeats and this is what creates the sounds that are heard.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */ 
void interrupt 13 noise_maker()
{
    tone(g_pitch);
} /* noise_maker */

/*
 *  NAME
 *      start_jingle
 *
 *  DESCRIPTION
 *      This function plays a sequence of notes when the vehicle first starts.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */ 
void start_jingle(void)
{
     
    uint8 incrementor;                      // to increment through arrays
    
    /* the notes of the song */
    uint16 notes[] = 
    {
        C, E, F
    };
    
    /* the length of each note */
    uint16 note_lengths[] = 
    {
        EIGTH_NOTE, EIGTH_NOTE, QUARTER_NOTE
    };
    
    TIE = CLEAR;                            // Clear any existing interrupts
    
    start_sound();
    
    /* play each note for the predetermined length of time */
    for(incrementor = 0; incrementor < START_JINGLE_SIZE; incrementor++)
    {
        g_pitch = notes[incrementor];       // note to be played when sound is turned on
       
        ms_delay(note_lengths[incrementor]);// wait for not to play its intended length   
    } /* for */
    
    stop_sound();                            // turn sound off and re-enable interrupts
} /* start_jingle */

/*
 *  NAME
 *      sound_effect
 *
 *  DESCRIPTION
 *      This function plays a single note for use as a sound effect.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */ 
void sound_effect(void)
{
    TIE = CLEAR;                            // Clear any existing interrupts
    
    g_pitch = C;
    start_sound();
    ms_delay(EIGTH_NOTE);                   // wait for note to play intended duration
    stop_sound();
} /* sound_effect */

/*
 *  NAME
 *      explore_jingle
 *
 *  AUTHORS
 *      Rowan Daly and Anthony Lansing
 *
 *  DESCRIPTION
 *      This function plays a series of notes for when the vehicle starts exploring.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */ 
void explore_jingle(void)
{
    //asm(bclr TIE, #$FF);                    // Disables interrupts
    
    uint8 incrementor;                      // to increment through arrays
    
    /* the notes of the jingle */
    uint16 notes[] = 
    {
        C, E, C, F, G, A
    };
    
    /* the length each note should be played */
    uint16 note_lengths[] = 
    {
        QUARTER_NOTE, EIGTH_NOTE, EIGTH_NOTE, EIGTH_NOTE, EIGTH_NOTE, QUARTER_NOTE
    };
    
    TIE = CLEAR;                            // Clear any existing interrupts
    
    start_sound();
    
    /* play each note for the predetermined length of time */
    for(incrementor = 0; incrementor < EXPLORE_JINGLE_SIZE; incrementor++)
    {
        g_pitch = notes[incrementor];       // note to be played when sound is turned on
       
        ms_delay(note_lengths[incrementor]);// wait for not to play its intended length   
    } /* for */
    
    stop_sound();                            // turn off sound and re-enable interrupts
} /* explore_jingle */

/*
 *  NAME
 *      end_jingle
 *
 *  DESCRIPTION
 *      This function plays a series of notes for when the vehicle ends program.
 *
 *  PARAMETERS
 *      NONE
 *
 *  RETURN
 *      NONE
 */
void end_jingle(void)
{
    
    uint8 incrementer;                      // to increment through arrays
    
    /* the notes of the song */
    uint16 notes[] = 
    {
        F, E, C
    };
    
    /* the duration each note should play */
    uint16 note_lengths[] = 
    {
        EIGTH_NOTE, EIGTH_NOTE, QUARTER_NOTE
    };
    
    TIE = CLEAR;                            // Clear any existing interrupts
    
    start_sound();
     
    /* play each note for the predetermined length of time */
    for(incrementer = 0; incrementer < END_JINGLE_SIZE; incrementer++)
    {
        g_pitch = notes[incrementer];       // note to be played when sound is turned on
        
        ms_delay(note_lengths[incrementer]);// wait for not to play its intended length   
    } /* for */
    
    stop_sound();                            // turn off sound and re-enable interrupts
} /* end_jingle */