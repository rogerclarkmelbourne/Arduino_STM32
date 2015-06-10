/**
    SPI example code
    
    10 Jun 2015 by Vassilis Serasidis
    Home:   http://www.serasidis.gr
    email:  avrsite@yahoo.gr
    
    That example uses the Arduino_STM32 project plugin by Roger Clark.
    The Arduino_STM32 is a pack of files that adds the STM32 support to the
    Arduino IDE 1.6.x or later.
   
    The Arduino_STM32 can be found here: 
    https://github.com/rogerclarkmelbourne/Arduino_STM32
    
    and on my fork:
    https://github.com/Serasidis/Arduino_STM32
    
    Using the first SPI port (SPI)
    SS    <-->  PA4 <-->  BOARD_SPI1_NSS_PIN
    SCK   <-->  PA5 <-->  BOARD_SPI1_SCK_PIN
    MISO  <-->  PA6 <-->  BOARD_SPI1_MISO_PIN
    MOSI  <-->  PA7 <-->  BOARD_SPI1_MOSI_PIN
*/


#include <SPI.h>

byte data;

void setup() {
  SPI.begin(); //Initialize the SPI port.
  SPI.setBitOrder(MSBFIRST); // Set the SPI bit order
  SPI.setDataMode(SPI_MODE0); //Set the  SPI data mode
  SPI.setClockDivider(SPI_CLOCK_DIV16);      // Slow speed (72 / 16 = 4.5 MHz SPI speed) 

  pinMode(BOARD_SPI1_NSS_PIN, OUTPUT); // note: this must be after the SPI.begin() for gpio control of CSN
}

void loop() {
  digitalWrite(BOARD_SPI1_NSS_PIN, LOW); // manually take CSN low for spi transmission
  //gpio_write_bit(GPIOA, 4, LOW); // faster than digitalWrite()
  data = SPI.transfer(0x55); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  digitalWrite(BOARD_SPI1_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  //gpio_write_bit(GPIOA, 4, HIGH);  // faster than digitalWrite()

  delayMicroseconds(10);    //Delay 10 micro seconds.
}