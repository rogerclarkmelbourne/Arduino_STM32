
void setup() {
    pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop() {
    int switchValue = digitalRead(2);
    digitalWrite(BOARD_LED_PIN, switchValue);
}
