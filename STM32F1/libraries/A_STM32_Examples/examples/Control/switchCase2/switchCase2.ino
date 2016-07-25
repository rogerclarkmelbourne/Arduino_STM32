/*
  Switch statement with serial input

  Demonstrates the use of a switch statement.  The switch statement
  allows you to choose from among a set of discrete values of a
  variable.  It's like a series of if statements.

  To see this sketch in action, open the Serial monitor and send any
  character.  The characters a, b, c, d, and e, will turn on LEDs.
  Any other character will turn the LEDs off.

  The circuit:
  * 5 LEDs attached to pins 2 through 6 through 220-ohm resistors

  created 1 Jul 2009
  by Tom Igoe

  Ported to the Maple 27 May 2010
  by Bryan Newbold

  http://leaflabs.com/docs/lang/cpp/switchcase.html
*/

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Initialize the LED pins:
    for (int thisPin = 2; thisPin <= 6; thisPin++) {
        pinMode(thisPin, OUTPUT);
    }
}

void loop() {
    // Read the sensor:
    if (Serial.available() > 0) {
        int inByte = Serial.read();
        // Do something different depending on the character received.
        // The switch statement expects single number values for each
        // case; in this example, though, you're using single quotes
        // to tell the controller to get the ASCII value for the
        // character.  For example 'a' = 97, 'b' = 98, and so forth:
        switch (inByte) {
        case 'a':
            digitalWrite(2, HIGH);
            break;
        case 'b':
            digitalWrite(3, HIGH);
            break;
        case 'c':
            digitalWrite(4, HIGH);
            break;
        case 'd':
            digitalWrite(5, HIGH);
            break;
        case 'e':
            digitalWrite(6, HIGH);
            break;
        default:
            // Turn all the LEDs off:
            for (int thisPin = 2; thisPin < 7; thisPin++) {
                digitalWrite(thisPin, LOW);
            }
        }
    }
}
