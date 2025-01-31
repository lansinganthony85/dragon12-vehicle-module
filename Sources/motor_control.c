//
//  AUTHORS
//      Rowan Daly and Anthony Lansing
// 
// Code to combine values from LR and FB controls. Values are
// added together, and if either is over 100 or under -100, both
// values are scaled to not exceed those limits.
//
// For more info see:
// https://electronics.stackexchange.com/a/19671
// INPUTS:
//         (int) global variables motor_left_speed and motor_right_speed
// OUTPUTS:
//         (int) modifies global inputs to their scaled values 
extern int motor_left_speed;
extern int motor_right_speed;


void scale_motor_speed(void)
{
  static float motor_scalar = 1;
  
  if ((motor_left_speed > 100) || (motor_left_speed < -100))
  {
    motor_scalar = abs_value(100.0/(float)motor_left_speed);

    motor_left_speed *= motor_scalar;
    motor_right_speed *= motor_scalar; 
  }
  else if ((motor_right_speed > 100) || (motor_right_speed < -100))
  {
    motor_scalar = abs_value(100.0/(float)motor_right_speed);

    motor_left_speed *= motor_scalar;
    motor_right_speed *= motor_scalar; 
  }
  else
  {
    motor_scalar = 1; 
  }
  
}



extern char g_low_battery_5_percent; 

// A function to set the PWM output to control the speed of the motors.
// If the battery level is under 10%, the robot will not move.
// INPUTS:
//         (char) motor_num: used to select motor 1 or 2 (PP1 and PP2)
//         (char) speed_plus_minus_100: a signed char between -100 and 100
//         (char) global variable g_low_battery_5_percent
// OUTPUTS:
//         none 
void set_motor_speed(char motor_num, char speed_plus_minus_100)
{
  char speed_1ms_to_2ms;
  
   // Refuse to move if battery level is below 5%
  if (g_low_battery_5_percent)
  {
    speed_plus_minus_100 = 0;
  }
  
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