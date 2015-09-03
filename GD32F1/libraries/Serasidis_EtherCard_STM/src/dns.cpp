// DNS look-up functions based on the udp client
// Author: Guido Socher
// Copyright: GPL V2
//
// 2010-05-20 <jc@wippler.nl>

#include "EtherCard_STM.h"
#include "net.h"

#define gPB ether.buffer

static byte dnstid_l; // a counter for transaction ID
#define DNSCLIENT_SRC_PORT_H 0xE0

static void dnsRequest (const char *hostname, bool fromRam) {
    ++dnstid_l; // increment for next request, finally wrap
    if (ether.dnsip[0] == 0)
        memset(ether.dnsip, 8, 4); // use 8.8.8.8 Google DNS as default
    ether.udpPrepare((DNSCLIENT_SRC_PORT_H << 8) | dnstid_l, ether.dnsip, 53);
    memset(gPB + UDP_DATA_P, 0, 12);

    byte *p = gPB + UDP_DATA_P + 12;
    char c;
    do {
        byte n = 0;
        for(;;) {
            c = fromRam ? *hostname : pgm_read_byte(hostname);
            ++hostname;
            if (c == '.' || c == 0)
                break;
            p[++n] = c;
        }
        *p++ = n;
        p += n;
    } while (c != 0);

    *p++ = 0; // terminate with zero, means root domain.
    *p++ = 0;
    *p++ = 1; // type A
    *p++ = 0;
    *p++ = 1; // class IN
    byte i = p - gPB - UDP_DATA_P;
    gPB[UDP_DATA_P] = i;
    gPB[UDP_DATA_P+1] = dnstid_l;
    gPB[UDP_DATA_P+2] = 1; // flags, standard recursive query
    gPB[UDP_DATA_P+5] = 1; // 1 question
    ether.udpTransmit(i);
}

/** @brief  Check if packet is DNS response.
    @param  plen Size of packet
    @return <i>bool</i> True if DNS response has error. False if not DNS response or DNS response OK.
    @note   hisip contains IP address of requested host or 0.0.0.0 on failure
*/
static bool checkForDnsAnswer (uint16_t plen) {
    byte *p = gPB + UDP_DATA_P; //start of UDP payload
    if (plen < 70 || gPB[UDP_SRC_PORT_L_P] != 53 || //from DNS source port
            gPB[UDP_DST_PORT_H_P] != DNSCLIENT_SRC_PORT_H || //response to same port as we sent from (MSB)
            gPB[UDP_DST_PORT_L_P] != dnstid_l || //response to same port as we sent from (LSB)
            p[1] != dnstid_l) //message id same as we sent
        return false; //not our DNS response
    if((p[3] & 0x0F) != 0)
        return true; //DNS response recieved with error

    p += *p; // we encoded the query len into tid
    for (;;) {
        if (*p & 0xC0)
            p += 2;
        else
            while (++p < gPB + plen) {
                if (*p == 0) {
                    ++p;
                    break;
                }
            }
        if (p + 14 > gPB + plen)
            break;
        if (p[1] == 1 && p[9] == 4) { // type "A" and IPv4
            ether.copyIp(ether.hisip, p + 10);
            break;
        }
        p += p[9] + 10;
    }
    return false; //No error
}

// use during setup, as this discards all incoming requests until it returns
bool EtherCard::dnsLookup (const char* name, bool fromRam) {
    uint16_t start = millis();

    while(!isLinkUp())
    {
        if ((uint16_t) (millis() - start) >= 30000)
            return false; //timeout waiting for link
    }
    while(clientWaitingGw())
    {
        packetLoop(packetReceive());
        if ((uint16_t) (millis() - start) >= 30000)
            return false; //timeout waiting for gateway ARP
    }

    memset(hisip, 0, 4);
    dnsRequest(name, fromRam);

    start = millis();
    while (hisip[0] == 0) {
        if ((uint16_t) (millis() - start) >= 30000)
            return false; //timout waiting for dns response
        uint16_t len = packetReceive();
        if (len > 0 && packetLoop(len) == 0) //packet not handled by tcp/ip packet loop
            if(checkForDnsAnswer(len))
                return false; //DNS response recieved with error
    }

    return true;
}
