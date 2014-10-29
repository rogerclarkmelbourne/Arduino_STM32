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

/* STM32F1 performance line vector table */

#include <libmaple/stm32.h>

	.section	".stm32.interrupt_vector"

	.globl	__stm32_vector_table
	.type	__stm32_vector_table, %object

__stm32_vector_table:
/* CM3 core interrupts */
	.long	__msp_init
	.long	__exc_reset
	.long	__exc_nmi
	.long	__exc_hardfault
	.long	__exc_memmanage
	.long	__exc_busfault
	.long	__exc_usagefault
	.long	__stm32reservedexception7
	.long	__stm32reservedexception8
	.long	__stm32reservedexception9
	.long	__stm32reservedexception10
	.long	__exc_svc
	.long	__exc_debug_monitor
	.long	__stm32reservedexception13
	.long	__exc_pendsv
	.long	__exc_systick
/* Peripheral interrupts */
	.long	__irq_wwdg
	.long	__irq_pvd
	.long	__irq_tamper
	.long	__irq_rtc
	.long	__irq_flash
	.long	__irq_rcc
	.long	__irq_exti0
	.long	__irq_exti1
	.long	__irq_exti2
	.long	__irq_exti3
	.long	__irq_exti4
	.long	__irq_dma1_channel1
	.long	__irq_dma1_channel2
	.long	__irq_dma1_channel3
	.long	__irq_dma1_channel4
	.long	__irq_dma1_channel5
	.long	__irq_dma1_channel6
	.long	__irq_dma1_channel7
	.long	__irq_adc
	.long	__irq_usb_hp_can_tx
	.long	__irq_usb_lp_can_rx0
	.long	__irq_can_rx1
	.long	__irq_can_sce
	.long	__irq_exti9_5
	.long	__irq_tim1_brk
	.long	__irq_tim1_up
	.long	__irq_tim1_trg_com
	.long	__irq_tim1_cc
	.long	__irq_tim2
	.long	__irq_tim3
	.long	__irq_tim4
	.long	__irq_i2c1_ev
	.long	__irq_i2c1_er
	.long	__irq_i2c2_ev
	.long	__irq_i2c2_er
	.long	__irq_spi1
	.long	__irq_spi2
	.long	__irq_usart1
	.long	__irq_usart2
	.long	__irq_usart3
	.long	__irq_exti15_10
	.long	__irq_rtcalarm
	.long	__irq_usbwakeup
#if defined (STM32_HIGH_DENSITY)
	.long	__irq_tim8_brk
	.long	__irq_tim8_up
	.long	__irq_tim8_trg_com
	.long	__irq_tim8_cc
	.long	__irq_adc3
	.long	__irq_fsmc
	.long	__irq_sdio
	.long	__irq_tim5
	.long	__irq_spi3
	.long	__irq_uart4
	.long	__irq_uart5
	.long	__irq_tim6
	.long	__irq_tim7
	.long	__irq_dma2_channel1
	.long	__irq_dma2_channel2
	.long	__irq_dma2_channel3
	.long	__irq_dma2_channel4_5
#endif /* STM32_HIGH_DENSITY */

	.size	__stm32_vector_table, . - __stm32_vector_table
