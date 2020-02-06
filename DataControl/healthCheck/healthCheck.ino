// set this to the hardware serial port you wish to use
#define HWSERIAL Serial3

void setup() {
  HWSERIAL.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  HWSERIAL.print("hello");
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
