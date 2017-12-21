/* Arduino SdSpiAltDriver Library
 * Copyright (C) 2016 by William Greiman
 *
 * STM32F1 code for Maple and Maple Mini support, 2015 by Victor Perez
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
#if defined(ESP8266)
#include "SdSpiDriver.h"
//------------------------------------------------------------------------------
/** Initialize the SPI bus.
 *
 * \param[in] chipSelectPin SD card chip select pin.
 */
void SdSpiAltDriver::begin(uint8_t csPin) {
  m_csPin = csPin;
  pinMode(m_csPin, OUTPUT);
  digitalWrite(m_csPin, HIGH);
  SPI.begin();
}
//------------------------------------------------------------------------------
/** Set SPI options for access to SD/SDHC cards.
 *
 */
void SdSpiAltDriver::activate() {
  SPI.beginTransaction(m_spiSettings);
}
//------------------------------------------------------------------------------
void SdSpiAltDriver::deactivate() {
  // Note: endTransaction is an empty function on ESP8266.
  SPI.endTransaction();
}
//------------------------------------------------------------------------------
/** Receive a byte.
 *
 * \return The byte.
 */
uint8_t SdSpiAltDriver::receive() {
  return SPI.transfer(0XFF);
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
  // Works without 32-bit alignment of buf.
  SPI.transferBytes(0, buf, n);
  return 0;
}
//------------------------------------------------------------------------------
/** Send a byte.
 *
 * \param[in] b Byte to send
 */
void SdSpiAltDriver::send(uint8_t b) {
  SPI.transfer(b);
}
//------------------------------------------------------------------------------
/** Send multiple bytes.
 *
 * \param[in] buf Buffer for data to be sent.
 * \param[in] n Number of bytes to send.
 */
void SdSpiAltDriver::send(const uint8_t* buf , size_t n) {
  // Adjust to 32-bit alignment.
  while ((reinterpret_cast<uintptr_t>(buf) & 0X3) && n) {
    SPI.transfer(*buf++);
    n--;
  }
  SPI.transferBytes(const_cast<uint8_t*>(buf), 0, n);
}
#endif  // defined(ESP8266)
