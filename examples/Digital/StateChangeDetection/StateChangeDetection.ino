/*
  State change detection (edge detection)

  Often, you don't need to know the state of a digital input all the
  time, but you just need to know when the input changes from one state
  to another.  For example, you want to know when a button goes from
  OFF to ON.  This is called state change detection, or edge detection.

  This example shows how to detect when the built-in button changes
  from off to on and on to off.

  To use this example, connect to the Maple using the USB serial port.
  Then push the button a few times and see what happens.

  created  27 Sep 2005
  modified 30 Dec 2009
  by Tom Igoe

  Ported to the Maple 27 May 2010
  by Bryan Newbold
*/

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // initialize the button pin as a input:
    pinMode(BOARD_BUTTON_PIN, INPUT);
    // initialize the LED as an output:
    pinMode(33, OUTPUT);
}

void loop() {
    // read the pushbutton input pin:
    buttonState = digitalRead(BOARD_BUTTON_PIN);

    // compare the buttonState to its previous state
    if (buttonState != lastButtonState) {
        // if the state has changed, increment the counter
        if (buttonState == HIGH) {
            // if the current state is HIGH, then the button went from
            // off to on:
            buttonPushCounter++;
            Serial.println("on");
            Serial.print("number of button pushes:  ");
            Serial.println(buttonPushCounter, DEC);
        }
        else {
            // if the current state is LOW, then the button went from
            // on to off:
            Serial.println("off");
        }

        // save the current state as the last state, for next time
        // through the loop
        lastButtonState = buttonState;
    }

    // turns on the LED every four button pushes by checking the
    // modulo of the button push counter.  Modulo (percent sign, %)
    // gives you the remainder of the division of two numbers:
    if (buttonPushCounter % 4 == 0) {
        digitalWrite(33, HIGH);
    } else {
        digitalWrite(33, LOW);
    }
}
