/*
  This example shows how to use the ADC library sample
  one channels/pin at a time.
  A bit like analogRead, but with the possibility of having an interrupt.
*/
#include <STM32ADC.h>

ADC myAdc(ADC1);
uint8 pin[] = {D11};


void int_func() {
  Serial.println(adc_result);
};

void setup() {
  Serial.begin(19200);
  myADC.setTrigger(ADC_EXT_EV_SWSTART);//start on SWStart bit
  myADC.setChannels(pin, 1); //this is actually the pin you want to measure
  myADC.attachADCInterrupt(int_func);
}; //end setup

void loop(){

}; //end loop
