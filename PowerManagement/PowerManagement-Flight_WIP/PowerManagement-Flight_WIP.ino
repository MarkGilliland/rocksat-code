/*  Mines Rocksat-X 2020 Power management board flight code
 *  This code handles powering up the other subsystems and monitoring for overcurrent and overtemp.
 *  Note that you need the teensy 4.0 add-on for Arduino for this to function.
 *  Currently the refreshMonitor checks for faults at 13 Hz
 */
 
/* Analog Mux Channel Names:
 * 000-  0: 12V-CAM           8: 5VD-STA
 * 001-  1: 5VA-MAG           9: 5VD-MAG
 * 010-  2: 5VA-TX           10: 5VD-CAM
 * 011-  3: 12V-STA          11: 5VA-CAM
 * 100-  4: 5VA-LAS          12: 5VD-TX
 * 101-  5: Unused           13: 5VA Temperature
 * 110-  6: 5VA-STA          14: 5VD-LAS
 * 111-  7: 12V Temperature  15: 5VD Temperature
 */
//FLIGHT_MODE == 1 accounts for T-120s GSE turn on, FLIGHT_MODE == 0 ignores T-120s turn on for testing
//VERIFY WHAT MODE THE BOARD IS SET TO
#define FLIGHT_MODE 1    
#if FLIGHT_MODE == 1
  unsigned long TE_1 = 140000;    //Was 140,000
  unsigned long TE_2 = 205000;    //Was 205,000
  unsigned long T_OFF = 455000;   //Was 455,000
#elif FLIGHT_MODE == 0
  unsigned long TE_1 = 20000;    //Time that TE-1 line (computers) is turned on, in milliseconds
  unsigned long TE_2 = 85000;    //Time that TE-2 line (actuators) is turned on, in milliseconds
  unsigned long T_OFF = 335000;  //(Assumed) time that the payload should turn off
#endif

//Constant pin declarations:
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
//12V Channels
const int ENABLE_STA_12V = 14;
const int ENABLE_CAM_12V = A1;
const int ENABLES_12V[] = {ENABLE_STA_12V, ENABLE_CAM_12V};
//5V Active Channels
const int ENABLE_MAG_5VA = 16;
const int ENABLE_STA_5VA = A3;
const int ENABLE_TX_5VA  = 0;
const int ENABLE_LAS_5VA = 1;
const int ENABLE_CAM_5VA = 2;
const int ENABLES_5VA[] = {ENABLE_MAG_5VA, ENABLE_STA_5VA, ENABLE_TX_5VA, 
                           ENABLE_LAS_5VA, ENABLE_CAM_5VA};
//5V Digital Channels
const int ENABLE_MAG_5VD = 4;
const int ENABLE_STA_5VD = 3;
const int ENABLE_TX_5VD  = 7;
const int ENABLE_LAS_5VD = 5;
const int ENABLE_CAM_5VD = 6; 
const int ENABLES_5VD[] = {ENABLE_MAG_5VD, ENABLE_STA_5VD, ENABLE_TX_5VD, 
                           ENABLE_LAS_5VD, ENABLE_CAM_5VD };

//Global variables & constants
unsigned long upTime = 0;
float camCurrent_12V = 0.0;
float staCurrent_12V = 0.0;
float staCurrent_5VD = 0.0;
float camCurrent_5VD = 0.0;
float magCurrent_5VD = 0.0;
float txCurrent_5VD = 0.0;
float lasCurrent_5VD = 0.0;
float staCurrent_5VA = 0.0;
float camCurrent_5VA = 0.0;
float magCurrent_5VA = 0.0;
float txCurrent_5VA = 0.0;
float lasCurrent_5VA = 0.0;
int temp_12V = 0;
int temp_5VA = 0;
int temp_5VD = 0;
bool TE_1_Active = false;
bool TE_2_Active = false;

