/*
 * Example of the Timer Input Capture mode to measure PWM pulse width and period.
 *  
 * This example uses:
 * - Timer2 channel 1 as capture input
 * - Timer3 to generate a PWM trigger signal for capture input
 */

#include <Streaming.h>

const uint8_t pwmPulse = 20; // ms
const uint8_t pwmPeriod = 50; // ms
//-----------------------------------------------------------------------------
void toggle_led()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
//-----------------------------------------------------------------------------
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	// setup PA0 (Timer 2 channel 1) as input capture mode
	pinMode(PA0, INPUT);

	while (!Serial); delay(1000);

	Serial << "Example of the Timer Input Capture mode to measure PWM pulse width and period\n\n";

	// stop the timers before configuring them
	Timer2.pause();
	Timer3.pause();

	Timer2.setPrescaleFactor(72); // 1 microsecond resolution

	// setup timer 2 channel 1 capture on rising edge
	Timer2.setInputCaptureMode(TIMER_CH1, TIMER_IC_INPUT_DEFAULT); // use default input TI1
	// setup timer 2 channel 2 capture on falling edge
	Timer2.setInputCaptureMode(TIMER_CH2, TIMER_IC_INPUT_SWITCH); // use switched input TI1
	Timer2.setPolarity(TIMER_CH2, 1); // trigger on falling edge

	// counter setup as slave triggered by TI1 in reset mode
	Timer2.setSlaveFlags( TIMER_SMCR_TS_TI1FP1 | TIMER_SMCR_SMS_RESET );

	Timer2.refresh();
	Timer2.getCompare(TIMER_CH1); // clear capture flag
	Timer2.getCompare(TIMER_CH2); // clear capture flag
	Timer2.resume(); // let timer 2 run

	// setup PA6 (Timer3 channel 1) to generate 1 ms period PWM with 10% DC
	pinMode(PA6, PWM);
	Timer3.setPrescaleFactor(72); // 1 µs resolution
	Timer3.setCompare(TIMER_CH1, pwmPulse*1000);
	Timer3.setOverflow(pwmPeriod*1000);
	Timer3.refresh();

	Timer3.resume(); // let timer 3 run

	// discard first reading
	// wait for first period end
	while ( !Timer2.getInputCaptureFlag(TIMER_CH1) );
	Timer2.getCompare(TIMER_CH1); // clear capture flag
}
//-----------------------------------------------------------------------------
void loop()
{
	if ( Timer2.getInputCaptureFlag(TIMER_CH2) ) // high pulse end
	{
		Serial << "PWM pulse width: " << Timer2.getCompare(TIMER_CH2);
	}
	if ( Timer2.getInputCaptureFlag(TIMER_CH1) ) // period end
	{
		Serial << ", period: " << Timer2.getCompare(TIMER_CH1) << endl;
		toggle_led();
		//delay(500); // read the values only 2 times per second
	}
}
