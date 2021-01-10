//Encoder and motor test for rev2 boom control board

//Include encoder library
#include <Encoder.h>

//Define constants
#define ENABLE 10
#define M1CCW A0
#define M1CW A1
#define M2CCW A2
#define M2CW A3
//Attach encoder interrupts to pins 2 and 9
Encoder myEnc(2, 9);
void setup() {
  //Start serial connection
  Serial.begin(9600);
  
  //Set motor control pins as outputs
  pinMode(ENABLE, OUTPUT);
  pinMode(M1CCW, OUTPUT);
  pinMode(M1CW, OUTPUT);
  pinMode(M2CCW, OUTPUT);
  pinMode(M2CW, OUTPUT);

  //Start M1 CW and M2 CCW
  digitalWrite(ENABLE, HIGH);
  digitalWrite(M1CW, HIGH);
  digitalWrite(M1CCW, LOW);
  digitalWrite(M2CW, LOW);
  digitalWrite(M2CCW, HIGH);
}

void loop() {
  Serial.println(myEnc.read());
}
