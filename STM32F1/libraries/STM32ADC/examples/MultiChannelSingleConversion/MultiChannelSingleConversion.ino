/*
  This example shows how to use the ADC library to sample several
  channels/pins in one go. 
  This example attaches an interrupt to the DMA completion to notify the user. 
  The DMA must be set together with the start conversion, so both setDMA and startConversion
  Must be called. 

*/
#include <STM32ADC.h>

#define BOARD_LED D33 //PB0

uint8 pins[] = {11,10,9,8,7,6,5,4};

const int maxSamples = 8; // 8 channels 

// Array for the ADC data
uint16 dataPoints[maxSamples]; 

volatile static bool dma1_ch1_Active; //flag for interrupt

STM32ADC myADC(ADC1);

void setup() {
//start Serial
  Serial.begin(19200);
//Start up blink from Pig-O-Scope
  pinMode(BOARD_LED, OUTPUT);
  pinMode(D32, INPUT);
  digitalWrite(BOARD_LED, HIGH);
  delay(1000);
  digitalWrite(BOARD_LED, LOW);
  delay(1000);
//calibrate ADC.
  myADC.calibrate(); 
  
  myADC.setSampleRate(ADC_SMPR_1_5); //sample ratio
  myADC.setPins(pins, maxSamples);   //pins to be converted
  myADC.setScanMode();               //Set the ADC in Scan Mode
}

void loop(){
//start acquisition on button push. 
    if(digitalRead(D32) == 1 ) {
        Serial.println("begin");
        dma1_ch1_Active = 1;
        myADC.setDMA(dataPoints, 8, (DMA_MINC_MODE | DMA_TRNS_CMPLT), DMA1_CH1_Event); 
        myADC.startConversion();
        while (dma1_ch1_Active == 1); //wait for DMA to complete.  
        
        for(unsigned int i = 0; i < maxSamples; i ++) {
            Serial.print("sample[");
            Serial.print(i);
            Serial.print("] = ");
            Serial.println(dataPoints[i]);
        }
      while(digitalRead(D32) == 1); //stay here. Another button push is required. 
      }
}; //end loop

/*
    Interrupt handler. eh eh 
*/
static void DMA1_CH1_Event() {
  dma1_ch1_Active = 0;
}
