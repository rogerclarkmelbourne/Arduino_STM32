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

/* STM32F3 performance line ISR weak declarations */

#include <libmaple/stm32.h>

	.thumb

/* Default handler for all non-overridden interrupts and exceptions */
	.globl	__default_handler
	.type	__default_handler, %function

__default_handler:
	b .

	.weak	__exc_nmi
	.globl	__exc_nmi
	.set	__exc_nmi, __default_handler
	.weak	__exc_hardfault
	.globl	__exc_hardfault
	.set	__exc_hardfault, __default_handler
	.weak	__exc_memmanage
	.globl	__exc_memmanage
	.set	__exc_memmanage, __default_handler
	.weak	__exc_busfault
	.globl	__exc_busfault
	.set	__exc_busfault, __default_handler
	.weak	__exc_usagefault
	.globl	__exc_usagefault
	.set	__exc_usagefault, __default_handler
	.weak	__stm32reservedexception7
	.globl	__stm32reservedexception7
	.set	__stm32reservedexception7, __default_handler
	.weak	__stm32reservedexception8
	.globl	__stm32reservedexception8
	.set	__stm32reservedexception8, __default_handler
	.weak	__stm32reservedexception9
	.globl	__stm32reservedexception9
	.set	__stm32reservedexception9, __default_handler
	.weak	__stm32reservedexception10
	.globl	__stm32reservedexception10
	.set	__stm32reservedexception10, __default_handler
	.weak	__exc_svc
	.globl	__exc_svc
	.set	__exc_svc, __default_handler
	.weak	__exc_debug_monitor
	.globl	__exc_debug_monitor
	.set	__exc_debug_monitor, __default_handler
	.weak	__stm32reservedexception13
	.globl	__stm32reservedexception13
	.set	__stm32reservedexception13, __default_handler
	.weak	__exc_pendsv
	.globl	__exc_pendsv
	.set	__exc_pendsv, __default_handler
	.weak	__exc_systick
	.globl	__exc_systick
	.set	__exc_systick, __default_handler
	.weak	__irq_wwdg
	.globl	__irq_wwdg
	.set	__irq_wwdg, __default_handler
	.weak	__irq_pvd
	.globl	__irq_pvd
	.set	__irq_pvd, __default_handler
	.weak	__irq_tamp_stamp
	.globl	__irq_tamp_stamp
	.set	__irq_tamp_stamp, __default_handler
	.weak	__irq_rtc_wkup
	.globl	__irq_rtc_wkup
	.set	__irq_rtc_wkup, __default_handler
	.weak	__irq_flash
	.globl	__irq_flash
	.set	__irq_flash, __default_handler
	.weak	__irq_rcc
	.globl	__irq_rcc
	.set	__irq_rcc, __default_handler
	.weak	__irq_exti0
	.globl	__irq_exti0
	.set	__irq_exti0, __default_handler
	.weak	__irq_exti1
	.globl	__irq_exti1
	.set	__irq_exti1, __default_handler
	.weak	__irq_exti2
	.globl	__irq_exti2
	.set	__irq_exti2, __default_handler
	.weak	__irq_exti3
	.globl	__irq_exti3
	.set	__irq_exti3, __default_handler
	.weak	__irq_exti4
	.globl	__irq_exti4
	.set	__irq_exti4, __default_handler
	.weak	__irq_dma1_ch1
	.globl	__irq_dma1_ch1
	.set	__irq_dma1_ch1, __default_handler
	.weak	__irq_dma1_ch2
	.globl	__irq_dma1_ch2
	.set	__irq_dma1_ch2, __default_handler
	.weak	__irq_dma1_ch3
	.globl	__irq_dma1_ch3
	.set	__irq_dma1_ch3, __default_handler
	.weak	__irq_dma1_ch4
	.globl	__irq_dma1_ch4
	.set	__irq_dma1_ch4, __default_handler
	.weak	__irq_dma1_ch5
	.globl	__irq_dma1_ch5
	.set	__irq_dma1_ch5, __default_handler
	.weak	__irq_dma1_ch6
	.globl	__irq_dma1_ch6
	.set	__irq_dma1_ch6, __default_handler
	.weak	__irq_dma1_ch7
	.globl	__irq_dma1_ch7
	.set	__irq_dma1_ch7, __default_handler
	.weak	__irq_adc1_2
	.globl	__irq_adc1_2
	.set	__irq_adc1_2, __default_handler
	.weak	__irq_usb_hp_can_tx
	.globl	__irq_usb_hp_can_tx
	.set	__irq_usb_hp_can_tx, __default_handler
	.weak	__irq_usb_lp_can_rx0
	.globl	__irq_usb_lp_can_rx0
	.set	__irq_usb_lp_can_rx0, __default_handler
	.weak	__irq_can_rx1
	.globl	__irq_can_rx1
	.set	__irq_can_rx1, __default_handler
	.weak	__irq_can_sce
	.globl	__irq_can_sce
	.set	__irq_can_sce, __default_handler
	.weak	__irq_exti9_5
	.globl	__irq_exti9_5
	.set	__irq_exti9_5, __default_handler
	.weak	__irq_tim1_brk_tim15
	.globl	__irq_tim1_brk_tim15
	.set	__irq_tim1_brk_tim15, __default_handler
	.weak	__irq_tim1_up_tim16
	.globl	__irq_tim1_up_tim16
	.set	__irq_tim1_up_tim16, __default_handler
	.weak	__irq_tim1_trg_com_tim17
	.globl	__irq_tim1_trg_com_tim17
	.set	__irq_tim1_trg_com_tim17, __default_handler
	.weak	__irq_tim1_cc
	.globl	__irq_tim1_cc
	.set	__irq_tim1_cc, __default_handler
	.weak	__irq_tim2
	.globl	__irq_tim2
	.set	__irq_tim2, __default_handler
	.weak	__irq_tim3
	.globl	__irq_tim3
	.set	__irq_tim3, __default_handler
	.weak	__irq_tim4
	.globl	__irq_tim4
	.set	__irq_tim4, __default_handler
	.weak	__irq_i2c1_ev
	.globl	__irq_i2c1_ev
	.set	__irq_i2c1_ev, __default_handler
	.weak	__irq_i2c1_er
	.globl	__irq_i2c1_er
	.set	__irq_i2c1_er, __default_handler
	.weak	__irq_i2c2_ev
	.globl	__irq_i2c2_ev
	.set	__irq_i2c2_ev, __default_handler
	.weak	__irq_i2c2_er
	.globl	__irq_i2c2_er
	.set	__irq_i2c2_er, __default_handler
	.weak	__irq_spi1
	.globl	__irq_spi1
	.set	__irq_spi1, __default_handler
	.weak	__irq_spi2
	.globl	__irq_spi2
	.set	__irq_spi2, __default_handler
	.weak	__irq_usart1
	.globl	__irq_usart1
	.set	__irq_usart1, __default_handler
	.weak	__irq_usart2
	.globl	__irq_usart2
	.set	__irq_usart2, __default_handler
	.weak	__irq_usart3
	.globl	__irq_usart3
	.set	__irq_usart3, __default_handler
	.weak	__irq_exti15_10
	.globl	__irq_exti15_10
	.set	__irq_exti15_10, __default_handler
	.weak	__irq_rtc_alarm
	.globl	__irq_rtc_alarm
	.set	__irq_rtc_alarm, __default_handler
	.weak	__irq_usb_wkup
	.globl	__irq_usb_wkup
	.set	__irq_usb_wkup, __default_handler

	.weak	__irq_tim8_brk
	.globl	__irq_tim8_brk
	.set	__irq_tim8_brk, __default_handler
	.weak	__irq_tim8_up
	.globl	__irq_tim8_up
	.set	__irq_tim8_up, __default_handler
	.weak	__irq_tim8_trg_com
	.globl	__irq_tim8_trg_com
	.set	__irq_tim8_trg_com, __default_handler
	.weak	__irq_tim8_cc
	.globl	__irq_tim8_cc
	.set	__irq_tim8_cc, __default_handler
	.weak	__irq_adc3
	.globl	__irq_adc3
	.set	__irq_adc3, __default_handler
	.weak	__stm32reservedexception48
	.globl	__stm32reservedexception48
	.set	__stm32reservedexception48, __default_handler
	.weak	__stm32reservedexception49
	.globl	__stm32reservedexception49
	.set	__stm32reservedexception49, __default_handler
	.weak	__stm32reservedexception50
	.globl	__stm32reservedexception50
	.set	__stm32reservedexception50, __default_handler
	.weak	__irq_spi3
	.globl	__irq_spi3
	.set	__irq_spi3, __default_handler
	.weak	__irq_uart4
	.globl	__irq_uart4
	.set	__irq_uart4, __default_handler
	.weak	__irq_uart5
	.globl	__irq_uart5
	.set	__irq_uart5, __default_handler
	.weak	__irq_tim6_dacunder
	.globl	__irq_tim6_dacunder
	.set	__irq_tim6_dac, __default_handler
	.weak	__irq_tim7
	.globl	__irq_tim7
	.set	__irq_tim7, __default_handler
	.weak	__irq_dma2_ch1
	.globl	__irq_dma2_ch1
	.set	__irq_dma2_ch1, __default_handler
	.weak	__irq_dma2_ch2
	.globl	__irq_dma2_ch2
	.set	__irq_dma2_ch2, __default_handler
	.weak	__irq_dma2_ch3
	.globl	__irq_dma2_ch3
	.set	__irq_dma2_ch3, __default_handler
	.weak	__irq_dma2_ch4
	.globl	__irq_dma2_ch4
	.set	__irq_dma2_ch4, __default_handler
	.weak	__irq_dma2_ch5
	.globl	__irq_dma2_ch5
	.set	__irq_dma2_ch5, __default_handler
	.weak	__irq_adc4
	.globl	__irq_adc4
	.set	__irq_adc4, __default_handler
	.weak	__stm32reservedexception62
	.globl	__stm32reservedexception62
	.set	__stm32reservedexception62, __default_handler
	.weak	__stm32reservedexception63
	.globl	__stm32reservedexception63
	.set	__stm32reservedexception63, __default_handler
	.weak	__irq_comp123
	.globl	__irq_comp123
	.set	__irq_comp123, __default_handler
	.weak	__irq_comp456
	.globl	__irq_comp456
	.set	__irq_comp456, __default_handler
	.weak	__irq_comp7
	.globl	__irq_comp7
	.set	__irq_comp7, __default_handler
	.weak	__stm32reservedexception67
	.globl	__stm32reservedexception67
	.set	__stm32reservedexception67, __default_handler
	.weak	__stm32reservedexception68
	.globl	__stm32reservedexception68
	.set	__stm32reservedexception68, __default_handler
	.weak	__stm32reservedexception69
	.globl	__stm32reservedexception69
	.set	__stm32reservedexception69, __default_handler
	.weak	__stm32reservedexception70
	.globl	__stm32reservedexception70
	.set	__stm32reservedexception70, __default_handler
	.weak	__stm32reservedexception71
	.globl	__stm32reservedexception71
	.set	__stm32reservedexception71, __default_handler
	.weak	__stm32reservedexception72
	.globl	__stm32reservedexception72
	.set	__stm32reservedexception72, __default_handler
	.weak	__stm32reservedexception73
	.globl	__stm32reservedexception73
	.set	__stm32reservedexception73, __default_handler
	.weak	__irq_usb_hp
	.globl	__irq_usb_hp
	.set	__irq_usb_hp, __default_handler
	.weak	__irq_usb_lp
	.globl	__irq_usb_lp
	.set	__irq_usb_lp, __default_handler
	.weak	__irq_usb_wkup2
	.globl	__irq_usb_wkup2
	.set	__irq_usb_wkup2, __default_handler
	.weak	__stm32reservedexception77
	.globl	__stm32reservedexception77
	.set	__stm32reservedexception77, __default_handler
	.weak	__stm32reservedexception78
	.globl	__stm32reservedexception78
	.set	__stm32reservedexception78, __default_handler
	.weak	__stm32reservedexception79
	.globl	__stm32reservedexception79
	.set	__stm32reservedexception79, __default_handler
	.weak	__stm32reservedexception80
	.globl	__stm32reservedexception80
	.set	__stm32reservedexception80, __default_handler
	.weak	__irq_fpu
	.globl	__irq_fpu
	.set	__irq_fpu, __default_handler
	.weak	__reserved
	.globl	__reserved
	.set	__reserved, __default_handler
