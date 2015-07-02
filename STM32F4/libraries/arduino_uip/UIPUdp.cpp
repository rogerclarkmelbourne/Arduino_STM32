/*
 UIPUdp.cpp - Arduino implementation of a uIP wrapper class.
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

#include "UIPEthernet.h"
#include "UIPUdp.h"
#include "Dns.h"

#ifdef UIPETHERNET_DEBUG_UDP
#include "HardwareSerial.h"
#endif

#define Serial SerialUSB

extern "C" {
#include "utility/uip-conf.h"
#include "utility/uip.h"
#include "utility/uip_arp.h"
}

#if UIP_UDP
#define UIP_ARPHDRSIZE 42
#define UDPBUF ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])

// Constructor
UIPUDP::UIPUDP() :
    _uip_udp_conn(NULL)
{
  memset(&appdata,0,sizeof(appdata));
}

// initialize, start listening on specified port. Returns 1 if successful, 0 if there are no sockets available to use
uint8_t
UIPUDP::begin(uint16_t port)
{
  if (!_uip_udp_conn)
    {
      _uip_udp_conn = uip_udp_new(NULL, 0);
    }
  if (_uip_udp_conn)
    {
      uip_udp_bind(_uip_udp_conn,htons(port));
      _uip_udp_conn->appstate = &appdata;
      return 1;
    }
  return 0;
}

// Finish with the UDP socket
void
UIPUDP::stop()
{
  if (_uip_udp_conn)
    {
      uip_udp_remove(_uip_udp_conn);
      _uip_udp_conn->appstate = NULL;
      _uip_udp_conn=NULL;
      Enc28J60Network::freeBlock(appdata.packet_in);
      Enc28J60Network::freeBlock(appdata.packet_next);
      Enc28J60Network::freeBlock(appdata.packet_out);
      memset(&appdata,0,sizeof(appdata));
    }
}

// Sending UDP packets

// Start building up a packet to send to the remote host specific in ip and port
// Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
int
UIPUDP::beginPacket(IPAddress ip, uint16_t port)
{
  UIPEthernetClass::tick();
  if (ip && port)
    {
      uip_ipaddr_t ripaddr;
      uip_ip_addr(&ripaddr, ip);
#ifdef UIPETHERNET_DEBUG_UDP
      Serial.print(F("udp beginPacket, "));
#endif
      if (_uip_udp_conn)
        {
          _uip_udp_conn->rport = htons(port);
          uip_ipaddr_copy(_uip_udp_conn->ripaddr, &ripaddr);
        }
      else
        {
          _uip_udp_conn = uip_udp_new(&ripaddr,htons(port));
          if (_uip_udp_conn)
            {
#ifdef UIPETHERNET_DEBUG_UDP
              Serial.print(F("new connection, "));
#endif
              _uip_udp_conn->appstate = &appdata;
            }
          else
            {
#ifdef UIPETHERNET_DEBUG_UDP
              Serial.println(F("failed to allocate new connection"));
#endif
              return 0;
            }
        }
#ifdef UIPETHERNET_DEBUG_UDP
          Serial.print(F("rip: "));
          Serial.print(ip);
          Serial.print(F(", port: "));
          Serial.println(port);
#endif
    }
  if (_uip_udp_conn)
    {
      if (appdata.packet_out == NOBLOCK)
        {
          appdata.packet_out = Enc28J60Network::allocBlock(UIP_UDP_MAXPACKETSIZE);
          appdata.out_pos = UIP_UDP_PHYH_LEN;
          if (appdata.packet_out != NOBLOCK)
            return 1;
#ifdef UIPETHERNET_DEBUG_UDP
          else
            Serial.println(F("failed to allocate memory for packet"));
#endif
        }
#ifdef UIPETHERNET_DEBUG_UDP
      else
        Serial.println(F("previous packet on that connection not sent yet"));
#endif
    }
  return 0;
}

// Start building up a packet to send to the remote host specific in host and port
// Returns 1 if successful, 0 if there was a problem resolving the hostname or port
int
UIPUDP::beginPacket(const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(UIPEthernet.dnsServerIP());
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return beginPacket(remote_addr, port);
  } else {
    return ret;
  }
}

// Finish off this packet and send it
// Returns 1 if the packet was sent successfully, 0 if there was an error
int
UIPUDP::endPacket()
{
  if (_uip_udp_conn && appdata.packet_out != NOBLOCK)
    {
      appdata.send = true;
      Enc28J60Network::resizeBlock(appdata.packet_out,0,appdata.out_pos);
      uip_udp_periodic_conn(_uip_udp_conn);
      if (uip_len > 0)
        {
          _send(&appdata);
          return 1;
        }
    }
  return 0;
}

// Write a single byte into the packet
size_t
UIPUDP::write(uint8_t c)
{
  return write(&c,1);
}

// Write size bytes from buffer into the packet
size_t
UIPUDP::write(const uint8_t *buffer, size_t size)
{
  if (appdata.packet_out != NOBLOCK)
    {
      size_t ret = Enc28J60Network::writePacket(appdata.packet_out,appdata.out_pos,(uint8_t*)buffer,size);
      appdata.out_pos += ret;
      return ret;
    }
  return 0;
}

// Start processing the next available incoming packet
// Returns the size of the packet in bytes, or 0 if no packets are available
int
UIPUDP::parsePacket()
{
  UIPEthernetClass::tick();
#ifdef UIPETHERNET_DEBUG_UDP
  if (appdata.packet_in != NOBLOCK)
    {
      Serial.print(F("udp parsePacket freeing previous packet: "));
      Serial.println(appdata.packet_in);
    }
#endif
  Enc28J60Network::freeBlock(appdata.packet_in);

  appdata.packet_in = appdata.packet_next;
  appdata.packet_next = NOBLOCK;

#ifdef UIPETHERNET_DEBUG_UDP
  if (appdata.packet_in != NOBLOCK)
    {
      Serial.print(F("udp parsePacket received packet: "));
      Serial.print(appdata.packet_in);
    }
#endif
  int size = Enc28J60Network::blockSize(appdata.packet_in);
#ifdef UIPETHERNET_DEBUG_UDP
  if (appdata.packet_in != NOBLOCK)
    {
      Serial.print(F(", size: "));
      Serial.println(size);
    }
#endif
  return size;
}

// Number of bytes remaining in the current packet
int
UIPUDP::available()
{
  UIPEthernetClass::tick();
  return Enc28J60Network::blockSize(appdata.packet_in);
}

// Read a single byte from the current packet
int
UIPUDP::read()
{
  unsigned char c;
  if (read(&c,1) > 0)
    {
      return c;
    }
  return -1;
}

// Read up to len bytes from the current packet and place them into buffer
// Returns the number of bytes read, or 0 if none are available
int
UIPUDP::read(unsigned char* buffer, size_t len)
{
  UIPEthernetClass::tick();
  if (appdata.packet_in != NOBLOCK)
    {
      memaddress read = Enc28J60Network::readPacket(appdata.packet_in,0,buffer,len);
      if (read == Enc28J60Network::blockSize(appdata.packet_in))
        {
          Enc28J60Network::freeBlock(appdata.packet_in);
          appdata.packet_in = NOBLOCK;
        }
      else
        Enc28J60Network::resizeBlock(appdata.packet_in,read);
      return read;
    }
  return 0;
}

// Return the next byte from the current packet without moving on to the next byte
int
UIPUDP::peek()
{
  UIPEthernetClass::tick();
  if (appdata.packet_in != NOBLOCK)
    {
      unsigned char c;
      if (Enc28J60Network::readPacket(appdata.packet_in,0,&c,1) == 1)
        return c;
    }
  return -1;
}

// Finish reading the current packet
void
UIPUDP::flush()
{
  UIPEthernetClass::tick();
  Enc28J60Network::freeBlock(appdata.packet_in);
  appdata.packet_in = NOBLOCK;
}

// Return the IP address of the host who sent the current incoming packet
IPAddress
UIPUDP::remoteIP()
{
  return _uip_udp_conn ? ip_addr_uip(_uip_udp_conn->ripaddr) : IPAddress();
}

// Return the port of the host who sent the current incoming packet
uint16_t
UIPUDP::remotePort()
{
  return _uip_udp_conn ? ntohs(_uip_udp_conn->rport) : 0;
}

// uIP callback function

void
uipudp_appcall(void) {
  if (uip_udp_userdata_t *data = (uip_udp_userdata_t *)(uip_udp_conn->appstate))
    {
      if (uip_newdata())
        {
          if (data->packet_next == NOBLOCK)
            {
              uip_udp_conn->rport = UDPBUF->srcport;
              uip_ipaddr_copy(uip_udp_conn->ripaddr,UDPBUF->srcipaddr);
              data->packet_next = Enc28J60Network::allocBlock(ntohs(UDPBUF->udplen)-UIP_UDPH_LEN);
                  //if we are unable to allocate memory the packet is dropped. udp doesn't guarantee packet delivery
              if (data->packet_next != NOBLOCK)
                {
                  //discard Linklevel and IP and udp-header and any trailing bytes:
                  Enc28J60Network::copyPacket(data->packet_next,0,UIPEthernetClass::in_packet,UIP_UDP_PHYH_LEN,Enc28J60Network::blockSize(data->packet_next));
    #ifdef UIPETHERNET_DEBUG_UDP
                  Serial.print(F("udp, uip_newdata received packet: "));
                  Serial.print(data->packet_next);
                  Serial.print(F(", size: "));
                  Serial.println(Enc28J60Network::blockSize(data->packet_next));
    #endif
                }
            }
        }
      if (uip_poll() && data->send)
        {
          //set uip_slen (uip private) by calling uip_udp_send
#ifdef UIPETHERNET_DEBUG_UDP
          Serial.print(F("udp, uip_poll preparing packet to send: "));
          Serial.print(data->packet_out);
          Serial.print(F(", size: "));
          Serial.println(Enc28J60Network::blockSize(data->packet_out));
#endif
          UIPEthernetClass::uip_packet = data->packet_out;
          UIPEthernetClass::uip_hdrlen = UIP_UDP_PHYH_LEN;
          uip_udp_send(data->out_pos - (UIP_UDP_PHYH_LEN));
        }
    }
}

void
UIPUDP::_send(uip_udp_userdata_t *data) {
  uip_arp_out(); //add arp
  if (uip_len == UIP_ARPHDRSIZE)
    {
      UIPEthernetClass::uip_packet = NOBLOCK;
      UIPEthernetClass::packetstate &= ~UIPETHERNET_SENDPACKET;
#ifdef UIPETHERNET_DEBUG_UDP
      Serial.println(F("udp, uip_poll results in ARP-packet"));
#endif
    }
  else
  //arp found ethaddr for ip (otherwise packet is replaced by arp-request)
    {
      data->send = false;
      data->packet_out = NOBLOCK;
      UIPEthernetClass::packetstate |= UIPETHERNET_SENDPACKET;
#ifdef UIPETHERNET_DEBUG_UDP
      Serial.print(F("udp, uip_packet to send: "));
      Serial.println(UIPEthernetClass::uip_packet);
#endif
    }
  UIPEthernetClass::network_send();
}
#endif
