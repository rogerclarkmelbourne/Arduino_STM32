/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file wirish/include/wirish/HardwareSPI.h
 * @brief High-level SPI interface
 *
 * This is a "bare essentials" polling driver for now.
 */

/* TODO [0.1.0] Remove deprecated methods. */



#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <libmaple/libmaple_types.h>
#include <libmaple/spi.h>
#include <libmaple/dma.h>
#include <MapleCoOS116.h>

#include <boards.h>
#include <stdint.h>
#include <wirish.h>

// SPI_HAS_TRANSACTION means SPI has
//   - beginTransaction()
//   - endTransaction()
//   - usingInterrupt()
//   - SPISetting(clock, bitOrder, dataMode)
//#define SPI_HAS_TRANSACTION

#define SPI_CLOCK_DIV2   SPI_BAUD_PCLK_DIV_2
#define SPI_CLOCK_DIV4   SPI_BAUD_PCLK_DIV_4
#define SPI_CLOCK_DIV8   SPI_BAUD_PCLK_DIV_8
#define SPI_CLOCK_DIV16  SPI_BAUD_PCLK_DIV_16
#define SPI_CLOCK_DIV32  SPI_BAUD_PCLK_DIV_32
#define SPI_CLOCK_DIV64  SPI_BAUD_PCLK_DIV_64
#define SPI_CLOCK_DIV128 SPI_BAUD_PCLK_DIV_128
#define SPI_CLOCK_DIV256 SPI_BAUD_PCLK_DIV_256

/*
 * Roger Clark. 20150106
 * Commented out redundant AVR defined
 * 
#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

// define SPI_AVR_EIMSK for AVR boards with external interrupt pins
#if defined(EIMSK)
  #define SPI_AVR_EIMSK  EIMSK
#elif defined(GICR)
  #define SPI_AVR_EIMSK  GICR
#elif defined(GIMSK)
  #define SPI_AVR_EIMSK  GIMSK
#endif
*/

#ifndef STM32_LSBFIRST
#define STM32_LSBFIRST 0
#endif
#ifndef STM32_MSBFIRST
#define STM32_MSBFIRST 1
#endif

// PC13 or PA4
//#define BOARD_SPI_DEFAULT_SS PA4
#define BOARD_SPI_DEFAULT_SS PC13

#define SPI_MODE0 SPI_MODE_0
#define SPI_MODE1 SPI_MODE_1
#define SPI_MODE2 SPI_MODE_2
#define SPI_MODE3 SPI_MODE_3

class SPISettings {
public:
	SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		if (__builtin_constant_p(clock)) {
			init_AlwaysInline(clock, bitOrder, dataMode);
		} else {
			init_MightInline(clock, bitOrder, dataMode);
		}
	}
	SPISettings() { init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0); }
private:
	void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		init_AlwaysInline(clock, bitOrder, dataMode);
	}
	void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) __attribute__((__always_inline__)) {
		this->clock = clock;
		this->bitOrder = bitOrder;
		this->dataMode = dataMode;
	}
	uint32_t clock;
	BitOrder bitOrder;
	uint8_t dataMode;
	friend class SPIClass;
};


volatile static bool dma1_ch3_Active;
volatile static OS_EventID sem_dma1free;

/**
 * @brief Wirish SPI interface.
 *
 * This implementation uses software slave management, so the caller
 * is responsible for controlling the slave select line.
 */
class SPIClass {
public:



    /**
     * @param spiPortNumber Number of the SPI port to manage.
     */
    SPIClass(uint32 spiPortNumber);

    /*
     * Set up/tear down
     */



    /**
     * @brief Equivalent to begin(SPI_1_125MHZ, MSBFIRST, 0).
     */
    void begin(void);

    /**
     * @brief Turn on a SPI port and set its GPIO pin modes for use as a slave.
     *
     * SPI port is enabled in full duplex mode, with software slave management.
     *
     * @param bitOrder Either LSBFIRST (little-endian) or MSBFIRST(big-endian)
     * @param mode SPI mode to use
     */
    void beginSlave(uint32 bitOrder, uint32 mode);

    /**
     * @brief Equivalent to beginSlave(MSBFIRST, 0).
     */
    void beginSlave(void);

    /**
     * @brief Disables the SPI port, but leaves its GPIO pin modes unchanged.
     */
    void end(void);

	void beginTransaction(SPISettings settings) { beginTransaction(BOARD_SPI_DEFAULT_SS, settings); }
	void beginTransaction(uint8_t pin, SPISettings settings);
	void endTransaction(void);

	void setClockDivider(uint32_t clockDivider);
	void setBitOrder(BitOrder bitOrder);	
	void setDataMode(uint8_t dataMode);		
	
	// SPI Configuration methods
	void attachInterrupt(void);
	void detachInterrupt(void);
	
