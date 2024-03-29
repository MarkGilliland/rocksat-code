/* Mines Rocksat-X 2020 Data control board flight code
 * This code runs on the Teensy 4.0 on the data control board.
 * This proram is responsible for commanding the experiment control boards
 * and talking to the raspberry pis.
 * 
 * Device addresses:
 * 0x02: Laser
 * 0x03: Static
 * 0x04: Magnet
 * 0x05: Boom
 */

//Include necessary libraries
#include <Wire.h>

//Constant pin declarations:
const int LED_PIN = 13;
const int SHIFT_OUT = 3;
const int SHIFT_CLK = 4;
const int SHIFT_LAT = 5;
const int PDB_STROBE = 22;


//Global variables and constants
const int TE_1 = 0;         //Time from board power on that TE-1 line (computers) is turned on, in milliseconds
const int TE_2 = 65000;     //Time from board power on that TE-2 line (actuators) is turned on, in milliseconds
const int T_OFF = 335000;   //(Assumed) time that the payload should turn off
const int STA_PWR_ON = 125000; //Time that static experiment powers on
int upTime = 0;
volatile byte lasTelemData = 0;
volatile byte staTelemData = 0;
volatile byte magTelemData = 0;
volatile byte boomTelemData = 0;
volatile bool lasStaOrMagBoom = 0;
volatile long timesIntTriggered = 0;
 
//Array of experiment statuses
volatile int expStatus[4];

void parallelOut(){
  //Write code to clock out parallel data here.
  if(timesIntTriggered == 130){
    requestExpBrdData();
    digitalWrite(SHIFT_LAT, LOW);
    if(lasStaOrMagBoom == 0){
      shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, lasTelemData);
      shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, staTelemData);
    }
    if(lasStaOrMagBoom == 1){
      shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, magTelemData);
      shiftOut(SHIFT_OUT, SHIFT_CLK, MSBFIRST, boomTelemData);
    }
    //Shifting parallel data complete
    digitalWrite(SHIFT_LAT, HIGH);
    //Write data to serial busses
    //Write to rocket UART
    Serial2.write("L:");
    Serial2.write(lasTelemData);
    Serial2.write("S:");
    Serial2.write(staTelemData);
    Serial2.write("M:");
    Serial2.write(magTelemData);
    Serial2.write("B:");
    Serial2.write(boomTelemData);
    //Write to debug UART
    Serial.write("L:");
    Serial.write(lasTelemData);
    Serial.write("S:");
    Serial.write(staTelemData);
    Serial.write("M:");
    Serial.write(magTelemData);
    Serial.write("B:");
    Serial.write(boomTelemData);
    
    timesIntTriggered = 0;
  }
    timesIntTriggered++;
}

void requestExpBrdData(){
  //noInterrupts();
  //Request data from laser board
  Wire.requestFrom(0x02, 1);
  lasTelemData = Wire.read();
  //Request data from static board
  Wire.requestFrom(0x03, 1);
  staTelemData = Wire.read();
  //Request data from the magnet board
  Wire.requestFrom(0x04, 1);
  magTelemData = Wire.read();
  //Request data from the Boom board
  Wire.requestFrom(0x05, 1);
  boomTelemData = Wire.read();
  //interrupts();
}

void setup() {
  //Turn on led
  digitalWrite(13, HIGH);
  // Start debug serial interface
  Serial.begin(115200);
  // Start RS-232 serial interface
  Serial2.begin(19200, SERIAL_8N1);
  
  //Start I2C interface to experiments as bus master
  Wire.begin();
  //Start I2C interface to Pis as bus master
  Wire2.begin();
  
  //Set up input/output pins
  pinMode(PDB_STROBE, INPUT);
  pinMode(LED_PIN, OUTPUT);
  //Set pin 11 as output for PWM
  pinMode(11, OUTPUT);
  //Set pin 12 as input for interrupt
  pinMode(12, INPUT);

  //Set up PWM on pin 11, attached to pin 12 to trigger pin change interrupt
  //Pins 11 and 12 MUST be bridged for this to work
  analogWrite(11, 1);
  attachInterrupt(12, parallelOut, RISING);
  
  //Attach interupt on Parallel data bus strobe pin
  //attachInterrupt(PDB_STROBE, parallelOut, RISING);
  
  //Begin status check of experiment control boards
  //A response of ASCII 6 'Acknowledge' shows life
  /*
  for (int i = 0; i < 4; i++){
    Wire.requestFrom((i+1), 1);
    expStatus[i] = Wire.read();
  }
  if((expStatus[0] == 6) && (expStatus[1] = 6) && (expStatus[2] == 6) && (expStatus[3] == 6)){
    Serial.println("All boards alive and well.");
  }
  */

}

void loop() {
  //All boards running autonomously so all the data board does is handle telemetry
  //While loop that runs once the experiments have run their course.
  while(true){
    //Do nothing  
  }
}
