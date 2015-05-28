// IP, ARP, UDP and TCP functions.
// Author: Guido Socher
// Copyright: GPL V2
//
// The TCP implementation uses some size optimisations which are valid
// only if all data can be sent in one single packet. This is however
// not a big limitation for a microcontroller as you will anyhow use
// small web-pages. The web server must send the entire web page in one
// packet. The client "web browser" as implemented here can also receive
// large pages.
//
// 2010-05-20 <jc@wippler.nl>

#include "EtherCard_STM.h"
#include "net.h"
#undef word // arduino nonsense

#define gPB ether.buffer

#define PINGPATTERN 0x42

// Avoid spurious pgmspace warnings - http://forum.jeelabs.net/node/327
// See also http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
//#undef PROGMEM
//#define PROGMEM __attribute__(( section(".progmem.data") ))
//#undef PSTR
//#define PSTR(s) (__extension__({static prog_char c[] PROGMEM = (s); &c[0];}))

#define TCPCLIENT_SRC_PORT_H 11 //Source port (MSB) for TCP/IP client connections - hardcode all TCP/IP client connection from ports in range 2816-3071
static uint8_t tcpclient_src_port_l=1; // Source port (LSB) for tcp/ip client connections - increments on each TCP/IP request
static uint8_t tcp_fd; // a file descriptor, will be encoded into the port
static uint8_t tcp_client_state; //TCP connection state: 1=Send SYN, 2=SYN sent awaiting SYN+ACK, 3=Established, 4=Not used, 5=Closing, 6=Closed
static uint8_t tcp_client_port_h; // Destination port (MSB) of TCP/IP client connection
static uint8_t tcp_client_port_l; // Destination port (LSB) of TCP/IP client connection
static uint8_t (*client_tcp_result_cb)(uint8_t,uint8_t,uint16_t,uint16_t); // Pointer to callback function to handle response to current TCP/IP request
static uint16_t (*client_tcp_datafill_cb)(uint8_t); //Pointer to callback function to handle payload data in response to current TCP/IP request
static uint8_t www_fd; // ID of current http request (only one http request at a time - one of the 8 possible concurrent TCP/IP connections)
static void (*client_browser_cb)(uint8_t,uint16_t,uint16_t); // Pointer to callback function to handle result of current HTTP request
static const char *client_additionalheaderline; // Pointer to c-string additional http request header info
static const char *client_postval;
static const char *client_urlbuf; // Pointer to c-string path part of HTTP request URL
static const char *client_urlbuf_var; // Pointer to c-string filename part of HTTP request URL
static const char *client_hoststr; // Pointer to c-string hostname of current HTTP request
static void (*icmp_cb)(uint8_t *ip); // Pointer to callback function for ICMP ECHO response handler (triggers when localhost recieves ping respnse (pong))
static uint8_t destmacaddr[6]; // storing both dns server and destination mac addresses, but at different times because both are never needed at same time.
static boolean waiting_for_dns_mac = false; //might be better to use bit flags and bitmask operations for these conditions
static boolean has_dns_mac = false;
static boolean waiting_for_dest_mac = false;
static boolean has_dest_mac = false;
static uint8_t gwmacaddr[6]; // Hardware (MAC) address of gateway router
static uint8_t waitgwmac; // Bitwise flags of gateway router status - see below for states
//Define gatweay router ARP statuses
#define WGW_INITIAL_ARP 1 // First request, no answer yet
#define WGW_HAVE_GW_MAC 2 // Have gateway router MAC
#define WGW_REFRESHING 4 // Refreshing but already have gateway MAC
#define WGW_ACCEPT_ARP_REPLY 8 // Accept an ARP reply

static uint16_t info_data_len; // Length of TCP/IP payload
static uint8_t seqnum = 0xa; // My initial tcp sequence number
static uint8_t result_fd = 123; // Session id of last reply
static const char* result_ptr; // Pointer to TCP/IP data
static unsigned long SEQ; // TCP/IP sequence number

#define CLIENTMSS 550
#define TCP_DATA_START ((uint16_t)TCP_SRC_PORT_H_P+(gPB[TCP_HEADER_LEN_P]>>4)*4) // Get offset of TCP/IP payload data

