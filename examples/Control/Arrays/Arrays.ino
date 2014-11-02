/*
  Arrays

  Demonstrates the use of an array to hold pin numbers in order to
  iterate over the pins in a sequence.  Lights multiple LEDs in
  sequence, then in reverse.

  Unlike the for loop tutorial, where the pins have to be
  contiguous, here the pins can be in any random order.

  The circuit:
  * LEDs from pins 2 through 7 to ground, through resistors

  created 2006
  by David A. Mellis
  modified 5 Jul 2009
  by Tom Igoe
  modifed for Maple
  by LeafLabs

  http://leaflabs.com/docs/lang/cpp/array.html
*/

int delayTime = 100;      // The higher the number, the slower the timing.
int ledPins[] = {
  2, 7, 4, 6, 5, 3 };     // An array of pin numbers to which LEDs are attached
int pinCount = 6;         // The number of pins (i.e. the length of the array)

void setup() {
    int thisPin;
    // The array elements are numbered from 0 to (pinCount - 1).
    // Use a for loop to initialize each pin as an output:
    for (int thisPin = 0; thisPin < pinCount; thisPin++)  {
        pinMode(ledPins[thisPin], OUTPUT);
    }
}

void loop() {
  // Loop from the lowest pin to the highest:
  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
    // Turn the pin on:
    digitalWrite(ledPins[thisPin], HIGH);
    delay(delayTime);
    // Turn the pin off:
    digitalWrite(ledPins[thisPin], LOW);
  }

  // Loop from the highest pin to the lowest:
  for (int thisPin = pinCount - 1; thisPin >= 0; thisPin--) {
    // Turn the pin on:
    digitalWrite(ledPins[thisPin], HIGH);
    delay(delayTime);
    // Turn the pin off:
    digitalWrite(ledPins[thisPin], LOW);
  }
}
