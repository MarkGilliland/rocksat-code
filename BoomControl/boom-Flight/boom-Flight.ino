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
#define LIMIT_SWITCH_2 5
#define LIMIT_SWITCH_3 6
#define LIMIT_SWITCH_4 7
#define MOTORS_ENABLE_PIN 10 
#define MOTOR_1_PIN_A A0
#define MOTOR_1_PIN_B A1
#define MOTOR_2_PIN_A A2
#define MOTOR_2_PIN_B A3
#define CAMERA_CONTROL_PIN 3 //Camera attached to Servo header, D3
#define M_LED_PIN 4
#define SSR_PIN 5
//define mechanical constants
#define REQUIRED_CAM_EXT_TIME 50000   //May need to be more than this
#define AUTONOMOUS_MODE_ENABLE 1

//define global variables used in the program
long encoderCounts = 0;
long cameraRotations = 0;
long cameraPosition = 0;
long camExtensionTime = 0;
long camRetractionTime = 0;
long flightTime = 0;
bool camBoomExtended = false;

//Setup motor encoder
Encoder myEnc(ENCODER_1_PIN, ENCODER_2_PIN);

/*
void receiveCommand(int numBytes){
  int currentCommand = 0; //Initialize variable that will temporarily hold command
  while(Wire.available() > 0){
    currentCommand = Wire.read();
  }
  switch (currentCommand){
    case 0x0B:
      extendCamBoom();
      break;
    case 0x10:
      retractCamBoom();
      break;
    case 0x15:
      takePhoto();
      break;
    case 0x1A:
      turnOnCamera();
      break;
    case 0x1F:
      turnOffCamera();
      break;
    case 0x16:
      switchMode();
      break;
    case 0x55:
      turnOnM();
      break;
  }
  //Last thing before exiting the function is to clear currentCommand, probably unnecessary, but overly safe. 
  currentCommand = 0;
}


void requestCommand(){
  Wire.write(6);
}
*/

void setup() {
  // Start serial for debugging, comment out for production software
  Serial.begin(9600);
  Serial.println("Online.");

  // Start I2C for communication to master
  // Join I2C bus as a slave with address 0x05 AKA 0b0000001 AKA 5.
  //Wire.begin(0x05);
  if(AUTONOMOUS_MODE_ENABLE == 0){    
    // When a command is received from the master, jump to the receiveCommand function and execute the proper command
    //Wire.onReceive(receiveCommand);
    //Wire.onRequest(requestCommand);
  }
  
  // Setup motor driver pins
  pinMode(MOTORS_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_1_PIN_A, OUTPUT);
  pinMode(MOTOR_1_PIN_B, OUTPUT);
  pinMode(MOTOR_2_PIN_A, OUTPUT);
  pinMode(MOTOR_2_PIN_B, OUTPUT);
  //Make camera control pin an output
  pinMode(CAMERA_CONTROL_PIN, OUTPUT);
  // Setup input pins
  //pinMode(LIMIT_SWITCH_1, OUTPUT);
  pinMode(LIMIT_SWITCH_2, INPUT);
  pinMode(LIMIT_SWITCH_3, INPUT);
  pinMode(LIMIT_SWITCH_4, INPUT);
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

  //Manual boom control for testing
  //turnOnM();
  //retractCamBoom();
}

void loop() {
  //AUTONOMOUS_MODE_ENABLE set to 0 means central control by data control board. AUTONOMOUS_MODE_ENABLE
  if(AUTONOMOUS_MODE_ENABLE == 0){
    // Check if extenstion limit switches have been pressed, if so, turn off respective motor
    // If camera boom is either fully retracted or fully extended, or encoder limits are exceeded, stop the camera boom motor.
    if(digitalRead(LIMIT_SWITCH_2) == HIGH || digitalRead(LIMIT_SWITCH_1) == HIGH || myEnc.read() < -120000){
      digitalWrite(MOTOR_1_PIN_A, LOW);
      digitalWrite(MOTOR_1_PIN_B, LOW);
    }
    // If the sufficient amount of time to extend the camera boom has passed, turn off the boom motor
    if((millis() > (camExtensionTime + REQUIRED_CAM_EXT_TIME)) || (millis() > (camRetractionTime + REQUIRED_CAM_EXT_TIME))){
      digitalWrite(MOTOR_1_PIN_A, LOW);
      digitalWrite(MOTOR_1_PIN_B, LOW);  
    }
  }
  else if (AUTONOMOUS_MODE_ENABLE == 1){
    turnOnCamera();
    while(millis() < 65000){ //Board powers on at T+20 so this is T+65 to compensate for that 
      //Do nothing until T+85s
    }
    turnOnM();
    //Begin boom extension
    Serial.println("Extending camera boom now.");
    //extendCamBoom();
    digitalWrite(MOTORS_ENABLE_PIN, HIGH);
    //If boom goes the wrong way, make pin A HIGH and pin B LOW
    digitalWrite(MOTOR_1_PIN_A, LOW); // Actually LOW
    digitalWrite(MOTOR_1_PIN_B, HIGH);// Actually HIGH
    delay(1000);
    camExtensionTime = millis();
    while(camBoomExtended == false){
      Serial.println(myEnc.read());
      // Check if extenstion limit switches have been pressed, if so, turn off respective motor
      // If camera boom is either fully retracted or fully extended, or encoder limits are exceeded, stop the camera boom motor.
      if(abs(myEnc.read()) > 119000){
        digitalWrite(M_LED_PIN, HIGH);
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Boom should be extended. Powered off by Encoder counts");
        camBoomExtended = true;
      }
      // If sufficient time to extend the camera boom has passed, turn off the boom motor
      if(millis() > (camExtensionTime + REQUIRED_CAM_EXT_TIME)){
        digitalWrite(M_LED_PIN, HIGH);
        digitalWrite(MOTORS_ENABLE_PIN, LOW);
        digitalWrite(MOTOR_1_PIN_A, LOW);
        digitalWrite(MOTOR_1_PIN_B, LOW);
        Serial.println("Sufficient time elapsed, Motor off.");
        camBoomExtended = true;
      }
    }
    //Boom extended, continue
    for(int i=0; i<10; i++){
      takePhoto();
      delay(5000);
    }
    while(millis() < 275000){
      //Do nothing until T+275
    }
    retractCamBoom();
    //Turn on 360 camera
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
        Serial.println("Sufficient time elapsed, Motor off.");
        camBoomExtended = true;
      }
    }
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
