/*
 Enc28J60NetworkClass.h
 UIPEthernet network driver for Microchip ENC28J60 Ethernet Interface.

 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 based on enc28j60.c file from the AVRlib library by Pascal Stang.
 For AVRlib See http://www.procyonengineering.com/

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

#include "Enc28J60Network.h"
#include "Arduino.h"

extern "C" {
//#include <avr/io.h>
#include "enc28j60.h"
#include "uip.h"
}

#ifdef ENC28J60DEBUG
#include "HardwareSerial.h"
#endif

//#define Serial SerialUSB
//#define SPI SPIEth
extern SPIClass SPI;

// set CS to 0 = active
#define CSACTIVE digitalWrite(ENC28J60_CONTROL_CS, LOW)
// set CS to 1 = passive
#define CSPASSIVE digitalWrite(ENC28J60_CONTROL_CS, HIGH)
//
#define waitspi() while(!(SPSR&(1<<SPIF)))

uint16_t Enc28J60Network::nextPacketPtr;
uint8_t Enc28J60Network::bank=0xff;

struct memblock Enc28J60Network::receivePkt;

bool Enc28J60Network::broadcast_enabled = false;
static byte selectPin;


void Enc28J60Network::init(uint8_t* macaddr)
{
  uint32 timeout = 0;
  MemoryPool::init(); // 1 byte in between RX_STOP_INIT and pool to allow prepending of controlbyte
  // initialize I/O
  // ss as output:
  pinMode(ENC28J60_CONTROL_CS, OUTPUT);
  CSPASSIVE; // ss=0
  //

#ifdef ENC28J60DEBUG
  Serial.println("ENC28J60::initialize / before initSPI()");
#endif
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
//  SPI.setDataMode(SPI_MODE0);
//  SPI.setClockDivider(SPI_CLOCK_DIV16);
#ifdef ENC28J60DEBUG
  Serial.println("ENC28J60::initialize / after initSPI()");
  Serial.print("ENC28J60::initialize / csPin = ");
  Serial.println(SPI.nssPin());
  Serial.print("ENC28J60::initialize / miso = ");
  Serial.println(SPI.misoPin());
  Serial.print("ENC28J60::initialize / mosi = ");
  Serial.println(SPI.mosiPin());
  Serial.print("ENC28J60::initialize / sck = ");
  Serial.println(SPI.sckPin());
#endif
  selectPin = ENC28J60_CONTROL_CS;
  pinMode(selectPin, OUTPUT);
  digitalWrite(selectPin, HIGH);
	
  // perform system reset
  writeOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
  delay(2); // errata B7/2
  delay(50);
  // check CLKRDY bit to see if reset is complete
  // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
  //while(!(readReg(ESTAT) & ESTAT_CLKRDY));
  // do bank 0 stuff
  // initialize receive buffer
  // 16-bit transfers, must write low byte first
  // set receive buffer start address
#ifdef ENC28J60DEBUG
  Serial.println("ENC28J60::initialize / before readOp(ENC28J60_READ_CTRL_REG, ESTAT)");
#endif
  while (!readOp(ENC28J60_READ_CTRL_REG, ESTAT) & ESTAT_CLKRDY) {
    if (++timeout > 100000) {
      Serial.println("ENC28J60::initialize TIMEOUT !!!\r\n");
      return;
    }
  }
#ifdef ENC28J60DEBUG
  Serial.println("ENC28J60::initialize / after readOp(ENC28J60_READ_CTRL_REG, ESTAT)");
#endif
  nextPacketPtr = RXSTART_INIT;
  // Rx start
  writeRegPair(ERXSTL, RXSTART_INIT);
  // set receive pointer address
  writeRegPair(ERXRDPTL, RXSTART_INIT);
  // RX end
  writeRegPair(ERXNDL, RXSTOP_INIT);
  // TX start
  //-------------writeRegPair(ETXSTL, TXSTART_INIT);
  // TX end
  //-------------writeRegPair(ETXNDL, TXSTOP_INIT);
  // do bank 1 stuff, packet filter:
  // For broadcast packets we allow only ARP packtets
  // All other packets should be unicast only for our mac (MAADR)
  //
  // The pattern to match on is therefore
  // Type     ETH.DST
  // ARP      BROADCAST
  // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
  // in binary these poitions are:11 0000 0011 1111
  // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
  //TODO define specific pattern to receive dhcp-broadcast packages instead of setting ERFCON_BCEN!
//    enableBroadcast(); // change to add ERXFCON_BCEN recommended by epam
  writeReg(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
  writeRegPair(EPMM0, 0x303f);
  writeRegPair(EPMCSL, 0xf7f9);
  //
  //
  // do bank 2 stuff
  // enable MAC receive
  // and bring MAC out of reset (writes 0x00 to MACON2)
  writeRegPair(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
  //----------------writeRegPair(MACON2, 0x00);
  // enable automatic padding to 60bytes and CRC operations
  writeOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);
  // set inter-frame gap (non-back-to-back)
  writeRegPair(MAIPGL, 0x0C12);
  // set inter-frame gap (back-to-back)
  writeReg(MABBIPG, 0x12);
  // Set the maximum packet size which the controller will accept
  // Do not send packets longer than MAX_FRAMELEN:
  writeRegPair(MAMXFLL, MAX_FRAMELEN);
  // do bank 3 stuff
  // write MAC address
  // NOTE: MAC address in ENC28J60 is byte-backward
  writeReg(MAADR5, macaddr[0]);
  writeReg(MAADR4, macaddr[1]);
  writeReg(MAADR3, macaddr[2]);
  writeReg(MAADR2, macaddr[3]);
  writeReg(MAADR1, macaddr[4]);
  writeReg(MAADR0, macaddr[5]);
  // no loopback of transmitted frames
  phyWrite(PHCON2, PHCON2_HDLDIS);
  // switch to bank 0
  setBank(ECON1);
  // enable interrutps
  writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
  // enable packet reception
  writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
  //Configure leds
  phyWrite(PHLCON,0x476);

  byte rev = readReg(EREVID);
  // microchip forgot to step the number on the silcon when they
  // released the revision B7. 6 is now rev B7. We still have
  // to see what they do when they release B8. At the moment
  // there is no B8 out yet
  if (rev > 5) ++rev;
#ifdef ENC28J60DEBUG
  Serial.print("ENC28J60::initialize returns ");
  Serial.println(rev);
#endif
//  return rev;
}


memhandle
Enc28J60Network::receivePacket()
{
  uint8_t rxstat;
  uint16_t len;
  // check if a packet has been received and buffered
  //if( !(readReg(EIR) & EIR_PKTIF) ){
  // The above does not work. See Rev. B4 Silicon Errata point 6.
  if (readReg(EPKTCNT) != 0)
    {
      uint16_t readPtr = nextPacketPtr+6 > RXSTOP_INIT ? nextPacketPtr+6-RXSTOP_INIT+RXSTART_INIT : nextPacketPtr+6;
      // Set the read pointer to the start of the received packet
      writeRegPair(ERDPTL, nextPacketPtr);
      // read the next packet pointer
      nextPacketPtr = readOp(ENC28J60_READ_BUF_MEM, 0);
      nextPacketPtr |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
      // read the packet length (see datasheet page 43)
      len = readOp(ENC28J60_READ_BUF_MEM, 0);
      len |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
      len -= 4; //remove the CRC count
      // read the receive status (see datasheet page 43)
      rxstat = readOp(ENC28J60_READ_BUF_MEM, 0);
      //rxstat |= readOp(ENC28J60_READ_BUF_MEM, 0) << 8;
#ifdef ENC28J60DEBUG
      Serial.print("receivePacket [");
      Serial.print(readPtr,HEX);
      Serial.print("-");
      Serial.print((readPtr+len) % (RXSTOP_INIT+1),HEX);
      Serial.print("], next: ");
      Serial.print(nextPacketPtr,HEX);
      Serial.print(", stat: ");
      Serial.print(rxstat,HEX);
      Serial.print(", count: ");
      Serial.print(readReg(EPKTCNT));
      Serial.print(" -> ");
      Serial.println((rxstat & 0x80)!=0 ? "OK" : "failed");
#endif
      // decrement the packet counter indicate we are done with this packet
      writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
      // check CRC and symbol errors (see datasheet page 44, table 7-3):
      // The ERXFCON.CRCEN is set by default. Normally we should not
      // need to check this.
      if ((rxstat & 0x80) != 0)
        {
          receivePkt.begin = readPtr;
          receivePkt.size = len;
#ifdef ENC28J60DEBUG
          Serial.print("receivePkt.size=");
          Serial.println(len);
#endif
          return UIP_RECEIVEBUFFERHANDLE;
        }
      // Move the RX read pointer to the start of the next received packet
      // This frees the memory we just read out
      setERXRDPT();
    }
  return (NOBLOCK);
}

void
Enc28J60Network::setERXRDPT()
{
  writeRegPair(ERXRDPTL, nextPacketPtr == RXSTART_INIT ? RXSTOP_INIT : nextPacketPtr-1);
}

memaddress
Enc28J60Network::blockSize(memhandle handle)
{
  return handle == NOBLOCK ? 0 : handle == UIP_RECEIVEBUFFERHANDLE ? receivePkt.size : blocks[handle].size;
}

void
Enc28J60Network::sendPacket(memhandle handle)
{
  memblock *packet = &blocks[handle];
  uint16_t start = packet->begin-1;
  uint16_t end = start + packet->size;

  // backup data at control-byte position
  uint8_t data = readByte(start);
  // write control-byte (if not 0 anyway)
  if (data)
    writeByte(start, 0);

#ifdef ENC28J60DEBUG
  Serial.print("sendPacket(");
  Serial.print(handle);
  Serial.print(") [");
  Serial.print(start,HEX);
  Serial.print("-");
  Serial.print(end,HEX);
  Serial.print("]: ");
  for (uint16_t i=start; i<=end; i++)
    {
      Serial.print(readByte(i),HEX);
      Serial.print(" ");
    }
  Serial.println();
#endif

  // TX start
  writeRegPair(ETXSTL, start);
  // Set the TXND pointer to correspond to the packet size given
  writeRegPair(ETXNDL, end);
  // send the contents of the transmit buffer onto the network
  writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
  // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
  if( (readReg(EIR) & EIR_TXERIF) )
    {
      writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }

  //restore data on control-byte position
  if (data)
    writeByte(start, data);
}

uint16_t
Enc28J60Network::setReadPtr(memhandle handle, memaddress position, uint16_t len)
{
  memblock *packet = handle == UIP_RECEIVEBUFFERHANDLE ? &receivePkt : &blocks[handle];
  memaddress start = handle == UIP_RECEIVEBUFFERHANDLE && packet->begin + position > RXSTOP_INIT ? packet->begin + position-RXSTOP_INIT+RXSTART_INIT : packet->begin + position;

  writeRegPair(ERDPTL, start);
  
  if (len > packet->size - position)
    len = packet->size - position;
  return len;
}

uint16_t
Enc28J60Network::readPacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len)
{
  len = setReadPtr(handle, position, len);
  readBuffer(len, buffer);
  return len;
}

uint16_t
Enc28J60Network::writePacket(memhandle handle, memaddress position, uint8_t* buffer, uint16_t len)
{
  memblock *packet = &blocks[handle];
  uint16_t start = packet->begin + position;

  writeRegPair(EWRPTL, start);

  if (len > packet->size - position)
    len = packet->size - position;
  writeBuffer(len, buffer);
  return len;
}


void Enc28J60Network::enableBroadcast (bool temporary) {
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_BCEN);
    if(!temporary)
        broadcast_enabled = true;
}

void Enc28J60Network::disableBroadcast (bool temporary) {
    if(!temporary)
        broadcast_enabled = false;
    if(!broadcast_enabled)
        writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_BCEN);
}

void Enc28J60Network::enableMulticast () {
    writeRegByte(ERXFCON, readRegByte(ERXFCON) | ERXFCON_MCEN);
}

void Enc28J60Network::disableMulticast () {
    writeRegByte(ERXFCON, readRegByte(ERXFCON) & ~ERXFCON_MCEN);
}

byte Enc28J60Network::readRegByte (uint8_t address) {
    setBank(address);
    return readOp(ENC28J60_READ_CTRL_REG, address);
}

void Enc28J60Network::writeRegByte (uint8_t address, uint8_t data) {
    setBank(address);
    writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}


uint8_t Enc28J60Network::readByte(uint16_t addr)
{
  writeRegPair(ERDPTL, addr);

  CSACTIVE;
  SPI.transfer(ENC28J60_READ_BUF_MEM);
  uint8_t data = SPI.transfer(0x00);
  CSPASSIVE;
  return data;
}

void Enc28J60Network::writeByte(uint16_t addr, uint8_t data)
{
  writeRegPair(EWRPTL, addr);

  CSACTIVE;
  SPI.transfer(ENC28J60_WRITE_BUF_MEM);
  SPI.transfer(data);
  CSPASSIVE;
}

void
Enc28J60Network::copyPacket(memhandle dest_pkt, memaddress dest_pos, memhandle src_pkt, memaddress src_pos, uint16_t len)
{
  memblock *dest = &blocks[dest_pkt];
  memblock *src = src_pkt == UIP_RECEIVEBUFFERHANDLE ? &receivePkt : &blocks[src_pkt];
  memaddress start = src_pkt == UIP_RECEIVEBUFFERHANDLE && src->begin + src_pos > RXSTOP_INIT ? src->begin + src_pos-RXSTOP_INIT+RXSTART_INIT : src->begin + src_pos;
  enc28J60_mempool_block_move_callback(dest->begin+dest_pos,start,len);
  // Move the RX read pointer to the start of the next received packet
  // This frees the memory we just read out
  setERXRDPT();
}

void
enc28J60_mempool_block_move_callback(memaddress dest, memaddress src, memaddress len)
{
//void
//Enc28J60Network::memblock_mv_cb(uint16_t dest, uint16_t src, uint16_t len)
//{
  //as ENC28J60 DMA is unable to copy single bytes:
  if (len == 1)
    {
      Enc28J60Network::writeByte(dest,Enc28J60Network::readByte(src));
    }
  else
    {
      // calculate address of last byte
      len += src - 1;

      /*  1. Appropriately program the EDMAST, EDMAND
       and EDMADST register pairs. The EDMAST
       registers should point to the first byte to copy
       from, the EDMAND registers should point to the
       last byte to copy and the EDMADST registers
       should point to the first byte in the destination
       range. The destination range will always be
       linear, never wrapping at any values except from
       8191 to 0 (the 8-Kbyte memory boundary).
       Extreme care should be taken when
       programming the start and end pointers to
       prevent a never ending DMA operation which
       would overwrite the entire 8-Kbyte buffer.
       */
      Enc28J60Network::writeRegPair(EDMASTL, src);
      Enc28J60Network::writeRegPair(EDMADSTL, dest);

      if ((src <= RXSTOP_INIT)&& (len > RXSTOP_INIT))len -= (RXSTOP_INIT-RXSTART_INIT);
      Enc28J60Network::writeRegPair(EDMANDL, len);

      /*
       2. If an interrupt at the end of the copy process is
       desired, set EIE.DMAIE and EIE.INTIE and
       clear EIR.DMAIF.

       3. Verify that ECON1.CSUMEN is clear. */
      Enc28J60Network::writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_CSUMEN);

      /* 4. Start the DMA copy by setting ECON1.DMAST. */
      Enc28J60Network::writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_DMAST);

      // wait until runnig DMA is completed
      while (Enc28J60Network::readOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_DMAST);
    }
}

