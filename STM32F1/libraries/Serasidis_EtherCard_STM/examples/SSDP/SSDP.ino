#include <EtherCard_STM.h>                                                                                                                                    |Mac adress|
#include <SPI.h>

const char SSDP_RESPONSE[] PROGMEM = "HTTP/1.1 200 OK\r\nCACHE-CONTROL: max-age=1200\r\nEXT:\r\nSERVER:Arduino\r\nST: upnp:rootdevice\r\nUSN: uuid:abcdefgh-7dec-11d0-a765-7499692d3040\r\nLOCATION: http://"; //dont forget our mac adress USN: uuid:abcdefgh-7dec-11d0-a765-Mac addr 
const char SSDP_RESPONSE_XML[] PROGMEM = "/??\r\n\r\n"; // here is the adress of xml file /?? in this exemple but you could use another /upnp.xml\r\n\r\n 
const char XML_DESCRIP[] PROGMEM = "HTTP/1.1 200 OK\r\nContent-Type: text/xml\r\n\r\n<?xml version='1.0'?>\r<root xmlns='urn:schemas-upnp-org:device-1-0'><device><deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType><presentationURL>/</presentationURL><friendlyName>Arduino</friendlyName><manufacturer>Fredycpu</manufacturer><manufacturerURL>http://fredycpu.pro</manufacturerURL><serialNumber>1</serialNumber><UDN>uuid:abcdefgh-7dec-11d0-a765-7499692d3040</UDN></device></root>     ";
const char SSDP_NOTIFY[] PROGMEM = "NOTIFY * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nCACHE-CONTROL: max-age=1200\r\nNT: upnp:rootdevice\r\nUSN: uuid:abcdefga-7dec-11d0-a765-7499692d3040::upnp:rootdevice\r\nNTS: ssdp:alive\r\nSERVER: Arduino UPnP/1.0\r\nLOCATION: http://"; //dont forget our mac adress USN: uuid:abcdefgh-7dec-11d0-a765-Mac addr 
//  in XML_DESCRIP // <deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType> // declare as home automation
//  in XML_DESCRIP // <friendlyName>Arduino</friendlyName> // declare the name of the service here Arduino
//  in XML_DESCRIP // <presentationURL>/</presentationURL> // adress of the page who would opened on service double click ,you could use http://ip  but if you use dhcp it's better so and dont wase memory
// this is the entire protocol, but you can try to use SSDP_NOTIFY as SSDP_RESPONSE with most systems will work and you can free a bit of flash mem.
static byte myip[] = { 
  192,168,1,204 };
static byte gwip[] = { 
  192,168,1,1 };
static byte ssdp[] = { 
  239,255,255,250 };
static byte mymac[] = { 
  0x74,0x99,0x69,0x2D,0x30,0x40 }; // if you change it you must update SSDP_RESPONSE and XML_DESCRIP
byte Ethernet::buffer[750]; // tcp ip send and receive buffer
unsigned long timer=9999;
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
"<a href='/test.jpg'>Image test</a><br>"
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

void setup(){
  ether.begin(sizeof Ethernet::buffer, mymac);// 53 for the mega ethernet shield and  10 for normal ethernet shield
  ether.staticSetup(myip, gwip);
  ENC28J60::disableMulticast(); //disable multicast filter means enable multicast reception
  Serial.begin(57600);
}

void loop(){
wait:
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
      memcpy_P(ether.tcpOffset(), pageD, sizeof pageD); // send fourth packet and send the terminate flag
      ether.httpServerReply_with_flags(sizeof pageD - 1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V);
      goto wait;  
    }
    if (strncmp("GET /??", data, 7) == 0) { // description of services (normaly an xml file but here .....)
      ether.httpServerReplyAck();
      memcpy_P(Ethernet::buffer + TCP_OPTIONS_P,XML_DESCRIP, sizeof XML_DESCRIP);
      ether.httpServerReply_with_flags(sizeof XML_DESCRIP - 1 ,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V);
      goto wait;  
    }
    if (strncmp("M-SEARCH", data, 8) == 0) { // service discovery request comes here (udp protocol)
      ssdpresp();
      goto wait;  
    }
  }
  if (((millis()-timer)>50000)||(timer>millis())) {
  timer=millis();
  ssdpnotify();
  }
}
void ssdpresp() { //response to m-search 
  byte ip_dst[4];
  unsigned int port_dst=Ethernet::buffer[34]*256+Ethernet::buffer[35];//extract source port of request
  for(  int i=0; i<4;i++) { //extract source IP of request
    ip_dst[i]=Ethernet::buffer[i+26];
  }
  int udppos = UDP_DATA_P;
  
  EtherCard::udpPrepare(1900,ip_dst,port_dst);
  memcpy_P(Ethernet::buffer + udppos, SSDP_RESPONSE, sizeof SSDP_RESPONSE);
  udppos = udppos  + sizeof SSDP_RESPONSE-1;
  addip(udppos);
}

void ssdpnotify() { //Notification 
  int udppos = UDP_DATA_P;
  EtherCard::udpPrepare(1900,ssdp,1900);
  memcpy_P(Ethernet::buffer + udppos, SSDP_NOTIFY, sizeof SSDP_NOTIFY);
udppos = udppos  + sizeof SSDP_NOTIFY-1;
addip(udppos);
}

void addip(int udppos) { // add current ip to the request and send it
  int adr;
  for(int i=0;i<4;i++) { // extract the current ip of arduino
    adr = ether.myip[i]/100;
    if (adr)  {
      Ethernet::buffer[udppos]=adr+48;
      udppos++;
    }
    adr=(ether.myip[i]%100)/10;
    if (adr)  {
      Ethernet::buffer[udppos]=adr+48;
      udppos++;
    }
    adr=ether.myip[i]%10;
    Ethernet::buffer[udppos]=adr+48;
    udppos++;
    Ethernet::buffer[udppos]=46;
    udppos++; //"."
  }
  udppos--;//erase the last point
  memcpy_P(Ethernet::buffer + udppos,SSDP_RESPONSE_XML,sizeof SSDP_RESPONSE_XML);
  udppos = udppos  + sizeof SSDP_RESPONSE_XML;
  udppos--;
  EtherCard::udpTransmit(udppos-UDP_DATA_P); // send all to the computer who make the request on her ip and port who make the request
}

