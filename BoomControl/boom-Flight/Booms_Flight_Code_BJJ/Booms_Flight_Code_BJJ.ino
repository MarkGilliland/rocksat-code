/*
 * Flight code for Boom Control Board for Rocksat-X 2019-2021 payload.
 * Author: Bradley Jesteadt
 * Date Modified: January the 9th, 2021
 * 
 */

#include <ArduinoQueue.h>

//  Pin Declarations
#define ENABLE 10
#define M1CCW A0    // Motor 1 is the camera boom
#define M1CW A1
#define M2CCW A2    // Motor 2 is the comms boom
#define M2CW A3
#define FRONT_SENSOR 4
#define BACK_SENSOR 5

//  Define Coding Constants
const int COMMS_EXTENSION_TIME = 9000;
const int CAMERA_EXTENSION_TIME = 36000; // ~1.25"/sec, ~36sec for full extension
                                         // TODO: figure out how to reduce speed

//  Define Function Stubs and Related Variables
//ArduinoQueue<Picture> pictures(5);  // Creates a queue of maximum 5 pictures to be transmitted
                                      // TODO: Implement Picture class
void takePicture();
bool availablePicture = false;
void transmitPicture();

void setup() {
  //  PinMode Declarations
  pinMode(ENABLE, OUTPUT);
  pinMode(M1CCW, OUTPUT);
  pinMode(M1CW, OUTPUT);
  pinMode(M2CCW, OUTPUT);
  pinMode(M2CW, OUTPUT);
  pinMode(FRONT_SENSOR, INPUT);
  pinMode(BACK_SENSOR, INPUT);

  //  Initialize Control Pins
  digitalWrite(ENABLE,LOW);
  digitalWrite(M1CW,LOW);
  digitalWrite(M1CCW,LOW);
  digitalWrite(M2CW,LOW);
  digitalWrite(M2CCW,LOW);
}

//  Initialize Timer
int startTime = millis();
int currentTime = millis();
int elapsedTime = 0;

void loop() {
  //  Update Clock
  currentTime = millis();
  elapsedTime = (currentTime - startTime);

  //  Comms Boom Decision Tree
  if(!digitalRead(FRONT_SENSOR) && (elapsedTime < 1.5*COMMS_EXTENSION_TIME)){  //Initial deployment
    digitalWrite(M2CW,LOW);
    digitalWrite(M2CCW,HIGH);
  }else if(!digitalRead(BACK_SENSOR) && (elapsedTime > (180000-1.5*COMMS_EXTENSION_TIME)) && (elapsedTime < 180000)){ // Retraction
    digitalWrite(M2CW,HIGH);
    digitalWrite(M2CCW,LOW);
  }else{ // During experiments and at end of flight
    digitalWrite(M2CW,LOW);
    digitalWrite(M2CCW,LOW);
  }

  //  Camera Boom Decision Tree
  if(elapsedTime < CAMERA_EXTENSION_TIME){  //Initial deployment
    digitalWrite(M1CW,LOW);
    digitalWrite(M1CCW,HIGH);
  }else if((elapsedTime > (180000-CAMERA_EXTENSION_TIME)) && (elapsedTime < 180000)){ // Retraction
    digitalWrite(M1CW,HIGH);
    digitalWrite(M1CCW,LOW);
  }else{ // During experiments and at end of flight
    digitalWrite(M1CW,LOW);
    digitalWrite(M1CCW,LOW);
  }

  // Camera Control
  if(elapsedTime > CAMERA_EXTENSION_TIME && elapsedTime < (180000-CAMERA_EXTENSION_TIME)){
    takePicture();
  }

  // Comms Control
  if(elapsedTime > 1.5*COMMS_EXTENSION_TIME && elapsedTime < (180000-1.5*COMMS_EXTENSION_TIME)){
    if(availablePicture){
      transmitPicture();
    }
  }

  //  Disable Motors For Reentry
  if(elapsedTime > 180000){
    digitalWrite(ENABLE,LOW);
    while(true){}
  }
}

//  Function Definitions
void takePicture(){
  //TODO: Write function
  /*
   * Pseudocode:
   * 
   * Take picture
   * 
   * Add it to queue of pictures
   * 
   * Save it sd card / pass it to the data boards
   * 
   * Set availablePicture to true
   * 
   */
  
}

void transmitPicture(){
  //TODO: Write function
  /*
   * Psuedocode:
   * 
   * Retrieve picture from queue of pictures
   * 
   * If queue of pictures is empty, set availablePicture to false
   * 
   * Send picture to transceiver
   * 
   */
}
