/*
 Fading:  This example shows how to fade an LED using the pwmWrite() function.

  Created 1 Nov 2008
  By David A. Mellis
  Modified 17 June 2009
  By Tom Igoe

  Modified by LeafLabs for Maple http://arduino.cc/en/Tutorial/Fading

  For differences between Maple's pwmWrite() and Arduino's analogWrite():
  http://leaflabs.com/docs/lang/api/analogwrite.html 
*/

// int ledPin = 9; // Connect an LED to digital pin 9, or any other
                // PWM-capable pin
int ledPin = 33;
void setup() {
    pinMode(ledPin, PWM);  // setup the pin as PWM
}

void loop()  {
    // Fade in from min to max in increments of 1280 points:
    for (int fadeValue = 0; fadeValue <= 65535; fadeValue += 1280) {
        // Sets the value (range from 0 to 65535):
        pwmWrite(ledPin, fadeValue);
        // Wait for 30 milliseconds to see the dimming effect:
        delay(30);
    }

    // Fade out from max to min in increments of 1280 points:
    for (int fadeValue = 65535 ; fadeValue >= 0; fadeValue -= 1280) {
        // Sets the value (range from 0 to 1280):
        pwmWrite(ledPin, fadeValue);
        // Wait for 30 milliseconds to see the dimming effect:
        delay(30);
    }
}
