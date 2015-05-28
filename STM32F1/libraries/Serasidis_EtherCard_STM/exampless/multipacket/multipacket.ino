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

#define TCP_FLAGS_FIN_V 1 //as declared in net.h
#define TCP_FLAGS_ACK_V 0x10 //as declared in net.h
static byte myip[] = { 192,168,1,203 };
static byte gwip[] = { 192,168,1,1 };
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x39 };
byte Ethernet::buffer[900]; // tcp ip send and receive buffer
const char pageA[] PROGMEM =
"HTTP/1.0 200 OK\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<html>"
  "<head><title>"
    "multipackets Test"
  "</title></head>"
  "<body>"
    "<a href='/'>Start here</a><br>"
    "<h3>packet 1</h3>"
    "<p><em>"
      "the first packet send "
    "</em></p>"
;
const char pageB[] PROGMEM =
    "<h3>packet 2</h3>"
    "<p><em>"
      "if you read this it mean it works"
    "</em></p>"
;
const char pageC[] PROGMEM =
     "<h3>packet 3</h3>"
    "<p><em>"
      "if you read this it mean it works"
    "</em></p>"
;
const char pageD[] PROGMEM =
      "<h3>packet 4</h3>"
    "<p><em>"
      "if you read this it mean it works"
    "</em></p>"
;
const char pageE[] PROGMEM =
      "<h3>packet 5</h3>"
    "<p><em>"
      "this is the last packet"
    "</em></p>"
;


void setup(){
  ether.begin(sizeof Ethernet::buffer, mymac);// 53 for the mega ethernet shield and 10 for normal ethernet shield
  ether.staticSetup(myip, gwip);
}
void loop(){
    word pos = ether.packetLoop(ether.packetReceive());
    // check if valid tcp data is received
    if (pos) {
        char* data = (char *) Ethernet::buffer + pos;
        if (strncmp("GET / ", data, 6) == 0) {
            ether.httpServerReplyAck(); // send ack to the request
            memcpy_P(ether.tcpOffset(), pageA, sizeof pageA); // send first packet and not send the terminate flag
            ether.httpServerReply_with_flags(sizeof pageA - 1,TCP_FLAGS_ACK_V);
            memcpy_P(ether.tcpOffset(), pageB, sizeof pageB); // send second packet and not send the terminate flag
            ether.httpServerReply_with_flags(sizeof pageB - 1,TCP_FLAGS_ACK_V);
            memcpy_P(ether.tcpOffset(), pageC, sizeof pageC); // send thirdt packet and not send the terminate flag
            ether.httpServerReply_with_flags(sizeof pageC - 1,TCP_FLAGS_ACK_V);
            memcpy_P(ether.tcpOffset(), pageD, sizeof pageD); // send fourth packet and not send the terminate flag
            ether.httpServerReply_with_flags(sizeof pageD - 1,TCP_FLAGS_ACK_V);
            memcpy_P(ether.tcpOffset(), pageE, sizeof pageE); // send fiveth packet and send the terminate flag
            ether.httpServerReply_with_flags(sizeof pageE - 1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V); }
        else
        {
            ether.httpServerReplyAck(); // send ack to the request
            memcpy_P(ether.tcpOffset(), pageA, sizeof pageA);//only the first part will sended 
            ether.httpServerReply_with_flags(sizeof pageA - 1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V);
        }
  }
}