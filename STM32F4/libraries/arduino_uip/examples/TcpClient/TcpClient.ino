/*
 * UIPEthernet TcpClient example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This TcpClient example gets its local ip-address via dhcp and sets
 * up a tcp socket-connection to 192.168.0.1 port 5000 every 5 Seconds.
 * After sending a message it waits for a response. After receiving the
 * response the client disconnects and tries to reconnect after 5 seconds.
 *
 * Copyright (C) 2013 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#include <SPI.h>
#include <UIPEthernet.h>

EthernetClient client;
signed long next;

void setup() {

  Serial.begin(9600);

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  Ethernet.begin(mac);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP().toString());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask().toString());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP().toString());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP().toString());

  next = 0;
}

void loop() {

  if (((signed long)(millis() - next)) > 0)
    {
      next = millis() + 5000;
      Serial.println("Client connect");
      // replace hostname with name of machine running tcpserver.pl
//      if (client.connect("server.local",5000))
      if (client.connect(IPAddress(192,168,0,1),5000))
        {
          Serial.println("Client connected");
          client.println("DATA from Client");
          while(client.available()==0)
            {
              if (next - millis() < 0)
                goto close;
            }
          int size;
          while((size = client.available()) > 0)
            {
              uint8_t* msg = (uint8_t*)malloc(size);
              size = client.read(msg,size);
              Serial.write(msg,size);
              free(msg);
            }
close:
          //disconnect client
          Serial.println("Client disconnect");
          client.stop();
        }
      else
        Serial.println("Client connect failed");
    }
}
