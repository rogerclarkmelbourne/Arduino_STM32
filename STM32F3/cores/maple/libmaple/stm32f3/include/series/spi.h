/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file libmaple/stm32f3/include/series/spi.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 * 				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 SPI/I2S series header.
 */

#ifndef _LIBMAPLE_STM32F3_SPI_H_
#define _LIBMAPLE_STM32F3_SPI_H_

#include <libmaple/libmaple_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Register map base pointers
 */

struct spi_reg_map;

#define SPI1_BASE                       ((struct spi_reg_map*)0x40013000)
#define SPI2_BASE                       ((struct spi_reg_map*)0x40003800)
#define SPI3_BASE                       ((struct spi_reg_map*)0x40003C00)

/*
 * F3 additional registry items
 */

/* Control register 1 */
/* CRC length bitfield in F3 overwrites the data length field in F1/F2 */
#define SPI_CR1_CRCL_BIT						11

#define SPI_CR1_CRCL								(1U << SPI_CR1_CRCL_BIT)

/* Control register 2 */
#define SPI_CR2_LDMA_TX_BIT					14
#define SPI_CR2_LDMA_RX_BIT					13
#define SPI_CR2_FRXTH_BIT						12
#define SPI_CR2_DS_SHIFT						8
#define SPI_CR2_FRF_BIT							4
#define SPI_CR2_NSSP_BIT						3

#define	SPI_CR2_LDMA_TX							(1U << SPI_CR2_LDMA_TX_BIT)
#define	SPI_CR2_LDMA_RX							(1U << SPI_CR2_LDMA_RX_BIT)
#define	SPI_CR2_FRXTH								(1U << SPI_CR2_FRXTH_BIT)
#define	SPI_CR2_DS									(0x7 << SPI_CR2_DS_SHIFT)
#define	SPI_CR2_FRF									(1U << SPI_CR2_FRF_BIT)
#define	SPI_CR2_NSSP								(1U << SPI_CR2_NSSP_BIT)

/* Status register */
#define SPI_SR_FTLVL_SHIFT					11
#define SPI_SR_FRLVL_SHIFT					9
#define SPI_SR_FRE_BIT							8

#define SPI_SR_FTLVL								(0x3 << SPI_SR_FTLVL_SHIFT)
#define SPI_SR_FRLVL								(0x3 << SPI_SR_FRLVL_SHIFT)
#define SPI_SR_FRE									(1U << SPI_SR_FRE_SHIFT)

/**
 * @brief TODO document me
 */
typedef enum spi_crc_size {
		SPI_CRC_SIZE_8_BIT		= (0x0 << SPI_CR1_CRCL_BIT),
		SPI_CRC_SIZE_16_BIT		= (0x1 << SPI_CR1_CRCL_BIT),
} spi_crc_size;

/**
 * @brief TODO document me
 */
typedef enum spi_frame_format {
		SPI_FRAME_FORMAT_MOTOROLA		= (0x0 << SPI_CR2_FRF_BIT),
		SPI_FRAME_FORMAT_TI					= (0x1 << SPI_CR2_FRF_BIT),
} spi_frame_format;

/**
 * @brief TODO document me
 */
typedef enum spi_ds {
    SPI_DATA_SIZE_DEFAULT	= (0x0 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_4_BIT		= (0x3 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_5_BIT		= (0x4 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_6_BIT		= (0x5 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_7_BIT		= (0x6 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_8_BIT		= (0x7 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_9_BIT		= (0x8 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_10_BIT	= (0x9 << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_11_BIT	= (0xA << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_12_BIT	= (0xB << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_13_BIT	= (0xC << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_14_BIT	= (0xD << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_15_BIT	= (0xE << SPI_CR2_DS_SHIFT),
    SPI_DATA_SIZE_16_BIT	= (0xF << SPI_CR2_DS_SHIFT),
} spi_ds;

/**
 * @brief TODO document me
 */
typedef enum spi_fifo_transmission_level {
		SPI_FIFO_TRANSMISSION_LEVEL_EMPTY		= (0x0 << SPI_SR_FTLVL_SHIFT),
		SPI_FIFO_TRANSMISSION_LEVEL_QUARTER	= (0x1 << SPI_SR_FTLVL_SHIFT),
		SPI_FIFO_TRANSMISSION_LEVEL_HALF		= (0x2 << SPI_SR_FTLVL_SHIFT),
		SPI_FIFO_TRANSMISSION_LEVEL_FULL		= (0x3 << SPI_SR_FTLVL_SHIFT),
} spi_fifo_transmission_level;

/**
 * @brief TODO document me
 */
typedef enum spi_fifo_reception_level {
		SPI_FIFO_RECEPTION_LEVEL_EMPTY		= (0x0 << SPI_SR_FRLVL_SHIFT),
		SPI_FIFO_RECEPTION_LEVEL_QUARTER	= (0x1 << SPI_SR_FRLVL_SHIFT),
		SPI_FIFO_RECEPTION_LEVEL_HALF			= (0x2 << SPI_SR_FRLVL_SHIFT),
		SPI_FIFO_RECEPTION_LEVEL_FULL			= (0x3 << SPI_SR_FRLVL_SHIFT),
} spi_fifo_reception_level;

/*
 * Device pointers
 */

struct spi_dev;

extern struct spi_dev *SPI1;
extern struct spi_dev *SPI2;
extern struct spi_dev *SPI3;

/*
 * Routines
 */

struct gpio_dev;
extern void spi_config_gpios(struct spi_dev*, uint8,
                             struct gpio_dev*, uint8,
                             struct gpio_dev*, uint8, uint8, uint8);

/**
 * @brief Set the data size of the given SPI device.
 *
 * @param dev SPI device
 * @param ds SPI data size
 * @see spi_ds
 * @see spi_reconfigure()
 */
extern void spi_data_size(struct spi_dev *, spi_ds);

#ifdef __cplusplus
}
#endif

#endif
