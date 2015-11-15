/**
    SPI_1 and SPI_2 port example code

    Description:
    This sketch sends one byte with value 0x55 over the SPI_1 or SPI_2 port.
    The received byte (the answer from the SPI slave device) is stored to the <data> variable.

    The sketch as it is, works with SPI_1 port. For using the SPI_2 port, just
    un-comment all the nessesary code lines marked with <SPI_2> word.

    Created on 10 Jun 2015 by Vassilis Serasidis
    email:  avrsite@yahoo.gr

    Using the first SPI port (SPI_1)
    SS    <-->  PA4 <-->  BOARD_SPI1_NSS_PIN
    SCK   <-->  PA5 <-->  BOARD_SPI1_SCK_PIN
    MISO  <-->  PA6 <-->  BOARD_SPI1_MISO_PIN
    MOSI  <-->  PA7 <-->  BOARD_SPI1_MOSI_PIN

    Using the second SPI port (SPI_2)
    SS    <-->  PB12 <-->  BOARD_SPI2_NSS_PIN
    SCK   <-->  PB13 <-->  BOARD_SPI2_SCK_PIN
    MISO  <-->  PB14 <-->  BOARD_SPI2_MISO_PIN
    MOSI  <-->  PB15 <-->  BOARD_SPI2_MOSI_PIN
*/


#include <SPI.h>

#define SPI1_NSS_PIN PA4    //SPI_1 Chip Select pin is PA4. You can change it to the STM32 pin you want.
#define SPI2_NSS_PIN PB12   //SPI_2 Chip Select pin is PB12. You can change it to the STM32 pin you want.

SPIClass SPI_2(2); //Create an instance of the SPI Class called SPI_2 that uses the 2nd SPI Port
byte data;

void setup() {

  // Setup SPI 1
  SPI.begin(); //Initialize the SPI_1 port.
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  SPI.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  SPI.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)
  pinMode(SPI1_NSS_PIN, OUTPUT);

  // Setup SPI 2
  SPI_2.begin(); //Initialize the SPI_2 port.
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);  // Use a different speed to SPI 1
  pinMode(SPI2_NSS_PIN, OUTPUT);


}

void loop() {

  sendSPI();
  sendSPI2();

  delayMicroseconds(10);    //Delay 10 micro seconds.
}

void sendSPI()
{
  digitalWrite(SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  data = SPI.transfer(0x55); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
}


void sendSPI2()
{
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  data = SPI_2.transfer(0x55); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
}