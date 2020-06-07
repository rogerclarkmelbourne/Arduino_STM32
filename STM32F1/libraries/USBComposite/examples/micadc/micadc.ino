#include <USBComposite.h>
#include <STM32ADC.h>

int audio_left_pin = PA0;
int audio_right_pin = PA1;

uint8 adc_pins[] = {audio_left_pin, audio_right_pin};

uint16_t buffer[2];
uint8 buf[2];
unsigned long last_audio_read_ms;

int sample_rate = 24000;

USBCompositeSerial CompositeSerial;
STM32ADC ADC(ADC1);
USBAUDIO AUDIO;

void setup()
{
  pinMode(adc_pins[0], INPUT);
  pinMode(adc_pins[1], INPUT);

  ADC.calibrate();
  ADC.setSampleRate(ADC_SMPR_1_5);
  ADC.setScanMode();
  ADC.setPins(adc_pins, 2);
  ADC.setContinuous();
  ADC.setDMA(buffer, 2, (DMA_MINC_MODE | DMA_CIRC_MODE), NULL);
  ADC.startConversion();

  AUDIO.begin(CompositeSerial, MIC_STEREO | AUDIO_CLASS_1, sample_rate);

  last_audio_read_ms = micros();
}

void loop()
{
  unsigned long current_micros = micros();
  unsigned long elapsed = current_micros - last_audio_read_ms;

  if (elapsed >= AUDIO.getSamplePeriod())
  {
    for (int i = 0; i < 2; i++)
    {
      /* 255 (uint8 range) / 1279 (ADC range) */
      buf[i] = buffer[i] * 0.2;
    }

    while (!AUDIO.write(buf, 2));

    last_audio_read_ms = current_micros;
  }

  /* Echo characters input to serial */
  if (CompositeSerial.available())
  {
    char ch = CompositeSerial.read();
    CompositeSerial.println(ch);
  }
}