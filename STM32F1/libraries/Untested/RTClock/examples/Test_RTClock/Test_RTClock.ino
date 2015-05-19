#include <RTClock.h>
//#include <time.h>

RTClock rt (RTCSEL_LSE); 
uint32 tt; 
//time_t timer;

void blink () {
 toggleLED(); 

}

void setup() {
pinMode(PB1, OUTPUT);// Roger Clark. Updated for removal of LED pin as a global define (PB1 is Maple mini LED Pin)

rt.attachSecondsInterrupt(blink);
}



void loop() {
  tt = rt.getTime();
  
  SerialUSB.print("time is: ");
  SerialUSB.println(tt);
//  toggleLED();
}
