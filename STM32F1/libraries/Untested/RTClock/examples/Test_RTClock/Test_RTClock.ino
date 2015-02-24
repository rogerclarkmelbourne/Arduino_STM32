#include <RTClock.h>
//#include <time.h>

RTClock rt (RTCSEL_LSE); 
uint32 tt; 
//time_t timer;

void blink () {
 toggleLED(); 

}

void setup() {
pinMode(BOARD_LED_PIN, OUTPUT);

rt.attachSecondsInterrupt(blink);
}



void loop() {
  tt = rt.getTime();
  
  SerialUSB.print("time is: ");
  SerialUSB.println(tt);
//  toggleLED();
}
