// This code slightly follows the conventions of, but is not derived from:
//      EHTERSHIELD_H library for Arduino etherShield
//      Copyright (c) 2008 Xing Yu.  All right reserved. (this is LGPL v2.1)
// It is however derived from the enc28j60 and ip code (which is GPL v2)
//      Author: Pascal Stang
//      Modified by: Guido Socher
//      DHCP code: Andrew Lindsay
// Hence: GPL V2
//
// 2010-05-19 <jc@wippler.nl>
//
//-----------------------------------------------------------------
//  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
//  Home:  http://www.serasidis.gr
//  email: avrsite@yahoo.gr
//
// PIN Connections (Using STM32F103):
//
// ENC28J60 -  STM32F103
//   VCC    -    3.3V
//   GND    -    GND
//   SCK    -    Pin PA5
//   SO     -    Pin PA6
//   SI     -    Pin PA7
//   CS     -    Pin PA8
//-----------------------------------------------------------------


#include <EtherCard_STM.h>
#include <stdarg.h>
//#include <avr/eeprom.h>

//#define FLOATEMIT // uncomment line to enable $T in emit_P for float emitting

byte Stash::map[256/8];
Stash::Block Stash::bufs[2];

uint8_t Stash::allocBlock () {
    for (uint8_t i = 0; i < sizeof map; ++i)
        if (map[i] != 0)
            for (uint8_t j = 0; j < 8; ++j)
                if (bitRead(map[i], j)) {
                    bitClear(map[i], j);
                    return (i << 3) + j;
                }
    return 0;
}

void Stash::freeBlock (uint8_t block) {
    bitSet(map[block>>3], block & 7);
}

uint8_t Stash::fetchByte (uint8_t blk, uint8_t off) {
    return blk == bufs[0].bnum ? bufs[0].bytes[off] :
           blk == bufs[1].bnum ? bufs[1].bytes[off] :
           ether.peekin(blk, off);
}

void Stash::initMap (uint8_t last) {
    while (--last > 0)
        freeBlock(last);
}

void Stash::load (uint8_t idx, uint8_t blk) {
    if (blk != bufs[idx].bnum) {
        if (idx == 0) {
            ether.copyout(bufs[idx].bnum, bufs[idx].bytes);
            if (blk == bufs[1].bnum)
                bufs[1].bnum = 255; // forget read page if same
        } else if (blk == bufs[0].bnum) {
            // special case: read page is same as write buffer
            memcpy(&bufs[1], &bufs[0], sizeof bufs[0]);
            return;
        }
        bufs[idx].bnum = blk;
        ether.copyin(bufs[idx].bnum, bufs[idx].bytes);
    }
}

uint8_t Stash::freeCount () {
    uint8_t count = 0;
    for (uint8_t i = 0; i < 256/8; ++i)
        for (uint8_t m = 0x80; m != 0; m >>= 1)
            if (map[i] & m)
                ++count;
    return count;
}

uint8_t Stash::create () {
    uint8_t blk = allocBlock();
    load(0, blk);
    bufs[0].head.count = 0;
    bufs[0].head.first = bufs[0].head.last = blk;
    bufs[0].tail = sizeof (StashHeader);
    bufs[0].next = 0;
    return open(blk);
}

uint8_t Stash::open (uint8_t blk) {
    curr = blk;
    offs = sizeof (StashHeader);
    load(1, curr);
    memcpy((StashHeader*) this, bufs[1].bytes, sizeof (StashHeader));
    return curr;
}

void Stash::save () {
    load(0, first);
    memcpy(bufs[0].bytes, (StashHeader*) this, sizeof (StashHeader));
    if (bufs[1].bnum == first)
        load(1, 0); // invalidates original in case it was the same block
}

void Stash::release () {
    while (first > 0) {
        freeBlock(first);
        first = ether.peekin(first, 63);
    }
}

void Stash::put (char c) {
    load(0, last);
    uint8_t t = bufs[0].tail;
    bufs[0].bytes[t++] = c;
    if (t <= 62)
        bufs[0].tail = t;
    else {
        bufs[0].next = allocBlock();
        last = bufs[0].next;
        load(0, last);
        bufs[0].tail = bufs[0].next = 0;
        ++count;
    }
}

