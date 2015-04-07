/*
 Blink: Turns on the built-in LED on for one second, then off for one second, repeatedly.
 Arduino 1.6.0rc1
   Sketch uses 11,900 bytes (11%) of program storage space. Maximum is 108,000 bytes.
   Global variables use 2,592 bytes of dynamic memory.
 Ported to Maple from the Arduino example 27 May 2011 By Marti Bolivar
*/

void setup() {
    // Set up the built-in LED pin as an output:
    pinMode(BOARD_LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(BOARD_LED_PIN,!digitalRead(BOARD_LED_PIN));// Turn the LED from off to on, or on to off
    delay(1000);          // Wait for 1 second (1000 milliseconds)
}
