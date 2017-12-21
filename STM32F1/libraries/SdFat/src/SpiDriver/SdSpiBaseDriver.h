/* Arduino SdCard Library
 * Copyright (C) 2016 by William Greiman
 *
 * This file is part of the Arduino SdSpiCard Library
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
 * along with the Arduino SdSpiCard Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdSpiBaseDriver_h
#define SdSpiBaseDriver_h
/**
 * \class SdSpiBaseDriver
 * \brief SPI base driver.
 */
class SdSpiBaseDriver {
 public:
  /** Set SPI options for access to SD/SDHC cards.
   *
   */
  virtual void activate() = 0;
  /** Initialize the SPI bus.
   *
   * \param[in] chipSelectPin SD card chip select pin.
   */
  virtual void begin(uint8_t chipSelectPin) = 0;
  /**
   * End SPI transaction.
   */
  virtual void deactivate() = 0;
  /** Receive a byte.
   *
   * \return The byte.
   */
  virtual uint8_t receive() = 0;
  /** Receive multiple bytes.
   *
   * \param[out] buf Buffer to receive the data.
   * \param[in] n Number of bytes to receive.
   *
   * \return Zero for no error or nonzero error code.
   */
  virtual uint8_t receive(uint8_t* buf, size_t n) = 0;
  /** Send a byte.
   *
   * \param[in] data Byte to send
   */
  virtual void send(uint8_t data) = 0;
  /** Send multiple bytes.
  *
  * \param[in] buf Buffer for data to be sent.
  * \param[in] n Number of bytes to send.
  */
  virtual void send(const uint8_t* buf, size_t n) = 0;
  /** Set CS low. */
  virtual void select() = 0;
  /** Save SPI settings.
   * \param[in] spiSettings SPI speed, mode, and bit order.
   */
  virtual void setSpiSettings(SPISettings spiSettings) = 0;
  /** Set CS high. */
  virtual void unselect() = 0;
};
#endif  // SdSpiBaseDriver_h
