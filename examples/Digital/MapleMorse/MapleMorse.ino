/*
 Blink

 Turns on the built-in LED on for one second, then off for one second,
 repeatedly.

 Ported to Maple from the Arduino example 27 May 2011
 By Marti Bolivar
 
 Updated for to remove definition for BOARD_LED_PIN by Roger Clark www.rogerclark.net 25th April 2015
 
 Note. This code only works on the Maple mini and other boards where the LED is on GPIO PB1 aka pin 33 on the Maple mini silk screen
 
 I could not resist making this a bit more interesting than the simple blinking on and off, as I expect most users of this hardware will understand how 
 
 
*/
#define LED_PIN PB1 
#define TIMECONSTANT 100

void setup() {
    // Set up the built-in LED pin as an output:
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    sendMorse("-- .- .--. .-.. .   ");
}

void sendMorse(char *dotsAndDashes)
{
  while(*dotsAndDashes!=0)
  {
    switch(*dotsAndDashes)
    {
      case '-':
        digitalWrite(LED_PIN,HIGH);
        delay(TIMECONSTANT*3);
        digitalWrite(LED_PIN,LOW);
        delay(TIMECONSTANT);
        break;
      case '.':
        digitalWrite(LED_PIN,HIGH);
        delay(TIMECONSTANT);
        digitalWrite(LED_PIN,LOW);
        delay(TIMECONSTANT);
        break;   
      case ' ':   
      default:
        digitalWrite(LED_PIN,LOW);
        delay(TIMECONSTANT*3);
        break;   
    }
    dotsAndDashes++;
  }
}
