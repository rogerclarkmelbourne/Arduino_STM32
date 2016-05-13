void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    pinMode(2, INPUT);
}

void loop() {
    int sensorValue = digitalRead(2);
    Serial.println(sensorValue, DEC);
}
