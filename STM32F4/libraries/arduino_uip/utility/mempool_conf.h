#ifndef MEMPOOLCONF_H
#define MEMPOOLCONF_H
#include "uipethernet-conf.h"
extern "C" {
  #include "uipopt.h"
  #include "enc28j60.h"
}
#include <inttypes.h>

typedef uint16_t memaddress;
typedef uint8_t memhandle;

#if UIP_SOCKET_NUMPACKETS and UIP_CONNS
#define NUM_TCP_MEMBLOCKS (UIP_SOCKET_NUMPACKETS*2)*UIP_CONNS
#else
#define NUM_TCP_MEMBLOCKS 0
#endif

#if UIP_UDP and UIP_UDP_CONNS
#define NUM_UDP_MEMBLOCKS 3*UIP_UDP_CONNS
#else
#define NUM_UDP_MEMBLOCKS 0
#endif

#define MEMPOOL_NUM_MEMBLOCKS (NUM_TCP_MEMBLOCKS+NUM_UDP_MEMBLOCKS)

#define MEMPOOL_STARTADDRESS TXSTART_INIT+1
#define MEMPOOL_SIZE TXSTOP_INIT-TXSTART_INIT

void enc28J60_mempool_block_move_callback(memaddress,memaddress,memaddress);

#define MEMPOOL_MEMBLOCK_MV(dest,src,size) enc28J60_mempool_block_move_callback(dest,src,size)

#endif
