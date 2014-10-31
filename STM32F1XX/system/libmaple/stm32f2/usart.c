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
 * @file libmaple/stm32f2/usart.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 USART.
 */

#include <libmaple/usart.h>
#include <libmaple/gpio.h>
#include "usart_private.h"

/*
 * Devices
 */

static ring_buffer usart1_rb;
static usart_dev usart1 = {
    .regs     = USART1_BASE,
    .rb       = &usart1_rb,
    .max_baud = 4500000UL,      /* TODO: are these correct? */
    .clk_id   = RCC_USART1,
    .irq_num  = NVIC_USART1,
};
/** USART1 device */
usart_dev *USART1 = &usart1;

static ring_buffer usart2_rb;
static usart_dev usart2 = {
    .regs     = USART2_BASE,
    .rb       = &usart2_rb,
    .max_baud = 2250000UL,      /* TODO: are these correct? */
    .clk_id   = RCC_USART2,
    .irq_num  = NVIC_USART2,
};
/** USART2 device */
usart_dev *USART2 = &usart2;

static ring_buffer usart3_rb;
static usart_dev usart3 = {
    .regs     = USART3_BASE,
    .rb       = &usart3_rb,
    .max_baud = 2250000UL,      /* TODO: are these correct? */
    .clk_id   = RCC_USART3,
    .irq_num  = NVIC_USART3,
};
/** USART3 device */
usart_dev *USART3 = &usart3;

static ring_buffer uart4_rb;
static usart_dev uart4 = {
    .regs     = UART4_BASE,
    .rb       = &uart4_rb,
    .max_baud = 2250000UL,      /* TODO: are these correct? */
    .clk_id   = RCC_UART4,
    .irq_num  = NVIC_UART4,
};
/** UART4 device */
usart_dev *UART4 = &uart4;

static ring_buffer uart5_rb;
static usart_dev uart5 = {
    .regs     = UART5_BASE,
    .rb       = &uart5_rb,
    .max_baud = 2250000UL,      /* TODO: are these correct? */
    .clk_id   = RCC_UART5,
    .irq_num  = NVIC_UART5,
};
/** UART5 device */
usart_dev *UART5 = &uart5;

static ring_buffer usart6_rb;
static usart_dev usart6 = {
    .regs = USART6_BASE,
    .rb = &usart6_rb,
    .max_baud = 4500000UL,      /* TODO: are these correct? */
    .clk_id = RCC_USART6,
    .irq_num = NVIC_USART6,
};
usart_dev *USART6 = &usart6;

/*
 * Routines
 */

void usart_config_gpios_async(usart_dev *udev,
                              gpio_dev *rx_dev, uint8 rx,
                              gpio_dev *tx_dev, uint8 tx,
                              unsigned flags) {
    gpio_af af = usart_get_af(udev);
    gpio_set_modef(rx_dev, rx, GPIO_MODE_AF, 0);
    gpio_set_modef(tx_dev, tx, GPIO_MODE_AF, 0);
    gpio_set_af(rx_dev, rx, af);
    gpio_set_af(tx_dev, tx, af);
}

void usart_set_baud_rate(usart_dev *dev, uint32 clock_speed, uint32 baud) {
    uint32 integer_part;
    uint32 fractional_part;
    uint32 tmp;
    uint32 over8 = !!(dev->regs->CR1 & USART_CR1_OVER8);

    ASSERT(!over8); /* OVER8 is currently unsupported. */

    /* Figure out the clock speed, if the user doesn't give one. */
    if (clock_speed == 0) {
        clock_speed = _usart_clock_freq(dev);
    }
    ASSERT(clock_speed);

    /* Convert desired baud rate to baud rate register setting. */
    integer_part = (25 * clock_speed) / (2 * (2 - over8) * baud);
    tmp = (integer_part / 100) << 4;
    fractional_part = integer_part - (100 * (tmp >> 4));
    tmp |= ((fractional_part * 16 + 50) / 100) & (uint8)0x0F;

    dev->regs->BRR = tmp;
}

/**
 * @brief Call a function on each USART.
 * @param fn Function to call.
 */
void usart_foreach(void (*fn)(usart_dev*)) {
    fn(USART1);
    fn(USART2);
    fn(USART3);
    fn(UART4);
    fn(UART5);
    fn(USART6);
}

/**
 * @brief Get GPIO alternate function mode for a USART.
 * @param dev USART whose gpio_af to get.
 * @return gpio_af corresponding to dev.
 */
gpio_af usart_get_af(usart_dev *dev) {
    switch (dev->clk_id) {
    case RCC_USART1:
    case RCC_USART2:
    case RCC_USART3:
        return GPIO_AF_USART_1_2_3;
    case RCC_UART4:
    case RCC_UART5:
    case RCC_USART6:
        return GPIO_AF_USART_4_5_6;
    default:
        ASSERT(0);              /* Can't happen */
        return (gpio_af)-1;
    }
}

/*
 * Interrupt handlers.
 */

void __irq_usart1(void) {
    usart_irq(&usart1_rb, USART1_BASE);
}

void __irq_usart2(void) {
    usart_irq(&usart2_rb, USART2_BASE);
}

void __irq_usart3(void) {
    usart_irq(&usart3_rb, USART3_BASE);
}

void __irq_uart4(void) {
    usart_irq(&uart4_rb, UART4_BASE);
}

void __irq_uart5(void) {
    usart_irq(&uart5_rb, UART5_BASE);
}

void __irq_usart6(void) {
    usart_irq(&usart6_rb, USART6_BASE);
}
