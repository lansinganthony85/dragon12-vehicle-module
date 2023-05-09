#define BATTERY_ADC_CHANNEL          3 // Port A bit 11
#define ADC_MAX_VAL                  0x3FF
#define ADC_4_P_822V                 0x3DA
#define ADC_3_P_444V                 0x2C0
#define ONE_HUNDRED_PERCENT          100

#define ADC_TO_BATTERY_CONV(input)   (((input-ADC_3_P_444V)*ONE_HUNDRED_PERCENT)/(ADC_4_P_822V-ADC_3_P_444V))



// Read the battery level via ADC
//
// INPUTS:
//         none
// OUTPUTS:
//         (char) battery level between 0 and 100%
char read_battery_level(void)
{
  int ADC_battery_reading;
  char battery_level;
  
  // Returns 10 bit reading. Should be reading between 3.444v
  // and 4.822v 
  ADC_battery_reading = ad1conv(BATTERY_ADC_CHANNEL);

  battery_level = ADC_TO_BATTERY_CONV(ADC_battery_reading);
  
  return (battery_level);
}