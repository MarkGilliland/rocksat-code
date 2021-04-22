// WIP flight code for the magnet experiment for Mines Rocksat-X 2020
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
#define AUTONOMOUS_MODE_ENABLE 1

//define motor properties:
static int STEPS_PER_REVOLUTION = 2048;  //for stepper library
static int STEPS_PER_LAUNCH = 187;       //roughly 513.0/11.0
static int MAX_STEPPER_SPEED = 16;      //probably need to change this value
//declare stepper object
Stepper mirrorStepper(STEPS_PER_REVOLUTION, STEPPER_1_PIN, STEPPER_2_PIN, STEPPER_3_PIN, STEPPER_4_PIN);

//Global Variables
int debrisLauncherDegrees = 0;
int debrisLaunched = 0;

//Define function that is run whenever the Arduino receives a command from the master
void receiveCommand(int numBytes){
  int currentCommand = 0; //Initialize variable that will temporarily hold command
  while(Wire.available() > 0){
    currentCommand = Wire.read();
  }
  //Case statement calls the function that was called for
  switch (currentCommand) {
    case 71:  //0x47
      //Launch Debris
      launchDebris();
      break;
    default:
      //Do nothing
      break;
  }
  //Last thing before exiting the function is to clear currentCommand, probably unnecessary, but overly safe. 
  currentCommand = 0;
}

void requestCommand(){
  Wire.write(4);
}

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x02 for Laser board, 0x03 for Static board,
  // 0x04 for Magnet board, 0x05 for Boom control board.
  Wire.begin(0x04);          
  if(AUTONOMOUS_MODE_ENABLE == 0){
    // When a command is received from the master, jump to the receiveCommand function
    Wire.onReceive(receiveCommand); 
    Wire.onRequest(requestCommand);
  }
  
  // Setup motor driver pins
  pinMode(STEPPER_1_PIN, OUTPUT);
  pinMode(STEPPER_2_PIN, OUTPUT);
  pinMode(STEPPER_3_PIN, OUTPUT);
  pinMode(STEPPER_4_PIN, OUTPUT);
  //setup MOSFET Pins
  pinMode(MOSFET_1_PIN, OUTPUT);
  pinMode(MOSFET_2_PIN, OUTPUT);

  //set all outputs to "off" to start
  //digitalWrite(STEPPER_1_PIN, STEPPER_OFF);
  //digitalWrite(STEPPER_2_PIN, STEPPER_OFF);
  //digitalWrite(STEPPER_3_PIN, STEPPER_OFF);
  //digitalWrite(STEPPER_4_PIN, STEPPER_OFF);
  digitalWrite(MOSFET_1_PIN, MOSFET_OFF);
  digitalWrite(MOSFET_2_PIN, MOSFET_OFF); 

  //Init the stepper object with an initial speed
  mirrorStepper.setSpeed(MAX_STEPPER_SPEED);
}

void loop() {
  // Nothing here, all functionality is provided by recieveCommand and custom functions
  if(AUTONOMOUS_MODE_ENABLE == 0){
    //Do nothing, all functionality proved by receiveCommand
  }
  if(AUTONOMOUS_MODE_ENABLE == 1){
    //Run without central control from the data control board
    while(millis() < 65000){
      //Wait until we reach T+85
    }
    digitalWrite(MOSFET_1_PIN,HIGH);
    for(int i = 0; i < 10; i++){
      //launchDebris();
      mirrorStepper.step(STEPS_PER_LAUNCH);
      delay(10000);
    }
    digitalWrite(LED_PIN, HIGH);
    while(true){
      //Do nothing, program has completed running
    }
  }
}

void launchDebris(){
  // Launchers hold 10 BB's, so if we've launched less than 11, launch again. Else do nothing.
  if(debrisLaunched >= 10){
    //do nothing
  }
  else if(debrisLaunched < 10){
    //Rotate stepper to launch one BB
    mirrorStepper.step(STEPS_PER_LAUNCH);
    debrisLaunched++;
  }
}
