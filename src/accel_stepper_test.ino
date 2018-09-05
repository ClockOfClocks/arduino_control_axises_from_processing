#include <Arduino.h>
#include <AccelStepper.h>
#define HALFSTEP 8

// Motor pin definitions
#define motor1Pin1  23     // IN1 on the ULN2003 driver 1
#define motor1Pin2  25     // IN2 on the ULN2003 driver 1
#define motor1Pin3  27     // IN3 on the ULN2003 driver 1
#define motor1Pin4  29     // IN4 on the ULN2003 driver 1

#define motor2Pin1  31     // IN1 on the ULN2003 driver 2
#define motor2Pin2  33     // IN2 on the ULN2003 driver 2
#define motor2Pin3  35     // IN3 on the ULN2003 driver 2
#define motor2Pin4  37     // IN4 on the ULN2003 driver 2

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motor1Pin1, motor1Pin3, motor1Pin2, motor1Pin4);
AccelStepper stepper2(HALFSTEP, motor2Pin1, motor2Pin3, motor2Pin2, motor2Pin4);

int n = 0;
int fullRotationSteps = 4075;
int oneDegreeSteps  = floor(fullRotationSteps / 360);

// the possible states of the state-machine
typedef enum {  NONE, GOT_R, GOT_S, GOT_G } states;

// current state-machine state
states state = NONE;
// current partial number
unsigned int currentValue;

void setup() {
  stepper1.setAcceleration(ceil(fullRotationSteps/4));
  stepper1.setSpeed(ceil(fullRotationSteps/4));
  stepper1.setCurrentPosition(0);
  stepper1.moveTo(ceil(fullRotationSteps/12)); // just for test

  stepper2.setAcceleration(ceil(fullRotationSteps/4));
  stepper2.setSpeed(ceil(fullRotationSteps/4));
  stepper2.setCurrentPosition(0);
  stepper2.moveTo(ceil(fullRotationSteps/12)); // just for test

  Serial.begin(9600);
  state = NONE;
}

void processRPM (const unsigned int value)
{
  // do something with RPM
  //Serial.print ("RPM = ");
  //Serial.println (value);
} // end of processRPM

void processSpeed (const unsigned int value)
{
  // do something with speed
  //Serial.print ("Speed = ");
  //Serial.println (value);
  stepper2.moveTo(value * oneDegreeSteps);

} // end of processSpeed

void processGear (const unsigned int value)
{
  // do something with gear
  //Serial.print ("Gear = ");
  Serial.println (value);


  stepper1.moveTo(value * oneDegreeSteps);

} // end of processGear

void handlePreviousState ()
{
  switch (state)
  {
  case GOT_R:
    processRPM (currentValue);
    break;
  case GOT_S:
    processSpeed (currentValue);
    break;
  case GOT_G:
    processGear (currentValue);
    break;
  }  // end of switch

  currentValue = 0;
}  // end of handlePreviousState

void processIncomingByte (const byte c)
{
  if (isdigit (c))
  {
    currentValue *= 10;
    currentValue += c - '0';
  }  // end of digit
  else
  {

    // The end of the number signals a state change
    handlePreviousState ();

    // set the new state, if we recognize it
    switch (c)
    {
    case 'R':
      state = GOT_R;
      break;
    case 'S':
      state = GOT_S;
      break;
    case 'G':
      state = GOT_G;
      break;
    default:
      state = NONE;
      break;
    }  // end of switch on incoming byte
  } // end of not digit

} // end of processIncomingByte

void loop() {
  while (Serial.available ()){
    processIncomingByte (Serial.read ());
  }

  // do other stuff in loop as required
  stepper1.run();
  stepper2.run();
}
