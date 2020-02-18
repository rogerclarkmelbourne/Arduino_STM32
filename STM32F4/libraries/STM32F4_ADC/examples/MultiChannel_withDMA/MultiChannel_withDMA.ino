/*
  Multiple channel analog input stored in circular buffer example.
  The sampling rate is given by timer 3 update frequency.
  At each timer overflow the DMA will store the ADC values to buffer.
  Data is output to serial as soon as a half (upper or lower) buffer is full.

  Usage:
  - define an array of analog input pins according to the wanted channels.
    Possible values:
    ----------------
    ADC_CH0  = PA0
    ...
    ADC_CH7  = PA7
    ADC_CH8  = PB0
    ADC_CH9  = PB1
    ADC_CH10 = PC0
    ...
    ADC_CH15 = PC5
    ADC_CH16 = temperature sensor
    ADC_CH17 = Vref
    ADC_CH18 = Vbat

  - when using channels 16/17 you need to call: myADC.enableTsVref();
  - when using channel 18 you need to call: myADC.enableVbat()
  - specify the number of inputs to be sampled
  - select the sampling mode (single, continuous, scan, ...)
  - start acquisition.

*/

#include <STM32F4ADC.h>


#define ADC_CHANNELS 4 // Channels to be sampled.

const uint8 adcPins[ADC_CHANNELS] = {PA0, PA1, PA2, PA3};

#define maxSamples  200
#define BUFF_SIZE (maxSamples*ADC_CHANNELS)

uint16_t buffer[BUFF_SIZE]; // stores the values sampled by ADC
                            // make it uint32_t for dual ADC.

uint16_t * volatile readPtr; // pointer from where to start read out the values

#define sampleFreqHz     1000                      // !!! minimum = 20 Hz !!!
#define samplePeriod    ( 1000000 / sampleFreqHz ) // µs resolution

STM32ADC myADC(ADC1);

#define ADC_DEVICE_SINGLE 0 // use this if only one device is needed
#define ADC_DEVICE_DUAL   1 // use this to activate both ADC1 and 2
                            // The DMA will store 32 bits per sample.

#define ADC_CONVERSION_MODE_SINGLE   0 // use this for a single conversion sequence
                                       // ADC stops after the buffer is full.
#define ADC_CONVERSION_MODE_CIRCULAR 1 // use this for continuous conversion
                                       // ADC overwrites the buffer in circular mode.

uint8_t secondPart;
uint8_t adcConversionMode;
//-----------------------------------------------------------------------------
void blink(uint16_t dly)
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	if (dly) delay(dly);
}
//-----------------------------------------------------------------------------
void Timer_setup(void)
{
	Serial.println("Setup Timer3");
	// setup to generate pulses for sampling
	Timer3.pause(); // stop timer
	Timer3.setPeriod(samplePeriod);
	Timer3.setMasterMode(TIMER_MASTER_MODE_UPDATE); // update will be used as TRGO
	Timer3.refresh(); // will trigger the first sample
	Timer3.resume();
}
//-----------------------------------------------------------------------------
void DMA_setup(void)
{
	Serial.println("Configure DMA");
	//myADC.setSampleTime(ADC_SMPR_144); // same as default setting = 1µsecond.
	myADC.setPins(adcPins, ADC_CHANNELS);
	myADC.setDMA(buffer, (maxSamples*ADC_CHANNELS),
				 ADC_DEVICE_SINGLE, adcConversionMode,
				 (DMA_FROM_PER | DMA_MINC_MODE | DMA_CIRC_MODE | DMA_TRNS_HALF | DMA_TRNS_CMPLT),
				 DMA_ISR);
	myADC.setTrigger(ADC_EXTSEL_TIM3_TRGO);
	myADC.startConversion();
}
//-----------------------------------------------------------------------------
void DMA_ISR(void)
{
	if ( myADC.getDMAHalfFlag() )
	{
		readPtr = &buffer[0];
		secondPart = 0;
	}
	else if ( myADC.getDMACompleteFlag() )
	{
		readPtr = &buffer[maxSamples/2];
		secondPart = 1;
	}
	myADC.clearDMAIsrBits(); // mandatory
}
//-----------------------------------------------------------------------------
void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	for (uint8_t i=0; i<ADC_CHANNELS; i++)
		pinMode(adcPins[i], INPUT_ANALOG);

	readPtr = NULL;
	secondPart = 0;
	// set here the conversion mode.
	// possible values: ADC_CONVERSION_MODE_SINGLE or ADC_CONVERSION_MODE_CIRCULAR
	adcConversionMode = ADC_CONVERSION_MODE_CIRCULAR;

	Serial.begin(115200);
	while (!Serial); delay(10);

	Serial.println("START multiple channel at sample rate circular buffer example");

	DMA_setup();
	Timer_setup();

	Serial.println("Conversion started.\n");
}

//-----------------------------------------------------------------------------
void loop()
{
	if ( readPtr!=NULL )
	{
		uint16_t * p = readPtr;
		readPtr = NULL; // clear it here to detect overflow
		// process the first or second half of the buffer
		for (uint16_t i=0; i<(maxSamples/2); i++ )
		{
			Serial.write('x'); Serial.print((secondPart*(maxSamples/2))+i); Serial.print(" =");
			Serial.write(' '); Serial.print(*p++); // channel 1
			Serial.write(' '); Serial.print(*p++); // channel 2
			Serial.write(' '); Serial.print(*p++); // channel 3
			Serial.write(' '); Serial.print(*p++); // channel 4
			Serial.write('\n');
		}
		if ( secondPart ) Serial.write('\n');
		blink(0);
		// detect overflow: pointer updated before data processing is finished
		if ( readPtr!=NULL )
		{
			Serial.println("*** Overflow ***");
			while(1); // halt
		}
		if ( secondPart && (adcConversionMode == ADC_CONVERSION_MODE_SINGLE) )
			Serial.println("Conversion finished.");
	}
}
