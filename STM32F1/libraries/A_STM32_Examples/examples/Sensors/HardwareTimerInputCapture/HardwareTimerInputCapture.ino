/* Example of the Timer Input Capture mode
 *  
 * Thanks to @cesco on the stm32duino.com
 * 
 * This example uses Timer2. Other timers also support this mode, bnt on the F103C 
 * only Timer 2 has usable external trigger pins. PA0 and PA15
 * Because the only other usable pin on the F103C is one of the USB pins.
 * 
 * Upload to your favourite F103Cx 
 * Open the Serial plotter
 * Put your finger on either PA0 or PA15 (depenidng whether you USE the pin remapping)
 * 
 * Observe your local mains frequency in terms of pulse width time.
 *
 */

/* Timer2 external trigger  can be ether PA0 or PA15. 
 To use use afio_remap(FIO_REMAP_TIM2_PARTIAL_1)  or afio_remap(AFIO_REMAP_TIM2_FULL)
 Note. using afio_remap also changes the pins used by Timer2 CH1,CH2,CH3 amd CH4 
 * See page 184 of the STM32F10x Refernce manual (PDF downloadable from ST.com
 */

//#define USE_REMAP_PIN
#ifdef USE_REMAP_PIN
const int timer2ExternalTriggerPin = PA15;
#else
const int timer2ExternalTriggerPin = PA0;
#endif

volatile boolean hasCapture;
volatile uint16_t captureValue;
volatile uint16_t lastCaptureValue=0;
volatile uint16_t captureDifference=0;

void captureCallback(void) {

   hasCapture=true;
   captureValue = Timer2.getCompare(1);
   captureDifference = captureValue-lastCaptureValue;
   lastCaptureValue=captureValue;   
}

void setup() {
  
  pinMode(timer2ExternalTriggerPin,INPUT);

  #ifdef USE_REMAP_PIN
    afio_remap(AFIO_REMAP_TIM2_PARTIAL_1);
  #endif
  
  hasCapture = false;

  Timer2.setPrescaleFactor(72); // prescaler to count 72,000,000 / 72 counts per second. i.e count microseconds
  Timer2.setMode(TIMER_CH1, TIMER_INPUT_CAPTURE);
  Timer2.attachCompare1Interrupt(captureCallback);

}

void loop()  {

  // Printing inside the callback is not a good idea. 
  // Hence the capture callback sets a flag , which is detected here
  if (hasCapture)
  {
    Serial.println(captureDifference);
    hasCapture=false;
  }  
}