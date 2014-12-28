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
 * @file usart.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief USART control routines
 */

#include "usart.h"
#define USART_TX_IRQ

/*
 * Devices
 */

static usart_dev usart1 = {
    .regs     = USART1_BASE,
    .max_baud = 4500000UL,
    .clk_id   = RCC_USART1,
    .irq_num  = NVIC_USART1
};
/** USART1 device */
usart_dev *USART1 = &usart1;

static usart_dev usart2 = {
    .regs     = USART2_BASE,
    .max_baud = 2250000UL,
    .clk_id   = RCC_USART2,
    .irq_num  = NVIC_USART2
};
/** USART2 device */
usart_dev *USART2 = &usart2;

static usart_dev usart3 = {
    .regs     = USART3_BASE,
    .max_baud = 2250000UL,
    .clk_id   = RCC_USART3,
    .irq_num  = NVIC_USART3
};
/** USART3 device */
usart_dev *USART3 = &usart3;

#ifdef STM32_HIGH_DENSITY
static usart_dev uart4 = {
    .regs     = UART4_BASE,
    .max_baud = 2250000UL,
    .clk_id   = RCC_UART4,
    .irq_num  = NVIC_UART4
};
/** UART4 device */
usart_dev *UART4 = &uart4;

static usart_dev uart5 = {
    .regs     = UART5_BASE,
    .max_baud = 2250000UL,
    .clk_id   = RCC_UART5,
    .irq_num  = NVIC_UART5
};
/** UART5 device */
usart_dev *UART5 = &uart5;
#endif

/**
 * @brief Initialize a serial port.
 * @param dev         Serial port to be initialized
 */
void usart_init(usart_dev *dev) {
    rb_init(&dev->rbRX, USART_RX_BUF_SIZE, dev->rx_buf);
    rb_init(&dev->rbTX, USART_TX_BUF_SIZE, dev->tx_buf);
    rcc_clk_enable(dev->clk_id);
    nvic_irq_enable(dev->irq_num);
}

/**
 * @brief Configure a serial port's baud rate.
 *
 * @param dev         Serial port to be configured
 * @param clock_speed Clock speed, in megahertz.
 * @param baud        Baud rate for transmit/receive.
 */
void usart_set_baud_rate(usart_dev *dev, uint32 baud) {
    uint32 integer_part;
    uint32 fractional_part;
    uint32 tmp;

	uint32 clock_speed = rcc_dev_clk_speed(dev->clk_id);

    /* See ST RM0008 for the details on configuring the baud rate register */
    integer_part = (25 * clock_speed) / (4 * baud);
    tmp = (integer_part / 100) << 4;
    fractional_part = integer_part - (100 * (tmp >> 4));
    tmp |= (((fractional_part * 16) + 50) / 100) & ((uint8)0x0F);

    dev->regs->BRR = (uint16)tmp;
}

/**
 * @brief Enable a serial port.
 *
 * USART is enabled in single buffer transmission mode, multibuffer
 * receiver mode, 8n1.
 *
 * Serial port must have a baud rate configured to work properly.
 *
 * @param dev Serial port to enable.
 * @see usart_set_baud_rate()
 */
void usart_enable(usart_dev *dev) {
    usart_reg_map *regs = dev->regs;
    regs->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    regs->CR1 |= USART_CR1_UE;
}


/**
 * @brief Enable serial port tx interrupt.
 *
 * @param dev Serial port.
 */
void usart_tx_irq_enable(usart_dev *dev) {
    bb_peri_set_bit(&dev->regs->CR1, USART_CR1_TXEIE_BIT, 1);
}

/**
 * @brief Disable serial port tx interrupt.
 *
 * @param dev Serial port.
 */
void usart_tx_irq_disable(usart_dev *dev) {
    bb_peri_set_bit(&dev->regs->CR1, USART_CR1_TXEIE_BIT, 0);
}

/**
 * @brief Turn off a serial port.
 * @param dev Serial port to be disabled
 */
void usart_disable(usart_dev *dev) {
    /* FIXME this misbehaves if you try to use PWM on TX afterwards */
    usart_reg_map *regs = dev->regs;

    // flush output buffer
    while(usart_data_pending(dev) > 0)
    	;

    /* TC bit must be high before disabling the USART */
    while((regs->CR1 & USART_CR1_UE) && !(regs->SR & USART_SR_TC))
        ;

    /* Disable UE */
    regs->CR1 &= ~USART_CR1_UE;

    /* Clean up buffer */
    usart_reset_rx(dev);
}

/**
 *  @brief Call a function on each USART.
 *  @param fn Function to call.
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

/**
 * @brief Nonblocking USART transmit
 * @param dev Serial port to transmit over
 * @param buf Buffer to transmit
 * @param len Maximum number of bytes to transmit
 * @return Number of bytes transmitted
 */
uint32 usart_tx(usart_dev *dev, const uint8 *buf, uint32 len) {
    uint32 txed = 0;
#ifdef USART_TX_IRQ
    while (txed < len && rb_safe_insert(&dev->rbTX, buf[txed])) {
    	usart_tx_irq_enable(dev);
    	txed++;
    }

#else
    usart_reg_map *regs = dev->regs;
    while ((regs->SR & USART_SR_TXE) && (txed < len)) {
        regs->DR = buf[txed++];
    }
#endif

    return txed;
}

/**
 * @brief Transmit an unsigned integer to the specified serial port in
 *        decimal format.
 *
 * This function blocks until the integer's digits have been
 * completely transmitted.
 *
 * @param dev Serial port to send on
 * @param val Number to print
 */
void usart_putudec(usart_dev *dev, uint32 val) {
    char digits[12];
    int i = 0;

    do {
        digits[i++] = val % 10 + '0';
        val /= 10;
    } while (val > 0);

    while (--i >= 0) {
        usart_putc(dev, digits[i]);
    }
}

/*
 * Interrupt handlers.
 */
static inline void usart_irq(usart_dev *dev) {
	volatile int sr = dev->regs->SR;
	if(sr & USART_SR_RXNE) {
#ifdef USART_SAFE_INSERT
		/* If the buffer is full and the user defines USART_SAFE_INSERT,
		 * ignore new bytes. */
		rb_safe_insert(&dev->rbRX, (uint8)dev->regs->DR);
#else
		/* By default, push bytes around in the ring buffer. */
		rb_push_insert(&dev->rbRX, (uint8)dev->regs->DR);
#endif

#ifdef USART_TX_IRQ
	} else if(sr & USART_SR_TXE) {
		if(rb_full_count(&dev->rbTX) > 0) {
			dev->regs->DR = rb_remove(&dev->rbTX);
		} else {
			usart_tx_irq_disable(dev); // disable tx irq
			// nops needed to deactivate the irq before irq handler is left
		    asm volatile("nop");
		    asm volatile("nop");
		}
#endif

	}
}

void __irq_usart1(void) {
    usart_irq(USART1);
}

void __irq_usart2(void) {
    usart_irq(USART2);
}

void __irq_usart3(void) {
    usart_irq(USART3);
}

#ifdef STM32_HIGH_DENSITY
void __irq_uart4(void) {
    usart_irq(UART4);
}

void __irq_uart5(void) {
    usart_irq(UART5);
}
#endif
