// A function to set the PWM output to control the speed of the motors.
// INPUTS:
//         (char) motor_num: used to select motor 1 or 2 (PP1 and PP2)
//         (char) speed_plus_minus_100: a signed char between -100 and 100
// OUTPUTS:
//         none 
void set_motor_speed(char motor_num, char speed_plus_minus_100)
{
  char speed_1ms_to_2ms;
  
  // Changes scale from 1ms to 2ms if 255 = 10ms
  // 25.5 is 1/10 of 255, so 1ms.
  // The formula is a standard range conversion formula from (-100 to 100),
  // to (0 to 25.5) or (0ms to 1ms). An offset of 25.5 (1ms) is added to
  // give a final range of (1ms to 2ms).
  speed_1ms_to_2ms = 25.5+(((speed_plus_minus_100 + 100.0)/200.0)*25.5);
    
  if (motor_num == 1)
  {
    motor1(speed_1ms_to_2ms);
  }
  
  else if (motor_num == 2)
  {
    motor2(speed_1ms_to_2ms); 
  }

}