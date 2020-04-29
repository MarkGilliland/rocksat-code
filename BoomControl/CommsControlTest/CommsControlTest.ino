/*
 * Code to test base functionality of comms boom extension/retraction.
 * EXTREMELY basic code meant to be used with two limit switches and a SINGLE-DIRECTIONAL motor controller.
 * Code should simply run the motor until it hits an endstop. To reverse, switch the polarity of the motor wires by hand, then unplug the currently-engaged limit switch.
 * Author: Bradley Jesteadt
 * Date: April the 28th, 2020
 */

void setup() {
  pinMode(13, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

}

int dir = 0;

void loop() {
  if(digitalRead(6) && digitalRead(7)){
    digitalWrite(13,LOW);
  }else{
    digitalWrite(13,HIGH);
    delay(50);
  }
}
