/*
 Twitter Serial GW
 
 Sample : Serial port => Twitter gateway
  Run this program on Arduino and connect via some Serial port terminal software.
  Type a message and hit enter key, and it will be posted to Twitter.
  Arduino IDE Serial Monitor is not usable because it doesn't seem able to send Enter key code.

 Notice
 -The library uses this site as a proxy server for OAuth stuff. Your tweet may not be applied during maintenance of this site.
 -Please avoid sending more than 1 request per minute not to overload the server.
 -Twitter seems to reject repeated tweets with the same contenet (returns error 403).
 
 Reference & Lincense
 URL : http://arduino-tweet.appspot.com/
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 2011 
 by NeoCat - (ver1.3 - Support IDE 1.0)
 tested & 
 modified 13 Aug 2013 
 by Soohwan Kim - (ver1.3 - Support IDE 1.0.5)
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

#include <SPI.h> // needed in Arduino 0019 or later
#include <Ethernet_STM.h>
#include "./Twitter.h" // Twitter.h in library/Ethernet folder. @diffinsight 2013-08-19

// The includion of EthernetDNS is not needed in Arduino IDE 1.0 or later.
// Please uncomment below in Arduino IDE 0022 or earlier.
//#include <EthernetDNS.h>

// Ethernet Shield Settings
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif  

// If you don't specify the IP address, DHCP is used(only in Arduino 1.0 or later).
// fill in an available IP address on your network here,
IPAddress ip(1,1,1,1);
IPAddress gw(1,1,1,1);
IPAddress snip(0,0,0,0);
IPAddress dnsip(0,0,0,0);

// Your Token to Tweet (get it from http://arduino-tweet.appspot.com/)
//Twitter twitter("YOUR-TOKEN-HERE");


char msg[141] = "";
int len = 0;

void setup()
{
  delay(1000);
 // start the Ethernet connection:
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip, dnsip, gw,snip);
#else
 Ethernet.begin(mac, ip, dnsip, gw,snip);
#endif
// or you can use DHCP for autoomatic IP address configuration.
// start the Ethernet connection:
//#if defined(WIZ550io_WITH_MACADDRESS)
//  Ethernet.begin();  
//#else
// Ethernet.begin(mac);
//#endif
  

  Serial.begin(9600);
  Serial.print("> ");
}

void loop()
{
  if (Serial.available() > 0) {
    char recv = msg[len++] = Serial.read();
    if (recv == '\b' || recv == 127) { // Backspace
      if (len > 1) {
        len -= 2;
        Serial.print("\b \b");
      }
    }
    else if (recv == '\r' || recv == '\n') { // send CR/LF to post
      Serial.print("\r\n");
      len--;
      msg[len] = 0;
      if (len > 0)
        post();
      len = 0;
      Serial.print("\r\n> ");
    }
    else if (len > 140)
      len = 140;
    else
      Serial.print(recv);
  }
}

void post()
{
  Serial.println("connecting ...");
  if (twitter.post(msg)) {
    int status = twitter.wait();
    if (status == 200) {
      Serial.println("OK.");
    } else {
      Serial.print("failed : code ");
      Serial.println(status);
    }
  } else {
    Serial.println("connection failed.");
  }
  delay(1000);
}
