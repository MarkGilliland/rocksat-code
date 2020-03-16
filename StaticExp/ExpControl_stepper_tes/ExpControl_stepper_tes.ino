// Uses the standard Arduino stepper library to run the stepper
// motor attached to experiment control boards.
// Our debris launcher steppers should have 513 steps per 1 rotation
// By: CSM Rocksat-X - Graham Braly
// 03/16/2020

// Include stepper library
#include <Stepper.h>
// Initialize stepper object and name it myStepper
Stepper myStepper = Stepper(513, 2, 4, 3, 5);
void setup() {
  // Set stepper control pins as outputs
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  // Set the speed (in RPM) at which the stepper will turn when you call step()
  myStepper.setSpeed(75);
  // Command the stepper to rotate 2000 steps.
  myStepper.step(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
