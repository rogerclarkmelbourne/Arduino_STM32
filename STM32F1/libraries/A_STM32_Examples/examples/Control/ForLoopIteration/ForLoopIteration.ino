/*
  for loop iteration

  Demonstrates the use of a for() loop.
  Lights multiple LEDs in sequence, then in reverse.

  The circuit:
  * LEDs from pins 2 through 7 to ground, through resistors

  created 2006
  by David A. Mellis
  modified 5 Jul 2009
  by Tom Igoe

  http://leaflabs.com/docs/lang/cpp/for.html

  Modified for Maple
  by LeafLabs
*/

int delayTime = 100;           // The higher the number, the slower the timing.

void setup() {
    // Use a for loop to initialize each pin as an output:
    for (int thisPin = 2; thisPin <= 7; thisPin++) {
        pinMode(thisPin, OUTPUT);
    }
}

void loop() {
    // Loop from the lowest pin to the highest:
    for (int thisPin = 2; thisPin <= 7; thisPin++) {
        // Turn the pin on:
        digitalWrite(thisPin, HIGH);
        delay(delayTime);
        // Turn the pin off:
        digitalWrite(thisPin, LOW);
    }

    // Loop from the highest pin to the lowest:
    for (int thisPin = 7; thisPin >= 2; thisPin--) {
        // Turn the pin on:
        digitalWrite(thisPin, HIGH);
        delay(delayTime);
        // Turn the pin off:
        digitalWrite(thisPin, LOW);
    }
}
