/*
 UIPClient.h - Arduino implementation of a uIP wrapper class.
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

#ifndef UIPCLIENT_H
#define UIPCLIENT_H

#include "ethernet_comp.h"
#include "Print.h"
#import "Client.h"
#import "utility/mempool.h"

extern "C" {
  #import "utility/uip.h"
}

#define UIP_SOCKET_DATALEN UIP_TCP_MSS
//#define UIP_SOCKET_NUMPACKETS UIP_RECEIVE_WINDOW/UIP_TCP_MSS+1
#ifndef UIP_SOCKET_NUMPACKETS
#define UIP_SOCKET_NUMPACKETS 5
#endif

#define UIP_CLIENT_CONNECTED 0x10
#define UIP_CLIENT_CLOSE 0x20
#define UIP_CLIENT_REMOTECLOSED 0x40
#define UIP_CLIENT_RESTART 0x80
#define UIP_CLIENT_STATEFLAGS (UIP_CLIENT_CONNECTED | UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED | UIP_CLIENT_RESTART)
#define UIP_CLIENT_SOCKETS ~UIP_CLIENT_STATEFLAGS

typedef uint8_t uip_socket_ptr;

typedef struct {
  uint8_t state;
  memhandle packets_in[UIP_SOCKET_NUMPACKETS];
  uint16_t lport;        /**< The local TCP port, in network byte order. */
} uip_userdata_closed_t;

typedef struct {
  uint8_t state;
  memhandle packets_in[UIP_SOCKET_NUMPACKETS];
  memhandle packets_out[UIP_SOCKET_NUMPACKETS];
  memaddress out_pos;
#if UIP_CLIENT_TIMER >= 0
  unsigned long timer;
#endif
} uip_userdata_t;

class UIPClient : public Client {

public:
  UIPClient();
  int connect(IPAddress ip, uint16_t port);
  int connect(const char *host, uint16_t port);
  int read(uint8_t *buf, size_t size);
  void stop();
  uint8_t connected();
  operator bool();
  virtual bool operator==(const EthernetClient&);
  virtual bool operator!=(const EthernetClient& rhs) { return !this->operator==(rhs); };

  size_t write(uint8_t);
  size_t write(const uint8_t *buf, size_t size);
  int available();
  int read();
  int peek();
  void flush();

  using Print::write;

private:
  UIPClient(struct uip_conn *_conn);
  UIPClient(uip_userdata_t* conn_data);

  uip_userdata_t* data;

  static uip_userdata_t all_data[UIP_CONNS];
  static uip_userdata_t* _allocateData();

  static size_t _write(uip_userdata_t *,const uint8_t *buf, size_t size);
  static int _available(uip_userdata_t *);

  static uint8_t _currentBlock(memhandle* blocks);
  static void _eatBlock(memhandle* blocks);
  static void _flushBlocks(memhandle* blocks);

#ifdef UIPETHERNET_DEBUG_CLIENT
  static void _dumpAllData();
#endif

  friend class UIPEthernetClass;
  friend class UIPServer;

  friend void uipclient_appcall(void);

};

#endif
