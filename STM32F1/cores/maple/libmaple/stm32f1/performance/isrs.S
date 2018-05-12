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
//	.weak	__exc_systick                                  // __exc_systick() defined in STM32F1/cores/maple/libmaple/systick.c
//	.globl	__exc_systick
//	.set	__exc_systick, __default_handler
	.weak	__irq_wwdg
	.globl	__irq_wwdg
	.set	__irq_wwdg, __default_handler
	.weak	__irq_pvd
	.globl	__irq_pvd
	.set	__irq_pvd, __default_handler
	.weak	__irq_tamper
	.globl	__irq_tamper
	.set	__irq_tamper, __default_handler
	.weak	__irq_rtc
	.globl	__irq_rtc
	.set	__irq_rtc, __default_handler
	.weak	__irq_flash
	.globl	__irq_flash
	.set	__irq_flash, __default_handler
	.weak	__irq_rcc
	.globl	__irq_rcc
	.set	__irq_rcc, __default_handler
//	.weak	__irq_exti0                                  // __irq_exti0() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti0
//	.set	__irq_exti0, __default_handler
//	.weak	__irq_exti1                                  // __irq_exti1() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti1
//	.set	__irq_exti1, __default_handler
//	.weak	__irq_exti2                                  // __irq_exti2() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti2
//	.set	__irq_exti2, __default_handler
//	.weak	__irq_exti3                                  // __irq_exti3() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti3
//	.set	__irq_exti3, __default_handler
//	.weak	__irq_exti4                                  // __irq_exti4() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti4
//	.set	__irq_exti4, __default_handler
//	.weak	__irq_dma1_channel1                          // __irq_dma1_channel1() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel1
//	.set	__irq_dma1_channel1, __default_handler
//	.weak	__irq_dma1_channel2                          // __irq_dma1_channel2() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel2
//	.set	__irq_dma1_channel2, __default_handler
//	.weak	__irq_dma1_channel3                          // __irq_dma1_channel3() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel3
//	.set	__irq_dma1_channel3, __default_handler
//	.weak	__irq_dma1_channel4                          // __irq_dma1_channel4() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel4
//	.set	__irq_dma1_channel4, __default_handler
//	.weak	__irq_dma1_channel5                          // __irq_dma1_channel5() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel5
//	.set	__irq_dma1_channel5, __default_handler
//	.weak	__irq_dma1_channel6                          // __irq_dma1_channel6() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel6
//	.set	__irq_dma1_channel6, __default_handler
//	.weak	__irq_dma1_channel7                          // __irq_dma1_channel7() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma1_channel7
//	.set	__irq_dma1_channel7, __default_handler
//	.weak	__irq_adc                                    // __irq_adc() defined in STM32F1/cores/maple/libmaple/adc_f1.c
//	.globl	__irq_adc
//	.set	__irq_adc, __default_handler
	.weak	__irq_usb_hp_can_tx
	.globl	__irq_usb_hp_can_tx
	.set	__irq_usb_hp_can_tx, __default_handler
//	.weak	__irq_usb_lp_can_rx0                         // __irq_usb_lp_can_rx0() defined in STM32F1/cores/maple/libmaple/usb/stm32f1/usb.c
//	.globl	__irq_usb_lp_can_rx0
//	.set	__irq_usb_lp_can_rx0, __default_handler
	.weak	__irq_can_rx1
	.globl	__irq_can_rx1
	.set	__irq_can_rx1, __default_handler
	.weak	__irq_can_sce
	.globl	__irq_can_sce
	.set	__irq_can_sce, __default_handler
//	.weak	__irq_exti9_5                                // __irq_exti9_5() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti9_5
//	.set	__irq_exti9_5, __default_handler
//	.weak	__irq_tim1_brk                               // __irq_tim1_brk() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim1_brk
//	.set	__irq_tim1_brk, __default_handler
//	.weak	__irq_tim1_up                                // __irq_tim1_up() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim1_up
//	.set	__irq_tim1_up, __default_handler
//	.weak	__irq_tim1_trg_com                           // __irq_tim1_trg_com() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim1_trg_com
//	.set	__irq_tim1_trg_com, __default_handler
//	.weak	__irq_tim1_cc                                // __irq_tim1_cc() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim1_cc
//	.set	__irq_tim1_cc, __default_handler
//
//	.weakref __irq_tim2, __default_handler               // __irq_tim2() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl  __irq_tim2
//	.weakref __irq_tim3, __default_handler               // __irq_tim3() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl  __irq_tim3
//	.weakref __irq_tim4, __default_handler               // __irq_tim4() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl  __irq_tim4	
//	
//	.weak	__irq_i2c1_ev                                // __irq_i2c1_ev() defined in STM32F1/cores/maple/libmaple/i2c_f1.c
//	.globl	__irq_i2c1_ev
//	.set	__irq_i2c1_ev, __default_handler
//	.weak	__irq_i2c1_er                                // __irq_i2c1_er() defined in STM32F1/cores/maple/libmaple/i2c_f1.c
//	.globl	__irq_i2c1_er
//	.set	__irq_i2c1_er, __default_handler
//	.weak	__irq_i2c2_ev                                // __irq_i2c2_ev() defined in STM32F1/cores/maple/libmaple/i2c_f1.c
//	.globl	__irq_i2c2_ev
//	.set	__irq_i2c2_ev, __default_handler
//	.weak	__irq_i2c2_er                                // __irq_i2c2_er() defined in STM32F1/cores/maple/libmaple/i2c_f1.c
//	.globl	__irq_i2c2_er
//	.set	__irq_i2c2_er, __default_handler
	.weak	__irq_spi1
	.globl	__irq_spi1
	.set	__irq_spi1, __default_handler
	.weak	__irq_spi2
	.globl	__irq_spi2
	.set	__irq_spi2, __default_handler
