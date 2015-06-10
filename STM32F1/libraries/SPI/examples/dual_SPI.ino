#include <SPI.h>

byte data;
#define SPI1_NSS_PIN PA4
#define SPI2_NSS_PIN PB12


SPIClass SPI_2(2); //Create an SPI2 object.


void setup() {
  // Initialize the SPI_1 object. SPI speed 1.125 MHz, MSB first, SPI mode = 0
  SPI.begin(SPI1_NSS_PIN, SPISettings(1125000, MSBFIRST, SPI_MODE0)); //Initialize the SPI_1 port.
  //SPI.begin(); //The standard SPI.begin() works also.
 
  // Initialize the SPI_1 object. SPI speed 4.5 MHz, MSB first, SPI mode = 0
  SPI_2.begin(SPI2_NSS_PIN, SPISettings(4500000, MSBFIRST, SPI_MODE0)); //Initialize the SPI_2 port.
}

void loop() {
  digitalWrite(SPI1_NSS_PIN, LOW);  // assert chip select
  data = SPI.transfer(0x55);        //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(SPI1_NSS_PIN, HIGH); // assert chip select
  delayMicroseconds(5);             //Delay 10 micro seconds.
 
  digitalWrite(SPI2_NSS_PIN, LOW);  // assert chip select
  data = SPI_2.transfer(0xAC);      //Send the HEX data 0xAC over SPI-2 port and store the received byte to the <data> variable.
  digitalWrite(SPI2_NSS_PIN, HIGH); // assert chip select
  delayMicroseconds(5);             //Delay 10 micro seconds.
}
