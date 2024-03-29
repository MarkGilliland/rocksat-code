// Boom Control Board code - Initial
// Drives camera boom, drives telemetry boom, reads camera boom encoder
// and both boom's limit switches.  
// LIMIT_SWITCH_1 = retracted camera boom, LIMIT_SWITCH_2 = extended camera boom
// LIMIT_SWITCH_2 = retracted telemetry boom, LIMIT_SWITCH_3 = extended telemetry boom
// Current execution time is ~6ms per loop
// At least with DFRobot motor, encoder counts are negative for CW and positive for CCW rotation.
// Telemetry boom leadscrew is 2mm/1 rotation or 0.0787 inches per rotation
// Camera boom pulley system equates to ________.
#include <Wire.h>
//#include <Encoder.h>//needed from an online library

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
#define CAMERA_CONTROL_PIN 3//Camera on servo header, D3
//define global variables used in the program
long encoderCounts = 0;
long cameraRotations = 0;
long cameraPosition = 0;
long telemetryRotations = 0;
long telemetryPosition = 0;

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
  // Join I2C bus as a slave with address 0x01 AKA 0b0000001 AKA 1.
  Wire.begin(0b0000001);          
  // When a command is received from the master, jump to the receiveCommand function and execute the proper command
  Wire.onReceive(receiveCommand); 
  
  // Setup motor driver pins
  pinMode(MOTORS_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_1_PIN_A, OUTPUT);
  pinMode(MOTOR_1_PIN_B, OUTPUT);
  pinMode(MOTOR_2_PIN_A, OUTPUT);
  pinMode(MOTOR_2_PIN_B, OUTPUT);
  // Setup input pins
  pinMode(LIMIT_SWITCH_1, INPUT);
  pinMode(LIMIT_SWITCH_2, INPUT);
  pinMode(LIMIT_SWITCH_3, INPUT);
  pinMode(LIMIT_SWITCH_4, INPUT);
  pinMode(ENCODER_1_PIN, INPUT);
  pinMode(ENCODER_2_PIN, INPUT);
  pinMode(CAMERA_CONTROL_PIN, OUTPUT);
 
  // Test for motor function
  digitalWrite(MOTORS_ENABLE_PIN, HIGH);
  digitalWrite(MOTOR_1_PIN_A, LOW);
  digitalWrite(MOTOR_1_PIN_B, HIGH);
  digitalWrite(MOTOR_2_PIN_A, LOW);
  digitalWrite(MOTOR_2_PIN_B, LOW);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);


  //boot the camera
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(5000);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
  delay(1500);
  //start video
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(500);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
  delay(1500);

  //take some video for 10 sec
  delay(10000);
  
  //stop video
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(500);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
  delay(500);
  //shut off camera
  digitalWrite(CAMERA_CONTROL_PIN, HIGH);
  delay(5000);
  digitalWrite(CAMERA_CONTROL_PIN, LOW);
  delay(500);
}

void loop() {
  //see setup for one-time code
}
