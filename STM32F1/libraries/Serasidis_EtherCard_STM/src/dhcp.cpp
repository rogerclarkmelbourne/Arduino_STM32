// DHCP look-up functions based on the udp client
// http://www.ietf.org/rfc/rfc2131.txt
//
// Author: Andrew Lindsay
// Rewritten and optimized by Jean-Claude Wippler, http://jeelabs.org/
//
// Rewritten dhcpStateMachine by Chris van den Hooven
// as to implement dhcp-renew when lease expires (jun 2012)
//
// Various modifications and bug fixes contributed by Victor Aprea (oct 2012)
//
// Copyright: GPL V2
// See http://www.gnu.org/licenses/gpl.html

//#define DHCPDEBUG

//-----------------------------------------------------------------
//  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
//  Home:  http://www.serasidis.gr
//  email: avrsite@yahoo.gr
//-----------------------------------------------------------------

#include "EtherCard_STM.h"
#include "net.h"

#define gPB ether.buffer

#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTRESPONSE 2

// DHCP Message Type (option 53) (ref RFC 2132)
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_DECLINE 4
#define DHCP_ACK 5
#define DHCP_NAK 6
#define DHCP_RELEASE 7

// DHCP States for access in applications (ref RFC 2131)
enum {
    DHCP_STATE_INIT,
    DHCP_STATE_SELECTING,
    DHCP_STATE_REQUESTING,
    DHCP_STATE_BOUND,
    DHCP_STATE_RENEWING,
};

/*
   op            1  Message op code / message type.
                    1 = BOOTREQUEST, 2 = BOOTREPLY
   htype         1  Hardware address type, see ARP section in "Assigned
                    Numbers" RFC; e.g., '1' = 10mb ethernet.
   hlen          1  Hardware address length (e.g.  '6' for 10mb
                    ethernet).
   hops          1  Client sets to zero, optionally used by relay agents
                    when booting via a relay agent.
   xid           4  Transaction ID, a random number chosen by the
                    client, used by the client and server to associate
                    messages and responses between a client and a
                    server.
   secs          2  Filled in by client, seconds elapsed since client
                    began address acquisition or renewal process.
   flags         2  Flags (see figure 2).
   ciaddr        4  Client IP address; only filled in if client is in
                    BOUND, RENEW or REBINDING state and can respond
                    to ARP requests.
   yiaddr        4  'your' (client) IP address.
   siaddr        4  IP address of next server to use in bootstrap;
                    returned in DHCPOFFER, DHCPACK by server.
   giaddr        4  Relay agent IP address, used in booting via a
                    relay agent.
   chaddr       16  Client hardware address.
   sname        64  Optional server host name, null terminated string.
   file        128  Boot file name, null terminated string; "generic"
                    name or null in DHCPDISCOVER, fully qualified
                    directory-path name in DHCPOFFER.
   options     var  Optional parameters field.  See the options
                    documents for a list of defined options.
 */




// size 236
typedef struct {
    byte op, htype, hlen, hops;
    uint32_t xid;
    uint16_t secs, flags;
    byte ciaddr[4], yiaddr[4], siaddr[4], giaddr[4];
    byte chaddr[16], sname[64], file[128];
    // options
} DHCPdata;

#define DHCP_SRC_PORT 67
#define DHCP_DEST_PORT 68

// timeouts im ms
#define DHCP_REQUEST_TIMEOUT 10000

#define DHCP_HOSTNAME_MAX_LEN 32

static byte dhcpState = DHCP_STATE_INIT;
static char hostname[DHCP_HOSTNAME_MAX_LEN] = "Arduino-00";
static uint32_t currentXid;
static uint32_t stateTimer;
static uint32_t leaseStart;
static uint32_t leaseTime;
static byte* bufPtr;

static uint8_t dhcpCustomOptionNum = 0;
static DhcpOptionCallback dhcpCustomOptionCallback = NULL;

// static uint8_t allOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static void addToBuf (byte b) {
    *bufPtr++ = b;
}

static void addBytes (byte len, const byte* data) {
    while (len-- > 0)
        addToBuf(*data++);
}


// Main DHCP sending function

// implemented
// state             / msgtype
// INIT              / DHCPDISCOVER
// SELECTING         / DHCPREQUEST
// BOUND (RENEWING)  / DHCPREQUEST

// ----------------------------------------------------------
// |              |SELECTING    |RENEWING     |INIT         |
// ----------------------------------------------------------
// |broad/unicast |broadcast    |unicast      |broadcast    |
// |server-ip     |MUST         |MUST NOT     |MUST NOT     | option 54
// |requested-ip  |MUST         |MUST NOT     |MUST NOT     | option 50
// |ciaddr        |zero         |IP address   |zero         |
// ----------------------------------------------------------

