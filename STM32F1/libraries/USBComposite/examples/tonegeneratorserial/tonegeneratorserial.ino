#include <USBComposite.h>

uint8 val;
uint16 sample_rate = 8000;
int sin_hz = 200;
int step = 50;
unsigned long last_audio_read_ms;
double theta;
double inc;

USBCompositeSerial CompositeSerial;
USBAUDIO AUDIO;

double get_increment_from_hz(int hz)
{
  return (2 * M_PI) / ((1000000.0 / hz) / AUDIO.getSamplePeriod());
}

void setup()
{
  theta = 0.0;

  AUDIO.begin(CompositeSerial, MIC_MONO, sample_rate);

  inc = get_increment_from_hz(sin_hz);

  last_audio_read_ms = micros();
}

void loop()
{
  unsigned long current_micros = micros();
  unsigned long elapsed = current_micros - last_audio_read_ms;

  if (elapsed >= AUDIO.getSamplePeriod())
  {
    theta += inc;

    if (theta > M_PI * 2)
      theta -= M_PI * 2;

    val = (uint8) (((sin(theta) + 1.0) / 2.0) * 255);

    while (!AUDIO.write(&val, 1));

    last_audio_read_ms = current_micros;
  }

  if (CompositeSerial.available())
  {
    char ch = CompositeSerial.read();
    if (ch == '+')
    {
      sin_hz += step;
      inc = get_increment_from_hz(sin_hz);
      CompositeSerial.println(inc);
    }
    else if (ch == '-')
    {
      sin_hz -= step;
      inc = get_increment_from_hz(sin_hz);
      CompositeSerial.println(inc);
    }
  }
}
