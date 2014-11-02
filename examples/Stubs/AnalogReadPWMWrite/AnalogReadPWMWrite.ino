void setup() {
    pinMode(2, INPUT_ANALOG);
    pinMode(6, PWM);
}

void loop() {
    int sensorValue = analogRead(2);
    int ledFadeValue = map(sensorValue, 0, 4095, 0, 65535);
    pwmWrite(6, ledFadeValue);
}
