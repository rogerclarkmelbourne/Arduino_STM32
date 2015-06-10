/**
    SPI_2 example code
    
    Created on 10 Jun 2015 by Vassilis Serasidis
    My home:  http://www.serasidis.gr
    email:    avrsite@yahoo.gr
    
    That example uses the Arduino_STM32 project plugin.
    The Arduino_STM32 adds the STM32 microcontroller support to the
    Arduino IDE 1.6.x or later.
   
    The Arduino_STM32 can be found here: 
    https://github.com/rogerclarkmelbourne/Arduino_STM32
    
    and on my fork:
    https://github.com/Serasidis/Arduino_STM32
    
    Using the first SPI port (SPI)
    SS    <-->  PB12 <-->  BOARD_SPI2_NSS_PIN
    SCK   <-->  PB13 <-->  BOARD_SPI2_SCK_PIN
    MISO  <-->  PB14 <-->  BOARD_SPI2_MISO_PIN
    MOSI  <-->  PB15 <-->  BOARD_SPI2_MOSI_PIN
*/


#include <SPI.h>

SPIClass SPI_2(2); //Create an SPI2 object.
byte data;

void setup() {
  SPI_2.begin(); //Initialize the SPI_2 port.
  SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
  SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode
  SPI_2.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz SPI_2 speed) 

  pinMode(BOARD_SPI2_NSS_PIN, OUTPUT); // note: this must be after the SPI_2.begin() for gpio control of CSN
}

void loop() {
  digitalWrite(BOARD_SPI2_NSS_PIN, LOW); // manually take CSN low for spi transmission
  //gpio_write_bit(GPIOB, 12, LOW); // faster than digitalWrite()
  data = SPI_2.transfer(0x55); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  digitalWrite(BOARD_SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  //gpio_write_bit(GPIOB, 12, HIGH);  // faster than digitalWrite()

  delayMicroseconds(10);    //Delay 10 micro seconds.
}
