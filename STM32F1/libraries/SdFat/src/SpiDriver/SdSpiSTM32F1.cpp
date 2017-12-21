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
#if defined(__STM32F1__)
#include "SdSpiDriver.h"
#define USE_STM32F1_DMAC 1
//------------------------------------------------------------------------------
static SPIClass m_SPI1(1);
#if BOARD_NR_SPI > 1
static SPIClass m_SPI2(2);
#endif  // BOARD_NR_SPI > 1
#if BOARD_NR_SPI > 2
static SPIClass m_SPI3(3);
#endif  // BOARD_NR_SPI > 2
//
static SPIClass* pSpi[] =
#if BOARD_NR_SPI == 1
  {&m_SPI1};
#elif BOARD_NR_SPI == 2
  {&m_SPI1, &m_SPI2};
#elif BOARD_NR_SPI == 3
  {&m_SPI1, &m_SPI2, &m_SPI3};
#else  // BOARD_NR_SPI
#error "BOARD_NR_SPI too large"
#endif  // BOARD_NR_SPI
//------------------------------------------------------------------------------
/** Set SPI options for access to SD/SDHC cards.
 *
 * \param[in] divisor SCK clock divider relative to the APB1 or APB2 clock.
 */
void SdSpiAltDriver::activate() {
  pSpi[m_spiPort]->beginTransaction(m_spiSettings);
}
//------------------------------------------------------------------------------
/** Initialize the SPI bus.
 *
 * \param[in] chipSelectPin SD card chip select pin.
 */
void SdSpiAltDriver::begin(uint8_t csPin) {
  m_csPin = csPin;
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);
  pSpi[m_spiPort]->begin();
}
//------------------------------------------------------------------------------
/**
 * End SPI transaction.
 */
void SdSpiAltDriver::deactivate() {
  pSpi[m_spiPort]->endTransaction();
}
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdSpiAltDriver::receive() {
  return pSpi[m_spiPort]->transfer(0XFF);
}
//------------------------------------------------------------------------------
/** Receive multiple bytes.
 *
 * \param[out] buf Buffer to receive the data.
 * \param[in] n Number of bytes to receive.
 *
 * \return Zero for no error or nonzero error code.
 */
uint8_t SdSpiAltDriver::receive(uint8_t* buf, size_t n) {
  int rtn = 0;
#if USE_STM32F1_DMAC
  rtn = pSpi[m_spiPort]->dmaTransfer(0, const_cast<uint8*>(buf), n);
#else  // USE_STM32F1_DMAC
  //  pSpi[m_spiPort]->read(buf, n); fails ?? use byte transfer
  for (size_t i = 0; i < n; i++) {
    buf[i] = pSpi[m_spiPort]->transfer(0XFF);
  }
#endif  // USE_STM32F1_DMAC
  return rtn;
}
//------------------------------------------------------------------------------
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdSpiAltDriver::send(uint8_t b) {
  pSpi[m_spiPort]->transfer(b);
}
//------------------------------------------------------------------------------
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
void SdSpiAltDriver::send(const uint8_t* buf , size_t n) {
#if USE_STM32F1_DMAC
  pSpi[m_spiPort]->dmaSend(const_cast<uint8*>(buf), n);
#else  // #if USE_STM32F1_DMAC
  pSpi[m_spiPort]->write(buf, n);
#endif  // USE_STM32F1_DMAC
}
//-----------------------------------------------------------------------------
void SdSpiAltDriver::setPort(uint8_t portNumber) {
  m_spiPort = portNumber < 1 || portNumber > BOARD_NR_SPI ? 0 : portNumber -1;
}
#endif  // defined(__STM32F1__)
