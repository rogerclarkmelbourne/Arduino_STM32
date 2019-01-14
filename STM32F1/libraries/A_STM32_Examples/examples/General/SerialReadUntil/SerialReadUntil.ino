
// http://arduino.cc/forum/index.php?topic=114035.0  `
/* Sketch uses 13,836 bytes (12%) of program storage space. Maximum is 108,000 bytes.
   Global variables use 3,696 bytes of dynamic memory.
   Read an unknown length string of ASCII characters terminated 
   with a line feed from the UART
*/

#define BAUD 9600

void setup() {
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(BAUD);  // BAUD has no effect on USB serial: placeholder for physical UAR
    // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  Serial.println("Serial Read Until Example:");
  Serial.print("Type a few characters & press ENTER\r\n(make certain serial monitor sends CR+LF)");
}

void loop() {
  char serialdata[80];
  int lf = 10;
 
  Serial.readBytesUntil(lf, serialdata, 80);

  Serial.println(serialdata);

}


