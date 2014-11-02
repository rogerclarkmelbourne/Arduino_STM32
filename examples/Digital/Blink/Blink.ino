/*
 Blink

 Turns on the built-in LED on for one second, then off for one second,
 repeatedly.

 Ported to Maple from the Arduino example 27 May 2011
 By Marti Bolivar
*/

void setup() {
    // Set up the built-in LED pin as an output:
    pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop() {
    toggleLED();          // Turn the LED from off to on, or on to off
    delay(1000);          // Wait for 1 second (1000 milliseconds)
}
