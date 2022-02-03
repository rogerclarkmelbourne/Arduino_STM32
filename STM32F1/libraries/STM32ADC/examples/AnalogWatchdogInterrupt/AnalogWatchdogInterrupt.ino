/*
	This is an example how to use the analog watchdog functionality of the ADC.

	The analog watchdog has two limits and will generate an IRQ if
	the input voltage is below the lower limit OR higher than the upper limit.
	
	Continuous or single conversion can be used.
	In continuous mode the interrupts will be coninuously generated in very 
	short time intervals as long as the trigger condition is satisfied.
	This will cause the ADC interrupts to be nested and leads to hang-up.
	In order to avoid this, the AWD interrupt is disabled whithin the first IRQ.

*/

#include <STM32ADC.h>

STM32ADC myADC(ADC1); 

uint8 adc_pin = PA0; 

volatile static bool triggered;
// the AWD interrupt will be re-enabled 3 seconds after the last trigger
volatile uint32 trigger_time;

//-----------------------------------------------------------------------------
// This function will be called when the AWD interrupt is triggered
//-----------------------------------------------------------------------------
void awd_int()
{
	myADC.disableAnalogWatchdog(); // to avoid ISR nesting
	// a second IRQ will be triggered while we process the first ISR even before we disable the AWD
	if (triggered==true) // avoid processing second IRQ
		return;
	triggered = true;
	Serial.println("*** AWD triggered! ***");
	// Serial.print("ADC = "); Serial.println(myADC.getData());
	trigger_time = millis();
}

//-----------------------------------------------------------------------------
void setup()
{
	// Serial.begin(19200); // useless for USB serial
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH); // off

	triggered = false;

	// disable the following line if you use a COM interface different from USB serial
	while (!Serial); delay(10);

	Serial.print("Setup...");
	myADC.setPins(&adc_pin, 1);
	myADC.calibrate();
	myADC.setSampleRate(ADC_SMPR_1_5);

	// this will trigger an IRQ if the input is set to 3.3V
	myADC.setAnalogWatchdog(0, 4000, 0); // channel of PA0 = 0, high limit, low limit

	myADC.attachAnalogWatchdogInterrupt(awd_int);
	myADC.enableAnalogWatchdog();

	// in single conversion mode you need to call startConversion() regularily in the main loop
	myADC.setContinuous();
	myADC.startConversion();

	Serial.println("done.");
	Serial.println("Waiting for AWD interrupt...");
}

//-----------------------------------------------------------------------------
void loop()
{
	// blink (optional), the IRQ will be triggered in the background
	digitalWrite(LED_BUILTIN, HIGH); // off
	delay(500);
	digitalWrite(LED_BUILTIN, LOW); // on
	delay(500);

	// re-enable the AWD interrupt 3 seconds after the last trigger
	if ( (triggered==true) && (millis()-trigger_time)>3000 )
	{
		Serial.println("\nRe-enabling AWD IRQ...");
		triggered = false;
		myADC.enableAnalogWatchdog();
	}
}
