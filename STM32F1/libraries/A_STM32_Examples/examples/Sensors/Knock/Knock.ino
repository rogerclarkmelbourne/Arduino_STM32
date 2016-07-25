/*
  Knock Sensor

  This sketch reads a piezo element to detect a knocking sound.  It
  reads an analog pin and compares the result to a set threshold.  If
  the result is greater than the threshold, it writes "knock" to the
  serial port, and toggles the LED on pin 13.

  The circuit:
  * + connection of the piezo attached to analog in 0
  * - connection of the piezo attached to ground
  * 1-megohm resistor attached from analog in 0 to ground

  http://www.arduino.cc/en/Tutorial/Knock

  created 25 Mar 2007
  by David Cuartielles <http://www.0j0.org>
  modified 30 Jun 2009
  by Tom Igoe

  Ported to the Maple
  by LeafLabs
*/

// these constants won't change:
const int knockSensor = 0;  // the piezo is connected to analog pin 0
const int threshold = 100;  // threshold value to decide when the detected sound is a knock or not

// these variables will change:
int sensorReading = 0;      // variable to store the value read from the sensor pin

void setup() {
	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Declare the knockSensor as an analog input:
    pinMode(knockSensor, INPUT_ANALOG);
    // declare the built-in LED pin as an output:
    pinMode(33, OUTPUT);
}

void loop() {
    // read the sensor and store it in the variable sensorReading:
    sensorReading = analogRead(knockSensor);

    // if the sensor reading is greater than the threshold:
    if (sensorReading >= threshold) {
        // toggle the built-in LED
        digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
        // send the string "Knock!" back to the computer, followed by newline
        Serial.println("Knock!");
    }
    delay(100);  // delay to avoid printing too often
}
