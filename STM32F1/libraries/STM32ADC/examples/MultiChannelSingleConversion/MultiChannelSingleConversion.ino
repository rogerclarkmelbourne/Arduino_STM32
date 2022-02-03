/*
  This example shows how to use the ADC library to sample several channels/pins in one go. 
  This example attaches an interrupt to the DMA completion to notify the user. 

*/
#include <STM32ADC.h>

#define BOARD_LED PC13

uint8 pins[] = {PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7};

const int maxSamples = 8; // 8 channels 

// Array for the ADC data
uint16 dataPoints[maxSamples]; 

volatile static bool dma1_ch1_Active; //flag for interrupt

STM32ADC myADC(ADC1);

//-----------------------------------------------------------------------------
//  Interrupt handler.
//-----------------------------------------------------------------------------
static void DMA1_CH1_Event()
{
	dma1_ch1_Active = 0;
}
//-----------------------------------------------------------------------------
void setup()
{
	//start Serial
	Serial.begin(19200);
	pinMode(BOARD_LED, OUTPUT);
	digitalWrite(BOARD_LED, HIGH);

	while(!Serial); delay(10);

	Serial.println("MultiChannelSingleConversion demo started\n");
	// calibrate ADC.
	myADC.calibrate(); 
	// setup ADC
	myADC.setSampleRate(ADC_SMPR_1_5); //sample ratio
	myADC.setPins(pins, maxSamples);   //pins to be converted
	myADC.setScanMode();               //Set the ADC in Scan Mode
	myADC.setDMA(dataPoints, (DMA_MINC_MODE | DMA_TRNS_CMPLT), DMA1_CH1_Event);
}
//-----------------------------------------------------------------------------
void loop()
{
// blink
	digitalWrite(BOARD_LED, HIGH); // turn LED off
	delay(1000);
	digitalWrite(BOARD_LED, LOW); // turn LED on

// start acquisition
	Serial.println("begin");
	dma1_ch1_Active = 1;

	myADC.startDMA(maxSamples);
	myADC.startConversion();

	while (dma1_ch1_Active == 1); //wait for DMA to complete.  
	// print out the sampled values
	for(unsigned int i = 0; i < maxSamples; i ++) {
		Serial.print("sample[");
		Serial.print(i);
		Serial.print("] = ");
		Serial.println(dataPoints[i]);
	}

	delay(1000);
}
