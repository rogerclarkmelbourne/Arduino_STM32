/*
  IntegerInput_FloatOutput by m. Ray Burnette - PUBLIC DOMAIN EXAMPLE
  Maple Mini: Compiled under Arduino 1.6.0rc1
    Sketch uses 19,868 bytes (18%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 3,704 bytes of dynamic memory.
*/

#define BAUD 9600
#define timeoutPeriod 2147483647    // Long var... about 25 days

float a;
float b;


void setup()
{
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(BAUD);  // BAUD has no effect on USB serial: placeholder for physical UART
  Serial.setTimeout(timeoutPeriod); // default is 1 second
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  Serial.println("Integer Input - Floating Output");
  Serial.println("You may wish to try 355 and 113 (Pi)");
}

void loop()
{
  Serial.println("Enter first integer: ");

  a = Serial.parseInt();
  Serial.print("a = ");
  Serial.println(a);

  Serial.println("Enter second integer: ");

  b = Serial.parseInt();
  Serial.print("b = ");
  Serial.println(b);

  a = a / b;
  Serial.print( "a/b = ");
  Serial.println(a);
  Serial.println();
}

