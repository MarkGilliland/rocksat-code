/*  MOSFET test code for all experiment control boards.
 *  This code simply turns on both MOSFET channels for 1 second, then off for 1 second. 
 *  Check to see what voltage MOSFET for configured to use by looking at the solder jumpers
 *  near the bottom right corner. The middle pad soldered to the upper pad means that channel
 *  is configured for 12 volts. The middle pad soldered to the lower pad means that that
 *  channel is configured for 5 volts. 
 */
#define MOSFET1 9
#define MOSFET2 A2
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize MOSFET channeles 1 & 2 as outputs.
  pinMode(MOSFET1, OUTPUT);
  pinMode(MOSFET2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(MOSFET1, HIGH);   // turn the MOSFET on (HIGH is the voltage level)
  digitalWrite(MOSFET2, HIGH);   // comment this line out if you do not want to control channel 2
  delay(1000);                   // wait for a second
  digitalWrite(MOSFET1, LOW);    // turn the MOSFET off by making the voltage LOW
  digitalWrite(MOSFET2, LOW);    // commen this line out if you do not want to control channel 2
  delay(1000);                   // wait for a second
}
