// FINISHED FINAL FLIGHT CODE FOR LASER, 4/27/2021
// Experiment Control Board code
// Drives stepper motor, MOSFET-switched outputs
// flight code for the laser experiment for Mines Rocksat-X 2020
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
#define STEPS_PER_REVOLUTION 2048  //change this to 513 for the weaker motor
#define MAX_STEPPER_SPEED 16      //change this to 20 for the weaker motor
//declare stepper object
//Stepper mirrorStepper(STEPS_PER_REVOLUTION, STEPPER_1_PIN, STEPPER_2_PIN, STEPPER_3_PIN, STEPPER_4_PIN);  // This is for the weaker motor
Stepper mirrorStepper(STEPS_PER_REVOLUTION, STEPPER_2_PIN, STEPPER_4_PIN, STEPPER_1_PIN, STEPPER_3_PIN);  // SWITCH TO THIS FOR BEEFIER MOTOR

//Global Variables
int debrisLauncherDegrees = 0;
int currentPos = 0;

//Function that is executed when master requests data
void requestCommand(){
  Wire.write(currentPos);
}

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x02 for Laser board, 0x03 for Static board,
  // 0x04 for Magnet board, 0x05 for Boom control board.
  Wire.begin(0x02);          
  // When a command is received from the master, jump to the receiveCommand function and execute the proper command
  //Wire.onReceive(receiveCommand); 
  // When a request for data is received from the master, jump to requestCommand function and send
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
  while(millis() < 65000){
    // Wait till T+85 to act. (Board powers on at T+20, so 65000 is 85s.)
  }    
  homeLaser();                                          
  heatSMA();
  delay(30000);                // Wait 30s for debris carrier to open.  Springs will still be powered even after this.
  turnOnLaser();
  changeTarget(0);
  delay(15000);                // Try to move the first target for 15s.
  changeTarget(1);
  delay(15000);                // Try to move the second target for 15s.
  changeTarget(2);
  delay(15000);                // Try to move the third target for 15s.
  turnOffLaser();                             
  coolSMA();
  while(true){
    // Turn everything off and become idle.  Completion time at ~ T+163.
  };                           
}




void turnOnLaser(){
  digitalWrite(MOSFET_2_PIN, MOSFET_ON);
}

void turnOffLaser(){
  digitalWrite(MOSFET_2_PIN, MOSFET_OFF);
}

void changeTarget(int target){
  mirrorStepper.step(target*10-currentPos);
  currentPos = target*10;
}

void heatSMA(){
  digitalWrite(MOSFET_1_PIN, MOSFET_ON);
}

void coolSMA(){
  digitalWrite(MOSFET_1_PIN, MOSFET_OFF);
}

void homeLaser(){
  mirrorStepper.step(-STEPS_PER_REVOLUTION/8);
  currentPos = 0;
}
