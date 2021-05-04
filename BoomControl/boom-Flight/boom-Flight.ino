// Boom Control Board code - FINAL FINISHED FOR FLIGHT, 5/1/21
// Drives camera boom, controls M LEDs and 360 camera.
// At least with DFRobot motor, encoder counts are negative for CW and positive for CCW rotation.

#include <Wire.h>
#include <Encoder.h> //needed from an online library

//Constants and global variables.
//define pins
#define ENCODER_1_PIN 2
#define ENCODER_2_PIN 9
#define MOTORS_ENABLE_PIN 10 
#define MOTOR_1_PIN_A A0
#define MOTOR_1_PIN_B A1
#define MOTOR_2_PIN_A A2
#define MOTOR_2_PIN_B A3
#define SSR_GND 5            // Using pin 5 as a ground for SSRs
#define CAMERA_CONTROL_PIN 6 // SSR to control 360 camera pogo pins
#define M_LED_PIN 4
#define GND_CONTROL_PIN 7    // SSR to connect/disconnect GND to 360 camera
//define mechanical constants
#define REQUIRED_CAM_EXT_TIME 47000   //May need to be more than this
#define REQUIRED_CAM_RET_TIME 50000

//define global variables used in the program
volatile unsigned long encoderCounts = 0;
unsigned long cameraRotations = 0;
unsigned long cameraPosition = 0;
unsigned long camExtensionTime = 0;
unsigned long camRetractionTime = 0;
unsigned long flightTime = 0;
bool camBoomExtended = false;
volatile byte I2CreturnValue = 0;

//Setup motor encoder
Encoder myEnc(ENCODER_1_PIN, ENCODER_2_PIN);

void requestCommand(){
  I2CreturnValue = map(myEnc.read(), 0, 119000, 0, 100);
  Wire.write(I2CreturnValue);
}

void setup() {
  // Start serial for debugging, comment out for flight software
  Serial.begin(9600);
  Serial.println("Online.");

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x05 AKA 0b0000001 AKA 5.
  Wire.begin(0x05);   
  Wire.onRequest(requestCommand);
  
  // Setup motor driver pins
  pinMode(MOTORS_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_1_PIN_A, OUTPUT);
  pinMode(MOTOR_1_PIN_B, OUTPUT);
  pinMode(MOTOR_2_PIN_A, OUTPUT);
  pinMode(MOTOR_2_PIN_B, OUTPUT);

  // Setup input pins
  pinMode(ENCODER_1_PIN, INPUT);
  pinMode(ENCODER_2_PIN, INPUT);

  //Make sure M is off
  pinMode(M_LED_PIN, OUTPUT);
  digitalWrite(M_LED_PIN, HIGH);
  
  //Ensure that motors are turned off initially
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  digitalWrite(MOTOR_1_PIN_A, LOW);
  digitalWrite(MOTOR_1_PIN_B, LOW);
  digitalWrite(MOTOR_2_PIN_A, LOW);
  digitalWrite(MOTOR_2_PIN_B, LOW);

  //Set up pins 5, 6, 7 for SSR control
  pinMode(SSR_GND, OUTPUT);
  digitalWrite(SSR_GND, LOW);
  //Make camera control pin an output
  pinMode(CAMERA_CONTROL_PIN, OUTPUT);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
  pinMode(GND_CONTROL_PIN, OUTPUT);
  digitalWrite(GND_CONTROL_PIN, LOW);
}

void loop() {
    while(millis() < 65000){ //Board powers on at T+20 so this is T+65 to compensate for that 
      //Do nothing until T+85s
    }
    turnOnCamera();
    turnOnM();
    //Begin boom extension
    Serial.println("Extending camera boom now.");
    digitalWrite(13, HIGH);
    extendCamBoom();
    camExtensionTime = millis();
    while(camBoomExtended == false){
      // If sufficient time to extend the camera boom has passed, turn off the boom motor
      if(millis() > (camExtensionTime + REQUIRED_CAM_EXT_TIME)){
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        digitalWrite(MOTOR_2_PIN_A, LOW);
        digitalWrite(MOTOR_2_PIN_B, LOW);
        Serial.println("Sufficient time elapsed, Motor off.");
        camBoomExtended = true;
      }
    }
    delay(3000); //3 second delay to make up for boom extenstion reduction from 50 to 47 secondss
    digitalWrite(13, LOW);
    delay(1000);
    //Boom extended, continue
    for(int i=0; i<30; i++){
      takePhoto();
      delay(1500);
    }
    //Put camera in transfer mode here
    //switchToTransferMode();
    //Power off camera
    //turnOffCamera();
    //delay(1000);
    //turnOnCamera();
    //Begin copying photos to Pi
    
    while(millis() < 255000){
      //Do nothing until T+275
    }
    retractCamBoom();
    digitalWrite(13, HIGH);
    //Update camExtensionTime to current time before entering while loop
    camRetractionTime = millis();
    //Check for complete retraction of the boom, either by time or by encoder counts
    while(camBoomExtended == true){
      Serial.println(myEnc.read());
      // If the sufficient amount of time to extend the camera boom has passed, turn off the boom motor
      if(millis() > (camRetractionTime + REQUIRED_CAM_RET_TIME)){
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        digitalWrite(MOTOR_2_PIN_A, LOW);
        digitalWrite(MOTOR_2_PIN_B, LOW);
        Serial.println("Boom should be retracted. Stopped by time limit.");
        camBoomExtended = false;
      }
    }
    digitalWrite(13, LOW);
    digitalWrite(MOTORS_ENABLE_PIN, LOW);
    //Turn off 360 camera
    turnOffCamera();
    while(true){
      //Do nothing
    }
}

void extendCamBoom(){
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  //If boom goes the wrong way, make pin A HIGH and pin B LOW
  digitalWrite(MOTOR_1_PIN_A, LOW); // Actually LOW
  digitalWrite(MOTOR_1_PIN_B, HIGH);// Actually HIGH
  digitalWrite(MOTOR_2_PIN_A, LOW);
  digitalWrite(MOTOR_2_PIN_B, HIGH);
  camExtensionTime = millis();
}

void retractCamBoom(){
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  //If boom goes the wrong way, make pin A LOW and pin B HIGH
  digitalWrite(MOTOR_1_PIN_A, HIGH);
  digitalWrite(MOTOR_1_PIN_B, LOW);
  digitalWrite(MOTOR_2_PIN_A, HIGH);
  digitalWrite(MOTOR_2_PIN_B, LOW);
  camRetractionTime = millis();
}

void takePhoto(){
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(500); //100?
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
}

void turnOnCamera(){
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(5000); //800
  digitalWrite(CAMERA_CONTROL_PIN, LOW); 
}

void turnOffCamera(){
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(5000); //2000
  digitalWrite(CAMERA_CONTROL_PIN, LOW); 
}

void switchMode(){
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(2000);
  digitalWrite(CAMERA_CONTROL_PIN, LOW); 
}

void turnOnM(){
  digitalWrite(M_LED_PIN, LOW);
}

void switchToTransferMode(){
  digitalWrite(GND_CONTROL_PIN, HIGH);
}

void switchToCaptureMode(){
  digitalWrite(GND_CONTROL_PIN, LOW);
}
