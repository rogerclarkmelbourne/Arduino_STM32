/*
  Analog input example.

  Uasge:
  - define an array of analog input pins according to the wanted channels.
    Possible values:
    ----------------
    ADC_CH0  = PA0
    ADC_CH1  = PA1
    ADC_CH2  = PA2 // PF0
    ADC_CH3  = PA3
    ADC_CH4  = PA4 // PF6
    ADC_CH5  = PA5 // PF7
    ADC_CH6  = PA6 // PF8
    ADC_CH7  = PA7 // PF9
    ADC_CH8  = PB0 // PF10
    ADC_CH9  = PB1 // PF3
    ADC_CH10 = PC0
    ADC_CH11 = PC1
    ADC_CH12 = PC2
    ADC_CH13 = PC3
    ADC_CH14 = PC4  // PF4
    ADC_CH15 = PC5  // PF5
    ADC_CH16 = temperature sensor (intern)
    ADC_CH17 = Vref (intern)
    ADC_CH18 = Vbat (intern)

  - when using channels 16/17 you need to call: myADC.enableTsVref();
  - when using channel 18 you need to call: myADC.enableVbat()
  - specify the number of inputs to be sampled
  - select the sampling mode (single, continuous, scan, ...)
  - start acquisition.

*/
#include <STM32F4ADC.h>

STM32ADC myADC(ADC1);

// Array of analog input pins
uint8_t analog_pins[] = { PA0, PA1, PA2, PA3, PA4, PA5 };

//-----------------------------------------------------------------------------
void blink()
{
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

//-----------------------------------------------------------------------------
void setup()
{
    // Declare the LED_BUILTIN's pin as an OUTPUT.
    pinMode(LED_BUILTIN, OUTPUT);
    // Declare the sensorPin as INPUT_ANALOG:
	for (uint8_t i = 0; i<sizeof(analog_pins); i++)
		pinMode(analog_pins[i], INPUT_ANALOG);

	Serial.begin(115200);
	while ( !Serial ); delay(10);

	Serial.println("*** Simple analog read demo ***\n");
	
	//myADC.enableVBAT(); // needed if reading Vbat, channel 18
	myADC.enableTsVref(); // needed if reading internal temperature sensor
	myADC.setSamplingTime(ADC_SMPR_480); // 3µsec, needed for temp sensor
}

//-----------------------------------------------------------------------------
char dbg[150]; // for sprintf
//-----------------------------------------------------------------------------
void loop()
{
    // Read internal values:
	//Serial.print("Vref: "), Serial.println(myADC.readVref());
	Serial.print(millis()); Serial.print(": Temp = ");
	float val = myADC.readTemp();
	Serial.print(val); Serial.write(',');
    // Read the voltages form analog input pins:
	for (uint8_t i=0; i<sizeof(analog_pins); i++)
	{
		sprintf(dbg, " in%u = %4u,", i, myADC.readInput(analog_pins[i]));
		Serial.print(dbg);
	}
	Serial.write('\n'); // new line
    blink();
    delay(100);
}
