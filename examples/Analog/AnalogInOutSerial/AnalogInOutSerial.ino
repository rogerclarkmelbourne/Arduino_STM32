/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to
  65535 and uses the result to set the pulse width modulation (PWM) of
  an output pin.  Also prints the results to the serial monitor.

  (You may need to change the pin numbers analogInPin and analogOutPin
  if you're not using a Maple).

  The circuit:
  * Potentiometer connected to analog pin 15.
    Center pin of the potentiometer goes to the analog pin.
    Side pins of the potentiometer go to +3.3V and ground.
  * LED connected from digital pin 9 to ground

  created 29 Dec. 2008
  by Tom Igoe

  ported to Maple
  by LeafLabs
*/

// These constants won't change.  They're used to give names
// to the pins used:

const int analogInPin = 15; // Analog input pin that the potentiometer
                            // is attached to

const int pwmOutPin = 9;    // PWM pin that the LED is attached to

// These variables will change:
int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM

void setup() {
    // Configure the ADC pin
    pinMode(analogInPin, INPUT_ANALOG);
    // Configure LED pin
    pinMode(pwmOutPin, PWM);
	Serial.begin(115200); // Ignored by Maple. But needed by boards using Hardware serial via a USB to Serial Adaptor
}

void loop() {
    // read the analog in value:
    sensorValue = analogRead(analogInPin);
    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 1023, 0, 65535);
    // change the analog out value:
    pwmWrite(pwmOutPin, outputValue);

    // print the results to the serial monitor:
    Serial.print("sensor = " );
    Serial.print(sensorValue);
    Serial.print("\t output = ");
    Serial.println(outputValue);
}