// options used (both send/receive)
// 12  Host Name Option
// 50  Requested IP Address
// 51  IP Address Lease Time
// 53  DHCP message type
// 54  Server-identifier
// 55  Parameter request list
// 58  Renewal (T1) Time Value
// 61  Client-identifier
// 255 End

static void send_dhcp_message (void) {

    uint8_t allOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    memset(gPB, 0, UDP_DATA_P + sizeof( DHCPdata ));

    EtherCard::udpPrepare(DHCP_DEST_PORT,
                          (dhcpState == DHCP_STATE_BOUND ? EtherCard::dhcpip : allOnes),
                          DHCP_SRC_PORT);   // SRC<->DST ??

    if (dhcpState != DHCP_STATE_BOUND)
        EtherCard::copyMac(gPB + ETH_DST_MAC, allOnes); //force broadcast mac

    // Build DHCP Packet from buf[UDP_DATA_P]
    DHCPdata *dhcpPtr = (DHCPdata*) (gPB + UDP_DATA_P);
    dhcpPtr->op = DHCP_BOOTREQUEST;
    dhcpPtr->htype = 1;
    dhcpPtr->hlen = 6;
    dhcpPtr->xid = currentXid;
    if (dhcpState == DHCP_STATE_BOUND) {
        EtherCard::copyIp(dhcpPtr->ciaddr, EtherCard::myip);
    }
    EtherCard::copyMac(dhcpPtr->chaddr, EtherCard::mymac);

    // options defined as option, length, value
    bufPtr = gPB + UDP_DATA_P + sizeof( DHCPdata );
    // DHCP magic cookie, followed by message type
    static byte cookie[] = { 99, 130, 83, 99, 53, 1 };
    addBytes(sizeof cookie, cookie);
    // addToBuf(53);  // DHCP_STATE_SELECTING, DHCP_STATE_REQUESTING
    // addToBuf(1);   // Length
    addToBuf(dhcpState == DHCP_STATE_INIT ? DHCP_DISCOVER : DHCP_REQUEST);

    // Client Identifier Option, this is the client mac address
    addToBuf(61);     // Client identifier
    addToBuf(7);      // Length
    addToBuf(0x01);   // Ethernet
    addBytes(6, EtherCard::mymac);

    addToBuf(12);     // Host name Option
    addToBuf(10);
    addBytes(10, (byte*) hostname);

    if( dhcpState == DHCP_STATE_SELECTING) {
        addToBuf(50); // Request IP address
        addToBuf(4);
        addBytes(4, EtherCard::myip);

        // Request using server ip address
        addToBuf(54); // Server IP address
        addToBuf(4);
        addBytes(4, EtherCard::dhcpip);
    }

    // Additional info in parameter list - minimal list for what we need
    byte len = 3;
    if (dhcpCustomOptionNum)
        len++;
    addToBuf(55);     // Parameter request list
    addToBuf(len);    // Length
    addToBuf(1);      // Subnet mask
    addToBuf(3);      // Route/Gateway
    addToBuf(6);      // DNS Server
    if (dhcpCustomOptionNum)
        addToBuf(dhcpCustomOptionNum);  // Custom option
    addToBuf(255);    // end option

    // packet size will be under 300 bytes
    EtherCard::udpTransmit((bufPtr - gPB) - UDP_DATA_P);
}

static void process_dhcp_offer (uint16_t len) {
    // Map struct onto payload
    DHCPdata *dhcpPtr = (DHCPdata*) (gPB + UDP_DATA_P);
    // Offered IP address is in yiaddr
    EtherCard::copyIp(EtherCard::myip, dhcpPtr->yiaddr);
    // Scan through variable length option list identifying options we want
    byte *ptr = (byte*) (dhcpPtr + 1) + 4;
    bool done = false;
    do {
        byte option = *ptr++;
        byte optionLen = *ptr++;
        switch (option) {
            case 1:   EtherCard::copyIp(EtherCard::netmask, ptr);
                      break;
            case 3:   EtherCard::copyIp(EtherCard::gwip, ptr);
                      break;
            case 6:   EtherCard::copyIp(EtherCard::dnsip, ptr);
                      break;
            case 51:
            case 58:  leaseTime = 0; // option 58 = Renewal Time, 51 = Lease Time
                      for (byte i = 0; i<4; i++)
                          leaseTime = (leaseTime << 8) + ptr[i];
                      leaseTime *= 1000;      // milliseconds
                      break;
            case 54:  EtherCard::copyIp(EtherCard::dhcpip, ptr);
                      break;
            case 255: done = true;
                      break;
            default: {
                // Is is a custom configured option?
                if (dhcpCustomOptionCallback && option == dhcpCustomOptionNum) {
                    dhcpCustomOptionCallback(option, ptr, optionLen);
                }
            }
        }
        ptr += optionLen;
    } while (!done && ptr < gPB + len);
}

