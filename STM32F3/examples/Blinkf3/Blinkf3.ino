/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
  
  modified for stm32f3discovery 
  2015/02 by Frank-Michael Krause
 */


// the setup function runs once when you press reset or power the board  
void setup() {
  // initialize pins PE8, PE9 as an output.
  pinMode(PE8, OUTPUT);
  pinMode(PE9, OUTPUT);
  Serial2.begin(9600);
  Serial2.println("Hello world");

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PE8, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(PE9, LOW);    // turn the LED off by making the voltage LOW
  delay(100);                // wait for a second
  digitalWrite(PE8, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(PE9, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(300);                // wait for a second
}
