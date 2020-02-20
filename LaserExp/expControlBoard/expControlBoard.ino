// Experiment Control Board code - Initial
// Drives stepper motor, MOSFET-switched outputs
// Current execution time is ~6ms per loop

//Include I2C library
#include <Wire.h>

//Constants and global variables.
#define CW 0
#define CCW 1
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
  pinMode(motorsEn, OUTPUT);
  pinMode(motor1CCW, OUTPUT);
  pinMode(motor1CW, OUTPUT);
  pinMode(motor2CCW, OUTPUT);
  pinMode(motor2CW, OUTPUT);
  // Setup input pins
  pinMode(limitSwitch1, INPUT);
  pinMode(limitSwitch2, INPUT);
  pinMode(limitSwitch3, INPUT);
  pinMode(limitSwitch4, INPUT);
  pinMode(enc1, INPUT);
  pinMode(enc2, INPUT);
 
  // Test for motor function
  digitalWrite(motorsEn, HIGH);
  digitalWrite(motor1CCW, LOW);
  digitalWrite(motor1CW, HIGH);
  digitalWrite(motor2CCW, LOW);
  digitalWrite(motor2CW, LOW);
}

void loop() {
  
  // Check if extenstion limit switches have been pressed, if so, turn off respective motor
  // If camera boom is either fully retracted or fully extended, or encoder limits are exceeded, stop the camera boom motor.
  if(digitalRead(limitSwitch2) == HIGH || digitalRead(limitSwitch1) == HIGH || myEnc.read() < -20000){
    digitalWrite(motor1CCW, LOW);
    digitalWrite(motor1CW, LOW);
  }
  // If telemetry boom is either fully retracted or extended, stop the telemetry boom motor. 
  if(digitalRead(limitSwitch4) == HIGH || digitalRead(limitSwitch3) == HIGH){
    digitalWrite(motor2CCW, LOW);
    digitalWrite(motor2CW, LOW);
  }
  Serial.println(millis());
  
}
