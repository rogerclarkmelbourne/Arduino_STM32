/*
  Conditionals - while statement

  This example demonstrates the use of while() statements.

  While the built-in button is pressed, the sketch runs the
  calibration routine.  The sensor readings during the while loop
  define the minimum and maximum of expected values from the photo
  resistor.

  This is a variation of the Analog > Calibration example.

  The circuit:
  * Photo resistor connected from +3.3V to pin 15
  * 10K resistor connected from ground to pin 15
  * LED connected from digital pin 9 to ground through 220 ohm resistor
  * 10K resistor attached from pin 2 to ground

  created 17 Jan 2009
  modified 25 Jun 2009
  by Tom Igoe
  modified for Maple 13 February 2011
  by LeafLabs

  http://leaflabs.com/docs/lang/cpp/while.html
*/

// These constants won't change:
const int sensorPin = 2;     // pin that the sensor is attached to
const int ledPin = 9;        // pin that the LED is attached to

// These variables will change:
int sensorMin = 4095;  // minimum sensor value
int sensorMax = 0;     // maximum sensor value
int sensorValue = 0;   // the sensor value

void setup() {
    // set the LED pins as outputs and the switch pin as input:
    pinMode(ledPin, OUTPUT);          // LED on pin 9
    pinMode(33, OUTPUT);   // Built-in LED
    pinMode(BOARD_BUTTON_PIN, INPUT); // Built-in button
}

void loop() {
    // while the button is pressed, take calibration readings:
    while (digitalRead(BOARD_BUTTON_PIN) == HIGH) {
        // You could also use this:
        //while (isButtonPressed()) {
        calibrate();
    }
    // signal the end of the calibration period
    digitalWrite(33, LOW);

    // read the sensor:
    sensorValue = analogRead(sensorPin);

    // apply the calibration to the sensor reading
    sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 65535);

    // in case the sensor value is outside the range seen during calibration
    sensorValue = constrain(sensorValue, 0, 65535);

    // fade the LED using the calibrated value:
    pwmWrite(ledPin, sensorValue);
}

void calibrate() {
    // turn on the built-in LED to indicate that calibration is happening:
    digitalWrite(33, HIGH);
    // read the sensor:
    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
        sensorMax = sensorValue;
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
        sensorMin = sensorValue;
    }
}
