#include <HardwareTimer.h>
#include <STM32ADC.h>

#define pinLED  PC13
#define pinOUT  PB0

#define _SEND_BY_USB
#ifdef SEND_BY_USB
USBSerial usb;
#endif

// Channels to be acquired. 
// A0 (adc1 channel 1)
uint8 pins = 0;

#define maxSamples  2000
uint16_t buffer[maxSamples];
uint16_t *buffers[2];
uint8_t bufr;
uint8_t bufw;

#define sampleFreqKhz       200
#define samplePeriodus      1000 / sampleFreqKhz
#define ticksPerSecond      2 * sampleFreqKhz * 1000 / maxSamples


STM32ADC myADC(ADC1);


long ticks;
void TimerIRQ(void) {
  ticks++;
}
void DmaIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  bufw = (bufw+1)%2;
  ticks++;
}

void setup() {
  
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  pinMode(pins, INPUT_ANALOG);

  Serial.begin(115200);
  Serial.println("START");  

  #ifdef SEND_BY_USB
  usb.begin();
  while (!usb.isConnected()) ;
  #endif

  ticks = 0;
  bufr = 0;
  bufw = 0;
  buffers[0] = &buffer[0];
  buffers[1] = &buffer[maxSamples/2];

  Timer3.setPeriod(samplePeriodus);
  Timer3.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);

  myADC.calibrate();
  myADC.setSampleRate(ADC_SMPR_1_5); // ?
  myADC.setPins(&pins, 1);
  myADC.setDMA(buffer, maxSamples, (DMA_MINC_MODE | DMA_CIRC_MODE | DMA_HALF_TRNS | DMA_TRNS_CMPLT), DmaIRQ);
  myADC.setTrigger(ADC_EXT_EV_TIM3_TRGO);
  myADC.startConversion();
}

void loop() {

  if (bufr!=bufw) {
    // process data 
    
    #ifdef SEND_BY_USB
    if (usb.isConnected()) usb.write((uint8_t *)buffers[bufr],maxSamples); // send it : (maxSamples /2 )*2 (bytes)
    #endif

    bufr = (bufr+1)%2;
  }
  
  if (ticks==ticksPerSecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));
    ticks = 0;
  }
}
