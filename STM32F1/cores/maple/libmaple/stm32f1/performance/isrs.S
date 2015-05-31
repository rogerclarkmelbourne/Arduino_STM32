/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 Perry Hung.
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

/* STM32F1 performance line ISR weak declarations */

#include <libmaple/stm32.h>

	.thumb

/* Default handler for all non-overridden interrupts and exceptions */
	.globl	__default_handler
	.type	__default_handler, %function

__default_handler:
	b .

	.weakref __exc_nmi, __default_handler
	.globl	__exc_nmi
	.weakref __exc_hardfault, __default_handler
	.globl	__exc_hardfault
	.weakref __exc_memmanage, __default_handler
	.globl	__exc_memmanage
	.weakref __exc_busfault, __default_handler
	.globl	__exc_busfault
	.weakref __exc_usagefault, __default_handler
	.globl	__exc_usagefault
	.weakref __stm32reservedexception7, __default_handler
	.globl	__stm32reservedexception7
	.weakref __stm32reservedexception8, __default_handler
	.globl	__stm32reservedexception8
	.weakref __stm32reservedexception9, __default_handler
	.globl	__stm32reservedexception9
	.weakref __stm32reservedexception10, __default_handler
	.globl	__stm32reservedexception10
	.weakref __exc_svc, __default_handler
	.globl	__exc_svc
	.weakref __exc_debug_monitor, __default_handler
	.globl	__exc_debug_monitor
	.weakref __stm32reservedexception13, __default_handler
	.globl	__stm32reservedexception13
	.weakref __exc_pendsv, __default_handler
	.globl	__exc_pendsv
	.weakref __exc_systick, __default_handler
	.globl	__exc_systick
	.weakref __irq_wwdg, __default_handler
	.globl	__irq_wwdg
	.weakref __irq_pvd, __default_handler
	.globl	__irq_pvd
	.weakref __irq_tamper, __default_handler
	.globl	__irq_tamper
	.weakref __irq_rtc, __default_handler
	.globl	__irq_rtc
	.weakref __irq_flash, __default_handler
	.globl	__irq_flash
	.weakref __irq_rcc, __default_handler
	.globl	__irq_rcc
	.weakref __irq_exti0, __default_handler
	.globl	__irq_exti0
	.weakref __irq_exti1, __default_handler
	.globl	__irq_exti1
	.weakref __irq_exti2, __default_handler
	.globl	__irq_exti2
	.weakref __irq_exti3, __default_handler
	.globl	__irq_exti3
	.weakref __irq_exti4, __default_handler
	.globl	__irq_exti4
	.weakref __irq_dma1_channel1, __default_handler
	.globl	__irq_dma1_channel1
	.weakref __irq_dma1_channel2, __default_handler
	.globl	__irq_dma1_channel2
	.weakref __irq_dma1_channel3, __default_handler
	.globl	__irq_dma1_channel3
	.weakref __irq_dma1_channel4, __default_handler
	.globl	__irq_dma1_channel4
	.weakref __irq_dma1_channel5, __default_handler
	.globl	__irq_dma1_channel5
	.weakref __irq_dma1_channel6, __default_handler
	.globl	__irq_dma1_channel6
	.weakref __irq_dma1_channel7, __default_handler
	.globl	__irq_dma1_channel7
	.weakref __irq_adc, __default_handler
	.globl	__irq_adc
	.weakref __irq_usb_hp_can_tx, __default_handler
	.globl	__irq_usb_hp_can_tx
	.weakref __irq_usb_lp_can_rx0, __default_handler
	.globl	__irq_usb_lp_can_rx0
	.weakref __irq_can_rx1, __default_handler
	.globl	__irq_can_rx1
	.weakref __irq_can_sce, __default_handler
	.globl	__irq_can_sce
	.weakref __irq_exti9_5, __default_handler
	.globl	__irq_exti9_5
	.weakref __irq_tim1_brk, __default_handler
	.globl	__irq_tim1_brk
	.weakref __irq_tim1_up, __default_handler
	.globl	__irq_tim1_up
	.weakref __irq_tim1_trg_com, __default_handler
	.globl	__irq_tim1_trg_com
	.weakref __irq_tim1_cc, __default_handler
	.globl	__irq_tim1_cc
	.weakref __irq_tim2, __default_handler
	.globl	__irq_tim2
	.weakref __irq_tim3, __default_handler
	.globl	__irq_tim3
	.weakref __irq_tim4, __default_handler
	.globl	__irq_tim4
	.weakref __irq_i2c1_ev, __default_handler
	.globl	__irq_i2c1_ev
	.weakref __irq_i2c1_er, __default_handler
	.globl	__irq_i2c1_er
	.weakref __irq_i2c2_ev, __default_handler
	.globl	__irq_i2c2_ev
	.weakref __irq_i2c2_er, __default_handler
	.globl	__irq_i2c2_er
	.weakref __irq_spi1, __default_handler
	.globl	__irq_spi1
	.weakref __irq_spi2, __default_handler
	.globl	__irq_spi2
	.weakref __irq_usart1, __default_handler
	.globl	__irq_usart1
	.weakref __irq_usart2, __default_handler
	.globl	__irq_usart2
	.weakref __irq_usart3, __default_handler
	.globl	__irq_usart3
	.weakref __irq_exti15_10, __default_handler
	.globl	__irq_exti15_10
	.weakref __irq_rtcalarm, __default_handler
	.globl	__irq_rtcalarm
	.weakref __irq_usbwakeup, __default_handler
	.globl	__irq_usbwakeup
#if defined (STM32_HIGH_DENSITY)
	.weakref __irq_tim8_brk, __default_handler
	.globl	__irq_tim8_brk
	.weakref __irq_tim8_up, __default_handler
	.globl	__irq_tim8_up
	.weakref __irq_tim8_trg_com, __default_handler
	.globl	__irq_tim8_trg_com
	.weakref __irq_tim8_cc, __default_handler
	.globl	__irq_tim8_cc
	.weakref __irq_adc3, __default_handler
	.globl	__irq_adc3
	.weakref __irq_fsmc, __default_handler
	.globl	__irq_fsmc
	.weakref __irq_sdio, __default_handler
	.globl	__irq_sdio
	.weakref __irq_tim5, __default_handler
	.globl	__irq_tim5
	.weakref __irq_spi3, __default_handler
	.globl	__irq_spi3
	.weakref __irq_uart4, __default_handler
	.globl	__irq_uart4
	.weakref __irq_uart5, __default_handler
	.globl	__irq_uart5
	.weakref __irq_tim6, __default_handler
	.globl	__irq_tim6
	.weakref __irq_tim7, __default_handler
	.globl	__irq_tim7
	.weakref __irq_dma2_channel1, __default_handler
	.globl	__irq_dma2_channel1
	.weakref __irq_dma2_channel2, __default_handler
	.globl	__irq_dma2_channel2
	.weakref __irq_dma2_channel3, __default_handler
	.globl	__irq_dma2_channel3
	.weakref __irq_dma2_channel4_5, __default_handler
	.globl	__irq_dma2_channel4_5
#endif /* STM32_HIGH_DENSITY */
