/*
  PRINT_HEX - Arduino 1.6.0rc1
    Sketch uses 13,336 bytes (12%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 2,592 bytes of dynamic memory.
  Adapted to the Maple Mini by m. ray burnette
  Illustrates how to display a hexadecimal number with a fixed width.
  opyright, Peter H Anderson, Baltimore, MD, Nov, '07
  PUBLIC DOMAIN EXAMPLE
*/

#define BAUD 9600

void setup()                    // run once, when the sketch starts
{
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(BAUD);  // BAUD has no effect on USB serial: placeholder for physical UART
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  Serial.println("Print HEX Format");
}

void loop()
{
  while(1)
  {
      print_hex(1024+256+63, 13);
      Serial.println();
      delay(1000);

  }
}

void print_hex(int v, int num_places)
{
    int mask=0, n, num_nibbles, digit;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask; // truncate v to specified number of places

    num_nibbles = num_places / 4;
    if ((num_places % 4) != 0)
    {
        ++num_nibbles;
    }

    do
    {
        digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
        Serial.print(digit, HEX);
    } while(--num_nibbles);

}

