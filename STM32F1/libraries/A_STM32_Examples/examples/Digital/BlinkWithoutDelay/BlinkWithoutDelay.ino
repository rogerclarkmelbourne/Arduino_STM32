/*
  Blink without delay

  Turns on and off the built-in light emitting diode (LED), without
  using the delay() function.  This means that other code can run at
  the same time without being interrupted by the LED code.

  created 2005
  by David A. Mellis
  modified 17 Jun 2009
  by Tom Igoe
  modified for Maple 27 May 2011
  by Marti Bolivar
*/

// Variables:
int previousMillis = 0;        // will store the last time the LED was updated
int interval = 1000;            // interval at which to blink (in milliseconds)

void setup() {
    // Set up the built-in LED pin as output:
    pinMode(33, OUTPUT);
}

void loop() {
    // Check to see if it's time to blink the LED; that is, if the
    // difference between the current time and last time we blinked
    // the LED is bigger than the interval at which we want to blink
    // the LED.
    if (millis() - previousMillis > interval) {
        // Save the last time you blinked the LED
        previousMillis = millis();

        // If the LED is off, turn it on, and vice-versa:
        digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    }
}
