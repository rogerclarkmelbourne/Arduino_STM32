byte lPin = 33;

void setup() {
  pinMode(lPin, OUTPUT);
}

void loop() {
  delay(1500);
  Joystick.X(1000);
  delay(1500);
  Joystick.Y(1000);
  delay(1500);
  Joystick.X(0);
  delay(1500);
  Joystick.Y(0);
}
