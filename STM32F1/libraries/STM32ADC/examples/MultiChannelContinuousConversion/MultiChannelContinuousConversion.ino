/*
  This example shows how to use the ADC library to continuously sample
  several channels/pins. 
  The acquisition of the channels is done using DMA in circular mode. 
  
*/
#include <STM32ADC.h>


STM32ADC myADC(ADC1);

#define BOARD_LED D33 //this is for Maple Mini

//Channels to be acquired. 
uint8 pins[] = {11,10,9,8,7,6,5,4};


const int maxSamples = 8; // 8 channels 

// Array for the ADC data
uint16_t dataPoints[maxSamples];

void setup() {
  Serial.begin(19200);
  pinMode(BOARD_LED, OUTPUT);
  pinMode(D32, INPUT);
//startup blink... good idea from Pig-O-Scope
  digitalWrite(BOARD_LED, HIGH);
  delay(1000);
  digitalWrite(BOARD_LED, LOW);
  delay(1000);

//calibrate ADC
  myADC.calibrate();

  // Set up our analog pin(s)
  for (unsigned int j = 0; j <8; j++) 
    pinMode(pins[j], INPUT_ANALOG);
    
  myADC.setSampleRate(ADC_SMPR_1_5);//set the Sample Rate
  myADC.setScanMode();              //set the ADC in Scan mode. 
  myADC.setPins(pins, 8);           //set how many and which pins to convert.
  myADC.setContinuous();            //set the ADC in continuous mode.

//set the DMA transfer for the ADC. 
//in this case we want to increment the memory side and run it in circular mode
//By doing this, we can read the last value sampled from the channels by reading the dataPoints array
  myADC.setDMA(dataPoints, 8, (DMA_MINC_MODE | DMA_CIRC_MODE), NULL);

//start the conversion. 
//because the ADC is set as continuous mode and in circular fashion, this can be done 
//on setup().  
  myADC.startConversion();   

  
}

void loop(){
//send the latest data acquired when the button is pushed. 
      if(digitalRead(D32) == 1 ) {
      Serial.println("begin");
      // Take our samples

      for(unsigned int i = 0; i < maxSamples; i ++) {
        Serial.print("sample[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(dataPoints[i]);
        }
      while(digitalRead(D32) == 1); //stay here.  
      }
}; //end loop

