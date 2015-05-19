/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin
  0 and turning on and off the Maple's built-in light emitting diode
  (LED).  The amount of time the LED will be on and off depends on the
  value obtained by analogRead().

  Created by David Cuartielles
  Modified 16 Jun 2009
  By Tom Igoe

  http://leaflabs.com/docs/adc.html

  Ported to Maple 27 May 2010
  by Bryan Newbold
*/

int sensorPin = 0;   // Select the input pin for the potentiometer
int sensorValue = 0; // Variable to store the value coming from the sensor

void setup() {
    // Declare the sensorPin as INPUT_ANALOG:
    pinMode(sensorPin, INPUT_ANALOG);
    // Declare the LED's pin as an OUTPUT.  (33 is a built-in
    // constant which is the pin number of the built-in LED.  On the
    // Maple, it is 13.)
    pinMode(33, OUTPUT);
}

void loop() {
    // Read the value from the sensor:
    sensorValue = analogRead(sensorPin);
    // Turn the LED pin on:
    digitalWrite(33, HIGH);
    // Stop the program for <sensorValue> milliseconds:
    delay(sensorValue);
    // Turn the LED pin off:
    digitalWrite(33, LOW);
    // Stop the program for for <sensorValue> milliseconds:
    delay(sensorValue);
}