const unsigned char arpreqhdr[] PROGMEM = { 0,1,8,0,6,4,0,1 }; // ARP request header
const unsigned char iphdr[] PROGMEM = { 0x45,0,0,0x82,0,0,0x40,0,0x20 }; //IP header
const unsigned char ntpreqhdr[] PROGMEM = { 0xE3,0,4,0xFA,0,1,0,0,0,1 }; //NTP request header
const uint8_t allOnes[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // Used for hardware (MAC) and IP broadcast addresses

static void fill_checksum(uint8_t dest, uint8_t off, uint16_t len,uint8_t type) {
    const uint8_t* ptr = gPB + off;
    uint32_t sum = type==1 ? IP_PROTO_UDP_V+len-8 :
                   type==2 ? IP_PROTO_TCP_V+len-8 : 0;
    while(len >1) {
        sum += (uint16_t) (((uint32_t)*ptr<<8)|*(ptr+1));
        ptr+=2;
        len-=2;
    }
    if (len)
        sum += ((uint32_t)*ptr)<<8;
    while (sum>>16)
        sum = (uint16_t) sum + (sum >> 16);
    uint16_t ck = ~ (uint16_t) sum;
    gPB[dest] = ck>>8;
    gPB[dest+1] = ck;
}

static void setMACs (const uint8_t *mac) {
    EtherCard::copyMac(gPB + ETH_DST_MAC, mac);
    EtherCard::copyMac(gPB + ETH_SRC_MAC, EtherCard::mymac);
}

static void setMACandIPs (const uint8_t *mac, const uint8_t *dst) {
    setMACs(mac);
    EtherCard::copyIp(gPB + IP_DST_P, dst);
    EtherCard::copyIp(gPB + IP_SRC_P, EtherCard::myip);
}

static uint8_t check_ip_message_is_from(const uint8_t *ip) {
    return memcmp(gPB + IP_SRC_P, ip, 4) == 0;
}

static boolean is_lan(const uint8_t source[4], const uint8_t destination[4]) {
	if(source[0] == 0 || destination[0] == 0) {
		return false;
	}
	for(int i = 0; i < 4; i++)
		if((source[i] & EtherCard::netmask[i]) != (destination[i] & EtherCard::netmask[i])) {
			return false;
		}
	return true;
}

static uint8_t eth_type_is_arp_and_my_ip(uint16_t len) {
    return len >= 41 && gPB[ETH_TYPE_H_P] == ETHTYPE_ARP_H_V &&
           gPB[ETH_TYPE_L_P] == ETHTYPE_ARP_L_V &&
           memcmp(gPB + ETH_ARP_DST_IP_P, EtherCard::myip, 4) == 0;
}

static uint8_t eth_type_is_ip_and_my_ip(uint16_t len) {
    return len >= 42 && gPB[ETH_TYPE_H_P] == ETHTYPE_IP_H_V &&
           gPB[ETH_TYPE_L_P] == ETHTYPE_IP_L_V &&
           gPB[IP_HEADER_LEN_VER_P] == 0x45 &&
           (memcmp(gPB + IP_DST_P, EtherCard::myip, 4) == 0  //not my IP
            || (memcmp(gPB + IP_DST_P, EtherCard::broadcastip, 4) == 0) //not subnet broadcast
            || (memcmp(gPB + IP_DST_P, allOnes, 4) == 0)); //not global broadcasts
    //!@todo Handle multicast
}

static void fill_ip_hdr_checksum() {
    gPB[IP_CHECKSUM_P] = 0;
    gPB[IP_CHECKSUM_P+1] = 0;
    gPB[IP_FLAGS_P] = 0x40; // don't fragment
    gPB[IP_FLAGS_P+1] = 0;  // fragement offset
    gPB[IP_TTL_P] = 64; // ttl
    fill_checksum(IP_CHECKSUM_P, IP_P, IP_HEADER_LEN,0);
}

static void make_eth_ip() {
    setMACs(gPB + ETH_SRC_MAC);
    EtherCard::copyIp(gPB + IP_DST_P, gPB + IP_SRC_P);
    EtherCard::copyIp(gPB + IP_SRC_P, EtherCard::myip);
    fill_ip_hdr_checksum();
}

static void step_seq(uint16_t rel_ack_num,uint8_t cp_seq) {
    uint8_t i;
    uint8_t tseq;
    i = 4;
    while(i>0) {
        rel_ack_num = gPB[TCP_SEQ_H_P+i-1]+rel_ack_num;
        tseq = gPB[TCP_SEQACK_H_P+i-1];
        gPB[TCP_SEQACK_H_P+i-1] = rel_ack_num;
        if (cp_seq)
            gPB[TCP_SEQ_H_P+i-1] = tseq;
        else
            gPB[TCP_SEQ_H_P+i-1] = 0; // some preset value
        rel_ack_num = rel_ack_num>>8;
        i--;
    }
}

static void make_tcphead(uint16_t rel_ack_num,uint8_t cp_seq) {
    uint8_t i = gPB[TCP_DST_PORT_H_P];
    gPB[TCP_DST_PORT_H_P] = gPB[TCP_SRC_PORT_H_P];
    gPB[TCP_SRC_PORT_H_P] = i;
    uint8_t j = gPB[TCP_DST_PORT_L_P];
    gPB[TCP_DST_PORT_L_P] = gPB[TCP_SRC_PORT_L_P];
    gPB[TCP_SRC_PORT_L_P] = j;
    step_seq(rel_ack_num,cp_seq);
    gPB[TCP_CHECKSUM_H_P] = 0;
    gPB[TCP_CHECKSUM_L_P] = 0;
    gPB[TCP_HEADER_LEN_P] = 0x50;
}

static void make_arp_answer_from_request() {
    setMACs(gPB + ETH_SRC_MAC);
    gPB[ETH_ARP_OPCODE_H_P] = ETH_ARP_OPCODE_REPLY_H_V;
    gPB[ETH_ARP_OPCODE_L_P] = ETH_ARP_OPCODE_REPLY_L_V;
    EtherCard::copyMac(gPB + ETH_ARP_DST_MAC_P, gPB + ETH_ARP_SRC_MAC_P);
    EtherCard::copyMac(gPB + ETH_ARP_SRC_MAC_P, EtherCard::mymac);
    EtherCard::copyIp(gPB + ETH_ARP_DST_IP_P, gPB + ETH_ARP_SRC_IP_P);
    EtherCard::copyIp(gPB + ETH_ARP_SRC_IP_P, EtherCard::myip);
    EtherCard::packetSend(42);
}

static void make_echo_reply_from_request(uint16_t len) {
    make_eth_ip();
    gPB[ICMP_TYPE_P] = ICMP_TYPE_ECHOREPLY_V;
    if (gPB[ICMP_CHECKSUM_P] > (0xFF-0x08))
        gPB[ICMP_CHECKSUM_P+1]++;
    gPB[ICMP_CHECKSUM_P] += 0x08;
    EtherCard::packetSend(len);
}

void EtherCard::makeUdpReply (const char *data,uint8_t datalen,uint16_t port) {
    if (datalen>220)
        datalen = 220;
    gPB[IP_TOTLEN_H_P] = (IP_HEADER_LEN+UDP_HEADER_LEN+datalen) >>8;
    gPB[IP_TOTLEN_L_P] = IP_HEADER_LEN+UDP_HEADER_LEN+datalen;
    make_eth_ip();
    gPB[UDP_DST_PORT_H_P] = gPB[UDP_SRC_PORT_H_P];
    gPB[UDP_DST_PORT_L_P] = gPB[UDP_SRC_PORT_L_P];
    gPB[UDP_SRC_PORT_H_P] = port>>8;
    gPB[UDP_SRC_PORT_L_P] = port;
    gPB[UDP_LEN_H_P] = (UDP_HEADER_LEN+datalen) >> 8;
    gPB[UDP_LEN_L_P] = UDP_HEADER_LEN+datalen;
    gPB[UDP_CHECKSUM_H_P] = 0;
    gPB[UDP_CHECKSUM_L_P] = 0;
    memcpy(gPB + UDP_DATA_P, data, datalen);
    fill_checksum(UDP_CHECKSUM_H_P, IP_SRC_P, 16 + datalen,1);
    packetSend(UDP_HEADER_LEN+IP_HEADER_LEN+ETH_HEADER_LEN+datalen);
}

static void make_tcp_synack_from_syn() {
    gPB[IP_TOTLEN_H_P] = 0;
    gPB[IP_TOTLEN_L_P] = IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+4;
    make_eth_ip();
    gPB[TCP_FLAGS_P] = TCP_FLAGS_SYNACK_V;
    make_tcphead(1,0);
    gPB[TCP_SEQ_H_P+0] = 0;
    gPB[TCP_SEQ_H_P+1] = 0;
    gPB[TCP_SEQ_H_P+2] = seqnum;
    gPB[TCP_SEQ_H_P+3] = 0;
    seqnum += 3;
    gPB[TCP_OPTIONS_P] = 2;
    gPB[TCP_OPTIONS_P+1] = 4;
    gPB[TCP_OPTIONS_P+2] = 0x05;
    gPB[TCP_OPTIONS_P+3] = 0x0;
    gPB[TCP_HEADER_LEN_P] = 0x60;
    gPB[TCP_WIN_SIZE] = 0x5; // 1400=0x578
    gPB[TCP_WIN_SIZE+1] = 0x78;
    fill_checksum(TCP_CHECKSUM_H_P, IP_SRC_P, 8+TCP_HEADER_LEN_PLAIN+4,2);
    EtherCard::packetSend(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+4+ETH_HEADER_LEN);
}

static uint16_t get_tcp_data_len() {
    int16_t i = (((int16_t)gPB[IP_TOTLEN_H_P])<<8)|gPB[IP_TOTLEN_L_P];
    i -= IP_HEADER_LEN;
    i -= (gPB[TCP_HEADER_LEN_P]>>4)*4; // generate len in bytes;
    if (i<=0)
        i = 0;
    return (uint16_t)i;
}

static void make_tcp_ack_from_any(int16_t datlentoack,uint8_t addflags) {
    gPB[TCP_FLAGS_P] = TCP_FLAGS_ACK_V|addflags;
    if (addflags!=TCP_FLAGS_RST_V && datlentoack==0)
        datlentoack = 1;
    make_tcphead(datlentoack,1); // no options
    uint16_t j = IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN;
    gPB[IP_TOTLEN_H_P] = j>>8;
    gPB[IP_TOTLEN_L_P] = j;
    make_eth_ip();
    gPB[TCP_WIN_SIZE] = 0x4; // 1024=0x400, 1280=0x500 2048=0x800 768=0x300
    gPB[TCP_WIN_SIZE+1] = 0;
    fill_checksum(TCP_CHECKSUM_H_P, IP_SRC_P, 8+TCP_HEADER_LEN_PLAIN,2);
    EtherCard::packetSend(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+ETH_HEADER_LEN);
}

static void make_tcp_ack_with_data_noflags(uint16_t dlen) {
    uint16_t j = IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+dlen;
    gPB[IP_TOTLEN_H_P] = j>>8;
    gPB[IP_TOTLEN_L_P] = j;
    fill_ip_hdr_checksum();
    gPB[TCP_CHECKSUM_H_P] = 0;
    gPB[TCP_CHECKSUM_L_P] = 0;
    fill_checksum(TCP_CHECKSUM_H_P, IP_SRC_P, 8+TCP_HEADER_LEN_PLAIN+dlen,2);
    EtherCard::packetSend(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+dlen+ETH_HEADER_LEN);
}

void EtherCard::httpServerReply (uint16_t dlen) {
    make_tcp_ack_from_any(info_data_len,0); // send ack for http get
    gPB[TCP_FLAGS_P] = TCP_FLAGS_ACK_V|TCP_FLAGS_PUSH_V|TCP_FLAGS_FIN_V;
    make_tcp_ack_with_data_noflags(dlen); // send data
}

static void get_seq() { //get the sequence number of packets after an ack from GET
    SEQ =(((unsigned long)gPB[TCP_SEQ_H_P]*256+gPB[TCP_SEQ_H_P+1])*256+gPB[TCP_SEQ_H_P+2])*256+gPB[TCP_SEQ_H_P+3];
} //thanks to mstuetz for the missing (unsigned long)

static void set_seq() { //set the correct sequence number and calculate the next with the lenght of current packet
    gPB[TCP_SEQ_H_P]= (SEQ & 0xff000000 ) >> 24;
    gPB[TCP_SEQ_H_P+1]= (SEQ & 0xff0000 ) >> 16;
    gPB[TCP_SEQ_H_P+2]= (SEQ & 0xff00 ) >> 8;
    gPB[TCP_SEQ_H_P+3]= (SEQ & 0xff );
}

void EtherCard::httpServerReplyAck () {
    make_tcp_ack_from_any(info_data_len,0); // send ack for http get
    get_seq(); //get the sequence number of packets after an ack from GET
}

void EtherCard::httpServerReply_with_flags (uint16_t dlen , uint8_t flags) {
    set_seq();
    gPB[TCP_FLAGS_P] = flags; // final packet
    make_tcp_ack_with_data_noflags(dlen); // send data
    SEQ=SEQ+dlen;
}

void EtherCard::clientIcmpRequest(const uint8_t *destip) {
	if(is_lan(EtherCard::myip, destip)) {
		setMACandIPs(destmacaddr, destip);
	} else
		setMACandIPs(gwmacaddr, destip);
    gPB[ETH_TYPE_H_P] = ETHTYPE_IP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_IP_L_V;
    memcpy_P(gPB + IP_P,iphdr,9);
    gPB[IP_TOTLEN_L_P] = 0x54;
    gPB[IP_PROTO_P] = IP_PROTO_ICMP_V;
    fill_ip_hdr_checksum();
    gPB[ICMP_TYPE_P] = ICMP_TYPE_ECHOREQUEST_V;
    gPB[ICMP_TYPE_P+1] = 0; // code
    gPB[ICMP_CHECKSUM_H_P] = 0;
    gPB[ICMP_CHECKSUM_L_P] = 0;
    gPB[ICMP_IDENT_H_P] = 5; // some number
    gPB[ICMP_IDENT_L_P] = EtherCard::myip[3]; // last byte of my IP
    gPB[ICMP_IDENT_L_P+1] = 0; // seq number, high byte
    gPB[ICMP_IDENT_L_P+2] = 1; // seq number, low byte, we send only 1 ping at a time
    memset(gPB + ICMP_DATA_P, PINGPATTERN, 56);
    fill_checksum(ICMP_CHECKSUM_H_P, ICMP_TYPE_P, 56+8,0);
    packetSend(98);
}

void EtherCard::ntpRequest (uint8_t *ntpip,uint8_t srcport) {
	if(is_lan(myip, ntpip)) {
		setMACandIPs(destmacaddr, ntpip);
	} else
		setMACandIPs(gwmacaddr, ntpip);
    gPB[ETH_TYPE_H_P] = ETHTYPE_IP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_IP_L_V;
    memcpy_P(gPB + IP_P,iphdr,9);
    gPB[IP_TOTLEN_L_P] = 0x4c;
    gPB[IP_PROTO_P] = IP_PROTO_UDP_V;
    fill_ip_hdr_checksum();
    gPB[UDP_DST_PORT_H_P] = 0;
    gPB[UDP_DST_PORT_L_P] = 0x7b; // ntp = 123
    gPB[UDP_SRC_PORT_H_P] = 10;
    gPB[UDP_SRC_PORT_L_P] = srcport; // lower 8 bit of src port
    gPB[UDP_LEN_H_P] = 0;
    gPB[UDP_LEN_L_P] = 56; // fixed len
    gPB[UDP_CHECKSUM_H_P] = 0;
    gPB[UDP_CHECKSUM_L_P] = 0;
    memset(gPB + UDP_DATA_P, 0, 48);
    memcpy_P(gPB + UDP_DATA_P,ntpreqhdr,10);
    fill_checksum(UDP_CHECKSUM_H_P, IP_SRC_P, 16 + 48,1);
    packetSend(90);
}

uint8_t EtherCard::ntpProcessAnswer (uint32_t *time,uint8_t dstport_l) {
    if ((dstport_l && gPB[UDP_DST_PORT_L_P]!=dstport_l) || gPB[UDP_LEN_H_P]!=0 ||
            gPB[UDP_LEN_L_P]!=56 || gPB[UDP_SRC_PORT_L_P]!=0x7b)
        return 0;
    ((uint8_t*) time)[3] = gPB[0x52];
    ((uint8_t*) time)[2] = gPB[0x53];
    ((uint8_t*) time)[1] = gPB[0x54];
    ((uint8_t*) time)[0] = gPB[0x55];
    return 1;
}

void EtherCard::udpPrepare (uint16_t sport, const uint8_t *dip, uint16_t dport) {
	if(is_lan(myip, dip))  						   // this works because both dns mac and destinations mac are stored in same variable - destmacaddr
		setMACandIPs(destmacaddr, dip);			   // at different times. The program could have separate variable for dns mac, then here should be 
	else										   // checked if dip is dns ip and separately if dip is hisip and then use correct mac.
		setMACandIPs(gwmacaddr, dip);
    // see http://tldp.org/HOWTO/Multicast-HOWTO-2.html
    // multicast or broadcast address, https://github.com/jcw/ethercard/issues/59
    if ((dip[0] & 0xF0) == 0xE0 || *((unsigned long*) dip) == 0xFFFFFFFF)
        EtherCard::copyMac(gPB + ETH_DST_MAC, allOnes);
    gPB[ETH_TYPE_H_P] = ETHTYPE_IP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_IP_L_V;
    memcpy_P(gPB + IP_P,iphdr,9);
    gPB[IP_TOTLEN_H_P] = 0;
    gPB[IP_PROTO_P] = IP_PROTO_UDP_V;
    gPB[UDP_DST_PORT_H_P] = (dport>>8);
    gPB[UDP_DST_PORT_L_P] = dport;
    gPB[UDP_SRC_PORT_H_P] = (sport>>8);
    gPB[UDP_SRC_PORT_L_P] = sport;
    gPB[UDP_LEN_H_P] = 0;
    gPB[UDP_CHECKSUM_H_P] = 0;
    gPB[UDP_CHECKSUM_L_P] = 0;
}

void EtherCard::udpTransmit (uint16_t datalen) {
    gPB[IP_TOTLEN_H_P] = (IP_HEADER_LEN+UDP_HEADER_LEN+datalen) >> 8;
    gPB[IP_TOTLEN_L_P] = IP_HEADER_LEN+UDP_HEADER_LEN+datalen;
    fill_ip_hdr_checksum();
    gPB[UDP_LEN_H_P] = (UDP_HEADER_LEN+datalen) >>8;
    gPB[UDP_LEN_L_P] = UDP_HEADER_LEN+datalen;
    fill_checksum(UDP_CHECKSUM_H_P, IP_SRC_P, 16 + datalen,1);
    packetSend(UDP_HEADER_LEN+IP_HEADER_LEN+ETH_HEADER_LEN+datalen);
}

void EtherCard::sendUdp (const char *data, uint8_t datalen, uint16_t sport,
                         const uint8_t *dip, uint16_t dport) {
    udpPrepare(sport, dip, dport);
    if (datalen>220)
        datalen = 220;
    memcpy(gPB + UDP_DATA_P, data, datalen);
    udpTransmit(datalen);
}

void EtherCard::sendWol (uint8_t *wolmac) {
    setMACandIPs(allOnes, allOnes);
    gPB[ETH_TYPE_H_P] = ETHTYPE_IP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_IP_L_V;
    memcpy_P(gPB + IP_P,iphdr,9);
    gPB[IP_TOTLEN_L_P] = 0x82;
    gPB[IP_PROTO_P] = IP_PROTO_UDP_V;
    fill_ip_hdr_checksum();
    gPB[UDP_DST_PORT_H_P] = 0;
    gPB[UDP_DST_PORT_L_P] = 0x9; // wol = normally 9
    gPB[UDP_SRC_PORT_H_P] = 10;
    gPB[UDP_SRC_PORT_L_P] = 0x42; // source port does not matter
    gPB[UDP_LEN_H_P] = 0;
    gPB[UDP_LEN_L_P] = 110; // fixed len
    gPB[UDP_CHECKSUM_H_P] = 0;
    gPB[UDP_CHECKSUM_L_P] = 0;
    copyMac(gPB + UDP_DATA_P, allOnes);
    uint8_t pos = UDP_DATA_P;
    for (uint8_t m = 0; m < 16; ++m) {
        pos += 6;
        copyMac(gPB + pos, wolmac);
    }
    fill_checksum(UDP_CHECKSUM_H_P, IP_SRC_P, 16 + 102,1);
    packetSend(pos + 6);
}

// make a arp request
static void client_arp_whohas(uint8_t *ip_we_search) {
    setMACs(allOnes);
    gPB[ETH_TYPE_H_P] = ETHTYPE_ARP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_ARP_L_V;
    memcpy_P(gPB + ETH_ARP_P,arpreqhdr,8);
    memset(gPB + ETH_ARP_DST_MAC_P, 0, 6);
    EtherCard::copyMac(gPB + ETH_ARP_SRC_MAC_P, EtherCard::mymac);
    EtherCard::copyIp(gPB + ETH_ARP_DST_IP_P, ip_we_search);
    EtherCard::copyIp(gPB + ETH_ARP_SRC_IP_P, EtherCard::myip);
    EtherCard::packetSend(42);
}

uint8_t EtherCard::clientWaitingGw () {
    return !(waitgwmac & WGW_HAVE_GW_MAC);
}

static uint8_t client_store_mac(uint8_t *source_ip, uint8_t *mac) {
	if (memcmp(gPB + ETH_ARP_SRC_IP_P, source_ip, 4) != 0)
        return 0;
    EtherCard::copyMac(mac, gPB + ETH_ARP_SRC_MAC_P);
    return 1;
}

// static void client_gw_arp_refresh() {
//   if (waitgwmac & WGW_HAVE_GW_MAC)
//     waitgwmac |= WGW_REFRESHING;
// }

void EtherCard::setGwIp (const uint8_t *gwipaddr) {
    delaycnt = 0; //request gateway ARP lookup
    waitgwmac = WGW_INITIAL_ARP; // causes an arp request in the packet loop
    copyIp(gwip, gwipaddr);
}

void EtherCard::updateBroadcastAddress()
{
    for(uint8_t i=0; i<4; i++)
        broadcastip[i] = myip[i] | ~netmask[i];
}

static void client_syn(uint8_t srcport,uint8_t dstport_h,uint8_t dstport_l) {
	if(is_lan(EtherCard::myip, EtherCard::hisip)) { 
		setMACandIPs(destmacaddr, EtherCard::hisip);
	} else
		setMACandIPs(gwmacaddr, EtherCard::hisip);
    gPB[ETH_TYPE_H_P] = ETHTYPE_IP_H_V;
    gPB[ETH_TYPE_L_P] = ETHTYPE_IP_L_V;
    memcpy_P(gPB + IP_P,iphdr,9);
    gPB[IP_TOTLEN_L_P] = 44; // good for syn
    gPB[IP_PROTO_P] = IP_PROTO_TCP_V;
    fill_ip_hdr_checksum();
    gPB[TCP_DST_PORT_H_P] = dstport_h;
    gPB[TCP_DST_PORT_L_P] = dstport_l;
    gPB[TCP_SRC_PORT_H_P] = TCPCLIENT_SRC_PORT_H;
    gPB[TCP_SRC_PORT_L_P] = srcport; // lower 8 bit of src port
    memset(gPB + TCP_SEQ_H_P, 0, 8);
    gPB[TCP_SEQ_H_P+2] = seqnum;
    seqnum += 3;
    gPB[TCP_HEADER_LEN_P] = 0x60; // 0x60=24 len: (0x60>>4) * 4
    gPB[TCP_FLAGS_P] = TCP_FLAGS_SYN_V;
    gPB[TCP_WIN_SIZE] = 0x3; // 1024 = 0x400 768 = 0x300, initial window
    gPB[TCP_WIN_SIZE+1] = 0x0;
    gPB[TCP_CHECKSUM_H_P] = 0;
    gPB[TCP_CHECKSUM_L_P] = 0;
    gPB[TCP_CHECKSUM_L_P+1] = 0;
    gPB[TCP_CHECKSUM_L_P+2] = 0;
    gPB[TCP_OPTIONS_P] = 2;
    gPB[TCP_OPTIONS_P+1] = 4;
    gPB[TCP_OPTIONS_P+2] = (CLIENTMSS>>8);
    gPB[TCP_OPTIONS_P+3] = (uint8_t) CLIENTMSS;
    fill_checksum(TCP_CHECKSUM_H_P, IP_SRC_P, 8 +TCP_HEADER_LEN_PLAIN+4,2);
    // 4 is the tcp mss option:
    EtherCard::packetSend(IP_HEADER_LEN+TCP_HEADER_LEN_PLAIN+ETH_HEADER_LEN+4);
}

uint8_t EtherCard::clientTcpReq (uint8_t (*result_cb)(uint8_t,uint8_t,uint16_t,uint16_t),
                                 uint16_t (*datafill_cb)(uint8_t),uint16_t port) {
    client_tcp_result_cb = result_cb;
    client_tcp_datafill_cb = datafill_cb;
    tcp_client_port_h = port>>8;
    tcp_client_port_l = port;
    tcp_client_state = 1; // Flag to packetloop to initiate a TCP/IP session by send a syn
    tcp_fd = (tcp_fd + 1) & 7;
    return tcp_fd;
}

static uint16_t www_client_internal_datafill_cb(uint8_t fd) {
    BufferFiller bfill = EtherCard::tcpOffset();
    if (fd==www_fd) {
        if (client_postval == 0) {
            bfill.emit_p(PSTR("GET $F$S HTTP/1.0\r\n"
                              "Host: $F\r\n"
                              "$F\r\n"
                              "\r\n"), client_urlbuf,
                         client_urlbuf_var,
                         client_hoststr, client_additionalheaderline);
        } else {
            const char* ahl = client_additionalheaderline;
            bfill.emit_p(PSTR("POST $F HTTP/1.0\r\n"
                              "Host: $F\r\n"
                              "$F$S"
                              "Accept: */*\r\n"
                              "Content-Length: $D\r\n"
                              "Content-Type: application/x-www-form-urlencoded\r\n"
                              "\r\n"
                              "$S"), client_urlbuf,
                         client_hoststr,
                         ahl != 0 ? ahl : PSTR(""),
                         ahl != 0 ? "\r\n" : "",
                         strlen(client_postval),
                         client_postval);
        }
    }
    return bfill.position();
}

static uint8_t www_client_internal_result_cb(uint8_t fd, uint8_t statuscode, uint16_t datapos, uint16_t len_of_data) {
    if (fd!=www_fd)
        (*client_browser_cb)(4,0,0);
    else if (statuscode==0 && len_of_data>12 && client_browser_cb) {
        uint8_t f = strncmp("200",(char *)&(gPB[datapos+9]),3) != 0;
        (*client_browser_cb)(f, ((uint16_t)TCP_SRC_PORT_H_P+(gPB[TCP_HEADER_LEN_P]>>4)*4),len_of_data);
    }
    return 0;
}

void EtherCard::browseUrl (const char *urlbuf, const char *urlbuf_varpart, const char *hoststr, void (*callback)(uint8_t,uint16_t,uint16_t)) {
    browseUrl(urlbuf, urlbuf_varpart, hoststr, PSTR("Accept: text/html"), callback);
}

void EtherCard::browseUrl (const char *urlbuf, const char *urlbuf_varpart, const char *hoststr, const char *additionalheaderline, void (*callback)(uint8_t,uint16_t,uint16_t)) {
    client_urlbuf = urlbuf;
    client_urlbuf_var = urlbuf_varpart;
    client_hoststr = hoststr;
    client_additionalheaderline = additionalheaderline;
    client_postval = 0;
    client_browser_cb = callback;
    www_fd = clientTcpReq(&www_client_internal_result_cb,&www_client_internal_datafill_cb,hisport);
}

void EtherCard::httpPost (const char *urlbuf, const char *hoststr, const char *additionalheaderline, const char *postval, void (*callback)(uint8_t,uint16_t,uint16_t)) {
    client_urlbuf = urlbuf;
    client_hoststr = hoststr;
    client_additionalheaderline = additionalheaderline;
    client_postval = postval;
    client_browser_cb = callback;
    www_fd = clientTcpReq(&www_client_internal_result_cb,&www_client_internal_datafill_cb,hisport);
}

static uint16_t tcp_datafill_cb(uint8_t fd) {
    uint16_t len = Stash::length();
    Stash::extract(0, len, EtherCard::tcpOffset());
    Stash::cleanup();
    EtherCard::tcpOffset()[len] = 0;
#if SERIAL
    Serial.print("REQUEST: ");
    Serial.println(len);
    Serial.println((char*) EtherCard::tcpOffset());
#endif
    result_fd = 123; // bogus value
    return len;
}

static uint8_t tcp_result_cb(uint8_t fd, uint8_t status, uint16_t datapos, uint16_t datalen) {
    if (status == 0) {
        result_fd = fd; // a valid result has been received, remember its session id
        result_ptr = (char*) ether.buffer + datapos;
        // result_ptr[datalen] = 0;
    }
    return 1;
}

uint8_t EtherCard::tcpSend () {
    www_fd = clientTcpReq(&tcp_result_cb, &tcp_datafill_cb, hisport);
    return www_fd;
}

const char* EtherCard::tcpReply (uint8_t fd) {
    if (result_fd != fd)
        return 0;
    result_fd = 123; // set to a bogus value to prevent future match
    return result_ptr;
}

void EtherCard::registerPingCallback (void (*callback)(uint8_t *srcip)) {
    icmp_cb = callback;
}

uint8_t EtherCard::packetLoopIcmpCheckReply (const uint8_t *ip_monitoredhost) {
    return gPB[IP_PROTO_P]==IP_PROTO_ICMP_V &&
           gPB[ICMP_TYPE_P]==ICMP_TYPE_ECHOREPLY_V &&
           gPB[ICMP_DATA_P]== PINGPATTERN &&
           check_ip_message_is_from(ip_monitoredhost);
}

uint16_t EtherCard::accept(const uint16_t port, uint16_t plen) {
    uint16_t pos;

    if (gPB[TCP_DST_PORT_H_P] == (port >> 8) &&
            gPB[TCP_DST_PORT_L_P] == ((uint8_t) port))
    {   //Packet targetted at specified port
        if (gPB[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
            make_tcp_synack_from_syn(); //send SYN+ACK
        else if (gPB[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
        {   //This is an acknowledgement to our SYN+ACK so let's start processing that payload
            info_data_len = get_tcp_data_len();
            if (info_data_len > 0)
            {   //Got some data
                pos = TCP_DATA_START; // TCP_DATA_START is a formula
                if (pos <= plen - 8)
                    return pos;
            }
            else if (gPB[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
                make_tcp_ack_from_any(0,0); //No data so close connection
        }
    }
    return 0;
}

uint16_t EtherCard::packetLoop (uint16_t plen) {
    uint16_t len;
    if(using_dhcp){
        ether.DhcpStateMachine(plen);
    }

    if (plen==0) {
        //Check every 65536 (no-packet) cycles whether we need to retry ARP request for gateway
        if ((waitgwmac & WGW_INITIAL_ARP || waitgwmac & WGW_REFRESHING) &&
                delaycnt==0 && isLinkUp()) {
            client_arp_whohas(gwip);
			waitgwmac |= WGW_ACCEPT_ARP_REPLY;
		}

	
	
        delaycnt++;
        //Initiate TCP/IP session if pending
        if (tcp_client_state==1 && (waitgwmac & WGW_HAVE_GW_MAC)) { // send a syn
            tcp_client_state = 2;
            tcpclient_src_port_l++; // allocate a new port
            client_syn(((tcp_fd<<5) | (0x1f & tcpclient_src_port_l)),tcp_client_port_h,tcp_client_port_l);
        }
		//!@todo this is trying to find mac only once. Need some timeout to make another call if first one doesn't succeed.
		if(is_lan(myip, dnsip) && !has_dns_mac && !waiting_for_dns_mac) {
			client_arp_whohas(dnsip);
			waiting_for_dns_mac = true;
		}
		//!@todo this is trying to find mac only once. Need some timeout to make another call if first one doesn't succeed.	
		if(is_lan(myip, hisip) && !has_dest_mac && !waiting_for_dest_mac) {
			client_arp_whohas(hisip);
			waiting_for_dest_mac = true;
		}
			
		return 0;
    }
	
    if (eth_type_is_arp_and_my_ip(plen))
    {   //Service ARP request
        if (gPB[ETH_ARP_OPCODE_L_P]==ETH_ARP_OPCODE_REQ_L_V)
            make_arp_answer_from_request();
        if (waitgwmac & WGW_ACCEPT_ARP_REPLY && (gPB[ETH_ARP_OPCODE_L_P]==ETH_ARP_OPCODE_REPLY_L_V) && client_store_mac(gwip, gwmacaddr))
            waitgwmac = WGW_HAVE_GW_MAC;
		if (!has_dns_mac && waiting_for_dns_mac && client_store_mac(dnsip, destmacaddr)) {
			has_dns_mac = true;
			waiting_for_dns_mac = false;
		}
		if (!has_dest_mac && waiting_for_dest_mac && client_store_mac(hisip, destmacaddr)){
			has_dest_mac = true;
			waiting_for_dest_mac = false;
		}
        return 0;
    }
	
    if (eth_type_is_ip_and_my_ip(plen)==0)
    {   //Not IP so ignoring
        //!@todo Add other protocols (and make each optional at compile time)
        return 0;
    }
    if (gPB[IP_PROTO_P]==IP_PROTO_ICMP_V && gPB[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
    {   //Service ICMP echo request (ping)
        if (icmp_cb)
            (*icmp_cb)(&(gPB[IP_SRC_P]));
        make_echo_reply_from_request(plen);
        return 0;
    }
    if (ether.udpServerListening() && gPB[IP_PROTO_P]==IP_PROTO_UDP_V)
    {   //Call UDP server handler (callback) if one is defined for this packet
        if(ether.udpServerHasProcessedPacket(plen))
            return 0; //An UDP server handler (callback) has processed this packet
    }
    if (plen<54 && gPB[IP_PROTO_P]!=IP_PROTO_TCP_V )
        return 0; //Packet flagged as TCP but shorter than minimum TCP packet length
    if (gPB[TCP_DST_PORT_H_P]==TCPCLIENT_SRC_PORT_H)
    {   //Source port is in range reserved (by EtherCard) for client TCP/IP connections
        if (check_ip_message_is_from(hisip)==0)
            return 0; //Not current TCP/IP connection (only handle one at a time)
        if (gPB[TCP_FLAGS_P] & TCP_FLAGS_RST_V)
        {   //TCP reset flagged
            if (client_tcp_result_cb)
                (*client_tcp_result_cb)((gPB[TCP_DST_PORT_L_P]>>5)&0x7,3,0,0);
            tcp_client_state = 5;
            return 0;
        }
        len = get_tcp_data_len();
        if (tcp_client_state==2)
        {   //Waiting for SYN-ACK
            if ((gPB[TCP_FLAGS_P] & TCP_FLAGS_SYN_V) && (gPB[TCP_FLAGS_P] &TCP_FLAGS_ACK_V))
            {   //SYN and ACK flags set so this is an acknowledgement to our SYN
                make_tcp_ack_from_any(0,0);
                gPB[TCP_FLAGS_P] = TCP_FLAGS_ACK_V|TCP_FLAGS_PUSH_V;
                if (client_tcp_datafill_cb)
                    len = (*client_tcp_datafill_cb)((gPB[TCP_SRC_PORT_L_P]>>5)&0x7);
                else
                    len = 0;
                tcp_client_state = 3;
                make_tcp_ack_with_data_noflags(len);
            }
            else
            {   //Expecting SYN+ACK so reset and resend SYN
                tcp_client_state = 1; // retry
                len++;
                if (gPB[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
                    len = 0;
                make_tcp_ack_from_any(len,TCP_FLAGS_RST_V);
            }
            return 0;
        }
        if (tcp_client_state==3 && len>0)
        {   //TCP connection established so read data
            if (client_tcp_result_cb) {
                uint16_t tcpstart = TCP_DATA_START; // TCP_DATA_START is a formula
                if (tcpstart>plen-8)
                    tcpstart = plen-8; // dummy but save
                uint16_t save_len = len;
                if (tcpstart+len>plen)
                    save_len = plen-tcpstart;
                (*client_tcp_result_cb)((gPB[TCP_DST_PORT_L_P]>>5)&0x7,0,tcpstart,save_len); //Call TCP handler (callback) function

                if(persist_tcp_connection)
                {   //Keep connection alive by sending ACK
                    make_tcp_ack_from_any(len,TCP_FLAGS_PUSH_V);
                }
                else
                {   //Close connection
                    make_tcp_ack_from_any(len,TCP_FLAGS_PUSH_V|TCP_FLAGS_FIN_V);
                    tcp_client_state = 6;
                }
                return 0;
            }
        }
        if (tcp_client_state != 5)
        {   //
            if (gPB[TCP_FLAGS_P] & TCP_FLAGS_FIN_V) {
                if(tcp_client_state == 3) {
                    return 0; // In some instances FIN is received *before* DATA.  If that is the case, we just return here and keep looking for the data packet
                }
                make_tcp_ack_from_any(len+1,TCP_FLAGS_PUSH_V|TCP_FLAGS_FIN_V);
                tcp_client_state = 6; // connection terminated
            } else if (len>0) {
                make_tcp_ack_from_any(len,0);
            }
        }
        return 0;
    }

    //If we are here then this is a TCP/IP packet targetted at us and not related to out client connection so accept
    return accept(hisport, plen);
}

void EtherCard::persistTcpConnection(bool persist){
    persist_tcp_connection = persist;
}
