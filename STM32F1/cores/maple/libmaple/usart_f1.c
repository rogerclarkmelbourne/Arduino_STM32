/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f1/usart.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief STM32F1 USART.
 */

#include <libmaple/usart.h>
#include <libmaple/gpio.h>
#include "usart_private.h"

/*
 * Devices
 */

static ring_buffer usart1_rb;
static ring_buffer usart1_wb;
static usart_dev usart1 = {
    .regs     = USART1_BASE,
    .rb       = &usart1_rb,
    .wb       = &usart1_wb,
    .max_baud = 4500000UL,
    .clk_id   = RCC_USART1,
    .irq_num  = NVIC_USART1,
};
/** USART1 device */
usart_dev *USART1 = &usart1;

static ring_buffer usart2_rb;
static ring_buffer usart2_wb;
static usart_dev usart2 = {
    .regs     = USART2_BASE,
    .rb       = &usart2_rb,
    .wb       = &usart2_wb,
    .max_baud = 2250000UL,
    .clk_id   = RCC_USART2,
    .irq_num  = NVIC_USART2,
};
/** USART2 device */
usart_dev *USART2 = &usart2;

static ring_buffer usart3_rb;
static ring_buffer usart3_wb;
static usart_dev usart3 = {
    .regs     = USART3_BASE,
    .rb       = &usart3_rb,
    .wb       = &usart3_wb,
    .max_baud = 2250000UL,
    .clk_id   = RCC_USART3,
    .irq_num  = NVIC_USART3,
};
/** USART3 device */
usart_dev *USART3 = &usart3;

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
static ring_buffer uart4_rb;
static ring_buffer uart4_wb;
static usart_dev uart4 = {
    .regs     = UART4_BASE,
    .rb       = &uart4_rb,
    .wb       = &uart4_wb,
    .max_baud = 2250000UL,
    .clk_id   = RCC_UART4,
    .irq_num  = NVIC_UART4,
};
/** UART4 device */
usart_dev *UART4 = &uart4;

static ring_buffer uart5_rb;
static ring_buffer uart5_wb;
static usart_dev uart5 = {
    .regs     = UART5_BASE,
    .rb       = &uart5_rb,
    .wb       = &uart5_wb,
    .max_baud = 2250000UL,
    .clk_id   = RCC_UART5,
    .irq_num  = NVIC_UART5,
};
/** UART5 device */
usart_dev *UART5 = &uart5;
#endif

/*
 * Routines
 */

void usart_config_gpios_async(usart_dev *udev,
                              gpio_dev *rx_dev, uint8 rx,
                              gpio_dev *tx_dev, uint8 tx,
                              unsigned flags) {
    gpio_set_mode(rx_dev, rx, GPIO_INPUT_FLOATING);
    gpio_set_mode(tx_dev, tx, GPIO_AF_OUTPUT_PP);
/*
CR1 bit 12 Word length 0=8  1=9 
CR1 bit 11 wake (default value is 0) we can safely set this value to 0 (zero) each time 
CR1 bit 10 parity enable (1 = enabled)
CR1 bit 9  Parity selection 0 = Even  1 = Odd
CR2 bits 13 and 12  00 = 1 01 = 0.5 10 = 2 11 = 1.5
Not all USARTs support 1.5 or 0.5 bits so its best to avoid them.
When parity enabled the word length must be increased (CR1 bit 12 set).
Word length of 9 bit with parity is not supported.
	CR2  CR1
	0B00 0000
	0B10 0000
	0B00 1000
	0B10 1000	

	0B00 0010
	0B10 0010
	0B00 1010
	0B10 1010	

	0B00 0011
	0B10 0011
	0B00 1011
	0B10 1011		
	
#define SERIAL_8N1	0B 0000 0000
#define SERIAL_8N2	0B 0010 0000
#define SERIAL_9N1	0B 0000 1000
#define SERIAL_9N2	0B 0010 1000	

#define SERIAL_8E1	0B 0000 1010
#define SERIAL_8E2	0B 0010 1010
//#define SERIAL_9E1	0B 0000 1010
//#define SERIAL_9E2	0B 0010 1010

#define SERIAL_8O1	0B 0000 1011
#define SERIAL_8O2	0B 0010 1011
//#define SERIAL_9O1	0B 0000 1011
//#define SERIAL_9O2	0B 0010 1011
*/

	udev->regs->CR1  = (udev->regs->CR1 & 0B1110000111111111) | ((uint32_t)(flags&0x0F)<<9);
	udev->regs->CR2  = (udev->regs->CR2 & 0B1100111111111111) | ((uint32_t)(flags&0x30)<<8);
}

void usart_set_baud_rate(usart_dev *dev, uint32 clock_speed, uint32 baud) {
    uint32 integer_part;
    uint32 fractional_part;
    uint32 tmp;

    /* Figure out the clock speed, if the user doesn't give one. */
    if (clock_speed == 0) {
        clock_speed = _usart_clock_freq(dev);
    }
    ASSERT(clock_speed);

    /* Convert desired baud rate to baud rate register setting. */
    integer_part = (25 * clock_speed) / (4 * baud);
    tmp = (integer_part / 100) << 4;
    fractional_part = integer_part - (100 * (tmp >> 4));
    tmp |= (((fractional_part * 16) + 50) / 100) & ((uint8)0x0F);

    dev->regs->BRR = (uint16)tmp;
}

/**
 * @brief Call a function on each USART.
 * @param fn Function to call.
 */
void usart_foreach(void (*fn)(usart_dev*)) {
    fn(USART1);
    fn(USART2);
    fn(USART3);
#ifdef STM32_HIGH_DENSITY
    fn(UART4);
    fn(UART5);
#endif
}

/*
 * Interrupt handlers.
 */

__weak void __irq_usart1(void) {
    usart_irq(&usart1_rb, &usart1_wb, USART1_BASE);
}

__weak void __irq_usart2(void) {
    usart_irq(&usart2_rb, &usart2_wb, USART2_BASE);
}

__weak void __irq_usart3(void) {
    usart_irq(&usart3_rb, &usart3_wb, USART3_BASE);
}

#ifdef STM32_HIGH_DENSITY
__weak void __irq_uart4(void) {
    usart_irq(&uart4_rb, &uart4_wb, UART4_BASE);
}

__weak void __irq_uart5(void) {
    usart_irq(&uart5_rb, &uart5_wb, UART5_BASE);
}
#endif
