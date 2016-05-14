void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    pinMode(0, INPUT_ANALOG);
}

void loop() {
  int sensorValue = analogRead(0);
  Serial.println(sensorValue, DEC);
}



