/*
  Debounce

  Each time the input pin goes from LOW to HIGH (e.g. because of a push-button
  press), the output pin is toggled from LOW to HIGH or HIGH to LOW.  There's
  a minimum delay between toggles to debounce the circuit (i.e. to ignore
  noise).

  created 21 November 2006
  by David A. Mellis
  modified 3 Jul 2009
  by Limor Fried
  modified 15 Jul 2010
  by Bryan Newbold; thanks adamfeuer!
*/

// Variables:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int lastDebounceTime = 0;   // the last time the output pin was toggled
int debounceDelay = 50;     // the debounce time; increase if the output flickers

void setup() {
    pinMode(BOARD_BUTTON_PIN, INPUT);
    pinMode(33, OUTPUT);
}

void loop() {
    // read the state of the switch into a local variable:
    int reading = digitalRead(BOARD_BUTTON_PIN);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState) {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:
        buttonState = reading;
    }

    // set the LED using the state of the button:
    digitalWrite(33, buttonState);

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;
}
