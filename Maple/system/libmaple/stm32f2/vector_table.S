/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
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

/* STM32F2 vector table */

        .section ".stm32.interrupt_vector"

        .globl __stm32_vector_table
        .type __stm32_vector_table, %object

__stm32_vector_table:
/* CM3 core interrupts */
        .long __msp_init
        .long __exc_reset
        .long __exc_nmi
        .long __exc_hardfault
        .long __exc_memmanage
        .long __exc_busfault
        .long __exc_usagefault
        .long __stm32reservedexception7
        .long __stm32reservedexception8
        .long __stm32reservedexception9
        .long __stm32reservedexception10
        .long __exc_svc
        .long __exc_debug_monitor
        .long __stm32reservedexception13
        .long __exc_pendsv
        .long __exc_systick
/* Peripheral interrupts */
        .long __irq_wwdg
        .long __irq_pvd
        .long __irq_tamp_stamp
        .long __irq_rtc_wkup
        .long __irq_flash
        .long __irq_rcc
        .long __irq_exti0
        .long __irq_exti1
        .long __irq_exti2
        .long __irq_exti3
        .long __irq_exti4
        .long __irq_dma1_stream0
        .long __irq_dma1_stream1
        .long __irq_dma1_stream2
        .long __irq_dma1_stream3
        .long __irq_dma1_stream4
        .long __irq_dma1_stream5
        .long __irq_dma1_stream6
        .long __irq_adc
        .long __irq_can1_tx
        .long __irq_can1_rx0
        .long __irq_can1_rx1
        .long __irq_can1_sce
        .long __irq_exti9_5
        .long __irq_tim1_brk_tim9
        .long __irq_tim1_up_tim10
        .long __irq_tim1_trg_com_tim11
        .long __irq_tim1_cc
        .long __irq_tim2
        .long __irq_tim3
        .long __irq_tim4
        .long __irq_i2c1_ev
        .long __irq_i2c1_er
        .long __irq_i2c2_ev
        .long __irq_i2c2_er
        .long __irq_spi1
        .long __irq_spi2
        .long __irq_usart1
        .long __irq_usart2
        .long __irq_usart3
        .long __irq_exti15_10
        .long __irq_rtc_alarm
        .long __irq_otg_fs_wkup
        .long __irq_tim8_brk_tim12
        .long __irq_tim8_up_tim13
        .long __irq_tim8_trg_com_tim14
        .long __irq_tim8_cc
        .long __irq_dma1_stream7
        .long __irq_fsmc
        .long __irq_sdio
        .long __irq_tim5
        .long __irq_spi3
        .long __irq_uart4
        .long __irq_uart5
        .long __irq_tim6_dac
        .long __irq_tim7
        .long __irq_dma2_stream0
        .long __irq_dma2_stream1
        .long __irq_dma2_stream2
        .long __irq_dma2_stream3
        .long __irq_dma2_stream4
        .long __irq_eth
        .long __irq_eth_wkup
        .long __irq_can2_tx
        .long __irq_can2_rx0
        .long __irq_can2_rx1
        .long __irq_can2_sce
        .long __irq_otg_fs
        .long __irq_dma2_stream5
        .long __irq_dma2_stream6
        .long __irq_dma2_stream7
        .long __irq_usart6
        .long __irq_i2c3_ev
        .long __irq_i2c3_er
        .long __irq_otg_hs_ep1_out
        .long __irq_otg_hs_ep1_in
        .long __irq_otg_hs_wkup
        .long __irq_otg_hs
        .long __irq_dcmi
        .long __irq_cryp
        .long __irq_hash_rng

        .size __stm32_vector_table, . - __stm32_vector_table
