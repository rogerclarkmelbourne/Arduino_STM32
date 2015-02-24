/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung (from libmaple/util.c).
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

/*
 * STM32F1 implementations for libmaple/util.c hooks
 *
 * These need more love and attention before being made public API
 * (this includes being easily overridable by user code).
 */

#include <libmaple/nvic.h>
#include <libmaple/gpio.h>
#include <libmaple/stm32.h>
#include <libmaple/timer.h>
#include <libmaple/adc.h>
#include <libmaple/usart.h>

/* Failed ASSERT()s send out a message using this USART config. */
#ifndef ERROR_USART
#define ERROR_USART            USART2
#define ERROR_USART_BAUD       9600
#define ERROR_TX_PORT          GPIOA
#define ERROR_TX_PIN           2
#endif

/*
 * Disables all peripheral interrupts except USB (when available),
 * turns off commonly-used peripherals. Called by __error() with
 * global interrupts disabled.
 */
void __lm_error(void) {
    /* Turn off peripheral interrupts */
    nvic_irq_disable_all();

    /* Turn off timers */
    timer_disable_all();

    /* Turn off ADC */
    adc_disable_all();

    /* Turn off all USARTs */
    usart_disable_all();

#if STM32_HAVE_USB
    /* Turn the USB interrupt back on so the bootloader keeps on functioning */
    nvic_irq_enable(NVIC_USB_HP_CAN_TX);
    nvic_irq_enable(NVIC_USB_LP_CAN_RX0);
#endif
}

/*
 * Enable the error USART for writing.
 */
usart_dev* __lm_enable_error_usart() {
    gpio_set_mode(ERROR_TX_PORT, ERROR_TX_PIN, GPIO_AF_OUTPUT_PP);
    usart_init(ERROR_USART);
    usart_set_baud_rate(ERROR_USART, USART_USE_PCLK, ERROR_USART_BAUD);
    return ERROR_USART;
}
