// Simple UDP listening server
//
// Author: Brian Lee
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html

#include "EtherCard_STM.h"
#include "net.h"

#define gPB ether.buffer

#define UDPSERVER_MAXLISTENERS 8    //the maximum number of port listeners.

typedef struct {
    UdpServerCallback callback;
    uint16_t port;
    bool listening;
} UdpServerListener;

UdpServerListener listeners[UDPSERVER_MAXLISTENERS];
byte numListeners = 0;

void EtherCard::udpServerListenOnPort(UdpServerCallback callback, uint16_t port) {
    if(numListeners < UDPSERVER_MAXLISTENERS)
    {
        listeners[numListeners] = (UdpServerListener) {
            callback, port, true
        };
        numListeners++;
    }
}

void EtherCard::udpServerPauseListenOnPort(uint16_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = false;
        }
    }
}

void EtherCard::udpServerResumeListenOnPort(uint16_t port) {
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port)) {
            listeners[i].listening = true;
        }
    }
}

bool EtherCard::udpServerListening() {
    return numListeners > 0;
}

bool EtherCard::udpServerHasProcessedPacket(uint16_t plen) {
    bool packetProcessed = false;
    for(int i = 0; i < numListeners; i++)
    {
        if(gPB[UDP_DST_PORT_H_P] == (listeners[i].port >> 8) && gPB[UDP_DST_PORT_L_P] == ((byte) listeners[i].port) && listeners[i].listening)
        {
            uint16_t datalen = (uint16_t) (gPB[UDP_LEN_H_P] << 8)  + gPB[UDP_LEN_L_P] - UDP_HEADER_LEN;
            listeners[i].callback(
                listeners[i].port,
                gPB + IP_SRC_P,
                (gPB[UDP_SRC_PORT_H_P] << 8) | gPB[UDP_SRC_PORT_L_P],
                (const char *) (gPB + UDP_DATA_P),
                datalen);
            packetProcessed = true;
        }
    }
    return packetProcessed;
}
