///////////////////////////////////////////////////////////////////////
//
// tone(pin,frequency[,duration])     generate a tone on a given pin
//
// noTone(pin)                        switch off the tone on the pin
//
// setToneTimerChannel(timer,channel) force use of given timer/channel
//
///////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include <HardwareTimer.h>


#define PinTimer(pin) (PIN_MAP[pin].timer_device->clk_id-RCC_TIMER1+1)
#define PinChannel(pin) (PIN_MAP[pin].timer_channel)

// if USE_PIN_TIMER is set, the PWM timer/channel is used for PWM pins
#define USE_PIN_TIMER

// if USE_BSRR is set the tone pin will be written via the fast BSRR register
// instead of using the slow digitalWrite() function in the interrupt handler
#define USE_BSRR

// construct static timer array (


#ifdef STM32_HIGH_DENSITY
// define default timer and channel
	#ifndef TONE_TIMER
	#define TONE_TIMER 8
	#endif
	#ifndef TONE_CHANNEL
	#define TONE_CHANNEL 8
	#endif

	HardwareTimer TTimer1(1), TTimer2(2), TTimer3(3), TTimer4(4),TTimer5(5), TTimer6(6), TTimer7(7), TTimer8(8);
	HardwareTimer *TTimer[8] =  { &TTimer1,&TTimer2,&TTimer3,&TTimer4,&TTimer5,&TTimer6,&TTimer7,&TTimer8 };
#else
	// define default timer and channel
	#ifndef TONE_TIMER
	#define TONE_TIMER 4
	#endif
	#ifndef TONE_CHANNEL
	#define TONE_CHANNEL 4
	#endif

	HardwareTimer TTimer1(1), TTimer2(2), TTimer3(3), TTimer4(4);
	HardwareTimer *TTimer[4] =  { &TTimer1,&TTimer2,&TTimer3,&TTimer4 };
#endif


uint8_t tone_force_channel = 0;                 // forced timer channel
uint8_t tone_force_ntimer = 0;                  // forced timer

HardwareTimer *tone_timer;// = TTimer[TONE_TIMER-1]; // timer used to generate frequency
uint8_t tone_channel = TONE_CHANNEL;            // timer channel used to generate frequency
uint8_t tone_ntimer = TONE_TIMER;               // timer used to generate frequency

bool tone_state = true;             // last pin state for toggling
short tone_pin = -1;                // pin for outputting sound
short tone_freq = 444;              // tone frequency (0=pause)
volatile uint32_t tone_nhw = 0;     // tone duration in number of half waves
uint16_t tone_tcount = 0;           // time between handler calls in 1/36 usec
uint16_t tone_ncount = 0;           // handler call between toggling
uint16_t tone_n = 0;                // remaining handler calls before toggling
uint32_t tone_next = 0;             // counter value of next interrupt

#ifdef USE_BSRR
volatile uint32_t *tone_bsrr;       // BSRR set register (lower 16 bits)
uint32_t tone_smask=0;              // BSRR set bitmask
uint32_t tone_rmask=0;              // BSRR reset bitmask
#endif
 

////////////////////////////////////////////////////////////////////////////////
// timer hander for tone with no duration specified,
// will keep going until noTone() is called
void tone_handler_1(void) {
   tone_next += tone_tcount;            // comparator value for next interrupt
   tone_timer->setCompare(tone_channel, tone_next); // and install it
   if(--tone_n == 0){
      tone_state = !tone_state;         // toggle tone output

#ifdef USE_BSRR
      if(tone_state)
         *tone_bsrr = tone_smask;
      else
         *tone_bsrr = tone_rmask;
#else
      digitalWrite(tone_pin,tone_state);// and output it
#endif

      tone_n = tone_ncount;             // reset interrupt counter
   }
}

////////////////////////////////////////////////////////////////////////////////
// timer hander for tone with a specified duration,
// will stop automatically when duration time is up.
void tone_handler_2(void) {
   tone_next += tone_tcount;
   tone_timer->setCompare(tone_channel, tone_next);
   if(--tone_n == 0){
      if(tone_freq>0){ // toggle pin
         tone_state = !tone_state;
#ifdef USE_BSRR
         if(tone_state)
            *tone_bsrr = tone_smask;
         else
            *tone_bsrr = tone_rmask;
#else
         digitalWrite(tone_pin,tone_state);// and output it
#endif
       }
       tone_n = tone_ncount;
       if(!--tone_nhw){ // check if tone duration has finished
          tone_timer->pause();       // disable timer
          pinMode(tone_pin, INPUT); // disable tone pin
       }
    }
}

////////////////////////////////////////////////////////////////////////////////
//  play a tone on given pin with given frequency and optional duration in msec
void tone(uint32_t pin, uint32_t freq, uint32_t duration) {
   tone_pin = pin;

#ifdef USE_PIN_TIMER
   // if the pin has a PWM timer/channel, use it (unless the timer/channel are forced)
   if(PinChannel(tone_pin) && !tone_force_channel){
      tone_channel = PinChannel(tone_pin);
      tone_ntimer = PinTimer(tone_pin);
   } else
#endif
   {
      // set timer and channel to default resp values forced with setToneTimerChannel
      tone_ntimer = tone_force_channel?tone_force_ntimer:TONE_TIMER;
      tone_channel = tone_force_channel?tone_force_channel:TONE_CHANNEL;
   }

   tone_timer = TTimer[tone_ntimer-1];
   tone_freq = freq;
   tone_nhw = 0;
   tone_next = 0;

   tone_timer->pause();

   if(freq > 0){
      uint32_t count = (F_CPU/4)/freq;       // timer counts per half wave
      tone_ncount = tone_n = (count>>16)+1; // number of 16-bit count chunk
      tone_tcount = count/tone_ncount;      // size of count chunk
      if(duration > 0) // number of half waves to be generated
         tone_nhw = duration*freq*2/1000;
      else  // no duration specified, continuous sound until noTone() called
         tone_nhw = 0;

      pinMode(tone_pin, PWM);    // configure output pin
      pinMode(tone_pin, OUTPUT); // configure output pin

#ifdef USE_BSRR
      // Set up BSRR register values for fast ISR
      tone_bsrr = &((PIN_MAP[tone_pin].gpio_device)->regs->BSRR);
      tone_smask = (BIT(PIN_MAP[tone_pin].gpio_bit));
      tone_rmask = tone_smask<<16;
#endif

      // Set up an interrupt on given timer and channel
      tone_next = tone_tcount; // prepare channel compare register
      tone_timer->setMode(tone_channel,TIMER_OUTPUT_COMPARE);
      tone_timer->setCompare(tone_channel,tone_next);
      // attach corresponding handler routine
      tone_timer->attachInterrupt(tone_channel,tone_nhw?tone_handler_2:tone_handler_1);

      // Refresh the tone timer
      tone_timer->refresh();

      // Start the timer counting
      tone_timer->resume();
     
   } else {

      // detach handler routine
      tone_timer->detachInterrupt(tone_channel);
      // disactive pin by configuring it as input
      pinMode(tone_pin, INPUT);

   }
   
   //while(tone_nhw) ; // blocks till duration elapsed
}

////////////////////////////////////////////////////////////////////////////////
// disable tone on specified pin, if any
void noTone(uint32_t pin){
   tone(pin,0,0);  // it's all handled in tone()
}

////////////////////////////////////////////////////////////////////////////////
// set timer and channel to some different value
// must be called before calling tone() or after noTone() was called
void setToneTimerChannel(uint8_t ntimer, uint8_t channel){
   tone_force_ntimer = ntimer;
   tone_force_channel = channel;
}
