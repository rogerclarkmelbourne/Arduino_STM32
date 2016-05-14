/*
  Conditionals - If statement

  This example demonstrates the use of if() statements.  It reads the
  state of a potentiometer (an analog input) and turns on an LED only
  if the LED goes above a certain threshold level. It prints the
  analog value regardless of the level.

  The circuit:
  * Potentiometer connected to pin 15.
  Center pin of the potentiometer goes to the Maple pin.
  Side pins of the potentiometer go to +3.3V and ground

  created 17 Jan 2009
  by Tom Igoe

  Ported to the Maple 27 May 2010
  by Bryan Newbold

  http://leaflabs.com/docs/lang/cpp/if.html
*/

// These constants won't change:

const int analogPin = 15;     // Pin that the sensor is attached to

const int threshold = 400;    // A random threshold level that's in
                              // the range of the analog input

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Initialize the built-in LED pin as an output:
    pinMode(33, OUTPUT);

    // Initialize the potentiometer pin as an analog input:
    pinMode(analogPin, INPUT_ANALOG);
}

void loop() {
    // Read the value of the potentiometer:
    int analogValue = analogRead(analogPin);

    // If the analog value is high enough, turn on the LED:
    if (analogValue > threshold) {
        digitalWrite(33, HIGH);
    }
    else {
        digitalWrite(33, LOW);
    }

    // Print the analog value:
    Serial.println(analogValue, DEC);
}
