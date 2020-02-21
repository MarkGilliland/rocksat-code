// Experiment Control Board code - Initial
// Drives stepper motor, MOSFET-switched outputs

//Include I2C library
#include <Wire.h>//built in I2C lib
#include <Stepper.h>//built-in stepper lib

//Constants and global variables.
//Define motor direction states
#define CW 0
#define CCW 1
//Define MOSFET and stepper States
#define STEPPER_OFF 1
#define STEPPER_ON 0 //note- stepper motor driver uses inverted logic
#define MOSFET_OFF 0
#define MOSFET_ON 1
//define pins
#define MOSFET_1_PIN 9//PB1
#define MOSFET_2_PIN A2//PC2
#define STEPPER_1_PIN 2//PD2
#define STEPPER_2_PIN 3//PD3
#define STEPPER_3_PIN 4//PD4
#define STEPPER_4_PIN 5//PD5
#define LED_PIN 13
#define SERVO_HEADER_PIN 10

//define motor properties:
#define STEPS_PER_REVOLUTION 513//for stepper library
#define STEPS_PER_LAUNCH 47//roughly 513.0/11.0
#define MAX_STEPPER_SPEED 60//probably need to change this value
//declare stepper object
Stepper launcherStepper(STEPS_PER_REVOLUTION, STEPPER_1_PIN, STEPPER_2_PIN, STEPPER_3_PIN, STEPPER_4_PIN);

int debrisLauncherDegrees = 0;

//Define function that is run whenever the Arduino receives a command from the master
bool receiveCommand(){
  byte currentCommand = 0; //Initialize variable that will temporarily hold command
  while(Wire.available() > 0){
    currentCommand = Wire.read();
  }
  //Define commands below this point
  //Example command
  if(currentCommand == 1){
    //Do whatever that command means
  }
  //Last thing before exiting the function is to clear currentCommand, probably unnecessary, but overly safe. 
  currentCommand = 0;
}

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x02 for Static board, 0x03 for laser board,
  // 0x04 for 
  Wire.begin(0b0000002);          
  // When a command is received from the master, jump to the receiveCommand function and execute the proper command
  Wire.onReceive(receiveCommand); 
  
  // Setup motor driver pins
  pinMode(STEPPER_1_PIN, OUTPUT);
  pinMode(STEPPER_2_PIN, OUTPUT);
  pinMode(STEPPER_3_PIN, OUTPUT);
  pinMode(STEPPER_4_PIN, OUTPUT);
  //setup MOSFET Pins
  pinMode(MOSFET_1_PIN, OUTPUT);
  pinMode(MOSFET_2_PIN, OUTPUT);

  //set all outputs to "off" to start
  digitalWrite(STEPPER_1_PIN, STEPPER_OFF);
  digitalWrite(STEPPER_2_PIN, STEPPER_OFF);
  digitalWrite(STEPPER_3_PIN, STEPPER_OFF);
  digitalWrite(STEPPER_4_PIN, STEPPER_OFF);
  digitalWrite(MOSFET_1_PIN, MOSFET_OFF);
  digitalWrite(MOSFET_2_PIN, MOSFET_OFF); 

  //Init the stepper object with an initial speed
  myStepper.setSpeed(MAX_STEPPER_SPEED);
}

void loop() {
  
  
  
}
