/* PRINT_BINARY - Arduino 1.6.0rc1
  Adapted to Maple Mini by m. ray burnette
    Sketch uses 11,672 bytes (10%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 2,592 bytes of dynamic memory
  Prints a positive integer in binary format with a fixed withdth
  copyright, Peter H Anderson, Baltimore, MD, Nov, '07 
  PUBLIC DOMAIN EXAMPLE
*/

#define BAUD 9600

void setup()
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
  Serial.println("Print Binary Format");
}

void loop()
{
  while(1)
  {
      print_binary(1024+256+63, 12);
      Serial.println();
      delay(1000);

  }
}

void print_binary(int v, int num_places)
{
    int mask=0, n;

    for (n=1; n<=num_places; n++)
    {
        mask = (mask << 1) | 0x0001;
    }
    v = v & mask;  // truncate v to specified number of places

    while(num_places)
    {

        if (v & (0x0001 << num_places-1))
        {
             Serial.print("1");
        }
        else
        {
             Serial.print("0");
        }

        --num_places;
        if(((num_places%4) == 0) && (num_places != 0))
        {
            Serial.print("_");
        }
    }
}