void
Enc28J60Network::freePacket()
{
    setERXRDPT();
}

uint8_t
Enc28J60Network::readOp(uint8_t op, uint8_t address)
{
  CSACTIVE;
  // issue read command
  SPI.transfer(op | (address & ADDR_MASK));
  byte result = SPI.transfer(0x00);
  if (address & 0x80)
    result = SPI.transfer(0x00);	
  CSPASSIVE;
  return result;
}

void
Enc28J60Network::writeOp(uint8_t op, uint8_t address, uint8_t data)
{
  CSACTIVE;
  // issue write command
  SPI.transfer(op | (address & ADDR_MASK));
  SPI.transfer(data);
  CSPASSIVE;
}

void
Enc28J60Network::readBuffer(uint16_t len, uint8_t* data)
{
  CSACTIVE;
  // issue read command
  SPI.transfer(ENC28J60_READ_BUF_MEM);
  while (len--) {
    *data++ = SPI.transfer(0x00);
  }
  CSPASSIVE;
}

void
Enc28J60Network::writeBuffer(uint16_t len, uint8_t* data)
{
  CSACTIVE;
  // issue write command
  SPI.transfer(ENC28J60_WRITE_BUF_MEM);
  while (len--) {
    SPI.transfer(*data++);
  }
  CSPASSIVE;
}

