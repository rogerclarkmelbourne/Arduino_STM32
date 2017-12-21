/* Arduino SdFat Library
 * Copyright (C) 2012 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
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
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
/**
 * \file
 * \brief configuration definitions
 */
#ifndef SdFatConfig_h
#define SdFatConfig_h
#include <stdint.h>
#ifdef __AVR__
#include <avr/io.h>
#endif  // __AVR__
//------------------------------------------------------------------------------
/**
 * Set USE_LONG_FILE_NAMES nonzero to use long file names (LFN).
 * Long File Name are limited to a maximum length of 255 characters.
 *
 * This implementation allows 7-bit characters in the range
 * 0X20 to 0X7E except the following characters are not allowed:
 *
 *  < (less than)
 *  > (greater than)
 *  : (colon)
 *  " (double quote)
 *  / (forward slash)
 *  \ (backslash)
 *  | (vertical bar or pipe)
 *  ? (question mark)
 *  * (asterisk)
 *
 */
#define USE_LONG_FILE_NAMES 1
//------------------------------------------------------------------------------
/**
 * If the symbol ENABLE_EXTENDED_TRANSFER_CLASS is nonzero, the class SdFatEX
 * will be defined. If the symbol ENABLE_SOFTWARE_SPI_CLASS is also nonzero,
 * the class SdFatSoftSpiEX will be defined.
 *
 * These classes used extended multi-block SD I/O for better performance.
 * the SPI bus may not be shared with other devices in this mode.
 */
#define ENABLE_EXTENDED_TRANSFER_CLASS 0
//-----------------------------------------------------------------------------
/**
 * If the symbol USE_STANDARD_SPI_LIBRARY is nonzero, the classes SdFat and
 * SdFatEX use the standard Arduino SPI.h library. If USE_STANDARD_SPI_LIBRARY
 * is zero, an optimized custom SPI driver is used if it exists.
 */ 
#define USE_STANDARD_SPI_LIBRARY 0
//-----------------------------------------------------------------------------
/**
 * If the symbol ENABLE_SOFTWARE_SPI_CLASS is nonzero, the class SdFatSoftSpi
 * will be defined. If ENABLE_EXTENDED_TRANSFER_CLASS is also nonzero,
 * the class SdFatSoftSpiEX will be defined.
 */
#define ENABLE_SOFTWARE_SPI_CLASS 0
//------------------------------------------------------------------------------
/** 
 * Set MAINTAIN_FREE_CLUSTER_COUNT nonzero to keep the count of free clusters
 * updated.  This will increase the speed of the freeClusterCount() call
 * after the first call.  Extra flash will be required.
 */
#define MAINTAIN_FREE_CLUSTER_COUNT 0
//------------------------------------------------------------------------------
/**
 * To enable SD card CRC checking set USE_SD_CRC nonzero.
 *
 * Set USE_SD_CRC to 1 to use a smaller CRC-CCITT function.  This function
 * is slower for AVR but may be fast for ARM and other processors.
 *
 * Set USE_SD_CRC to 2 to used a larger table driven CRC-CCITT function.  This
 * function is faster for AVR but may be slower for ARM and other processors.
 */
#define USE_SD_CRC 0
//------------------------------------------------------------------------------
/**
 * Handle Watchdog Timer for WiFi modules.
 *
 * Yield will be called before accessing the SPI bus if it has been more
 * than WDT_YIELD_TIME_MICROS microseconds since the last yield call by SdFat.
 */
#if defined(PLATFORM_ID) || defined(ESP8266)
// If Particle device or ESP8266 call yield.
#define WDT_YIELD_TIME_MICROS 100000
#else
#define WDT_YIELD_TIME_MICROS 0
#endif
//------------------------------------------------------------------------------
/**
 * Set FAT12_SUPPORT nonzero to enable use if FAT12 volumes.
 * FAT12 has not been well tested and requires additional flash.
 */
#define FAT12_SUPPORT 0
//------------------------------------------------------------------------------
/**
 * Set DESTRUCTOR_CLOSES_FILE nonzero to close a file in its destructor.
 *
 * Causes use of lots of heap in ARM.
 */
#define DESTRUCTOR_CLOSES_FILE 0
//------------------------------------------------------------------------------
/**
 * Call flush for endl if ENDL_CALLS_FLUSH is nonzero
 *
 * The standard for iostreams is to call flush.  This is very costly for
 * SdFat.  Each call to flush causes 2048 bytes of I/O to the SD.
 *
 * SdFat has a single 512 byte buffer for SD I/O so it must write the current
 * data block to the SD, read the directory block from the SD, update the
 * directory entry, write the directory block to the SD and read the data
 * block back into the buffer.
 *
 * The SD flash memory controller is not designed for this many rewrites
 * so performance may be reduced by more than a factor of 100.
 *
 * If ENDL_CALLS_FLUSH is zero, you must call flush and/or close to force
 * all data to be written to the SD.
 */
#define ENDL_CALLS_FLUSH 0
//------------------------------------------------------------------------------
/**
 * Set USE_SEPARATE_FAT_CACHE nonzero to use a second 512 byte cache
 * for FAT table entries.  This improves performance for large writes
 * that are not a multiple of 512 bytes.
 */
#ifdef __arm__
#define USE_SEPARATE_FAT_CACHE 1
#else  // __arm__
#define USE_SEPARATE_FAT_CACHE 0
#endif  // __arm__
//------------------------------------------------------------------------------
/**
 * Set USE_MULTI_BLOCK_IO nonzero to use multi-block SD read/write.
 *
 * Don't use mult-block read/write on small AVR boards.
 */
#if defined(RAMEND) && RAMEND < 3000
#define USE_MULTI_BLOCK_IO 0
#else  // RAMEND
#define USE_MULTI_BLOCK_IO 1
#endif  // RAMEND
//-----------------------------------------------------------------------------
/** Enable SDIO driver if available. */
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
#define ENABLE_SDIO_CLASS 1
#define ENABLE_SDIOEX_CLASS 1
#elif defined(__STM32F1__)
#define ENABLE_SDIO_CLASS 1
#else  // ENABLE_SDIO_CLASS
#define ENABLE_SDIO_CLASS 0
#endif  // ENABLE_SDIO_CLASS
//------------------------------------------------------------------------------
/**
 * Determine the default SPI configuration.
 */
#if defined(__STM32F1__)
// has multiple SPI ports
#define SD_HAS_CUSTOM_SPI 2
#elif defined(__AVR__)\
  || defined(__SAM3X8E__) || defined(__SAM3X8H__)\
  || (defined(__arm__) && defined(CORE_TEENSY))\
  || defined(ESP8266)
#define SD_HAS_CUSTOM_SPI 1
#else  // SD_HAS_CUSTOM_SPI
// Use standard SPI library.
#define SD_HAS_CUSTOM_SPI 0
#endif  // SD_HAS_CUSTOM_SPI
//------------------------------------------------------------------------------
/**
 * Check if API to select HW SPI port is needed.
 */
#if (USE_STANDARD_SPI_LIBRARY || SD_HAS_CUSTOM_SPI < 2)
#define IMPLEMENT_SPI_PORT_SELECTION 0
#else  // USE_STANDARD_SPI_LIBRARY
#define IMPLEMENT_SPI_PORT_SELECTION 1
#endif  // USE_STANDARD_SPI_LIBRARY
#endif  // SdFatConfig_h
