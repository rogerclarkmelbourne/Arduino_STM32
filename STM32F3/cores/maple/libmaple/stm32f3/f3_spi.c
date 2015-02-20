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
 * @file libmaple/stm32f3/spi.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 SPI/I2S.
 */

#include <libmaple/spi.h>
#include <libmaple/gpio.h>
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
			gpio_set_modef(nss_dev, nss_bit, GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
			gpio_set_modef(comm_dev, sck_bit, GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
			gpio_set_modef(comm_dev, miso_bit, GPIO_MODE_AF, GPIO_MODEF_PUPD_NONE);
			gpio_set_modef(comm_dev, mosi_bit, GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);

			if ( (dev == SPI1) || (dev == SPI2) )
			{
				gpio_set_af(nss_dev, nss_bit, GPIO_AF_5);
				gpio_set_af(comm_dev, sck_bit, GPIO_AF_5);
				gpio_set_af(comm_dev, miso_bit, GPIO_AF_5);
				gpio_set_af(comm_dev, mosi_bit, GPIO_AF_5);
			}
			else if (dev == SPI3)
			{
				gpio_set_af(nss_dev, nss_bit, GPIO_AF_6);
				gpio_set_af(comm_dev, sck_bit, GPIO_AF_6);
				gpio_set_af(comm_dev, miso_bit, GPIO_AF_6);
				gpio_set_af(comm_dev, mosi_bit, GPIO_AF_6);
			}
}

void spi_foreach(void (*fn)(spi_dev*)) {
    fn(SPI1);
    fn(SPI2);
    fn(SPI3);
}

void spi_data_size(struct spi_dev *dev, spi_ds ds) {
    uint8 byte_frame = ds <= SPI_DATA_SIZE_8_BIT;
		uint32 cr2 = dev->regs->CR2;
		cr2 &= ~(SPI_CR2_DS | SPI_CR2_FRXTH);
		cr2 |= ds;
		if (byte_frame)
			cr2 |= SPI_CR2_FRXTH;
		dev->regs->CR2 = cr2;
}

/*
 * SPI auxiliary routines
 */
void spi_reconfigure(spi_dev *dev, uint32 cr1_config) {
    spi_irq_disable(dev, SPI_INTERRUPTS_ALL);
    spi_peripheral_disable(dev);

    uint8 byte_frame = (dev->regs->CR2 & SPI_CR2_DS) <= SPI_DATA_SIZE_8_BIT;
		uint32 cr2 = dev->regs->CR2;
		cr2 &= ~SPI_CR2_FRXTH;
		if (byte_frame)
			cr2 |= SPI_CR2_FRXTH;
		dev->regs->CR2 = cr2;

    dev->regs->CR1 = cr1_config;
    spi_peripheral_enable(dev);
}

uint16 spi_rx_reg(spi_dev *dev) {
    uint8 byte_frame = (dev->regs->CR2 & SPI_CR2_DS) <= SPI_DATA_SIZE_8_BIT;
    if (byte_frame) {
			__io uint8 *dr8 = (__io uint8 *)&dev->regs->DR; /* we need to access as byte */
			return (uint16)*dr8;
		} else {
			__io uint16 *dr16 = (__io uint16 *)&dev->regs->DR; /* we need to access as half-word */
			return (uint16)*dr16;
		}
}

uint32 spi_tx(spi_dev *dev, const void *buf, uint32 len) {
    uint32 txed = 0;
    uint8 byte_frame = (dev->regs->CR2 & SPI_CR2_DS) <= SPI_DATA_SIZE_8_BIT;
    while (spi_is_tx_empty(dev) && (txed < len)) {
        if (byte_frame) {
						__io uint8 *dr8 = (__io uint8 *)&dev->regs->DR; /* we need to access as byte */
						*dr8 = ((const uint8 *)buf)[txed++];
        } else {
						__io uint16 *dr16 = (__io uint16 *)&dev->regs->DR; /* we need to access as half-word */
						*dr16 = ((const uint16 *)buf)[txed++];
        }
    }
    return txed;
}
