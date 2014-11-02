/*
 Button

 Turns on and off the built-in LED when the built-in button is
 pressed.

 Ported to Maple from the Arduino example 27 May 2011
 by Marti Bolivar
*/

void setup() {
  // Initialize the built-in LED pin as an output:
  pinMode(BOARD_LED_PIN, OUTPUT);
  // Initialize the built-in button (labeled BUT) as an input:
  pinMode(BOARD_BUTTON_PIN, INPUT);
}

void loop() {
    // Check if the button is pressed.
    if (isButtonPressed()) {
        // If so, turn the LED from on to off, or from off to on:
        toggleLED();
    }
}
