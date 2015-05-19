/*
   Slave mode for Quality Assurance test

   Used as follows:

       1) Connect all non-used pins on the test board to their
       corresponding pins on a board running InteractiveTest.

       2) Connect a serial monitor to the InteractiveTest board and
       enter "+" (a plus sign, without the quotes).

   This program pulses each unused pin in order, starting from pin 0.
   The InteractiveTest "+" command detects these pulses, and makes
   sure that no other pins change state at the same time.

   If you hold the button on the board running this program, the
   pulses run slower.

   Useful as a simple test of functionality for GPIO pins.
 */

#define INTER_TOGGLE_DELAY_NORMAL 5
#define INTER_TOGGLE_DELAY_SLOW   80

void interToggleDelay(void);

void setup() {
    pinMode(33, OUTPUT);
    pinMode(BOARD_BUTTON_PIN, INPUT);

    // All unused pins start out low.
    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

void loop() {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off

    for (int i = 0; i < BOARD_NR_GPIO_PINS; i++) {
        if (boardUsesPin(i))
            continue;

        // Bring just this pin high.
        digitalWrite(i, HIGH);
        // Give the master time to detect if any other pins also went high.
        interToggleDelay();
        // Bring this pin back low again; all pins should now be low.
        digitalWrite(i, LOW);
        // Give the master time to detect if any pins are still high.
        interToggleDelay();
    }
}

void interToggleDelay(void) {
    if (digitalRead(BOARD_BUTTON_PIN)) { // don't pay the debouncing time
        delay(INTER_TOGGLE_DELAY_SLOW);
    } else {
        delay(INTER_TOGGLE_DELAY_NORMAL);
    }
}
