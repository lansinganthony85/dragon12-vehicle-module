#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "main_asm.h" /* interface to the assembly module */



#define INTERRUPT_DISABLE_DELAY_MS         5     // At least 2x control pulse width  

#define PWM_HIGH_TO_PLUS_MINUS_100(input)  100*2.0*(input-2250.0)/1500.0 // -100 to 100


// Function prototypes
char deadzone_handler(char input_decimal);
char get_high_time_decimal(char channel_number);



// Function to read PWM high time from RC receiver. The interrupt is
// enabled for the respective input channel, it waits for at least 2x
// the PWM period, converts the range to +- 100, and disables the
// interrupt. It then passes the value through the deadzone handler
// before returning.
// INPUTS:
//         (char) channel_number: specify PWM channel 1 or 2 (PT1 and PT2)
// OUTPUTS:
//         (char) a signed char between -100 and 100 
char get_high_time_decimal(char channel_number)
{
  signed char high_time = 0;
  
  if (channel_number == 1)
  {
    //TIE |= 0x02;                    
    TIE = 0x02;                     // Enables interrupts on ONLY PTT1
    ms_delay(INTERRUPT_DISABLE_DELAY_MS);
    high_time = PWM_HIGH_TO_PLUS_MINUS_100(get_HI_time1());
    TIE &= ~0x02;                   // Disables interrupts on PTT1
    //asm(bclr TIE, #$02);          
    
  }
  
  else if (channel_number == 2)
  {
    //TIE |= 0x04;                    
    TIE = 0x04;                     // Enables interrupts on ONLY PTT2
    ms_delay(INTERRUPT_DISABLE_DELAY_MS);
    high_time = PWM_HIGH_TO_PLUS_MINUS_100(get_HI_time2());
    TIE &= ~0x04;                   // Disables interrupts on PTT2
    //asm(bclr TIE, #$04);           
  }
  
     
  return deadzone_handler(high_time); 
}



// A function to deal with the deadzones of the controller.
// If input value is within 5 of -100, 0, or 100, it is rounded
// to that value.
// INPUTS:
//         (char) input_decimal: a signed char between -100 and 100
// OUTPUTS:
//         (char) a signed char between -100 and 100
char deadzone_handler(char input_decimal)
{
  char output_decimal = input_decimal;
  
  // If within +-5 of 0, round to 0
  if ((input_decimal >= -5) && (input_decimal <= 5))
  {
    output_decimal = 0;
  }
  
  // If within 5 of 100, round to 100
  else if (input_decimal >= 95)
  {
    output_decimal = 100; 
  }
  
  // If within 5 of -100, round to -100
  else if (input_decimal <= -95)
  {
    output_decimal = -100; 
  }
  
  return output_decimal;
};