void refreshMonitor(){
  //Read both 000 channels to variables
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 0);
  delay(2);
  camCurrent_12V = countsToAmps(analogRead(MUX_AIN_1));
  staCurrent_5VD = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 001 channels to variables
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 1);
  delay(2);
  magCurrent_5VA = countsToAmps(analogRead(MUX_AIN_1));
  magCurrent_5VD = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 010 channels to variables
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 1);
  digitalWrite(MUX_SEL_A, 0);
  delay(2);
  txCurrent_5VA = countsToAmps(analogRead(MUX_AIN_1));
  camCurrent_5VD = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 011 channels to variables
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 1);
  digitalWrite(MUX_SEL_A, 1);
  delay(2);
  staCurrent_12V = countsToAmps(analogRead(MUX_AIN_1));
  camCurrent_5VA = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 100 channels to variables
  digitalWrite(MUX_SEL_C, 1);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 0);
  delay(2);
  lasCurrent_5VA = countsToAmps(analogRead(MUX_AIN_1));
  txCurrent_5VD = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 101 channels to variables
  digitalWrite(MUX_SEL_C, 1);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 1);
  delay(2);
  //null = countsToAmps(analogRead(MUX_AIN_1)); This is not used
  temp_5VA = countsToDegrees(analogRead(MUX_AIN_2));
  //Read both 110 channels to variables
  digitalWrite(MUX_SEL_C, 1);
  digitalWrite(MUX_SEL_B, 1);
  digitalWrite(MUX_SEL_A, 0);
  delay(2);
  staCurrent_5VA = countsToAmps(analogRead(MUX_AIN_1));
  lasCurrent_5VD = countsToAmps(analogRead(MUX_AIN_2));
  //Read both 111 channels to variables 
  digitalWrite(MUX_SEL_C, 1);
  digitalWrite(MUX_SEL_B, 1);
  digitalWrite(MUX_SEL_A, 1);
  delay(2);
  temp_12V = countsToDegrees(analogRead(MUX_AIN_1));
  temp_5VA = countsToDegrees(analogRead(MUX_AIN_2));
  //All temps and currents acquired. Verify that they are within bounds
  //Check for excessive 12V regulator temp
  if(temp_12V > 85.0) {
    Serial.println("Excessive 12V regulator temp.");
    digitalWrite(ENABLE_STA_12V, OUTPUT_DISABLE);
    digitalWrite(ENABLE_CAM_12V, OUTPUT_DISABLE);
  }
  //Check for excessive 5VA temp
  if(temp_5VA > 85.0){
    Serial.println("Excessive 5VA regulator temp.");
    for(int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VA[i], OUTPUT_DISABLE);
    }
  }
  //Check for excessive 5VD temp
  if(temp_5VD > 85.0){
    Serial.println("Excessive 5VD regulator temp.");
    for(int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VD[i], OUTPUT_DISABLE);
    }
  }
  //Check for excessive 12V current
  if((camCurrent_12V + staCurrent_12V) > 2.6){
    Serial.println("Excessive 12V current.");
    digitalWrite(ENABLE_STA_12V, OUTPUT_DISABLE);
    digitalWrite(ENABLE_CAM_12V, OUTPUT_DISABLE);
  }
  //Check for excessive 5VD current
  if((staCurrent_5VD + camCurrent_5VD + magCurrent_5VD + txCurrent_5VD + lasCurrent_5VD) > 4.0){
    Serial.println("Excessive 5VD current.");
    for(int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VD[i], OUTPUT_DISABLE);
    }
  }
  //Check for excessive 5VA current
  if((staCurrent_5VA + camCurrent_5VA + magCurrent_5VA + txCurrent_5VA + lasCurrent_5VA) > 6.0){
    Serial.println("Excessive 5VA current.");
    for(int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VA[i], OUTPUT_DISABLE);
    }
  }
}

void setup() {
  // Start serial interface
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

  //Set mux channels to an initial state of 000
  digitalWrite(MUX_SEL_C, 0);
  digitalWrite(MUX_SEL_B, 0);
  digitalWrite(MUX_SEL_A, 0);
  //Setup complete
  Serial.println("Setup complete.");
  if(FLIGHT_MODE == 0){
    Serial.println("Ignoring 120 second wait after GSE power up, is this desired?");
  }
  else if(FLIGHT_MODE == 1){
    Serial.println("Respecting 120 second wait after GSE power up until T+0, is this desired?");
  }
}

void loop() {
  static unsigned int iter = 0;
  refreshMonitor();
  upTime = millis();
  if(upTime/1000 > iter){
    iter = upTime/1000;
    Serial.println((iter-120));
    if(iter == 120) Serial.println("Mark T = 0. Launch.");
  }
  //Check if T_OFF has been reached, and turn off if true
  if (upTime > T_OFF){
    Serial.println("T_OFF reached, powering down outputs.");
    for (int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VD[i], OUTPUT_DISABLE);
      digitalWrite(ENABLES_5VA[i], OUTPUT_DISABLE);
    }
    digitalWrite(ENABLE_STA_12V, OUTPUT_DISABLE);
    digitalWrite(ENABLE_CAM_12V, OUTPUT_DISABLE);
    while(true){
      //Do nothing, system has been powered down so wait for GSE to turn off.
    }
  }
  //Check if TE_1 has been reached, if yes, turn on 5VD channels
  if((upTime > TE_1) && (!TE_1_Active)){
    Serial.println("TE_1 reached, powering on 5VD channels, besides 5VD_TX.");
    //5V digital lines for computers
    /*
    for (int i = 0; i < 5; i++){
      digitalWrite(ENABLES_5VD[i], OUTPUT_ENABLE);
    }
    */
    //Enable all 5VD lines besides 5VD_TX, which powers the 360 camera 5V line
    digitalWrite(ENABLE_MAG_5VD, OUTPUT_ENABLE);
    digitalWrite(ENABLE_STA_5VD, OUTPUT_ENABLE);
    digitalWrite(ENABLE_LAS_5VD, OUTPUT_ENABLE);
    digitalWrite(ENABLE_CAM_5VD, OUTPUT_ENABLE);
    //Set global flag to show that digital outputs are on
    TE_1_Active = true;
  }
  //Check if TE_2 has been reached, if yes, then turn on 5VA and 12V channels
  if((upTime > TE_2) && (!TE_2_Active)){
    Serial.println("TE_2 reached, powering on 5VA and 12V channels, as well as 5VD_TX.");
    //5V active lines for actuators
    digitalWrite(ENABLE_MAG_5VA, OUTPUT_ENABLE);
    digitalWrite(ENABLE_STA_5VA, OUTPUT_ENABLE);
    digitalWrite(ENABLE_TX_5VA, OUTPUT_ENABLE);
    digitalWrite(ENABLE_LAS_5VA, OUTPUT_ENABLE);
    digitalWrite(ENABLE_CAM_5VA, OUTPUT_ENABLE);
    //12V lines for actuators
    digitalWrite(ENABLE_STA_12V, OUTPUT_ENABLE);
    digitalWrite(ENABLE_CAM_12V, OUTPUT_ENABLE);
    //Set global flag to show that actuator outputs are on
    TE_2_Active = true;
  }
  if(upTime > 326000){
    //Power on 5V line to camera at T+105
    digitalWrite(ENABLE_TX_5VD, OUTPUT_ENABLE); 
  }
}