//	.weak	__irq_usart1                                // __irq_usart1() defined in STM32F1/cores/maple/libmaple/usart_f1.c
//	.globl	__irq_usart1
//	.set	__irq_usart1, __default_handler
//	.weak	__irq_usart2                                // __irq_usart2() defined in STM32F1/cores/maple/libmaple/usart_f1.c
//	.globl	__irq_usart2
//	.set	__irq_usart2, __default_handler
//	.weak	__irq_usart3                                // __irq_usart3() defined in STM32F1/cores/maple/libmaple/usart_f1.c
//	.globl	__irq_usart3
//	.set	__irq_usart3, __default_handler
//	.weak	__irq_exti15_10                             // __irq_exti15_10() defined in STM32F1/cores/maple/libmaple/exti.c
//	.globl	__irq_exti15_10
//	.set	__irq_exti15_10, __default_handler
	.weak	__irq_rtcalarm
	.globl	__irq_rtcalarm
	.set	__irq_rtcalarm, __default_handler
	.weak	__irq_usbwakeup
	.globl	__irq_usbwakeup
	.set	__irq_usbwakeup, __default_handler
#if defined (STM32_HIGH_DENSITY)
//	.weak	__irq_tim8_brk                              // __irq_tim8_brk() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim8_brk
//	.set	__irq_tim8_brk, __default_handler
//	.weak	__irq_tim8_up                               // __irq_tim8_up() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim8_up
//	.set	__irq_tim8_up, __default_handler
//	.weak	__irq_tim8_trg_com                          // __irq_tim8_trg_com() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim8_trg_com
//	.set	__irq_tim8_trg_com, __default_handler
//	.weak	__irq_tim8_cc                               // __irq_tim8_cc() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim8_cc
//	.set	__irq_tim8_cc, __default_handler
//	.weak	__irq_adc3                                  // __irq_adc3() defined in STM32F1/cores/maple/libmaple/adc_f1.c
//	.globl	__irq_adc3
//	.set	__irq_adc3, __default_handler
	.weak	__irq_fsmc
	.globl	__irq_fsmc
	.set	__irq_fsmc, __default_handler
	.weak	__irq_sdio
	.globl	__irq_sdio
	.set	__irq_sdio, __default_handler
//	.weak	__irq_tim5                                  // __irq_tim5() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim5
//	.set	__irq_tim5, __default_handler
	.weak	__irq_spi3
	.globl	__irq_spi3
	.set	__irq_spi3, __default_handler
//	.weak	__irq_uart4                                 // __irq_uart4() defined in STM32F1/cores/maple/libmaple/usart_f1.c
//	.globl	__irq_uart4
//	.set	__irq_uart4, __default_handler
//	.weak	__irq_uart5                                 // __irq_uart5() defined in STM32F1/cores/maple/libmaple/usart_f1.c
//	.globl	__irq_uart5
//	.set	__irq_uart5, __default_handler
//	.weak	__irq_tim6                                  // __irq_tim6() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim6
//	.set	__irq_tim6, __default_handler
//	.weak	__irq_tim7                                  // __irq_tim7() defined in STM32F1/cores/maple/libmaple/timer.c
//	.globl	__irq_tim7
//	.set	__irq_tim7, __default_handler
//	.weak	__irq_dma2_channel1                         // __irq_dma2_channel1() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma2_channel1
//	.set	__irq_dma2_channel1, __default_handler
//	.weak	__irq_dma2_channel2                         // __irq_dma2_channel2() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma2_channel2
//	.set	__irq_dma2_channel2, __default_handler
//	.weak	__irq_dma2_channel3                         // __irq_dma2_channel3() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma2_channel3
//	.set	__irq_dma2_channel3, __default_handler
//	.weak	__irq_dma2_channel4_5                       // __irq_dma2_channel4_5() defined in STM32F1/cores/maple/libmaple/dma_f1.c
//	.globl	__irq_dma2_channel4_5
//	.set	__irq_dma2_channel4_5, __default_handler
#endif /* STM32_HIGH_DENSITY */