char Stash::get () {
    load(1, curr);
    if (curr == last && offs >= bufs[1].tail)
        return 0;
    uint8_t b = bufs[1].bytes[offs];
    if (++offs >= 63 && curr != last) {
        curr = bufs[1].next;
        offs = 0;
    }
    return b;
}

uint16_t Stash::size () {
    return 63 * count + fetchByte(last, 62) - sizeof (StashHeader);
}

static char* wtoa (uint16_t value, char* ptr) {
    if (value > 9)
        ptr = wtoa(value / 10, ptr);
    *ptr = '0' + value % 10;
    *++ptr = 0;
    return ptr;
}

void Stash::prepare (PGM_P fmt, ...) {
    Stash::load(0, 0);
    uint16_t* segs = Stash::bufs[0].words;
    *segs++ = strlen_P(fmt);
#ifdef __AVR__
    *segs++ = (uint16_t) fmt;
#else
    *segs++ = (uint32_t) fmt;
    *segs++ = (uint32_t) fmt >> 16;
#endif
    va_list ap;
    va_start(ap, fmt);
    for (;;) {
        char c = pgm_read_byte(fmt++);
        if (c == 0)
            break;
        if (c == '$') {
#ifdef __AVR__
            uint16_t argval = va_arg(ap, uint16_t), arglen = 0;
#else
            uint32_t argval = va_arg(ap, int), arglen = 0;
#endif
            switch (pgm_read_byte(fmt++)) {
            case 'D': {
                char buf[7];
                wtoa(argval, buf);
                arglen = strlen(buf);
                break;
            }
            case 'S':
                arglen = strlen((const char*) argval);
                break;
            case 'F':
                arglen = strlen_P((PGM_P) argval);
                break;
            case 'E': {
                byte* s = (byte*) argval;
                char d;
                //while ((d = eeprom_read_byte(s++)) != 0)
                //    ++arglen;
                break;
            }
            case 'H': {
                Stash stash (argval);
                arglen = stash.size();
                break;
            }
            }
#ifdef __AVR__
            *segs++ = argval;
#else
            *segs++ = argval;
            *segs++ = argval >> 16;
#endif
            Stash::bufs[0].words[0] += arglen - 2;
        }
    }
    va_end(ap);
}

uint16_t Stash::length () {
    Stash::load(0, 0);
    return Stash::bufs[0].words[0];
}

void Stash::extract (uint16_t offset, uint16_t count, void* buf) {
    Stash::load(0, 0);
    uint16_t* segs = Stash::bufs[0].words;
#ifdef __AVR__
    PGM_P fmt = (PGM_P) *++segs;
#else
    PGM_P fmt = (PGM_P)((segs[2] << 16) | segs[1]);
    segs += 2;
#endif
    Stash stash;
    char mode = '@', tmp[7], *ptr = NULL, *out = (char*) buf;
    for (uint16_t i = 0; i < offset + count; ) {
        char c = 0;
        switch (mode) {
        case '@': {
            c = pgm_read_byte(fmt++);
            if (c == 0)
                return;
            if (c != '$')
                break;
#ifdef __AVR__
            uint16_t arg = *++segs;
#else
            uint32_t arg = *++segs;
            arg |= *++segs << 16;
#endif
            mode = pgm_read_byte(fmt++);
            switch (mode) {
            case 'D':
                wtoa(arg, tmp);
                ptr = tmp;
                break;
            case 'S':
            case 'F':
            case 'E':
                ptr = (char*) arg;
                break;
            case 'H':
                stash.open(arg);
                ptr = (char*) &stash;
                break;
            }
            continue;
        }
        case 'D':
        case 'S':
            c = *ptr++;
            break;
        case 'F':
            c = pgm_read_byte(ptr++);
            break;
        case 'E':
            //c = eeprom_read_byte((byte*) ptr++);
            break;
        case 'H':
            c = ((Stash*) ptr)->get();
            break;
        }
        if (c == 0) {
            mode = '@';
            continue;
        }
        if (i >= offset)
            *out++ = c;
        ++i;
    }
}

