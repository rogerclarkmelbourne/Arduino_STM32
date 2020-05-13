#include <USBComposite.h>

int audio_left_pin = PA0;
int audio_right_pin = PA1;

uint8 buf[2];
unsigned long last_audio_read_ms;

int sample_rate = 24000;

USBCompositeSerial CompositeSerial;
USBAUDIO AUDIO;

void setup()
{
  pinMode(audio_left_pin, INPUT);
  pinMode(audio_right_pin, INPUT);

  AUDIO.begin(CompositeSerial, MIC_STEREO | AUDIO_CLASS_1, sample_rate);

  last_audio_read_ms = micros();
}

void loop()
{
  unsigned long current_micros = micros();
  unsigned long elapsed = current_micros - last_audio_read_ms;

  if (elapsed >= AUDIO.getSamplePeriod())
  {
    /* 255 (uint8 range) / 4095 (analogRead range) = ~0.06 */
    buf[1] = analogRead(audio_left_pin) * (0.06);
    buf[0] = analogRead(audio_right_pin) * (0.06);

    last_audio_read_ms = current_micros;

    while (!AUDIO.write(buf, 2));
  }

  /* Echo characters input to serial */
  if (CompositeSerial.available())
  {
    char ch = CompositeSerial.read();
    CompositeSerial.println(ch);
  }
}
