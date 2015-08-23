#ifndef UIPETHERNET_CONF_H
#define UIPETHERNET_CONF_H

/* for TCP */
#define UIP_SOCKET_NUMPACKETS    5
#define UIP_CONF_MAX_CONNECTIONS 4

/* for UDP
 * set UIP_CONF_UDP to 0 to disable UDP (saves aprox. 5kb flash) */
#define UIP_CONF_UDP             1
#define UIP_CONF_BROADCAST       1
#define UIP_CONF_UDP_CONNS       4

/* number of attempts on write before returning number of bytes sent so far
 * set to -1 to block until connection is closed by timeout */
#define UIP_ATTEMPTS_ON_WRITE    -1

/* timeout after which UIPClient::connect gives up. The timeout is specified in seconds.
 * if set to a number <= 0 connect will timeout when uIP does (which might be longer than you expect...) */
#define UIP_CONNECT_TIMEOUT      -1

/* periodic timer for uip (in ms) */
#define UIP_PERIODIC_TIMER       250

/* timer to poll client for data after last write (in ms)
 * set to -1 to disable fast polling and rely on periodic only (saves 100 bytes flash) */
#define UIP_CLIENT_TIMER         10

#endif
