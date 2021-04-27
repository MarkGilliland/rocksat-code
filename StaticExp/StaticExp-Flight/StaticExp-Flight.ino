// FINISHED FINAL FLIGHT CODE FOR STATIC EXP, 4/27/2021
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
#define STEPS_PER_REVOLUTION 2048  //for stepper library
#define STEPS_PER_LAUNCH 187       //roughly 2048/11.0
#define MAX_STEPPER_SPEED 11       
//declare stepper object
Stepper mirrorStepper(STEPS_PER_REVOLUTION, STEPPER_2_PIN, STEPPER_4_PIN, STEPPER_1_PIN, STEPPER_3_PIN);

//Global Variables
int debrisLauncherDegrees = 0;
int debrisLaunched = 0;

void requestCommand(){
  Wire.write(debrisLaunched);
}

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x02 for Laser board, 0x03 for Static board,
  // 0x04 for Magnet board, 0x05 for Boom control board.
  Wire.begin(0x03);
  //Enable responding to I2C data requests      
  Wire.onRequest(requestCommand);
  // Setup motor driver pins
  pinMode(STEPPER_1_PIN, OUTPUT);
  pinMode(STEPPER_2_PIN, OUTPUT);
  pinMode(STEPPER_3_PIN, OUTPUT);
  pinMode(STEPPER_4_PIN, OUTPUT);
  //setup MOSFET Pins
  pinMode(MOSFET_1_PIN, OUTPUT);
  pinMode(MOSFET_2_PIN, OUTPUT);
  pinMode(SERVO_HEADER_PIN,OUTPUT);

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
  while(millis() < 65000){
    //Do nothing until we reach T+85
  }
  //Turn on pi camera lights
  turnOnLights();
  while(millis() < 218000){
    //Do nothing until we reach T+238
  }
  launchDebris();
  delay(10000);
  turnOnStatic();
  delay(5000);
  for(int i = 0; i < 10; i++){
    launchDebris();
    delay(5000);
  }
  digitalWrite(LED_PIN, HIGH);
  turnOffStatic();
  while(true){
    //Do nothing, program has completed running
  }
}

void turnOnStatic(){
  digitalWrite(MOSFET_1_PIN, HIGH);
}

void turnOffStatic(){
  digitalWrite(MOSFET_1_PIN, LOW);
}

void launchDebris(){
  //Rotate stepper to launch one BB
  mirrorStepper.step(STEPS_PER_LAUNCH);
  debrisLaunched++;
}

void turnOnLights(){
  digitalWrite(SERVO_HEADER_PIN, HIGH);
}

void turnOffLights(){
  digitalWrite(SERVO_HEADER_PIN, LOW);
}
