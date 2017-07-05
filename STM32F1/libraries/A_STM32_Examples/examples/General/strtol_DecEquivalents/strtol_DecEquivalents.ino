/*
  Modified for Arduino from: http://www.cplusplus.com/reference/cstdlib/strtol/
  Convert string to long integer: Maple Mini version by m. ray burnette: PUBLIC DOMAIN
  Sketch uses 13,924 bytes (12%) of program storage space. Maximum is 108,000 bytes.
  Global variables use 2,664 bytes of dynamic memory.
  Following C++ libs not needed after Arduino 1.0.2
      #include <stdio.h>
      #include <stdlib.h>
*/

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

  char szNumbers[] = "2001 60c0c0 -1101110100110100100000 0x6fffff";
  char * pEnd;
  long int li1, li2, li3, li4;

void setup() {
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(9600);
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  li1 = strtol (szNumbers,&pEnd,10);  // BASE 10
  li2 = strtol (pEnd,&pEnd,16);       // HEX
  li3 = strtol (pEnd,&pEnd,2);        // Binary
  li4 = strtol (pEnd,NULL,0);         // Integer constant with prefixed base Octal or Hex
  // Serial.print ("The decimal equivalents are: %ld, %ld, %ld and %ld.\n", li1, li2, li3, li4);
  Serial << "The decimal equivalents are: " << li1 << " " << li2 << " " << li3 << " " << li4;
  //return 0;
}



void loop() {
  // put your main code here, to run repeatedly: 
  
}
