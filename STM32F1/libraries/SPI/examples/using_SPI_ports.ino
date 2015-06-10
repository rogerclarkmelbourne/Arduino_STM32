/**
    SPI_1 port example code
    
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

//SPIClass SPI_2(2); //un-comment this line in case you want to use the SPI_2 port.
byte data;

void setup() {
  SPI.begin(); //Initialize the SPI_1 port.
  //SPI_2.begin(); //Initialize the SPI_2 port.
  
  SPI.setBitOrder(MSBFIRST); // Set the SPI_1 bit order
  //SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  
  SPI.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  //SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
  
  SPI.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz SPI_1 speed)
  //SPI_2.setClockDivider(SPI_CLOCK_DIV16);  // Slow speed (72 / 16 = 4.5 MHz SPI_2 speed) 

  pinMode(BOARD_SPI1_NSS_PIN, OUTPUT); // note: this must be after the SPI.begin() for gpio control of CSN
  //pinMode(BOARD_SPI2_NSS_PIN, OUTPUT); // note: this must be after the SPI_2.begin() for gpio control of CSN
}

void loop() {
  digitalWrite(BOARD_SPI1_NSS_PIN, LOW); // manually take CSN low for SPI_1 transmission
  //digitalWrite(BOARD_SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  
  //gpio_write_bit(GPIOA, BOARD_SPI1_NSS_PIN, LOW); // faster than digitalWrite() [For SPI_1]
  //gpio_write_bit(GPIOB, BOARD_SPI2_NSS_PIN, LOW); // faster than digitalWrite() [For SPI_2]
  
  data = SPI.transfer(0x55); //Send the HEX data 0x55 over SPI-1 port and store the received byte to the <data> variable.
  //data = SPI_2.transfer(0x55); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  
  digitalWrite(BOARD_SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  //digitalWrite(BOARD_SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  
  //gpio_write_bit(GPIOA, BOARD_SPI1_NSS_PIN, HIGH);  // faster than digitalWrite() [For SPI_1]
  //gpio_write_bit(GPIOB, BOARD_SPI2_NSS_PIN, HIGH);  // faster than digitalWrite() [For SPI_2]

  delayMicroseconds(10);    //Delay 10 micro seconds.
}