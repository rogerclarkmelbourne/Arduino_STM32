/*
  Multiple serial test

  Receives from Serial1, sends to Serial.

  The circuit:
  * Maple connected over Serial
  * Serial device (e.g. an Xbee radio, another Maple)

  created 30 Dec. 2008
  by Tom Igoe

  Ported to the Maple 27 May 2010
  by Bryan Newbold
*/

int inByte;                     // Byte read from Serial1

void setup() {
    // Initialize Serial1
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    Serial1.begin(115200);
}

void loop() {
    // Read from Serial1, send over USB on Maple (or uses hardware serial 1 and hardware serial 2 on non-maple boards:
    if (Serial1.available()) {
        inByte = Serial1.read();
        Serial.write(inByte);
    }
}
