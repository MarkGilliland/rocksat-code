#include <Stepper.h>
Stepper myStepper = Stepper(513, 2, 4, 3, 5);
int Deg = -20;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  myStepper.setSpeed(75);
  myStepper.step(Deg*513/360);
}

void loop() {
  // put your main code here, to run repeatedly:

}
