#define INTERRUPT_OUTPUT_PIN_FRONT    2
#define INTERRUPT_OUTPUT_PIN_BACK     3
#define READ_DELAY_MS                 100
#define TRIGGER_CLEAR_DELAY           2
#define ULTRASONIC_TRIGGER_DELAY      10
#define FRONT_LEFT_INPUT_PIN          7
#define FRONT_RIGHT_INPUT_PIN         8
#define BACK_INPUT_PIN                9
#define PULSE_TO_CM_CONV_FACTOR       0.01723
#define DISTANCE_THRESHOLD_CM         50

int cm_front_left = 0;
int cm_front_right = 0;
int cm_back = 0;




long readUltrasonicDistance(int triggerPin, int echoPin)
{
  // Clear the trigger
  pinMode(triggerPin, OUTPUT);  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(TRIGGER_CLEAR_DELAY);

  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(ULTRASONIC_TRIGGER_DELAY);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  
 // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void setup()
{
  pinMode(INTERRUPT_OUTPUT_PIN_FRONT, OUTPUT);
  pinMode(INTERRUPT_OUTPUT_PIN_BACK, OUTPUT);
}

void loop()
{
  // measure the ping time in cm for all three sensors
  cm_front_left = PULSE_TO_CM_CONV_FACTOR * readUltrasonicDistance(FRONT_LEFT_INPUT_PIN, FRONT_LEFT_INPUT_PIN);
  cm_front_right = PULSE_TO_CM_CONV_FACTOR * readUltrasonicDistance(FRONT_RIGHT_INPUT_PIN, FRONT_RIGHT_INPUT_PIN);
  cm_back = PULSE_TO_CM_CONV_FACTOR * readUltrasonicDistance(BACK_INPUT_PIN, BACK_INPUT_PIN);
  
  // If either of the front sensors detect an object nearby, pull output pin low
  // Else pull high
  if ((cm_front_left <= DISTANCE_THRESHOLD_CM) || (cm_front_right <= DISTANCE_THRESHOLD_CM))
  {
    digitalWrite(INTERRUPT_OUTPUT_PIN_FRONT, LOW);    
  }
  else
  {
    digitalWrite(INTERRUPT_OUTPUT_PIN_FRONT, HIGH);
  }

  // If the back sensor detects an object nearby, pull output pin low
  // Else pull high
  if (cm_back <= DISTANCE_THRESHOLD_CM)
    {
    digitalWrite(INTERRUPT_OUTPUT_PIN_BACK, LOW);    
  }
  else
  {
    digitalWrite(INTERRUPT_OUTPUT_PIN_BACK, HIGH);
  }


  delay(READ_DELAY_MS);
}