	/*	Victor Perez. Added to test changing datasize from 8 to 16 bit modes on the fly.
	*	Input parameter should be SPI_CR1_DFF set to 0 or 1 on a 32bit word.
	*	Requires an added function spi_data_size on  STM32F1 / cores / maple / libmaple / spi.c 
	*/
    void setDataSize(uint32 ds);
	
	
    /*
     * I/O
     */

    /**
     * @brief Return the next unread byte.
     *
     * If there is no unread byte waiting, this function will block
     * until one is received.
     */
    uint8 read(void);

    /**
     * @brief Read length bytes, storing them into buffer.
     * @param buffer Buffer to store received bytes into.
     * @param length Number of bytes to store in buffer.  This
     *               function will block until the desired number of
     *               bytes have been read.
     */
    void read(uint8 *buffer, uint32 length);

    /**
     * @brief Transmit a byte.
     * @param data Byte to transmit.
     */
    void write(uint8 data);

    /**
     * @brief Transmit a half word.
     * @param data to transmit.
     */
    void write(uint16 data);	
	
    /**
     * @brief Transmit multiple bytes.
     * @param buffer Bytes to transmit.
     * @param length Number of bytes in buffer to transmit.
     */
    void write(const uint8 *buffer, uint32 length);

    /**
     * @brief Transmit a byte, then return the next unread byte.
     *
     * This function transmits before receiving.
     *
     * @param data Byte to transmit.
     * @return Next unread byte.
     */
    uint8 transfer(uint8 data);
	
	/**
     * @brief Sets up a DMA Transfer for "length" bytes.
     *
     * This function transmits and receives to buffers.
     *
     * @param transmitBuf buffer Bytes to transmit. If passed as 0, it sends FF repeatedly for "length" bytes
     * @param receiveBuf buffer Bytes to save received data. 
     * @param length Number of bytes in buffer to transmit.
	 */
	uint8 dmaTransfer(uint8 *transmitBuf, uint8 *receiveBuf, uint16 length);

	/**
     * @brief Sets up a DMA Transmit for bytes.
     *
     * This function transmits and does not care about the RX fifo.
     *
     * @param transmitBuf buffer Bytes to transmit,
     * @param length Number of bytes in buffer to transmit.
	 * @param minc Set to use Memory Increment mode, clear to use Circular mode.
     */
	uint8 dmaSend(uint8 *transmitBuf, uint16 length, bool minc);
	
	/**
     * @brief Sets up a DMA Transmit for half words.
	 * SPI PERFIPHERAL MUST BE SET TO 16 BIT MODE BEFORE
     *
     * This function transmits and does not care about the RX fifo.
     *
     * @param data buffer half words to transmit,
     * @param length Number of bytes in buffer to transmit.
     * @param minc Set to use Memory Increment mode (default if blank), clear to use Circular mode.
     */
	uint8 dmaSend(uint16 *transmitBuf, uint16 length, bool minc);

    /*
     * Pin accessors
     */

    /**
     * @brief Return the number of the MISO (master in, slave out) pin
     */
    uint8 misoPin(void);

    /**
     * @brief Return the number of the MOSI (master out, slave in) pin
     */
    uint8 mosiPin(void);

    /**
     * @brief Return the number of the SCK (serial clock) pin
     */
    uint8 sckPin(void);

    /**
     * @brief Return the number of the NSS (slave select) pin
     */
    uint8 nssPin(void);

    /* Escape hatch */

    /**
     * @brief Get a pointer to the underlying libmaple spi_dev for
     *        this HardwareSPI instance.
     */
    spi_dev* c_dev(void) { return this->spi_d; }

    /* -- The following methods are deprecated --------------------------- */

    /**
     * @brief Deprecated.
     *
     * Use HardwareSPI::transfer() instead.
     *
     * @see HardwareSPI::transfer()
     */
    uint8 send(uint8 data);

    /**
     * @brief Deprecated.
     *
     * Use HardwareSPI::write() in combination with
     * HardwareSPI::read() (or HardwareSPI::transfer()) instead.
     *
     * @see HardwareSPI::write()
     * @see HardwareSPI::read()
     * @see HardwareSPI::transfer()
     */
    uint8 send(uint8 *data, uint32 length);

    /**
     * @brief Deprecated.
     *
     * Use HardwareSPI::read() instead.
     *
     * @see HardwareSPI::read()
     */
    uint8 recv(void);
	
	spi_dev *dev(){ return spi_d;}
	
	
	
private:

	static inline void DMA1_CH3_Event() {
		CoEnterISR (); // Enter ISR 
		isr_PostSem (sem_dma1free);
		dma1_ch3_Active = 0;
//		dma_disable(DMA1, DMA_CH3);
//		dma_disable(DMA1, DMA_CH2);
		
		// To Do. Need to wait for 
		CoExitISR (); // Exit ISR
	}
	spi_dev *spi_d;
	uint8_t _SSPin;
	uint32_t clockDivider;
	uint8_t dataMode;
	BitOrder bitOrder;
};


extern SPIClass SPI;//(1);// dummy params
#endif
