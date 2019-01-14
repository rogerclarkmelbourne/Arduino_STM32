/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f2/include/series/spi.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 SPI/I2S series header.
 */

#ifndef _LIBMAPLE_STM32F2_SPI_H_
#define _LIBMAPLE_STM32F2_SPI_H_

#include <libmaple/gpio.h>      /* for gpio_af */

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
 * Register bit definitions
 */

/* Control register 2 */

#define SPI_CR2_FRF_BIT                 4

#define SPI_CR2_FRF                     (1U << SPI_CR2_FRF_BIT)

/* Status register */

#define SPI_SR_TIFRFE_BIT               8

#define SPI_SR_TIFRFE                   (1U << SPI_SR_TIFRFE_BIT)

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

gpio_af spi_get_af(struct spi_dev *dev);

#ifdef __cplusplus
}
#endif

#endif
