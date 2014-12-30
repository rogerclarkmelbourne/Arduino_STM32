/*
  Dimmer

  Demonstrates sending data from the computer to the Maple, in this
  case to control the brightness of an LED.  The data is sent in
  individual bytes, each of which ranges from 0 to 255.  Maple reads
  these bytes and uses them to set the brightness of the LED.

  The circuit:
  LED connected to pin 9.
  Serial connection to Processing, Max/MSP, or another serial application.

  created 2006
  by David A. Mellis
  modified 14 Apr 2009
  by Tom Igoe and Scott Fitzgerald

  http://www.arduino.cc/en/Tutorial/Dimmer
  http://leaflabs.com/docs/lang/api/pwmwrite.html

  Ported to the Maple 28 May 2010
*/

int ledPin = 9;

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Declare ledPin as an OUTPUT:
    pinMode(ledPin, OUTPUT);
}

void loop() {
    int brightness;

    // Check if data has been sent from the computer:
    if (Serial.available()) {
        // Read the most recent byte (which will be from 0 to 255), then
        // convert it to be between 0 and 65,535, which are the minimum
        // and maximum values usable for PWM:
        brightness = map(Serial.read(), 0, 255, 0, 65535);
        // Set the brightness of the LED:
        pwmWrite(ledPin, brightness);
    }
}