void
Enc28J60Network::setBank(uint8_t address)
{
  // set the bank (if needed)
  if((address & BANK_MASK) != bank)
  {
    // set the bank
    writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
    writeOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
    bank = (address & BANK_MASK);
  }
}

uint8_t
Enc28J60Network::readReg(uint8_t address)
{
  // set the bank
  setBank(address);
  // do the read
  return readOp(ENC28J60_READ_CTRL_REG, address);
}

void
Enc28J60Network::writeReg(uint8_t address, uint8_t data)
{
  // set the bank
  setBank(address);
  // do the write
  writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void
Enc28J60Network::writeRegPair(uint8_t address, uint16_t data)
{
  // set the bank
  setBank(address);
  // do the write
  writeOp(ENC28J60_WRITE_CTRL_REG, address, (data&0xFF));
  writeOp(ENC28J60_WRITE_CTRL_REG, address+1, (data) >> 8);
}

void
Enc28J60Network::phyWrite(uint8_t address, uint16_t data)
{
  // set the PHY register address
  writeReg(MIREGADR, address);
  // write the PHY data
  writeRegPair(MIWRL, data);
  // wait until the PHY write completes
  while(readReg(MISTAT) & MISTAT_BUSY){
    delayMicroseconds(15);
  }
}

uint16_t
Enc28J60Network::phyRead(uint8_t address)
{
  writeReg(MIREGADR,address);
  writeReg(MICMD, MICMD_MIIRD);
  // wait until the PHY read completes
  while(readReg(MISTAT) & MISTAT_BUSY){
    delayMicroseconds(15);
  }  //and MIRDH
  writeReg(MICMD, 0);
  return (readReg(MIRDL) | readReg(MIRDH) << 8);
}

void
Enc28J60Network::clkout(uint8_t clk)
{
  //setup clkout: 2 is 12.5MHz:
  writeReg(ECOCON, clk & 0x7);
}

// read the revision of the chip:
uint8_t
Enc28J60Network::getrev(void)
{
  return(readReg(EREVID));
}

uint16_t
Enc28J60Network::chksum(uint16_t sum, memhandle handle, memaddress pos, uint16_t len)
{
  uint16_t t;
  len = setReadPtr(handle, pos, len)-1;
  CSACTIVE;
  // issue read command
  SPI.transfer(ENC28J60_READ_BUF_MEM);
  uint16_t i;
  for (i = 0; i < len; i+=2)
  {
    // read data
    t = SPI.transfer(0x00) << 8;
    t += SPI.transfer(0x00);
    sum += t;
    if(sum < t) {
      sum++;            // carry 
    }
  }
  if(i == len) {
    t = (SPI.transfer(0x00) << 8) + 0;
    sum += t;
    if(sum < t) {
      sum++;            // carry 
    }
  }
  CSPASSIVE;

  // Return sum in host byte order. 

  return sum;
}

void
Enc28J60Network::powerOff()
{
  writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
  delay(50);
  writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
  delay(50);
  writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void
Enc28J60Network::powerOn()
{
  writeOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
  delay(50);
  writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
  delay(50);
}

bool
Enc28J60Network::linkStatus()
{
  return (phyRead(PHSTAT2) & 0x0400) > 0;
}

Enc28J60Network Enc28J60;
