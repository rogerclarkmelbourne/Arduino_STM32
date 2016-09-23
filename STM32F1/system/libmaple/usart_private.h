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
 * @file libmaple/usart_private.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Private USART header.
 */

#ifndef _LIBMAPLE_USART_PRIVATE_H_
#define _LIBMAPLE_USART_PRIVATE_H_

#include <libmaple/ring_buffer.h>
#include <libmaple/usart.h>

static inline __always_inline void usart_irq(ring_buffer *rb, ring_buffer *wb, usart_reg_map *regs) {
    /* Handling RXNEIE and TXEIE interrupts. 
     * RXNE signifies availability of a byte in DR.
     *
     * See table 198 (sec 27.4, p809) in STM document RM0008 rev 15.
     * We enable RXNEIE. */
    if ((regs->CR1 & USART_CR1_RXNEIE) && (regs->SR & USART_SR_RXNE)) {
#ifdef USART_SAFE_INSERT
        /* If the buffer is full and the user defines USART_SAFE_INSERT,
         * ignore new bytes. */
        rb_safe_insert(rb, (uint8)regs->DR);
#else
        /* By default, push bytes around in the ring buffer. */
        rb_push_insert(rb, (uint8)regs->DR);
#endif
    }
    /* TXE signifies readiness to send a byte to DR. */
    if ((regs->CR1 & USART_CR1_TXEIE) && (regs->SR & USART_SR_TXE)) {
        if (!rb_is_empty(wb))
            regs->DR=rb_remove(wb);
        else
            regs->CR1 &= ~((uint32)USART_CR1_TXEIE); // disable TXEIE
    }
}

uint32 _usart_clock_freq(usart_dev *dev);

#endif
