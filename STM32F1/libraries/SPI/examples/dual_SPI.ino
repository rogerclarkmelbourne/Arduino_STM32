/*
    SPI1 and SPI_2 examples
    
    Description:
    This sketch sends one byte with value 0x55 over the SPI_1 and
    the byte 0xAC over SPI_2 port.
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

byte data;
SPIClass SPI_2(2); //Create an SPI2 object.


void setup() {
  // Initialize the SPI_1 object. SPI speed 1.125 MHz, MSB first, SPI mode = 0
  SPI.begin(BOARD_SPI1_NSS_PIN, SPISettings(1125000, MSBFIRST, SPI_MODE0)); //Initialize the SPI_1 port.
 
  // Initialize the SPI_2 object. SPI speed 4.5 MHz, MSB first, SPI mode = 0
  SPI_2.begin(BOARD_SPI2_NSS_PIN, SPISettings(4500000, MSBFIRST, SPI_MODE0)); //Initialize the SPI_2 port.
}

void loop() {
  digitalWrite(BOARD_SPI1_NSS_PIN, LOW);  // assert chip select
  data = SPI.transfer(0x55);        //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  digitalWrite(BOARD_SPI1_NSS_PIN, HIGH); // assert chip select
  delayMicroseconds(5);             //Delay 10 micro seconds.
 
  digitalWrite(BOARD_SPI2_NSS_PIN, LOW);  // assert chip select
  data = SPI_2.transfer(0xAC);      //Send the HEX data 0xAC over SPI-2 port and store the received byte to the <data> variable.
  digitalWrite(BOARD_SPI2_NSS_PIN, HIGH); // assert chip select
  delayMicroseconds(5);             //Delay 10 micro seconds.
}
