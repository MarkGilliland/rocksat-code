const int M_1 = 8;
const int M_2 = 9;
const int M_3 = 10;
const int M_4 = 11;
const int pins[] = {M_1, M_2, M_3, M_4};
//#define FORWARD//leave uncommented for forward
#define BACKWARD//leave uncommented for backward

#ifdef FORWARD
const int states[4][4] =  {{0, 1, 1, 1},
                           {1, 0, 1, 1},
                           {1, 1, 0, 1},
                           {1, 1, 1, 0}};
#endif
#ifdef BACKWARD
const int states[4][4] =  {{1, 1, 1, 0},
                           {1, 1, 0, 1},
                           {1, 0, 1, 1},
                           {0, 1, 1, 1}};
#endif

//Speed parameters
const int numberOfFlings = 11;//11 slots in the gear
const int interFlingDelay = 1000;//delay in millis between shots
const int stepsPerFling = 46;//number of steps per bb fling
const float steppingFreq = 400;//frequency, Hz, that the motor operates at. Faster frequency means faster fling.

const int steppingDelay = 1000.0/steppingFreq;//the delay, in ms, between steps of the motor. Modify stepping freq if desiring faster speed

void setup() {
  // put your setup code here, to run once:
  pinMode(M_1, OUTPUT);
  pinMode(M_2, OUTPUT);
  pinMode(M_3, OUTPUT);
  pinMode(M_4, OUTPUT);
}

void loop() {
  for(int fling = 0; fling < 11; fling++){
    for(int j = 0; j < stepsPerFling / 4 + 1; j++){
      for(int state = 0; state < 4; state++){
        for(int i = 0; i < 4; i++){
          digitalWrite(pins[i], states[state][i]);
        }
        delay(steppingDelay);
      }
    }
    delay(interFlingDelay);
  }    
}
