#include <STM32ADC.h>

STM32ADC myADC(ADC1); 

uint8 pin = D7; 

#define BOARD_LED D33 //this is for Maple Mini
volatile static bool triggered = 0; 
uint32 dado_adc = 0; 


void int_func() {
  
  triggered = 1; 
  dado_adc = myADC.getData();
  }



void setup() {
  Serial.begin(19200); 
  pinMode(BOARD_LED, OUTPUT);
  pinMode(D32, INPUT);
  pinMode(D11, INPUT_ANALOG);//AD pin.
//startup blink... good idea from Pig-O-Scope
  digitalWrite(BOARD_LED, HIGH);
  delay(1000);
  digitalWrite(BOARD_LED, LOW);
  delay(1000);
  myADC.calibrate();
  myADC.setSampleRate(ADC_SMPR_1_5);
  myADC.attachInterrupt(int_func, ADC_EOC);
  myADC.setPins(&pin, 1);  
  
}



void loop() {
      if(digitalRead(D32) == 1 ) {
      Serial.println("begin");      
      // Take our samples
      myADC.startConversion(); 
      while (triggered == 0); //wait here... 
      
      Serial.print("Readin: ");
      Serial.println(dado_adc);      
      }

}
