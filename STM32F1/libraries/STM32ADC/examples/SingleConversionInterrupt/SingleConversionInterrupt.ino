#include <STM32ADC.h>

STM32ADC myADC(ADC1); 

uint8 adc_pin = PA0;

volatile static bool triggered; 
volatile uint32 adc_value; 

//-----------------------------------------------------------------------------
// This function will be called after the conversion is done
//-----------------------------------------------------------------------------
void adc_int()
{
	adc_value = myADC.getData();
	triggered = 1;
}

//-----------------------------------------------------------------------------
void setup()
{
	// Serial.begin(19200); // useless for USB serial
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH); // LED off

	while (!Serial); delay(10);

	Serial.print("Setup...");

	myADC.setPins(&adc_pin, 1);
	myADC.calibrate();
	myADC.setSampleRate(ADC_SMPR_1_5);
	myADC.attachInterrupt(adc_int);

	Serial.println("done.");
}

//-----------------------------------------------------------------------------
void loop()
{
	// Take our samples
	triggered = 0;
	myADC.startConversion();

	while (triggered == 0); // wait here till conversion is finished

	Serial.print("Readin = ");
	Serial.println(adc_value);

	digitalWrite(LED_BUILTIN, LOW); // on
	delay(500);
	digitalWrite(LED_BUILTIN, HIGH); // off
	delay(500);
}
