/*
  DHCP-based IP printer
 
 This sketch uses the DHCP extensions to the Ethernet library
 to get an IP address via DHCP and print the address obtained.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 12 April 2011
 modified 9 Apr 2012
 by Tom Igoe
 modified 12 Aug 2013
 by Soohwan Kim
 Modified 18 Aug 2015
 by Vassilis Serasidis
 
 =========================================================
 Ported to STM32F103 on 18 Aug 2015 by Vassilis Serasidis

 <---- Pinout ---->
 W5x00 <--> STM32F103
 SS    <-->  PA4 <-->  BOARD_SPI1_NSS_PIN
 SCK   <-->  PA5 <-->  BOARD_SPI1_SCK_PIN
 MISO  <-->  PA6 <-->  BOARD_SPI1_MISO_PIN
 MOSI  <-->  PA7 <-->  BOARD_SPI1_MOSI_PIN
 =========================================================
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif  

IPAddress ip(192,168,1,177);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // start the Ethernet connection:
  //Ethernet.begin(mac, ip); //Static IP
  Ethernet.begin(mac);   //It Gets the IP from the DHCP Server

  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}

void loop() {

}

