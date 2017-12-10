/*
  PrimeNos: by Nick Gammon
  Maple Mini port m. ray burnette: Compiled under Arduino 1.6.0rc1
    Sketch uses 13,644 bytes (12%) of program storage space. Maximum is 108,000 bytes.
    Global variables use 2,656 bytes of dynamic memory.
  PUBLIC DOMAIN EXAMPLE
*/

#define BAUD 9600

// just add more primes to the prime table for larger search
// byte data type to save memory - use a larger datatype with prime table entries above 255 :)
byte primes[]={ 
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101,
    102, 107, 109, 113, 127, 131,  137 , 139, 149, 151, 157, 163, 167, 173,  179, 181, 191, 193, 197, 
    199, 211, 223, 227, 229, 233, 239, 241, 251 };

// if you change the datatype of primes array to int, change next line to 
// "const int TopPrimeIndex = (sizeof(primes)/2) - 1;"

const unsigned int TopPrimeIndex = sizeof(primes) - 1;      
const unsigned long TopPrimeSquared = (long)primes[TopPrimeIndex] * (long)primes[TopPrimeIndex];
int primeFlag;


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
  Serial.println("Prime Number Generator");
    Serial.print("Number of primes in prime table = ");
    Serial.println(TopPrimeIndex);
    Serial.println();
    Serial.print("Last prime in table =  ");
    Serial.println((unsigned int)primes[TopPrimeIndex]);
    Serial.println();

    Serial.print("Calculating primes through ");
    Serial.println(TopPrimeSquared);
    Serial.println();


}
void loop()                     // run over and over again
{
    for (long x = 1; x < TopPrimeSquared; x+=2){  // skips even numbers, including 2, which is prime, but it makes algorithm tad faster

            for (long j=0; j < TopPrimeIndex; j++){
            primeFlag = true;

            if (x == primes[j]) break;

            if (x % primes[j] == 0){     // if the test number modolo (next number from prime table) == 0 
                primeFlag = false;       //  then test number is not prime, bailout and check the next number
                break;
            }
        }
        if (primeFlag == true){           // found a prime - print it
            Serial.println(x);
       }
    }
}
