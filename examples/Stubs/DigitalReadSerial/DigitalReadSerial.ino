void setup() {
    pinMode(2, INPUT);
}

void loop() {
    int sensorValue = digitalRead(2);
    SerialUSB.println(sensorValue, DEC);
}
