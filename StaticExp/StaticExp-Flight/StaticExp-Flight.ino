// WIP flight code for the static experiment for Mines Rocksat-X 2020
// By: Graham Braly

//Include necessary libraries
#include <Wire.h>   //built-in I2C lib
#include <Stepper.h>//built-in Stepper lib
#include <EEPROM.h> //built-in EEPROM lib

//Constants and global variables.
//Define motor direction states
#define CW 0
#define CCW 1
//Define MOSFET and stepper States
#define STEPPER_OFF 1
#define STEPPER_ON 0    //note- stepper motor driver uses inverted logic
#define MOSFET_OFF 0
#define MOSFET_ON 1
//define pins
#define MOSFET_1_PIN 9  //PB1
#define MOSFET_2_PIN A2 //PC2
#define STEPPER_1_PIN 2 //PD2
#define STEPPER_2_PIN 3 //PD3
#define STEPPER_3_PIN 4 //PD4
#define STEPPER_4_PIN 5 //PD5
#define LED_PIN 13
#define SERVO_HEADER_PIN 10
#define LIGHT_SENSOR_PIN A0 //PC0, need to add header to access this pin

//define motor properties:
#define STEPS_PER_REVOLUTION 513  //for stepper library
#define STEPS_PER_LAUNCH 47       //roughly 513.0/11.0
#define MAX_STEPPER_SPEED 60      //probably need to change this value
//declare stepper object
Stepper mirrorStepper(STEPS_PER_REVOLUTION, STEPPER_1_PIN, STEPPER_2_PIN, STEPPER_3_PIN, STEPPER_4_PIN);

//Global Variables
int debrisLauncherDegrees = 0;
int debrisLaunched = 0;

//Define function that is run whenever the Arduino receives a command from the master
void receiveCommand(){
  int currentCommand = 0; //Initialize variable that will temporarily hold command
  while(Wire.available() > 0){
    currentCommand = Wire.read();
  }
  //Case statement calls the function that was called for
  switch (currentCommand) {
    case 61:  //0x3D
      //Turn on Static generator
      turnOnStatic();
      break;
    case 66:  //0x42
      //Turn off Static generator
      turnOffStatic();
      break;
    case 71:  //0x47
      //Launch Debris
      launchDebris();
      break;
    case 76:  //0x4C
      //Turn on Pi Camera lights
      turnOnLights();
      break;
    case 81:  //0x4C
      //Turn off Pi Camera lights
      turnOffLights();
      break;
    default:
      //Do nothing
      break;
  }
  //Last thing before exiting the function is to clear currentCommand, probably unnecessary, but overly safe. 
  currentCommand = 0;
}

void requestCommand(){
  Wire.write(6);
}

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x02 for Laser board, 0x03 for Static board,
  // 0x04 for Magnet board, 0x05 for Boom control board.
  Wire.begin(0x03);          
  // When a command is received from the master, jump to the receiveCommand function and execute the proper command
  Wire.onReceive(receiveCommand);
  Wire.onRequest(requestCommand);
  
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
  mirrorStepper.setSpeed(MAX_STEPPER_SPEED);

}

void loop() {
  // Nothing here, all functionality is provided by recieveCommand and custom functions

}

void turnOnStatic(){
  digitalWrite(MOSFET_1_PIN, HIGH);
}

void turnOffStatic(){
  digitalWrite(MOSFET_1_PIN, LOW);
}

void launchDebris(){
  // Launchers hold 10 BB's, so if we've launched less than 11, launch again. Else do nothing.
  if(debrisLaunched >= 10){
    //do nothing
  }
  else if(debrisLaunched < 10){
    //Rotate stepper to launch one BB
    mirrorStepper.step(STEPS_PER_LAUNCH);
  }
}

void turnOnLights(){
  digitalWrite(SERVO_HEADER_PIN, HIGH);
}

void turnOffLights(){
  digitalWrite(SERVO_HEADER_PIN, LOW);
}
