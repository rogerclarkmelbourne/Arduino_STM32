/*
  PRINT_FLOAT - Arduino 1.6.0rc1
    Sketch uses 15,164 bytes (14%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 2,592 bytes of dynamic memory.
  Adapted to Maple Mini by m. ray burnette
  Illustrates how to display floats in the range of -999.999 to 999.999 with a specified
  number of digits after the decimal point.
  copyright, Peter H Anderson, Baltimore, MD, Nov, '07
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
  Serial.println("Print Float Format");
}

void loop()
{
  while(1)
  {
      print_float(0.6, 2);   // illustrate various test cases
      Serial.println();
      print_float(1.2, 1);
      Serial.println();
      print_float(10.27, 2);
      Serial.println();
      print_float(10.345, 3);
      Serial.println();
      print_float(107.345, 3);
      Serial.println();
      delay(1000);

      print_float(-0.6, 2);
      Serial.println();
      print_float(-1.2, 1);
      Serial.println();
      print_float(-10.27, 2);
      Serial.println();
      print_float(-10.345, 3);
      Serial.println();
      print_float(-107.345, 3);
      Serial.println();
      delay(1000);
  }
}

void print_float(float f, int num_digits)
{
    int f_int;
    int pows_of_ten[4] = {1, 10, 100, 1000};
    int multiplier, whole, fract, d, n;

    multiplier = pows_of_ten[num_digits];
    if (f < 0.0)
    {
        f = -f;
        Serial.print("-");
    }
    whole = (int) f;
    fract = (int) (multiplier * (f - (float)whole));

    Serial.print(whole);
    Serial.print(".");

    for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
    {
         d = fract / pows_of_ten[n];
         Serial.print(d);
         fract = fract % pows_of_ten[n];
    }
}


