/* Arduino SdSpiAltDriver Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino SdSpiAltDriver Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdSpiAltDriver Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "SdSpiDriver.h"
#if defined(__arm__) && defined(CORE_TEENSY)
// SPI definitions
#include "kinetis.h"

//------------------------------------------------------------------------------
void SdSpiAltDriver::activate() {
  SPI.beginTransaction(m_spiSettings);
}
//------------------------------------------------------------------------------
void SdSpiAltDriver::begin(uint8_t chipSelectPin) {
  m_csPin = chipSelectPin;
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);
  SPI.begin();
}
//------------------------------------------------------------------------------
void SdSpiAltDriver::deactivate() {
  SPI.endTransaction();
}
//==============================================================================
#ifdef KINETISK

// use 16-bit frame if SPI_USE_8BIT_FRAME is zero
#define SPI_USE_8BIT_FRAME 0
// Limit initial fifo to three entries to avoid fifo overrun
#define SPI_INITIAL_FIFO_DEPTH 3
// define some symbols that are not in mk20dx128.h
#ifndef SPI_SR_RXCTR
#define SPI_SR_RXCTR 0XF0
#endif  // SPI_SR_RXCTR
#ifndef SPI_PUSHR_CONT
#define SPI_PUSHR_CONT 0X80000000
#endif   // SPI_PUSHR_CONT
#ifndef SPI_PUSHR_CTAS
#define SPI_PUSHR_CTAS(n) (((n) & 7) << 28)
#endif  // SPI_PUSHR_CTAS
//------------------------------------------------------------------------------
/** SPI receive a byte */
uint8_t SdSpiAltDriver::receive() {
  SPI0_MCR |= SPI_MCR_CLR_RXF;
  SPI0_SR = SPI_SR_TCF;
  SPI0_PUSHR = 0xFF;
  while (!(SPI0_SR & SPI_SR_TCF)) {}
  return SPI0_POPR;
}
//------------------------------------------------------------------------------
/** SPI receive multiple bytes */
uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n) {
  // clear any data in RX FIFO
  SPI0_MCR = SPI_MCR_MSTR | SPI_MCR_CLR_RXF | SPI_MCR_PCSIS(0x1F);
#if SPI_USE_8BIT_FRAME
  // initial number of bytes to push into TX FIFO
  int nf = n < SPI_INITIAL_FIFO_DEPTH ? n : SPI_INITIAL_FIFO_DEPTH;
  for (int i = 0; i < nf; i++) {
    SPI0_PUSHR = 0XFF;
  }
  // limit for pushing dummy data into TX FIFO
  uint8_t* limit = buf + n - nf;
  while (buf < limit) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_PUSHR = 0XFF;
    *buf++ = SPI0_POPR;
  }
  // limit for rest of RX data
  limit += nf;
  while (buf < limit) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    *buf++ = SPI0_POPR;
  }
#else  // SPI_USE_8BIT_FRAME
  // use 16 bit frame to avoid TD delay between frames
  // get one byte if n is odd
  if (n & 1) {
    *buf++ = receive();
    n--;
  }
  // initial number of words to push into TX FIFO
  int nf = n/2 < SPI_INITIAL_FIFO_DEPTH ? n/2 : SPI_INITIAL_FIFO_DEPTH;
  for (int i = 0; i < nf; i++) {
    SPI0_PUSHR = SPI_PUSHR_CONT | SPI_PUSHR_CTAS(1) | 0XFFFF;
  }
  uint8_t* limit = buf + n - 2*nf;
  while (buf < limit) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_PUSHR = SPI_PUSHR_CONT | SPI_PUSHR_CTAS(1) | 0XFFFF;
    uint16_t w = SPI0_POPR;
    *buf++ = w >> 8;
    *buf++ = w & 0XFF;
  }
  // limit for rest of RX data
  limit += 2*nf;
  while (buf < limit) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    uint16_t w = SPI0_POPR;
    *buf++ = w >> 8;
    *buf++ = w & 0XFF;
  }
#endif  // SPI_USE_8BIT_FRAME
  return 0;
}
//------------------------------------------------------------------------------
/** SPI send a byte */
void SdSpiAltDriver::send(uint8_t b) {
  SPI0_MCR |= SPI_MCR_CLR_RXF;
  SPI0_SR = SPI_SR_TCF;
  SPI0_PUSHR = b;
  while (!(SPI0_SR & SPI_SR_TCF)) {}
}
//------------------------------------------------------------------------------
/** SPI send multiple bytes */
void SdSpiAltDriver::send(const uint8_t* buf , size_t n) {
  // clear any data in RX FIFO
  SPI0_MCR = SPI_MCR_MSTR | SPI_MCR_CLR_RXF | SPI_MCR_PCSIS(0x1F);
#if SPI_USE_8BIT_FRAME
  // initial number of bytes to push into TX FIFO
  int nf = n < SPI_INITIAL_FIFO_DEPTH ? n : SPI_INITIAL_FIFO_DEPTH;
  // limit for pushing data into TX fifo
  const uint8_t* limit = buf + n;
  for (int i = 0; i < nf; i++) {
    SPI0_PUSHR = *buf++;
  }
  // write data to TX FIFO
  while (buf < limit) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_PUSHR = *buf++;
    SPI0_POPR;
  }
  // wait for data to be sent
  while (nf) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_POPR;
    nf--;
  }
#else  // SPI_USE_8BIT_FRAME
  // use 16 bit frame to avoid TD delay between frames
  // send one byte if n is odd
  if (n & 1) {
    send(*buf++);
    n--;
  }
  // initial number of words to push into TX FIFO
  int nf = n/2 < SPI_INITIAL_FIFO_DEPTH ? n/2 : SPI_INITIAL_FIFO_DEPTH;
  // limit for pushing data into TX fifo
  const uint8_t* limit = buf + n;
  for (int i = 0; i < nf; i++) {
    uint16_t w = (*buf++) << 8;
    w |= *buf++;
    SPI0_PUSHR = SPI_PUSHR_CONT | SPI_PUSHR_CTAS(1) | w;
  }
  // write data to TX FIFO
  while (buf < limit) {
    uint16_t w = *buf++ << 8;
    w |= *buf++;
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_PUSHR = SPI_PUSHR_CONT | SPI_PUSHR_CTAS(1) | w;
    SPI0_POPR;
  }
  // wait for data to be sent
  while (nf) {
    while (!(SPI0_SR & SPI_SR_RXCTR)) {}
    SPI0_POPR;
    nf--;
  }
#endif  // SPI_USE_8BIT_FRAME
}
#else  // KINETISK
//==============================================================================
// Use standard SPI library if not KINETISK
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdSpiAltDriver::receive() {
  return SPI.transfer(0XFF);
}
/** Receive multiple bytes.
 *
 * \param[out] buf Buffer to receive the data.
 * \param[in] n Number of bytes to receive.
 *
 * \return Zero for no error or nonzero error code.
 */
uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n) {
  for (size_t i = 0; i < n; i++) {
    buf[i] = SPI.transfer(0XFF);
  }
  return 0;
}
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdSpiAltDriver::send(uint8_t b) {
  SPI.transfer(b);
}
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
void SdSpiAltDriver::send(const uint8_t* buf , size_t n) {
  for (size_t i = 0; i < n; i++) {
    SPI.transfer(buf[i]);
  }
}
#endif  // KINETISK
#endif  // defined(__arm__) && defined(CORE_TEENSY)
