/*
  Re: http://leaflabs.com/docs/hardware
  Arduino 1.6.0rc1 modifications by m. ray burnette
  Sketch uses 31,608 bytes (29%) of program storage space. Maximum is 108,000 bytes.
  Global variables use 3,752 bytes of dynamic memory.
  PUBLIC DOMAIN EXAMPLE
*/

//#include <stdio.h>
#include <libmaple/adc.h>

#define LED_PIN 33
#define A_RANDOM_ANALOG_PIN 15

void setup_temperature_sensor() {
  adc_reg_map *regs = ADC1->regs;

// 3. Set the TSVREFE bit in the ADC control register 2 (ADC_CR2) to wake up the
//    temperature sensor from power down mode.  Do this first 'cause according to
//    the Datasheet section 5.3.21 it takes from 4 to 10 uS to power up the sensor.

  regs->CR2 |= ADC_CR2_TSVREFE;

// 2. Select a sample time of 17.1 μs
// set channel 16 sample time to 239.5 cycles
// 239.5 cycles of the ADC clock (72MHz/6=12MHz) is over 17.1us (about 20us), but no smaller
// sample time exceeds 17.1us.

  regs->SMPR1 = (0b111 << (3*6));     // set channel 16, the temp. sensor
}

void setup(void)
{
// Set up the LED to blink
  pinMode(LED_PIN, OUTPUT);
  
// set up an analog input.  We want to test and make sure other analog reads don't screw this up.
  pinMode(A_RANDOM_ANALOG_PIN, INPUT_ANALOG);

// wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  
// init tem psensor
  setup_temperature_sensor();
  
// announce start up
  if(Serial)
    Serial.println("Temp mon startup");
}

//
// once per second perform a standard analog read and read the temperature sensor
// the analog read is to test whether our messing with the temp sensor will screw up analog reads
//    or visa versa.
// Also compute and display the time each operation takes.
//
void loop(void)
{
  uint16 vsense, alogpin;
  uint32 t1, t2, t3;
  char buf[64];
  
  t1 = micros();
  alogpin = analogRead(A_RANDOM_ANALOG_PIN);
  t2 = micros();
  vsense = adc_read(ADC1, 16);
  t3 = micros();
  if(Serial) {
    sprintf(buf,"%04x %08x  %04x %08x" , vsense, t3-t2, alogpin, t2-t1);
    Serial.println(buf);
  }
  digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
  delay(1000);
}