void Stash::cleanup () {
    Stash::load(0, 0);
    uint16_t* segs = Stash::bufs[0].words;
#ifdef __AVR__
    PGM_P fmt = (PGM_P) *++segs;
#else
    PGM_P fmt = (PGM_P)((segs[2] << 16) | segs[1]);
    segs += 2;
#endif
    for (;;) {
        char c = pgm_read_byte(fmt++);
        if (c == 0)
            break;
        if (c == '$') {
#ifdef __AVR__
            uint16_t arg = *++segs;
#else
            uint32_t arg = *++segs;
            arg |= *++segs << 16;
#endif
            if (pgm_read_byte(fmt++) == 'H') {
                Stash stash (arg);
                stash.release();
            }
        }
    }
}

void BufferFiller::emit_p(PGM_P fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    for (;;) {
        char c = pgm_read_byte(fmt++);
        if (c == 0)
            break;
        if (c != '$') {
            *ptr++ = c;
            continue;
        }
        c = pgm_read_byte(fmt++);
        switch (c) {
        case 'D':
#ifdef __AVR__
            wtoa(va_arg(ap, uint16_t), (char*) ptr);
#else
            wtoa(va_arg(ap, int), (char*) ptr);
#endif
            break;
#ifdef FLOATEMIT
        case 'T':
            dtostrf    (    va_arg(ap, double), 10, 3, (char*)ptr );
            break;
#endif
        case 'H': {
#ifdef __AVR__
            char p1 =  va_arg(ap, uint16_t);
#else
            char p1 =  va_arg(ap, int);
#endif
            char p2;
            p2 = (p1 >> 4) & 0x0F;
            p1 = p1 & 0x0F;
            if (p1 > 9) p1 += 0x07; // adjust 0x0a-0x0f to come out 'a'-'f'
            p1 += 0x30;             // and complete
            if (p2 > 9) p2 += 0x07; // adjust 0x0a-0x0f to come out 'a'-'f'
            p2 += 0x30;             // and complete
            *ptr++ = p2;
            *ptr++ = p1;
            continue;
        }
        case 'L':
            //ltoa(va_arg(ap, long), (char*) ptr, 10);
            break;
        case 'S':
            strcpy((char*) ptr, va_arg(ap, const char*));
            break;
        case 'F': {
            PGM_P s = va_arg(ap, PGM_P);
            char d;
            while ((d = pgm_read_byte(s++)) != 0)
                *ptr++ = d;
            continue;
        }
        case 'E': {
            byte* s = va_arg(ap, byte*);
            char d;
            //while ((d = eeprom_read_byte(s++)) != 0)
            //    *ptr++ = d;
            continue;
        }
        default:
            *ptr++ = c;
            continue;
        }
        ptr += strlen((char*) ptr);
    }
    va_end(ap);
}

EtherCard ether;

uint8_t EtherCard::mymac[6];  // my MAC address
uint8_t EtherCard::myip[4];   // my ip address
uint8_t EtherCard::netmask[4]; // subnet mask
uint8_t EtherCard::broadcastip[4]; // broadcast address
uint8_t EtherCard::gwip[4];   // gateway
uint8_t EtherCard::dhcpip[4]; // dhcp server
uint8_t EtherCard::dnsip[4];  // dns server
uint8_t EtherCard::hisip[4];  // ip address of remote host
uint16_t EtherCard::hisport = 80; // tcp port to browse to
bool EtherCard::using_dhcp = false;
bool EtherCard::persist_tcp_connection = false;
uint16_t EtherCard::delaycnt = 0; //request gateway ARP lookup

uint8_t EtherCard::begin (const uint16_t size,
                          const uint8_t* macaddr,
                          uint8_t csPin) {
    using_dhcp = false;
    Stash::initMap(56);
    copyMac(mymac, macaddr);
    return initialize(size, mymac, csPin);
}

bool EtherCard::staticSetup (const uint8_t* my_ip,
                             const uint8_t* gw_ip,
                             const uint8_t* dns_ip,
                             const uint8_t* mask) {
    using_dhcp = false;

    if (my_ip != 0)
        copyIp(myip, my_ip);
    if (gw_ip != 0)
        setGwIp(gw_ip);
    if (dns_ip != 0)
        copyIp(dnsip, dns_ip);
    if(mask != 0)
        copyIp(netmask, mask);
    updateBroadcastAddress();
    delaycnt = 0; //request gateway ARP lookup
    return true;
}
