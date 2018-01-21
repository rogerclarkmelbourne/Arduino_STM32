/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f1/spi.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F1 SPI/I2S.
 */

#include <libmaple/spi.h>
#include <libmaple/gpio.h>
#include "spi_private.h"

/*
 * Devices
 */

static spi_dev spi1 = SPI_DEV(1);
static spi_dev spi2 = SPI_DEV(2);

spi_dev *SPI1 = &spi1;
spi_dev *SPI2 = &spi2;

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
static spi_dev spi3 = SPI_DEV(3);
spi_dev *SPI3 = &spi3;
#endif

/*
 * Routines
 */

void spi_config_gpios(spi_dev *dev,
                      uint8 as_master,
                      uint8 nss_pin,
                      uint8 sck_pin,
                      uint8 miso_pin,
                      uint8 mosi_pin) {
    if (as_master) {
        gpio_set_mode(sck_pin, GPIO_AF_OUTPUT_PP);
        gpio_set_mode(miso_pin, GPIO_AF_INPUT_PD);		
        gpio_set_mode(mosi_pin, GPIO_AF_OUTPUT_PP);
    } else {
        gpio_set_mode(nss_pin, GPIO_INPUT_FLOATING);
        gpio_set_mode(sck_pin, GPIO_INPUT_FLOATING);
        gpio_set_mode(miso_pin, GPIO_AF_OUTPUT_PP);
        gpio_set_mode(mosi_pin, GPIO_INPUT_FLOATING);
    }

	uint8_t af_mode = 6;
	if(dev->clk_id <= RCC_SPI2) { af_mode = 5; }
	if(!as_master) {
		gpio_set_af_mode(nss_pin, af_mode);
	}
	gpio_set_af_mode(sck_pin, af_mode);
	gpio_set_af_mode(miso_pin, af_mode);
	gpio_set_af_mode(mosi_pin, af_mode);
}

void spi_foreach(void (*fn)(spi_dev*)) {
    fn(SPI1);
    fn(SPI2);
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
    fn(SPI3);
#endif
}
