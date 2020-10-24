/* This code turns all power lines on, and then prints the temperature and current info to the user.
 *  use the #defines below to change behavior.
 *  Note that you need the teensy 4.0 add-on for Arduino for this to function.
 */

/* Channel names:
 *  0: 12V-CAM           8: 5VD-STA
 *  1: 5VA-MAG           9: 5VD-MAG
 *  2: 5VA-TX           10: 5VD-CAM
 *  3: 12V-STA          11: 5VA-CAM
 *  4: 5VA-LAS          12: 5VD-TX
 *  5: Unused           13: 5VA Temperature
 *  6: 5VA-STA          14: 5VD-LAS
 *  7: 12V Temperature  15: 5VD Temperature
 */
//Defines for behavior of experiment
//#define SEQUENCE_LINES//uncomment to sequence a set of 3 lines on/off
#define ALL_ON//uncomment to have all lines on all the time. Should not be used with SEQUENCE_LINES, will cause unpredicatble behavior

//Misc Pins
const int LED_PIN = 13;//LED on teensy
//MUX pins
const int MUX_AIN_1 = A8;
const int MUX_AIN_2 = A9;
const int MUX_SEL_A = A7;//A is LSbit
const int MUX_SEL_B = A6;
const int MUX_SEL_C = A5;//C is MSbit
//Enable lines for MOSFETs
#define OUTPUT_DISABLE HIGH
#define OUTPUT_ENABLE LOW
//12V Chans
const int ENABLE_STA_12V = 14;
const int ENABLE_CAM_12V = A1;
const int ENABLES_12V[] = {ENABLE_STA_12V, ENABLE_CAM_12V};
//5V Act
const int ENABLE_MAG_5VA = 16;
const int ENABLE_STA_5VA = A3;
const int ENABLE_TX_5VA  = 0;
const int ENABLE_LAS_5VA = 1;
const int ENABLE_CAM_5VA = 2;
const int ENABLES_5VA[] = {ENABLE_MAG_5VA, ENABLE_STA_5VA, ENABLE_TX_5VA, 
                           ENABLE_LAS_5VA, ENABLE_CAM_5VA};
//5V Dig
const int ENABLE_MAG_5VD = 4;
const int ENABLE_STA_5VD = 3;
const int ENABLE_TX_5VD  = 7;
const int ENABLE_LAS_5VD = 5;
const int ENABLE_CAM_5VD = 6; 
const int ENABLES_5VD[] = {ENABLE_MAG_5VD, ENABLE_STA_5VD, ENABLE_TX_5VD, 
                           ENABLE_LAS_5VD, ENABLE_CAM_5VD };

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Set all enable lines as outputs
  for(int i = 0; i < 5; i++){
    pinMode(ENABLES_5VD[i], OUTPUT);
    pinMode(ENABLES_5VA[i], OUTPUT);
    digitalWrite(ENABLES_5VD[i], OUTPUT_DISABLE);
    digitalWrite(ENABLES_5VA[i], OUTPUT_DISABLE);
  }
  for(int i = 0; i < 2; i++){
    pinMode(ENABLES_12V[i], OUTPUT);
    digitalWrite(ENABLES_12V[i], OUTPUT_DISABLE);
  }
  pinMode(LED_PIN, OUTPUT);
  pinMode(MUX_AIN_1, INPUT);
  pinMode(MUX_AIN_2, INPUT);
  pinMode(MUX_SEL_A, OUTPUT);
  pinMode(MUX_SEL_B, OUTPUT);
  pinMode(MUX_SEL_C, OUTPUT);
  

  //set to output on, on 5VA-MAG
  //digitalWrite(ENABLE_LAS_5VA, OUTPUT_ENABLE);
  //Select 5VA-MAG-SNS
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 0);

  #ifdef ALL_ON
  //turn all channels to ENABLE
  for(int i = 0; i < 5; i++){
    digitalWrite(ENABLES_5VD[i], OUTPUT_ENABLE);
    digitalWrite(ENABLES_5VA[i], OUTPUT_ENABLE);
  }
  for(int i = 0; i < 2; i++){
    digitalWrite(ENABLES_12V[i], OUTPUT_ENABLE);
  }
  #endif
}

void loop() {
  
  #ifdef SEQUENCE_LINES
  // Blink 3 channels, rotating, 1sec each.
  digitalWrite(LED_PIN, LOW);
  digitalWrite(ENABLE_STA_12V, OUTPUT_DISABLE);
  digitalWrite(ENABLE_MAG_5VA, OUTPUT_ENABLE);
  delay(1000);
  digitalWrite(ENABLE_MAG_5VA, OUTPUT_DISABLE);
  digitalWrite(ENABLE_MAG_5VD, OUTPUT_ENABLE);
  delay(1000);
  digitalWrite(ENABLE_MAG_5VD, OUTPUT_DISABLE);
  digitalWrite(ENABLE_STA_12V, OUTPUT_ENABLE);
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  #endif
  Serial.println("-------------------------");//new set of data divider
  Serial.print("Timestamp: ");
  Serial.println(millis()/1000.0);
  //Monitor current and display it
  for(uint8_t i = 0; i < 8; i++){
    digitalWrite(MUX_SEL_C, i >> 2);
    digitalWrite(MUX_SEL_B, (i & 0b10) >> 1);
    digitalWrite(MUX_SEL_A, i & 0b1);
    delay(10);
    int ainRaw1 = analogRead(MUX_AIN_1);
    int ainRaw2 = analogRead(MUX_AIN_2);
    //if a current reading, display current
    
    if(i != 5 && i != 7){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(countsToAmps(ainRaw1));
      Serial.print("A, ");
      Serial.print(i+8);
      Serial.print(": ");
      Serial.print(countsToAmps(ainRaw2));
      Serial.println("A");
    }
    else if(i == 5){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(countsToVolts(ainRaw1));
      Serial.print("V, ");
      Serial.print(i+8);
      Serial.print(": ");
      Serial.print(countsToDegrees(ainRaw2));
      Serial.println("°C");
    }
    else if(i == 7){
      Serial.print(i);
      Serial.print(": ");
      Serial.print(countsToDegrees(ainRaw1));
      Serial.print("°C, ");
      Serial.print(i+8);
      Serial.print(": ");
      Serial.print(countsToDegrees(ainRaw2));
      Serial.println("°C");
    }
  }
  delay(2000);
  
}