static bool dhcp_received_message_type (uint16_t len, byte msgType) {
    // Map struct onto payload
    DHCPdata *dhcpPtr = (DHCPdata*) (gPB + UDP_DATA_P);

    if (len >= 70 && gPB[UDP_SRC_PORT_L_P] == DHCP_SRC_PORT &&
            dhcpPtr->xid == currentXid ) {

        byte *ptr = (byte*) (dhcpPtr + 1) + 4;
        do {
            byte option = *ptr++;
            byte optionLen = *ptr++;
            if(option == 53 && *ptr == msgType ) {
                // DHCP Message type match found
                return true;
            }
            ptr += optionLen;
        } while (ptr < gPB + len);
    }
    return false;
}

bool EtherCard::dhcpSetup (const char *hname, bool fromRam) {
	 // Use during setup, as this discards all incoming requests until it returns.
	 // That shouldn't be a problem, because we don't have an IP-address yet.
	 // Will try 60 secs to obtain DHCP-lease.

	 using_dhcp = true;
    
   if(hname != NULL){   
     if(fromRam){
    	 strncpy(hostname, hname, DHCP_HOSTNAME_MAX_LEN);
     }
     else{
       strncpy(hostname, hname, DHCP_HOSTNAME_MAX_LEN);
     }
   }
   else{
     // Set a unique hostname, use Arduino-?? with last octet of mac address
     hostname[8] = '0' + (mymac[5] >> 4);
     hostname[9] = '0' + (mymac[5] & 0x0F);   
   }

	 dhcpState = DHCP_STATE_INIT;
	 uint16_t start = millis();	

   while (dhcpState != DHCP_STATE_BOUND && (uint16_t) (millis() - start) < 60000) {
       if (isLinkUp()) DhcpStateMachine(packetReceive());
   }
   updateBroadcastAddress();
   delaycnt = 0;
   return dhcpState == DHCP_STATE_BOUND ;
}

void EtherCard::dhcpAddOptionCallback(uint8_t option, DhcpOptionCallback callback)
{
    dhcpCustomOptionNum = option;
    dhcpCustomOptionCallback = callback;
}

void EtherCard::DhcpStateMachine (uint16_t len) {

#ifdef DHCPDEBUG
    if (dhcpState != DHCP_STATE_BOUND) {
        Serial.print(millis());
        Serial.print(" State: ");
    }
    switch (dhcpState) {
    case DHCP_STATE_INIT:
        Serial.println("Init");
        break;
    case DHCP_STATE_SELECTING:
        Serial.println("Selecting");
        break;
    case DHCP_STATE_REQUESTING:
        Serial.println("Requesting");
        break;
    case DHCP_STATE_RENEWING:
        Serial.println("Renew");
        break;
    }
#endif

    switch (dhcpState) {

    case DHCP_STATE_BOUND:
        //!@todo Due to millis() 49 day wrap-around, DHCP renewal may not work as expected
        if (millis() >= leaseStart + leaseTime) {
            send_dhcp_message();
            dhcpState = DHCP_STATE_RENEWING;
            stateTimer = millis();
        }
        break;

    case DHCP_STATE_INIT:
        currentXid = millis();
        memset(myip,0,4); // force ip 0.0.0.0
        send_dhcp_message();
        enableBroadcast(true); //Temporarily enable broadcasts
        dhcpState = DHCP_STATE_SELECTING;
        stateTimer = millis();
        break;

    case DHCP_STATE_SELECTING:
        if (dhcp_received_message_type(len, DHCP_OFFER)) {
            process_dhcp_offer(len);
            send_dhcp_message();
            dhcpState = DHCP_STATE_REQUESTING;
            stateTimer = millis();
        } else {
            if (millis() > stateTimer + DHCP_REQUEST_TIMEOUT) {
                dhcpState = DHCP_STATE_INIT;
            }
        }
        break;

    case DHCP_STATE_REQUESTING:
    case DHCP_STATE_RENEWING:
        if (dhcp_received_message_type(len, DHCP_ACK)) {
            disableBroadcast(true); //Disable broadcast after temporary enable
            leaseStart = millis();
            if (gwip[0] != 0) setGwIp(gwip); // why is this? because it initiates an arp request
            dhcpState = DHCP_STATE_BOUND;
        } else {
            if (millis() > stateTimer + DHCP_REQUEST_TIMEOUT) {
                dhcpState = DHCP_STATE_INIT;
            }
        }
        break;

    }
}



