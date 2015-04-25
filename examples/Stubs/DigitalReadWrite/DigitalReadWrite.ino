
void setup() {
    pinMode(33, OUTPUT);
}

void loop() {
    int switchValue = digitalRead(2);
    digitalWrite(33, switchValue);
}
