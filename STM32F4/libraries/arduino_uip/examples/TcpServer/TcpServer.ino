/*
 * UIPEthernet EchoServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.1.6 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a TCP connection to
 * the Arduino.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 * Adaption to Enc28J60 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#include <SPI.h>
#include <IPAddress.h>
#include <UIPEthernet.h>



EthernetServer server = EthernetServer(23);


void setup()
{
  Serial.begin(115200);
  delay(3000);
  Serial.println("TCP Server starting ...");

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  IPAddress myIP(10,111,111,5);

  Ethernet.begin(mac, myIP); // Static IP
//  Ethernet.begin(mac); // DHCP
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP().toCharArray());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask().toCharArray());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP().toCharArray());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP().toCharArray());

  server.begin();
}

void loop()
{
  size_t size;

  if (EthernetClient client = server.available())
    {
      Serial.println("Client connected !");
      while((size = client.available()) > 0)
        {
          uint8_t* msg = (uint8_t*)malloc(size);
          size = client.read(msg, size);
	  Serial.print("Client Data received ! size=");
          Serial.println(size);
          Serial.write(msg, size);
          free(msg);
        }
      client.println("DATA from Server!");
      client.stop();
    }

}

