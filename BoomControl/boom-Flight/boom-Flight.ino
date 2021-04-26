// Boom Control Board code - Initial
// Drives camera boom, controls M LEDs and 360 camera
// and both boom's limit switches.  
// LIMIT_SWITCH_1 = retracted camera boom, LIMIT_SWITCH_2 = extended camera boom
// LIMIT_SWITCH_3 = Not used, LIMIT_SWITCH_4 = Not used
// Current execution time is ~6ms per loop
// At least with DFRobot motor, encoder counts are negative for CW and positive for CCW rotation.
// Camera boom pulley system equates to ________.

#include <Wire.h>
#include <Encoder.h> //needed from an online library

//Constants and global variables.
//Define motor direction states
#define CW 0
#define CCW 1
//define pins
#define ENCODER_1_PIN 2
#define ENCODER_2_PIN 9
#define LIMIT_SWITCH_1 4
//#define LIMIT_SWITCH_2 5
//#define LIMIT_SWITCH_3 6
//#define LIMIT_SWITCH_4 7
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
#define REQUIRED_CAM_EXT_TIME 50000   //May need to be more than this
#define AUTONOMOUS_MODE_ENABLE 1

//define global variables used in the program
volatile long encoderCounts = 0;
long cameraRotations = 0;
long cameraPosition = 0;
long camExtensionTime = 0;
long camRetractionTime = 0;
long flightTime = 0;
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
  //pinMode(LIMIT_SWITCH_1, OUTPUT);
  //pinMode(LIMIT_SWITCH_2, INPUT);
  //pinMode(LIMIT_SWITCH_3, INPUT);
  //pinMode(LIMIT_SWITCH_4, INPUT);
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
    extendCamBoom();
    camExtensionTime = millis();
    while(camBoomExtended == false){
      // Print current encoder counts to Serial
      Serial.println(myEnc.read());
      // Check if extension limit switches have been pressed, if so, turn off respective motor
      // If camera boom is either fully retracted or fully extended, or encoder limits are exceeded, stop the camera boom motor.
      if(abs(myEnc.read()) > 119000){
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Boom should be extended. Powered off by Encoder counts");
        camBoomExtended = true;
      }
      // If sufficient time to extend the camera boom has passed, turn off the boom motor
      if(millis() > (camExtensionTime + REQUIRED_CAM_EXT_TIME)){
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Sufficient time elapsed, Motor off.");
        camBoomExtended = true;
      }
    }
    delay(1000);
    //Boom extended, continue
    for(int i=0; i<5; i++){
      takePhoto();
      delay(1500);
    }
    //Put camera in transfer mode here
    switchToTransferMode();
    //Power off camera
    turnOffCamera();
    delay(1000);
    turnOnCamera();
    //Begin copying photos to Pi
    while(millis() < 157000){  //This will need to be changed based on time to transfer photos to Pi
      //Do nothing until T+177
    }
    //Turn off camera and turn back on in capture mode
    turnOffCamera();
    switchToCaptureMode();
    delay(1000);
    turnOnCamera();
    //Take 20 photos
    for(int i = 0; i < 20; i++){
      takePhoto();
      delay(1500);
    }
    //Turn off camera and turn back on in transfer mode
    turnOffCamera();
    switchToTransferMode();
    delay(1000);
    turnOnCamera();
    //T+239s~
    //Pi now transfers second batch of photos
    while(millis() < 255000){
      //Do nothing until T+275
    }
    retractCamBoom();
    //Turn off 360 camera
    turnOffCamera();
    //Update camExtensionTime to current time before entering while loop
    camExtensionTime = millis();
    //Check for complete retraction of the boom, either by time or by encoder counts
    while(camBoomExtended == true){
      Serial.println(myEnc.read());
      // Check if extenstion limit switches have been pressed, if so, turn off respective motor
      // If camera boom is either fully retracted or fully extended, or encoder limits are exceeded, stop the camera boom motor.
      if((abs(myEnc.read()) < 1000)){
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Boom should be retracted. Stopped by encoder counts");
        camBoomExtended = false;
      }
      // If the sufficient amount of time to extend the camera boom has passed, turn off the boom motor
      if(millis() > (camExtensionTime + REQUIRED_CAM_EXT_TIME)){
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Boom should be retracted. Stopped by time limit.");
        camBoomExtended = false;
      }
    }
    while(true){
      //Do nothing
    }
}

void extendCamBoom(){
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  //If boom goes the wrong way, make pin A HIGH and pin B LOW
  digitalWrite(MOTOR_1_PIN_A, LOW); // Actually LOW
  digitalWrite(MOTOR_1_PIN_B, HIGH);// Actually HIGH
  camExtensionTime = millis();
}

void retractCamBoom(){
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  //If boom goes the wrong way, make pin A LOW and pin B HIGH
  digitalWrite(MOTOR_1_PIN_A, HIGH);
  digitalWrite(MOTOR_1_PIN_B, LOW);
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
