#include <USBComposite.h>
#include <Wire.h>

int audio_left_pin = PA0;
int audio_right_pin = PA1;

uint8 adc_pins[] = {audio_left_pin, audio_right_pin};

uint16_t buffer[2];
uint8 buf[2];

USBCompositeSerial CompositeSerial;
USBAUDIO AUDIO;

unsigned long last_audio_read_ms;

int sample_rate = 24000;

void setup()
{
  pinMode(adc_pins[0], OUTPUT);
  pinMode(adc_pins[1], OUTPUT);

  AUDIO.begin(CompositeSerial, SPEAKER_STEREO, sample_rate);

  last_audio_read_ms = micros();
}

void loop()
{
  unsigned long current_micros = micros();
  if ((current_micros - last_audio_read_ms) >= AUDIO.getSamplePeriod())
  {
    while(!AUDIO.read(buf, 2));

    /* TODO: Find something useful to do with the data.
    analogWrite(adc_pins[0], buf[0] - 128);
    analogWrite(adc_pins[1], buf[1] - 128);
    */

    CompositeSerial.print("L: ");
    CompositeSerial.println(buf[0]);
    CompositeSerial.print("R: ");
    CompositeSerial.println(buf[1]);

    last_audio_read_ms = current_micros;
  }
}
