/*
 * Example of the Timer Input Capture mode combined with one pulse mode
 *  
 * This example uses:
 * - Timer2 channel 1 as capture input
 * - Timer2 channel 2 to generate the pulses,
 * - Timer 3 to generate a PWM trigger signal for capture input
 */

#include <Streaming.h>

const uint16_t pulseDelay = 200;
const uint16_t pulseWidth = 200;
//-----------------------------------------------------------------------------
void toggle_led()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//-----------------------------------------------------------------------------
void setup()
{
	// setup PA1 (Timer2 channel 2) to PWM (one pulse mode)
	pinMode(PA1, PWM);
	// setup PA0 (Timer 2 channel 1) as input (capture input mode)
	pinMode(PA0, INPUT);

	// stop the timers before configuring them
	Timer2.pause();

	Timer2.setPrescaleFactor(72); // 1 µs resolution
	Timer2.setCompare(TIMER_CH2, pulseDelay);
	Timer2.setOverflow(pulseWidth + pulseDelay-1);

	// counter setup in one pulse mode, as slave triggered by TI1
	TIMER2_BASE->CR1  = ( TIMER_CR1_OPM ); // one pulse mode
	Timer2.setSlaveFlags( TIMER_SMCR_TS_TI1FP1 | TIMER_SMCR_SMS_TRIGGER );

	// channel 1: capture input on rising edge
	Timer2.setMode(TIMER_CH1, TIMER_INPUT_CAPTURE);
	//Timer2.setPolarity(TIMER_CH1, 1); // trigger on falling edge
	// channel 2: invert polarity (we want low for CNT<CCR2)
	Timer2.setPolarity(TIMER_CH2, 1);

	// start timer 2
	Timer2.refresh();
	Timer2.resume(); // let timer 2 run

	// setup PA6 (Timer3 channel 1) to generate 1 ms period PWM with 10% DC
	pinMode(PA6, PWM);
	Timer3.pause();
	Timer3.setPrescaleFactor(72); // 1 µs resolution
	Timer3.setCompare(TIMER_CH1, 100);
	Timer3.setOverflow(1000);
	Timer3.refresh();
	Timer3.resume(); // let timer 3 run
}

uint32_t t;
//-----------------------------------------------------------------------------
void loop()
{
	if ( (millis()-t)>=1000 )
	{
		t = millis();
		toggle_led();
	}
}
