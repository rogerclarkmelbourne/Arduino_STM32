void setup() {
    pinMode(0, INPUT_ANALOG);
}

void loop() {
  int sensorValue = analogRead(0);
  SerialUSB.println(sensorValue, DEC);
}



