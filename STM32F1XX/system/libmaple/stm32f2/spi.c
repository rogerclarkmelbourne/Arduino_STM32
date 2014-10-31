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
 * @file libmaple/stm32f2/spi.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 SPI/I2S.
 */

#include <libmaple/spi.h>
#include "spi_private.h"

/*
 * Devices
 */

static spi_dev spi1 = SPI_DEV(1);
static spi_dev spi2 = SPI_DEV(2);
static spi_dev spi3 = SPI_DEV(3);

spi_dev *SPI1 = &spi1;
spi_dev *SPI2 = &spi2;
spi_dev *SPI3 = &spi3;

/*
 * Routines
 */

void spi_config_gpios(spi_dev *dev,
                      uint8 as_master,
                      gpio_dev *nss_dev,
                      uint8 nss_bit,
                      gpio_dev *comm_dev,
                      uint8 sck_bit,
                      uint8 miso_bit,
                      uint8 mosi_bit) {
    gpio_af dev_af = spi_get_af(dev);
    gpio_set_mode(nss_dev, nss_bit, GPIO_MODE_AF);
    gpio_set_mode(comm_dev, sck_bit, GPIO_MODE_AF);
    gpio_set_mode(comm_dev, miso_bit, GPIO_MODE_AF);
    gpio_set_mode(comm_dev, mosi_bit, GPIO_MODE_AF);
    gpio_set_af(nss_dev, nss_bit, dev_af);
    gpio_set_af(comm_dev, sck_bit, dev_af);
    gpio_set_af(comm_dev, miso_bit, dev_af);
    gpio_set_af(comm_dev, mosi_bit, dev_af);
}

void spi_foreach(void (*fn)(spi_dev*)) {
    fn(SPI1);
    fn(SPI2);
    fn(SPI3);
}

gpio_af spi_get_af(spi_dev *dev) {
    switch (dev->clk_id) {
    case RCC_SPI1:              /* Fall through */
    case RCC_SPI2:
        return GPIO_AF_SPI_1_2;
    case RCC_SPI3:
        return GPIO_AF_SPI3;
    default:
        ASSERT(0);              /* Can't happen */
        return (gpio_af)-1;
    }
}
