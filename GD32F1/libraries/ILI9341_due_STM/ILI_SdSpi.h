/* Arduino SdSpi Library
 * Copyright (C) 2013 by William Greiman
 *
 * This file is part of the Arduino SdSpi Library
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
 * along with the Arduino SdSpi Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
 /**
 * \file
 * \brief SdSpi class for V2 SD/SDHC cards
 */
#ifndef ILI_SdSpi_h
#define ILI_SdSpi_h
#include <Arduino.h>
#include "ILI_SdFatConfig.h"

#if !ILI_USE_ARDUINO_SPI_LIBRARY
// AVR Arduinos
#ifdef __AVR__
#if ILI_AVR_SOFT_SPI
#define ILI_USE_SOFTWARE_SPI 1
#elif ILI_LEONARDO_SOFT_SPI && defined(__AVR_ATmega32U4__) && !defined(CORE_TEENSY)
#define ILI_USE_SOFTWARE_SPI 1
#elif ILI_MEGA_SOFT_SPI && (defined(__AVR_ATmega1280__)\
      ||defined(__AVR_ATmega2560__))
#define ILI_USE_SOFTWARE_SPI 1
#else  // ILI_USE_SOFTWARE_SPI
#define ILI_USE_NATIVE_AVR_SPI 1
#endif  // ILI_USE_SOFTWARE_SPI
#endif  // __AVR__
// Due
#if defined(__arm__) && !defined(CORE_TEENSY) && !defined(__STM32F1__)
#if ILI_DUE_SOFT_SPI
#define ILI_USE_SOFTWARE_SPI 1
#else  // ILI_DUE_SOFT_SPI
/** Nonzero - use native SAM3X SPI */
#define ILI_USE_NATIVE_SAM3X_SPI 1
#endif  // ILI_DUE_SOFT_SPI
#endif  // defined(__arm__) && !defined(CORE_TEENSY | __STM32F1__)

// STM32F1 Maple Mini
#if defined(__arm__) && defined(__STM32F1__)
#if ILI_DUE_SOFT_SPI
#define ILI_USE_SOFTWARE_SPI 1
#else  // ILI_DUE_SOFT_SPI
/** Nonzero - use native STM32F1 SPI */
#define ILI_USE_NATIVE_STM32F1_SPI 1
#endif  // ILI_DUE_SOFT_SPI
#endif  // defined(__arm__) && defined(__STM32F1__)

// Teensy 3.0
#if defined(__arm__) && defined(CORE_TEENSY)
#if ILI_TEENSY3_SOFT_SPI
#define ILI_USE_SOFTWARE_SPI 1
#else  // ILI_TEENSY3_SOFT_SPI
/** Nonzero - use native MK20DX128 SPI */
#define ILI_USE_NATIVE_TEENSY3_SPI 1
#endif  // ILI_TEENSY3_SOFT_SPI
#endif  // defined(__arm__) && defined(CORE_TEENSY)
#endif  // !ILI_USE_ARDUINO_SPI_LIBRARY

#ifndef ILI_USE_SOFTWARE_SPI
#define ILI_USE_SOFTWARE_SPI 0
#endif  //  ILI_USE_SOFTWARE_SPI

#ifndef ILI_USE_NATIVE_AVR_SPI
#define ILI_USE_NATIVE_AVR_SPI 0
#endif

#ifndef ILI_USE_NATIVE_SAM3X_SPI
#define ILI_USE_NATIVE_SAM3X_SPI 0
#endif  // ILI_USE_NATIVE_SAM3X_SPI

#ifndef ILI_USE_NATIVE_STM32F1_SPI
#define ILI_USE_NATIVE_STM32F1_SPI 0
#endif  // ILI_USE_NATIVE_STM32F1_SPI

#ifndef ILI_USE_NATIVE_TEENSY3_SPI
#define ILI_USE_NATIVE_TEENSY3_SPI 0
#endif  // ILI_USE_NATIVE_TEENSY3_SPI
//------------------------------------------------------------------------------
// define default chip select pin
//
#if !ILI_USE_SOFTWARE_SPI
/** The default chip select pin for the SD card is SS. */
uint8_t const  ILI_SD_CHIP_SELECT_PIN = SS;
#else  // USE_AVR_SOFTWARE_SPI
/** SPI chip select pin for software SPI. */
uint8_t const ILI_SD_CHIP_SELECT_PIN = SOFT_SPI_CS_PIN;
#endif  // USE_AVR_SOFTWARE_SPI

//------------------------------------------------------------------------------
/**
 * \class SdSpi
 * \brief SPI class for access to SD and SDHC flash memory cards.
 */
class ILI_SdSpi {
 public:
  /** Initialize the SPI bus */
  void begin();
  /** Set SPI options for access to SD/SDHC cards.
   * 
   * \param[in] spiDivisor SCK clock divider relative to the system clock.
   */
  void init(uint8_t spiDivisor);
  /** Receive a byte. 
   *
   * \return The byte.
   */
  uint8_t receive();
  /** Receive multiple bytes.
   *
   * \param[out] buf Buffer to receive the data.   
   * \param[in] n Number of bytes to receive.
   *
   * \return Zero for no error or nonzero error code.
   */   
  uint8_t receive(uint8_t* buf, size_t n);
  /** Send a byte.
   *
   * \param[in] data Byte to send
   */
  void send(uint8_t data);
   /** Send multiple bytes.
   *
   * \param[in] buf Buffer for data to be sent.   
   * \param[in] n Number of bytes to send.
   */   
  void send(uint8_t* buf, size_t n);
};
//------------------------------------------------------------------------------
// Use of inline for AVR results in up to 10% better write performance.
// Inline also save a little flash memory.
/** inline avr native functions if nonzero. */
#define ILI_USE_AVR_NATIVE_SPI_INLINE 1
#if ILI_USE_NATIVE_AVR_SPI && ILI_USE_AVR_NATIVE_SPI_INLINE
inline uint8_t ILI_SdSpi::receive() {
  SPDR = 0XFF;
  while (!(SPSR & (1 << SPIF))) {}
  return SPDR;
}
inline uint8_t ILI_SdSpi::receive(uint8_t* buf, size_t n) {
  if (n-- == 0) return 0;
  SPDR = 0XFF;
  for (size_t i = 0; i < n; i++) {
    while (!(SPSR & (1 << SPIF))) {}
    uint8_t b = SPDR;
    SPDR = 0XFF;
    buf[i] = b;
  }
  while (!(SPSR & (1 << SPIF))) {}
  buf[n] = SPDR;
  return 0;
}
inline void ILI_SdSpi::send(uint8_t data) {
  SPDR = data;
  while (!(SPSR & (1 << SPIF))) {}
}
inline void ILI_SdSpi::send(const uint8_t* buf , size_t n) {
  if (n == 0) return;
  SPDR = buf[0];
  if (n > 1) {
    uint8_t b = buf[1];
    size_t i = 2;
    while (1) {
      while (!(SPSR & (1 << SPIF))) {}
      SPDR = b;
      if (i == n) break;
      b = buf[i++];
    }
  }
  while (!(SPSR & (1 << SPIF))) {}
}
#endif  // ILI_USE_NATIVE_AVR_SPI && ILI_USE_AVR_NATIVE_SPI_INLINE
#endif  // ILI_SdSpi_h

