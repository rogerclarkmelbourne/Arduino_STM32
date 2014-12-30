/*
  Analog input, serial output

  Reads an analog input pin, prints the results to the serial monitor.

  The circuit:

  * Potentiometer connected to analog pin 15.
  * Center pin of the potentiometer goes to the analog pin.
  * Side pins of the potentiometer go to +3.3V (VCC) and ground

  created over and over again
  by Tom Igoe and everyone who's ever used Arduino

  Ported to Maple 27 May, 2010 by Bryan Newbold
*/

// Analog input pin.  You may need to change this number if your board
// can't do analog input on pin 15.
const int analogInputPin = 15;

void setup() {
  // Declare analogInputPin as INPUT_ANALOG:
  pinMode(analogInputPin, INPUT_ANALOG);
  	Serial.begin(115200); // Ignored by Maple. But needed by boards using Hardware serial via a USB to Serial Adaptor
}

void loop() {
  // Read the analog input into a variable:
  int analogValue = analogRead(analogInputPin);

  // print the result:
  Serial.println(analogValue);
}
