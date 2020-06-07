#include <USBComposite.h>

int input_pin = PA0;

uint8 val;
unsigned long last_audio_read_ms;

USBAUDIO AUDIO;

void setup()
{
  pinMode(input_pin, INPUT);

  AUDIO.begin(MIC_MONO);

  last_audio_read_ms = micros();
}

void loop()
{
  unsigned long current_micros = micros();
  unsigned long elapsed = current_micros - last_audio_read_ms;

  if (elapsed >= AUDIO.getSamplePeriod())
  {
    /* 255 (uint8 range) / 4095 (analogRead range) = ~0.06 */
    val = analogRead(input_pin) * (0.06);

    last_audio_read_ms = current_micros;

    while (!AUDIO.write(&val, 1));
  }
}
