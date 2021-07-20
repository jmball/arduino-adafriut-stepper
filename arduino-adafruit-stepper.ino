#include <Wire.h>
#include <Adafruit_MotorShield.h>
                                                                                                                               
// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);

// set pins for limit switches
const int leftLimitPin =  2;
const int rightLimitPin =  3;

// define variables for storing limit switch states
int leftLimitState = 1;
int rightLimitState = 1;

// define variables for serial commands
String cmd = "";
String ret = "";
String func = "";
String steps = "";
String dir = "";

// define variables for storing comma indices in commands
int i0 = 0;
int i1 = 0;


void setup() {
  // set up Serial library at 115200 bps
  Serial.begin(19200);           
  Serial.setTimeout(-1);
  
  // set pin modes
  pinMode(leftLimitPin, INPUT);
  pinMode(rightLimitPin, INPUT);
  
  // initialise motor with default frequency 1.6KHz
  AFMS.begin(1600); 

  // set motor speed in rpm 
  myMotor->setSpeed(300);

  // check state of limit switches and turn back if reached
  leftLimitState = digitalRead(leftLimitPin);
  rightLimitState = digitalRead(rightLimitPin);
  if (leftLimitState == 0) {
    myMotor->step(50, BACKWARD, DOUBLE);
  }
  if (rightLimitState == 0) {
    myMotor->step(50, FORWARD, DOUBLE);
  }
  myMotor->release();
  
  // print status
  // ret = "ready";
  // ret += "\n";
  // Serial.print(ret);
}

void loop() {
  // read serial command
  cmd = Serial.readStringUntil(10);
  i0 = cmd.indexOf(',');
  
  if (cmd == "status?") {
    ret = "idle";
    ret += "\n";
    Serial.print(ret);
  }
  // Decompose serial command. Valid serial commands for the step
  // function are of the form: func,steps,dir
  else if (i0 != -1) {
    i1 = cmd.indexOf(',', i0+1);
    func = cmd.substring(0, i0);
    steps = cmd.substring(i0+1, i1);
    dir = cmd.substring(i1+1, cmd.length());
    ret = cmd;
    ret += "\n";
    Serial.print(ret);
  }
  else {
    ret = "invalid command";
    ret += "\n";
    Serial.print(ret);
  }
  
  // step motor if step function is called
  if (func == "step") {
    for (int i=0; i<=steps.toInt(); i++) {
      // step motor if valid command given
      if (dir == "backward") {
        myMotor->step(1, BACKWARD, DOUBLE);
      }
      else if (dir == "forward") {
        myMotor->step(1, FORWARD, DOUBLE);
      }

      // check state of limit switches
      leftLimitState = digitalRead(leftLimitPin);
      rightLimitState = digitalRead(rightLimitPin);
      if (leftLimitState == 0) {
        myMotor->step(50, BACKWARD, DOUBLE);
        break;
      }
      if (rightLimitState == 0) {
        myMotor->step(50, FORWARD, DOUBLE);
        break;
      }
    }
    // reset variables
    func = "";
    steps = "";
    dir = "";
    ret = "idle";
    ret += "\n";
    Serial.print(ret);
    myMotor->release();
  } 
}
