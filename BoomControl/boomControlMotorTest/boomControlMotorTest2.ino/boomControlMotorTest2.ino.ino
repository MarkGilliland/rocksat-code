//Encoder and motor test for rev2 boom control board

//Include encoder library
//#include <Encoder.h>

//Define constants
#define ENABLE 10
#define M1CCW A0    // Motor 1 is the camera boom
#define M1CW A1
#define M2CCW A2    // Motor 2 is the comms boom
#define M2CW A3
#define FRONT_SENSOR 4
#define BACK_SENSOR 5
#define DIRECTION 0 // 1 signifies out, 0 signifies in, on camera boom.
#define COMMS_EXTENSION_TIME 10000
#define CAMERA_EXTENSION_TIME 36000
#define TIMER 300//CAMERA_EXTENSION_TIME // 1.25" per sec, ~36sec for full extension on camera boom.
//Attach encoder interrupts to pins 2 and 9
//Encoder myEnc(2, 9);

bool commsStuck = false;

void setup() {
  //Start serial connection
  Serial.begin(9600);

  //Set motor control pins as outputs
  pinMode(ENABLE, OUTPUT);
  pinMode(M1CCW, OUTPUT);
  pinMode(M1CW, OUTPUT);
  pinMode(M2CCW, OUTPUT);
  pinMode(M2CW, OUTPUT);
  pinMode(FRONT_SENSOR, INPUT);
  pinMode(BACK_SENSOR, INPUT);

    //TESTING THE CAMERA BOOM
    digitalWrite(ENABLE, HIGH);
    if(DIRECTION){
      digitalWrite(M1CW, LOW);
      digitalWrite(M1CCW, HIGH);
    }else{
      digitalWrite(M1CW, HIGH);
      digitalWrite(M1CCW, LOW);
    }
    digitalWrite(13,HIGH);
    digitalWrite(M2CW, LOW);
    digitalWrite(M2CCW, LOW);
    delay(TIMER);
    digitalWrite(M1CW, LOW);
    digitalWrite(M1CCW, LOW);
    digitalWrite(13,LOW);

  // CYCLE TESTING COMMS BOOM
  //  bool deploying = true;
  //  int numCycles = 10;
  //  digitalWrite(13,HIGH);
  //  digitalWrite(M1CW, LOW);
  //  digitalWrite(M1CCW, LOW);
  //  if(digitalRead(FRONT_SENSOR)){
  //    digitalWrite(M2CW, HIGH);
  //    digitalWrite(M2CCW, LOW);
  //    deploying = false;
  //  }else{
  //    digitalWrite(M2CW, LOW);
  //    digitalWrite(M2CCW, HIGH);
  //  }
  //  for(int i = 0; i < numCycles; i++){
  //    while(deploying){
  //      digitalWrite(ENABLE,HIGH);
  //      if(digitalRead(FRONT_SENSOR)){
  //        digitalWrite(ENABLE,LOW);
  //        digitalWrite(M2CW, HIGH);
  //        digitalWrite(M2CCW, LOW);
  //        deploying = false;
  //        break;
  //      }
  //    }
  //    while(!deploying){
  //      digitalWrite(ENABLE,HIGH);
  //      if(digitalRead(BACK_SENSOR)){
  //        digitalWrite(ENABLE,LOW);
  //        digitalWrite(M2CW, LOW);
  //        digitalWrite(M2CCW, HIGH);
  //        deploying = true;
  //        break;
  //      }
  //    }
  //  }

  //  EXTEND/RETRACT COMMS BOOM
//  bool deploying = 0;
//  digitalWrite(13, HIGH);
//  digitalWrite(M1CW, LOW);
//  digitalWrite(M1CCW, LOW);
//  int startTime = millis();
//  int stopTime = startTime;
//  if (deploying) {
//    while (!digitalRead(FRONT_SENSOR)) {
//      digitalWrite(ENABLE, HIGH);
//      digitalWrite(M2CW, LOW);
//      digitalWrite(M2CCW, HIGH);
//      stopTime = millis();
//      if ((stopTime - startTime) > 1.5 * COMMS_EXTENSION_TIME) {
//        commsStuck = true;
//        digitalWrite(ENABLE, LOW);
//        break;
//      }
//    }
//    digitalWrite(ENABLE, LOW);
//  } else {
//    while (!digitalRead(BACK_SENSOR)) {
//      digitalWrite(ENABLE, HIGH);
//      digitalWrite(M2CW, HIGH);
//      digitalWrite(M2CCW, LOW);
//      stopTime = millis();
//      if ((stopTime - startTime) > 1.5 * COMMS_EXTENSION_TIME) {
//        commsStuck = true;
//        digitalWrite(ENABLE, LOW);
//        break;
//      }
//    }
//    digitalWrite(ENABLE, LOW);
//  }
}

void loop() {
  int blinkTime = 250;
  if (commsStuck) {
    blinkTime = 750;
    Serial.println("Comms boom stuck.");
  }
  //Serial.println(myEnc.read());
  digitalWrite(13, HIGH);
  delay(blinkTime);
  digitalWrite(13, LOW);
  delay(blinkTime);
  Serial.print("Front: ");
  Serial.println(digitalRead(FRONT_SENSOR));
  Serial.print("Back: ");
  Serial.println(digitalRead(BACK_SENSOR));

}
