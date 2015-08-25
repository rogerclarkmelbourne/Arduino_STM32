/*
 UIPClient.cpp - Arduino implementation of a uIP wrapper class.
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

extern "C"
{
#import "utility/uip-conf.h"
#import "utility/uip.h"
#import "utility/uip_arp.h"
#import "string.h"
}
#include "UIPEthernet.h"
#include "UIPClient.h"
#include "Dns.h"

#ifdef UIPETHERNET_DEBUG_CLIENT
#include "HardwareSerial.h"
#endif

#define Serial SerialUSB


#define UIP_TCP_PHYH_LEN UIP_LLH_LEN+UIP_IPTCPH_LEN

uip_userdata_t UIPClient::all_data[UIP_CONNS];

UIPClient::UIPClient() :
    data(NULL)
{
}

UIPClient::UIPClient(uip_userdata_t* conn_data) :
    data(conn_data)
{
}

int
UIPClient::connect(IPAddress ip, uint16_t port)
{
  stop();
  uip_ipaddr_t ipaddr;
  uip_ip_addr(ipaddr, ip);
  struct uip_conn* conn = uip_connect(&ipaddr, htons(port));
  if (conn)
    {
#if UIP_CONNECT_TIMEOUT > 0
      int32_t timeout = millis() + 1000 * UIP_CONNECT_TIMEOUT;
#endif
      while((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED)
        {
          UIPEthernetClass::tick();
          if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED)
            {
              data = (uip_userdata_t*) conn->appstate;
#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.print(F("connected, state: "));
              Serial.print(data->state);
              Serial.print(F(", first packet in: "));
              Serial.println(data->packets_in[0]);
#endif
              return 1;
            }
#if UIP_CONNECT_TIMEOUT > 0
          if (((int32_t)(millis() - timeout)) > 0)
            {
              conn->tcpstateflags = UIP_CLOSED;
              break;
            }
#endif
        }
    }
  return 0;
}

int
UIPClient::connect(const char *host, uint16_t port)
{
  // Look up the host first
  int ret = 0;
#if UIP_UDP
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(UIPEthernetClass::_dnsServerAddress);
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return connect(remote_addr, port);
  }
#endif
  return ret;
}

void
UIPClient::stop()
{
  if (data && data->state)
    {
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.println(F("before stop(), with data"));
      _dumpAllData();
#endif
      _flushBlocks(&data->packets_in[0]);
      if (data->state & UIP_CLIENT_REMOTECLOSED)
        {
          data->state = 0;
        }
      else
        {
          data->state |= UIP_CLIENT_CLOSE;
        }
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.println(F("after stop()"));
      _dumpAllData();
#endif
    }
#ifdef UIPETHERNET_DEBUG_CLIENT
  else
    {
      Serial.println(F("stop(), data: NULL"));
    }
#endif
  data = NULL;
  UIPEthernetClass::tick();
}

uint8_t
UIPClient::connected()
{
  return (data && (data->packets_in[0] != NOBLOCK || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
}

bool
UIPClient::operator==(const UIPClient& rhs)
{
  return data && rhs.data && (data == rhs.data);
}

UIPClient::operator bool()
{
  UIPEthernetClass::tick();
  return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in[0] != NOBLOCK);
}

size_t
UIPClient::write(uint8_t c)
{
  return _write(data, &c, 1);
}

size_t
UIPClient::write(const uint8_t *buf, size_t size)
{
  return _write(data, buf, size);
}

size_t
UIPClient::_write(uip_userdata_t* u, const uint8_t *buf, size_t size)
{
  int remain = size;
  uint16_t written;
#if UIP_ATTEMPTS_ON_WRITE > 0
  uint16_t attempts = UIP_ATTEMPTS_ON_WRITE;
#endif
  repeat:
  UIPEthernetClass::tick();
  if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
    {
      uint8_t p = _currentBlock(&u->packets_out[0]);
      if (u->packets_out[p] == NOBLOCK)
        {
newpacket:
          u->packets_out[p] = Enc28J60Network::allocBlock(UIP_SOCKET_DATALEN);
          if (u->packets_out[p] == NOBLOCK)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
              if ((--attempts)>0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                goto repeat;
#endif
              goto ready;
            }
          u->out_pos = 0;
        }
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.print(F("UIPClient.write: writePacket("));
      Serial.print(u->packets_out[p]);
      Serial.print(F(") pos: "));
      Serial.print(u->out_pos);
      Serial.print(F(", buf["));
      Serial.print(size-remain);
      Serial.print(F("-"));
      Serial.print(remain);
      Serial.print(F("]: '"));
      Serial.write((uint8_t*)buf+size-remain,remain);
      Serial.println(F("'"));
#endif
      written = Enc28J60Network::writePacket(u->packets_out[p],u->out_pos,(uint8_t*)buf+size-remain,remain);
      remain -= written;
      u->out_pos+=written;
      if (remain > 0)
        {
          if (p == UIP_SOCKET_NUMPACKETS-1)
            {
#if UIP_ATTEMPTS_ON_WRITE > 0
              if ((--attempts)>0)
#endif
#if UIP_ATTEMPTS_ON_WRITE != 0
                goto repeat;
#endif
              goto ready;
            }
          p++;
          goto newpacket;
        }
ready:
#if UIP_CLIENT_TIMER >= 0
      u->timer = millis()+UIP_CLIENT_TIMER;
#endif
      return size-remain;
    }
  return -1;
}

int
UIPClient::available()
{
  if (*this)
    return _available(data);
  return 0;
}

int
UIPClient::_available(uip_userdata_t *u)
{
  int len = 0;
  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
    {
      len += Enc28J60Network::blockSize(u->packets_in[i]);
    }
  return len;
}

int
UIPClient::read(uint8_t *buf, size_t size)
{
  if (*this)
    {
      uint16_t remain = size;
      if (data->packets_in[0] == NOBLOCK)
        return 0;
      uint16_t read;
      do
        {
          read = Enc28J60Network::readPacket(data->packets_in[0],0,buf+size-remain,remain);
          if (read == Enc28J60Network::blockSize(data->packets_in[0]))
            {
              remain -= read;
              _eatBlock(&data->packets_in[0]);
              if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
                data->state |= UIP_CLIENT_RESTART;
              if (data->packets_in[0] == NOBLOCK)
                {
                  if (data->state & UIP_CLIENT_REMOTECLOSED)
                    {
                      data->state = 0;
                      data = NULL;
                    }
                  return size-remain;
                }
            }
          else
            {
              Enc28J60Network::resizeBlock(data->packets_in[0],read);
              break;
            }
        }
      while(remain > 0);
      return size;
    }
  return -1;
}

int
UIPClient::read()
{
  uint8_t c;
  if (read(&c,1) < 0)
    return -1;
  return c;
}

int
UIPClient::peek()
{
  if (*this)
    {
      if (data->packets_in[0] != NOBLOCK)
        {
          uint8_t c;
          Enc28J60Network::readPacket(data->packets_in[0],0,&c,1);
          return c;
        }
    }
  return -1;
}

void
UIPClient::flush()
{
  if (*this)
    {
      _flushBlocks(&data->packets_in[0]);
    }
}

void
uipclient_appcall(void)
{
  uint16_t send_len = 0;
  uip_userdata_t *u = (uip_userdata_t*)uip_conn->appstate;
  if (!u && uip_connected())
    {
#ifdef UIPETHERNET_DEBUG_CLIENT
      Serial.println(F("UIPClient uip_connected"));
      UIPClient::_dumpAllData();
#endif
      u = (uip_userdata_t*) UIPClient::_allocateData();
      if (u)
        {
          uip_conn->appstate = u;
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient allocated state: "));
          Serial.println(u->state,BIN);
#endif
        }
#ifdef UIPETHERNET_DEBUG_CLIENT
      else
        Serial.println(F("UIPClient allocation failed"));
#endif
    }
  if (u)
    {
      if (uip_newdata())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.print(F("UIPClient uip_newdata, uip_len:"));
          Serial.println(uip_len);
#endif
          if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
              for (uint8_t i=0; i < UIP_SOCKET_NUMPACKETS; i++)
                {
                  if (u->packets_in[i] == NOBLOCK)
                    {
                      u->packets_in[i] = Enc28J60Network::allocBlock(uip_len);
                      if (u->packets_in[i] != NOBLOCK)
                        {
                          Enc28J60Network::copyPacket(u->packets_in[i],0,UIPEthernetClass::in_packet,((uint8_t*)uip_appdata)-uip_buf,uip_len);
                          if (i == UIP_SOCKET_NUMPACKETS-1)
                            uip_stop();
                          goto finish_newdata;
                        }
                    }
                }
              UIPEthernetClass::packetstate &= ~UIPETHERNET_FREEPACKET;
              uip_stop();
            }
        }
finish_newdata:
      if (u->state & UIP_CLIENT_RESTART)
        {
          u->state &= ~UIP_CLIENT_RESTART;
          uip_restart();
        }
      // If the connection has been closed, save received but unread data.
      if (uip_closed() || uip_timedout())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient uip_closed"));
          UIPClient::_dumpAllData();
#endif
          // drop outgoing packets not sent yet:
          UIPClient::_flushBlocks(&u->packets_out[0]);
          if (u->packets_in[0] != NOBLOCK)
            {
              ((uip_userdata_closed_t *)u)->lport = uip_conn->lport;
              u->state |= UIP_CLIENT_REMOTECLOSED;
            }
          else
            u->state = 0;
          // disassociate appdata.
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("after UIPClient uip_closed"));
          UIPClient::_dumpAllData();
#endif
          uip_conn->appstate = NULL;
          goto finish;
        }
      if (uip_acked())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient uip_acked"));
#endif
          UIPClient::_eatBlock(&u->packets_out[0]);
        }
      if (uip_poll() || uip_rexmit())
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          //Serial.println(F("UIPClient uip_poll"));
#endif
          if (u->packets_out[0] != NOBLOCK)
            {
              if (u->packets_out[1] == NOBLOCK)
                {
                  send_len = u->out_pos;
                  if (send_len > 0)
                    {
                      Enc28J60Network::resizeBlock(u->packets_out[0],0,send_len);
                    }
                }
              else
                send_len = Enc28J60Network::blockSize(u->packets_out[0]);
              if (send_len > 0)
                {
                  UIPEthernetClass::uip_hdrlen = ((uint8_t*)uip_appdata)-uip_buf;
                  UIPEthernetClass::uip_packet = Enc28J60Network::allocBlock(UIPEthernetClass::uip_hdrlen+send_len);
                  if (UIPEthernetClass::uip_packet != NOBLOCK)
                    {
                      Enc28J60Network::copyPacket(UIPEthernetClass::uip_packet,UIPEthernetClass::uip_hdrlen,u->packets_out[0],0,send_len);
                      UIPEthernetClass::packetstate |= UIPETHERNET_SENDPACKET;
                    }
                }
              goto finish;
            }
        }
      // don't close connection unless all outgoing packets are sent
      if (u->state & UIP_CLIENT_CLOSE)
        {
#ifdef UIPETHERNET_DEBUG_CLIENT
          Serial.println(F("UIPClient state UIP_CLIENT_CLOSE"));
          UIPClient::_dumpAllData();
#endif
          if (u->packets_out[0] == NOBLOCK)
            {
              u->state = 0;
              uip_conn->appstate = NULL;
              uip_close();
#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.println(F("no blocks out -> free userdata"));
              UIPClient::_dumpAllData();
#endif
            }
          else
            {
              uip_stop();
#ifdef UIPETHERNET_DEBUG_CLIENT
              Serial.println(F("blocks outstanding transfer -> uip_stop()"));
#endif
            }
        }
    }
  finish:
  uip_send(uip_appdata,send_len);
  uip_len = send_len;
}

uip_userdata_t *
UIPClient::_allocateData()
{
  for ( uint8_t sock = 0; sock < UIP_CONNS; sock++ )
    {
      uip_userdata_t* data = &UIPClient::all_data[sock];
      if (!data->state)
        {
          data->state = sock | UIP_CLIENT_CONNECTED;
          memset(&data->packets_in[0],0,sizeof(uip_userdata_t)-sizeof(data->state));
          return data;
        }
    }
  return NULL;
}

uint8_t
UIPClient::_currentBlock(memhandle* block)
{
  for (uint8_t i = 1; i < UIP_SOCKET_NUMPACKETS; i++)
    {
      if (block[i] == NOBLOCK)
        return i-1;
    }
  return UIP_SOCKET_NUMPACKETS-1;
}

void
UIPClient::_eatBlock(memhandle* block)
{
#ifdef UIPETHERNET_DEBUG_CLIENT
  memhandle* start = block;
  Serial.print(F("eatblock("));
  Serial.print(*block);
  Serial.print(F("): "));
  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
    {
      Serial.print(start[i]);
      Serial.print(F(" "));
    }
  Serial.print(F("-> "));
#endif
  Enc28J60Network::freeBlock(block[0]);
  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS-1; i++)
    {
      block[i] = block[i+1];
    }
  block[UIP_SOCKET_NUMPACKETS-1] = NOBLOCK;
#ifdef UIPETHERNET_DEBUG_CLIENT
  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
    {
      Serial.print(start[i]);
      Serial.print(F(" "));
    }
  Serial.println();
#endif
}

void
UIPClient::_flushBlocks(memhandle* block)
{
  for (uint8_t i = 0; i < UIP_SOCKET_NUMPACKETS; i++)
    {
      Enc28J60Network::freeBlock(block[i]);
      block[i] = NOBLOCK;
    }
}

#ifdef UIPETHERNET_DEBUG_CLIENT
void
UIPClient::_dumpAllData() {
  for (uint8_t i=0; i < UIP_CONNS; i++)
    {
      Serial.print(F("UIPClient::all_data["));
      Serial.print(i);
      Serial.print(F("], state:"));
      Serial.println(all_data[i].state, BIN);
      Serial.print(F("packets_in: "));
      for (uint8_t j=0; j < UIP_SOCKET_NUMPACKETS; j++)
        {
          Serial.print(all_data[i].packets_in[j]);
          Serial.print(F(" "));
        }
      Serial.println();
      if (all_data[i].state & UIP_CLIENT_REMOTECLOSED)
        {
          Serial.print(F("state remote closed, local port: "));
          Serial.println(htons(((uip_userdata_closed_t *)(&all_data[i]))->lport));
        }
      else
        {
          Serial.print(F("packets_out: "));
          for (uint8_t j=0; j < UIP_SOCKET_NUMPACKETS; j++)
            {
              Serial.print(all_data[i].packets_out[j]);
              Serial.print(F(" "));
            }
          Serial.println();
          Serial.print(F("out_pos: "));
          Serial.println(all_data[i].out_pos);
        }
    }
}
#endif
