/*
 * Hardware Timer as an Encoder interface. 
 * 
 * The STM32 Timers have the possibility of being used as an encoder interface. 
 * This can be both a quadrature encoder (mode 3) or pulse encoder with a signal to give direction (modes 1 and 2). 
 * The default behavior is for quadrature encoder. 
 * 
 * To avoid overflowing the encoder (which may or may not happen (although with 16 bits, it's likely), the following code 
 * will interrupt every time the number of pulses that each revolution gives to increment/decrement a variable (ints). 
 * 
 * This means that the total number of pulses given by the encoder will be (ints * PPR) + timer.getCount()
 * 
 * Attached is also a bit of code to simulate a quadrature encoder. 
 * To test this library, make the connections as below: 
 * 
 * TIMER2 inputs -> Digital Pins used to simulate.
 * D2 -> D4
 * D3 -> D5
 * 
 * COUNTING DIRECTION: 
 * 0 means that it is upcounting, meaning that Channel A is leading Channel B
 * 
 * EDGE COUNTING: 
 * 
 * mode 1 - only counts pulses on channel B
 * mode 2 - only counts pulses on Channel A
 * mode 3 - counts on both channels. 
 * 
*/



#include "HardwareTimer.h"

//Encoder simulation stuff
//NOT NEEDED WHEN CONNECTING A REAL ENCODER
unsigned char mode = 0;  //to issue steps...
unsigned char dir = 'F'; // direction of movement of the encoder.

unsigned int freq = 100; //update frequency.
unsigned long time = 0;        //time variable for millis()
unsigned char states[4];       //because I'm lazy...


//Encoder stuff

//Pulses per revolution
#define PPR   1024

HardwareTimer timer(2);

unsigned long ints = 0;

void func(){
if (timer.getDirection()){
  ints--;
} else{
  ints++;
}
}


void setup() {
  //define the Timer channels as inputs. 
  pinMode(D2, INPUT_PULLUP);  //channel A
  pinMode(D3, INPUT_PULLUP);  //channel B

//configure timer as encoder
  timer.setMode(0, TIMER_ENCODER); //set mode, the channel is not used when in this mode. 
  timer.pause(); //stop... 
  timer.setPrescaleFactor(1); //normal for encoder to have the lowest or no prescaler. 
  timer.setOverflow(PPR);    //use this to match the number of pulse per revolution of the encoder. Most industrial use 1024 single channel steps. 
  timer.setCount(0);          //reset the counter. 
  timer.setEdgeCounting(TIMER_SMCR_SMS_ENCODER3); //or TIMER_SMCR_SMS_ENCODER1 or TIMER_SMCR_SMS_ENCODER2. This uses both channels to count and ascertain direction. 
  timer.attachInterrupt(0, func); //channel doesn't mean much here either.  
  timer.resume();                 //start the encoder... 
  
//Setup encoder simulator  
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  digitalWrite(5, HIGH); 
  digitalWrite(4, LOW);
  states[0] = 0; //00
  states[1] = 1; //01
  states[2] = 3; //11
  states[3] = 2; //10


}

//Support variables.
unsigned long interval=0; //variable for status updates... 
char received = 0;

void loop() {
  //encoder code
  if (millis() - interval >= 1000) { 
     Serial.print(timer.getCount()); 
     Serial.println(" counts");
     Serial.print("direction ");
     Serial.println(timer.getDirection());
     Serial.print("Full Revs: ");
     Serial.println(ints);
     interval = millis(); //update interval for user. 
  }
  
  
  
/*
EENCODER SIMULATION PART. 


*/  
/*
 * 
 * Protocol... 
 * 
 * if received F - Move forward. 
 * if received B - Move BackWard
 * if received 1 - Mode 1 (Channel B counts)
 * if received 2 - Mode 2 (Channel A counts)
 * if received 3 - Mode 3 (Counts on both channels)
 * if received 4 - Change prescaler to 4
 * if received 0 - change prescaler to 1
 * if received - - Increase Speed
 * if received + - Decrease Speed
*/

//take care of comms...
  if (Serial.available() > 0) {
    received = Serial.read();
    if (received == 'F' || received == 'R') dir = received; //direction. Forward or Reverse.
    if (received == '1') timer.setEdgeCounting(TIMER_SMCR_SMS_ENCODER1); //count only the pulses from input 1
    if (received == '2') timer.setEdgeCounting(TIMER_SMCR_SMS_ENCODER2); //count only the pulses from input 2
    if (received == '3') timer.setEdgeCounting(TIMER_SMCR_SMS_ENCODER3); //count on both channels (default of the lib).
    if (received == '4') timer.setPrescaleFactor(4); //only updates on overflow, so you need to wait for an overflow. Not really used...  
    if (received == '0') timer.setPrescaleFactor(1); //only updates on overflow, so you need to wait for an overflow. 
    if (received == '-') freq+=50; //increase speed.
    if (received == '+') {
      freq-=50;
      if (freq <= 0) freq = 100; //smallest is 10 ms. 
    }
  }
  
//simulate encoder pulses.   
  if ( millis() - time >= freq) { 
    time = millis(); //prepare next
    
    if (dir == 'F')  mode++;
    if (dir == 'R')  mode --;
    
    digitalWrite(4, (states[mode%4] & 0x01));
    digitalWrite(5, (states[mode%4] >> 1));
  }

}
