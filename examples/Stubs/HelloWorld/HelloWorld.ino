void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
}

void loop() {
    Serial.println("Hello World!");
	delay(1000);
}
