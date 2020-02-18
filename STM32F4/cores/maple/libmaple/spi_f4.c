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

/*
 * Devices
 */
#define SPI_DEV(num)                \
    {                               \
        .regs    = SPI##num##_BASE, \
        .clk_id  = RCC_SPI##num,    \
        .irq_num = NVIC_SPI##num,   \
    }

spi_dev spi1 = SPI_DEV(1);
spi_dev spi2 = SPI_DEV(2);
spi_dev spi3 = SPI_DEV(3);
#if BOARD_NR_SPI>3
spi_dev spi4 = SPI_DEV(4);
#endif
#if BOARD_NR_SPI>4
spi_dev spi5 = SPI_DEV(5);
#endif


/*
 * Routines
 */

void spi_config_gpios(spi_dev *dev,
                      uint8 as_master,
                      const spi_pins_t *pins) {
    if (as_master) {
        gpio_set_mode(pins->sck, GPIO_AF_OUTPUT_PP);
        gpio_set_mode(pins->miso, GPIO_AF_INPUT_PD);		
        gpio_set_mode(pins->mosi, GPIO_AF_OUTPUT_PP);
    } else {
        gpio_set_mode(pins->nss, GPIO_INPUT_FLOATING);
        gpio_set_mode(pins->sck, GPIO_INPUT_FLOATING);
        gpio_set_mode(pins->miso, GPIO_AF_OUTPUT_PP);
        gpio_set_mode(pins->mosi, GPIO_INPUT_FLOATING);
    }

	gpio_af_mode af_mode = GPIO_AFMODE_SPI1_4;
	if ((dev->clk_id == RCC_SPI5) || (dev->clk_id == RCC_SPI3)) { af_mode = GPIO_AFMODE_SPI3_5; }
	if (!as_master) {
		gpio_set_af_mode(pins->nss, af_mode);
	}
	gpio_set_af_mode(pins->sck, af_mode);
	gpio_set_af_mode(pins->miso, af_mode);
	gpio_set_af_mode(pins->mosi, af_mode);
}

void spi_foreach(void (*fn)(spi_dev*)) {
    fn(SPI1);
    fn(SPI2);
    fn(SPI3);
}
