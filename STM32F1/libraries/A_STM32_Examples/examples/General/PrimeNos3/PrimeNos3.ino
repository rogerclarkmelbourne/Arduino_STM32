/*
  PrimeNos3: by Nick Gammon
  Maple Mini port m. ray burnette: Compiled under Arduino 1.6.0rc1
    Sketch uses 13,420 bytes (12%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 2,600 bytes of dynamic memory.
  PUBLIC DOMAIN EXAMPLE
*/

#define BAUD 9600
const int SHOW_EVERY = 500;  // how often to echo a prime to the serial port
int candidate;
int found = 5; // Number we found
int count = found - 1;


void setup() {
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(BAUD);  // BAUD has no effect on USB serial: placeholder for physical UART
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  Serial.println("Prime Number Generator V2");
}

void loop() {
    Serial.println("Prime numbers between 1 and 99999999 are:");
    Serial.println("2 \t");
    for (int i=3; i<99999999;i+=2) {
        // This loop stops either when j*j>i or when i is divisible by j.
        // The first condition means prime, the second, not prime.
        int j=3;
        for(;j*j<=i && i%j!=0; j+=2); // No loop body

        if (j*j>i) Serial.print(i);Serial.print( "\n\r");
    }
    Serial.println("\r\n");
}
