/*
 UIPEthernet.h - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#ifndef UIPETHERNET_H
#define UIPETHERNET_H

#define UIP_UDP 1

//#define UIPETHERNET_DEBUG
//#define UIPETHERNET_DEBUG_CHKSUM
//#define UIPETHERNET_DEBUG_UDP
//#define UIPETHERNET_DEBUG_CLIENT

#include "ethernet_comp.h"
#include <Arduino.h>
#include "Dhcp.h"
#include "IPAddress.h"
#include "utility/Enc28J60Network.h"
#include "UIPClient.h"
#include "UIPServer.h"
#include "UIPUdp.h"

extern "C"
{
#include "utility/uip_timer.h"
#include "utility/uip.h"
}

#define UIPETHERNET_FREEPACKET 1
#define UIPETHERNET_SENDPACKET 2
#define UIPETHERNET_BUFFERREAD 4

#define uip_ip_addr(addr, ip) do { \
                     ((u16_t *)(addr))[0] = HTONS(((ip[0]) << 8) | (ip[1])); \
                     ((u16_t *)(addr))[1] = HTONS(((ip[2]) << 8) | (ip[3])); \
                  } while(0)

#define ip_addr_uip(a) IPAddress(a[0] & 0xFF, a[0] >> 8 , a[1] & 0xFF, a[1] >> 8) //TODO this is not IPV6 capable

#define uip_seteth_addr(eaddr) do {uip_ethaddr.addr[0] = eaddr[0]; \
                              uip_ethaddr.addr[1] = eaddr[1];\
                              uip_ethaddr.addr[2] = eaddr[2];\
                              uip_ethaddr.addr[3] = eaddr[3];\
                              uip_ethaddr.addr[4] = eaddr[4];\
                              uip_ethaddr.addr[5] = eaddr[5];} while(0)

#define BUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

class UIPEthernetClass
{
public:
  UIPEthernetClass();

  int begin(const uint8_t* mac);
  void begin(const uint8_t* mac, IPAddress ip);
  void begin(const uint8_t* mac, IPAddress ip, IPAddress dns);
  void begin(const uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway);
  void begin(const uint8_t* mac, IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

  // maintain() must be called at regular intervals to process the incoming serial
  // data and issue IP events to the sketch.  It does not return until all IP
  // events have been processed. Renews dhcp-lease if required.
  int maintain();

  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsServerIP();

private:
  static memhandle in_packet;
  static memhandle uip_packet;
  static uint8_t uip_hdrlen;
  static uint8_t packetstate;
  
  static IPAddress _dnsServerAddress;
  static DhcpClass* _dhcp;

  static unsigned long periodic_timer;

  static void init(const uint8_t* mac);
  static void configure(IPAddress ip, IPAddress dns, IPAddress gateway, IPAddress subnet);

  static void tick();

  static boolean network_send();

  friend class UIPServer;

  friend class UIPClient;

  friend class UIPUDP;

  static uint16_t chksum(uint16_t sum, const uint8_t* data, uint16_t len);
  static uint16_t ipchksum(void);
#if UIP_UDP
  static uint16_t upper_layer_chksum(uint8_t proto);
#endif
  friend uint16_t uip_ipchksum(void);
  friend uint16_t uip_tcpchksum(void);
  friend uint16_t uip_udpchksum(void);

  friend void uipclient_appcall(void);
  friend void uipudp_appcall(void);

#if UIP_CONF_IPV6
  uint16_t uip_icmp6chksum(void);
#endif /* UIP_CONF_IPV6 */
};

extern UIPEthernetClass UIPEthernet;

#endif
