// This demo does web requests via DNS lookup, using a fixed gateway.
// 2010-11-27 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
//
//
//-----------------------------------------------------------------
//  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
//  Home:  http://www.serasidis.gr
//  email: avrsite@yahoo.gr
//
// PIN Connections (Using STM32F103):
//
// ENC28J60 -  STM32F103
//   VCC    -    3.3V
//   GND    -    GND
//   SCK    -    Pin PA5
//   SO     -    Pin PA6
//   SI     -    Pin PA7
//   CS     -    Pin PA8
//----------------------------------------------------------------- 
//
//

#include <EtherCard_STM.h>
#include <SPI.h>

#define REQUEST_RATE 5000 // milliseconds

// ethernet interface mac address
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
// ethernet interface ip address
static byte myip[] = { 192,168,1,203 };
// gateway ip address
static byte gwip[] = { 192,168,1,1 };
// domain name server (dns) address
static byte dnsip[] = { 192,168,1,1 };
// remote website name
const char website[] PROGMEM = "google.com";

byte Ethernet::buffer[300];   // a very small tcp/ip buffer is enough here
static long timer;

// called when the client request is complete
static void my_result_cb (byte status, uint16_t off, uint16_t len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}

void setup () {
  Serial.begin(57600);
  Serial.println("\n[getViaDNS]");
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
    Serial.println( "Failed to access Ethernet controller");

  ether.staticSetup(myip, gwip);
  
  ether.copyIp(ether.dnsip, dnsip);

  if (!ether.dnsLookup(website))
    Serial.println("DNS failed");
  ether.printIp("Server: ", ether.hisip);
  
  timer = - REQUEST_RATE; // start timing out right away
}

void loop () {
  ether.packetLoop(ether.packetReceive());
  
  if (millis() > timer + REQUEST_RATE) {
    timer = millis();
    Serial.println("\n>>> REQ");
    ether.browseUrl(PSTR("/foo/"), "bar", website, my_result_cb);
  }
}