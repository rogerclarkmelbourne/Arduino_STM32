/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 Perry Hung.
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

/* STM32F1 value line ISR weak declarations */

        .thumb

/* Default handler for all non-overridden interrupts and exceptions */
        .globl  __default_handler
        .type   __default_handler, %function

__default_handler:
        b .

        .weak  __msp_init
        .globl __msp_init
        .set   __msp_init, __default_handler
        .weak  __exc_reset
        .globl __exc_reset
        .set   __exc_reset, __default_handler
        .weak  __exc_nmi
        .globl __exc_nmi
        .set   __exc_nmi, __default_handler
        .weak  __exc_hardfault
        .globl __exc_hardfault
        .set   __exc_hardfault, __default_handler
        .weak  __exc_memmanage
        .globl __exc_memmanage
        .set   __exc_memmanage, __default_handler
        .weak  __exc_busfault
        .globl __exc_busfault
        .set   __exc_busfault, __default_handler
        .weak  __exc_usagefault
        .globl __exc_usagefault
        .set   __exc_usagefault, __default_handler
        .weak  __stm32reservedexception7
        .globl __stm32reservedexception7
        .set   __stm32reservedexception7, __default_handler
        .weak  __stm32reservedexception8
        .globl __stm32reservedexception8
        .set   __stm32reservedexception8, __default_handler
        .weak  __stm32reservedexception9
        .globl __stm32reservedexception9
        .set   __stm32reservedexception9, __default_handler
        .weak  __stm32reservedexception10
        .globl __stm32reservedexception10
        .set   __stm32reservedexception10, __default_handler
        .weak  __exc_svc
        .globl __exc_svc
        .set   __exc_svc, __default_handler
        .weak  __exc_debug_monitor
        .globl __exc_debug_monitor
        .set   __exc_debug_monitor, __default_handler
        .weak  __stm32reservedexception13
        .globl __stm32reservedexception13
        .set   __stm32reservedexception13, __default_handler
        .weak  __exc_pendsv
        .globl __exc_pendsv
        .set   __exc_pendsv, __default_handler
        .weak  __exc_systick
        .globl __exc_systick
        .set   __exc_systick, __default_handler

        .weak  __irq_wwdg
        .globl __irq_wwdg
        .set   __irq_wwdg, __default_handler
        .weak  __irq_pvd
        .globl __irq_pvd
        .set   __irq_pvd, __default_handler
        .weak  __irq_tamper
        .globl __irq_tamper
        .set   __irq_tamper, __default_handler
        .weak  __irq_rtc
        .globl __irq_rtc
        .set   __irq_rtc, __default_handler
        .weak  __irq_flash
        .globl __irq_flash
        .set   __irq_flash, __default_handler
        .weak  __irq_rcc
        .globl __irq_rcc
        .set   __irq_rcc, __default_handler
        .weak  __irq_exti0
        .globl __irq_exti0
        .set   __irq_exti0, __default_handler
        .weak  __irq_exti1
        .globl __irq_exti1
        .set   __irq_exti1, __default_handler
        .weak  __irq_exti2
        .globl __irq_exti2
        .set   __irq_exti2, __default_handler
        .weak  __irq_exti3
        .globl __irq_exti3
        .set   __irq_exti3, __default_handler
        .weak  __irq_exti4
        .globl __irq_exti4
        .set   __irq_exti4, __default_handler
        .weak  __irq_dma1_channel1
        .globl __irq_dma1_channel1
        .set   __irq_dma1_channel1, __default_handler
        .weak  __irq_dma1_channel2
        .globl __irq_dma1_channel2
        .set   __irq_dma1_channel2, __default_handler
        .weak  __irq_dma1_channel3
        .globl __irq_dma1_channel3
        .set   __irq_dma1_channel3, __default_handler
        .weak  __irq_dma1_channel4
        .globl __irq_dma1_channel4
        .set   __irq_dma1_channel4, __default_handler
        .weak  __irq_dma1_channel5
        .globl __irq_dma1_channel5
        .set   __irq_dma1_channel5, __default_handler
        .weak  __irq_dma1_channel6
        .globl __irq_dma1_channel6
        .set   __irq_dma1_channel6, __default_handler
        .weak  __irq_dma1_channel7
        .globl __irq_dma1_channel7
        .set   __irq_dma1_channel7, __default_handler
        .weak  __irq_adc1
        .globl __irq_adc1
        .set   __irq_adc1, __default_handler
        .weak  __stm32reservedexception14
        .globl __stm32reservedexception14
        .set   __stm32reservedexception14, __default_handler
        .weak  __stm32reservedexception15
        .globl __stm32reservedexception15
        .set   __stm32reservedexception15, __default_handler
        .weak  __stm32reservedexception16
        .globl __stm32reservedexception16
        .set   __stm32reservedexception16, __default_handler
        .weak  __stm32reservedexception17
        .globl __stm32reservedexception17
        .set   __stm32reservedexception17, __default_handler
        .weak  __irq_exti9_5
        .globl __irq_exti9_5
        .set   __irq_exti9_5, __default_handler
        .weak  __irq_tim1_brk
        .globl __irq_tim1_brk
        .set   __irq_tim1_brk, __default_handler
        .weak  __irq_tim1_up
        .globl __irq_tim1_up
        .set   __irq_tim1_up, __default_handler
        .weak  __irq_tim1_trg_com
        .globl __irq_tim1_trg_com
        .set   __irq_tim1_trg_com, __default_handler
        .weak  __irq_tim1_cc
        .globl __irq_tim1_cc
        .set   __irq_tim1_cc, __default_handler
        .weak  __irq_tim2
        .globl __irq_tim2
        .set   __irq_tim2, __default_handler
        .weak  __irq_tim3
        .globl __irq_tim3
        .set   __irq_tim3, __default_handler
        .weak  __irq_tim4
        .globl __irq_tim4
        .set   __irq_tim4, __default_handler
        .weak  __irq_i2c1_ev
        .globl __irq_i2c1_ev
        .set   __irq_i2c1_ev, __default_handler
        .weak  __irq_i2c1_er
        .globl __irq_i2c1_er
        .set   __irq_i2c1_er, __default_handler
        .weak  __irq_i2c2_ev
        .globl __irq_i2c2_ev
        .set   __irq_i2c2_ev, __default_handler
        .weak  __irq_i2c2_er
        .globl __irq_i2c2_er
        .set   __irq_i2c2_er, __default_handler
        .weak  __irq_spi1
        .globl __irq_spi1
        .set   __irq_spi1, __default_handler
        .weak  __irq_spi2
        .globl __irq_spi2
        .set   __irq_spi2, __default_handler
        .weak  __irq_usart1
        .globl __irq_usart1
        .set   __irq_usart1, __default_handler
        .weak  __irq_usart2
        .globl __irq_usart2
        .set   __irq_usart2, __default_handler
        .weak  __irq_usart3
        .globl __irq_usart3
        .set   __irq_usart3, __default_handler
        .weak  __irq_exti15_10
        .globl __irq_exti15_10
        .set   __irq_exti15_10, __default_handler
        .weak  __irq_rtcalarm
        .globl __irq_rtcalarm
        .set   __irq_rtcalarm, __default_handler
        .weak  __irq_cec
        .globl __irq_cec
        .set   __irq_cec, __default_handler
        .weak  __irq_tim12
        .globl __irq_tim12
        .set   __irq_tim12, __default_handler
        .weak  __irq_tim13
        .globl __irq_tim13
        .set   __irq_tim13, __default_handler
        .weak  __irq_tim14
        .globl __irq_tim14
        .set   __irq_tim14, __default_handler
        .weak  __stm32reservedexception18
        .globl __stm32reservedexception18
        .set   __stm32reservedexception18, __default_handler
        .weak  __stm32reservedexception19
        .globl __stm32reservedexception19
        .set   __stm32reservedexception19, __default_handler
        .weak  __irq_fsmc
        .globl __irq_fsmc
        .set   __irq_fsmc, __default_handler
        .weak  __stm32reservedexception20
        .globl __stm32reservedexception20
        .set   __stm32reservedexception20, __default_handler
        .weak  __irq_tim5
        .globl __irq_tim5
        .set   __irq_tim5, __default_handler
        .weak  __irq_spi3
        .globl __irq_spi3
        .set   __irq_spi3, __default_handler
        .weak  __irq_uart4
        .globl __irq_uart4
        .set   __irq_uart4, __default_handler
        .weak  __irq_uart5
        .globl __irq_uart5
        .set   __irq_uart5, __default_handler
        .weak  __irq_tim6
        .globl __irq_tim6
        .set   __irq_tim6, __default_handler
        .weak  __irq_tim7
        .globl __irq_tim7
        .set   __irq_tim7, __default_handler
        .weak  __irq_dma2_channel1
        .globl __irq_dma2_channel1
        .set   __irq_dma2_channel1, __default_handler
        .weak  __irq_dma2_channel2
        .globl __irq_dma2_channel2
        .set   __irq_dma2_channel2, __default_handler
        .weak  __irq_dma2_channel3
        .globl __irq_dma2_channel3
        .set   __irq_dma2_channel3, __default_handler
        .weak  __irq_dma2_channel4_5
        .globl __irq_dma2_channel4_5
        .set   __irq_dma2_channel4_5, __default_handler
        .weak  __irq_dma2_channel5 /* on remap only */
        .globl __irq_dma2_channel5
        .set   __irq_dma2_channel5, __default_handler
