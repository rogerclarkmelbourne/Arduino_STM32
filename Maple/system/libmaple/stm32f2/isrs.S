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

/* STM32F2 ISR weak declarations */

        .thumb

/* Default handler, as with STM32F1 */
        .globl __default_handler
        .type __default_handler, %function

__default_handler:
        b .

        .weak __exc_nmi
        .globl __exc_nmi
        .set __exc_nmi, __default_handler
        .weak __exc_hardfault
        .globl __exc_hardfault
        .set __exc_hardfault, __default_handler
        .weak __exc_memmanage
        .globl __exc_memmanage
        .set __exc_memmanage, __default_handler
        .weak __exc_busfault
        .globl __exc_busfault
        .set __exc_busfault, __default_handler
        .weak __exc_usagefault
        .globl __exc_usagefault
        .set __exc_usagefault, __default_handler
        .weak __stm32reservedexception7
        .globl __stm32reservedexception7
        .set __stm32reservedexception7, __default_handler
        .weak __stm32reservedexception8
        .globl __stm32reservedexception8
        .set __stm32reservedexception8, __default_handler
        .weak __stm32reservedexception9
        .globl __stm32reservedexception9
        .set __stm32reservedexception9, __default_handler
        .weak __stm32reservedexception10
        .globl __stm32reservedexception10
        .set __stm32reservedexception10, __default_handler
        .weak __exc_svc
        .globl __exc_svc
        .set __exc_svc, __default_handler
        .weak __exc_debug_monitor
        .globl __exc_debug_monitor
        .set __exc_debug_monitor, __default_handler
        .weak __stm32reservedexception13
        .globl __stm32reservedexception13
        .set __stm32reservedexception13, __default_handler
        .weak __exc_pendsv
        .globl __exc_pendsv
        .set __exc_pendsv, __default_handler
        .weak __exc_systick
        .globl __exc_systick
        .set __exc_systick, __default_handler
        .weak __irq_wwdg
        .globl __irq_wwdg
        .set __irq_wwdg, __default_handler
        .weak __irq_pvd
        .globl __irq_pvd
        .set __irq_pvd, __default_handler
        .weak __irq_tamp_stamp
        .globl __irq_tamp_stamp
        .set __irq_tamp_stamp, __default_handler
        .weak __irq_rtc_wkup
        .globl __irq_rtc_wkup
        .set __irq_rtc_wkup, __default_handler
        .weak __irq_flash
        .globl __irq_flash
        .set __irq_flash, __default_handler
        .weak __irq_rcc
        .globl __irq_rcc
        .set __irq_rcc, __default_handler
        .weak __irq_exti0
        .globl __irq_exti0
        .set __irq_exti0, __default_handler
        .weak __irq_exti1
        .globl __irq_exti1
        .set __irq_exti1, __default_handler
        .weak __irq_exti2
        .globl __irq_exti2
        .set __irq_exti2, __default_handler
        .weak __irq_exti3
        .globl __irq_exti3
        .set __irq_exti3, __default_handler
        .weak __irq_exti4
        .globl __irq_exti4
        .set __irq_exti4, __default_handler
        .weak __irq_dma1_stream0
        .globl __irq_dma1_stream0
        .set __irq_dma1_stream0, __default_handler
        .weak __irq_dma1_stream1
        .globl __irq_dma1_stream1
        .set __irq_dma1_stream1, __default_handler
        .weak __irq_dma1_stream2
        .globl __irq_dma1_stream2
        .set __irq_dma1_stream2, __default_handler
        .weak __irq_dma1_stream3
        .globl __irq_dma1_stream3
        .set __irq_dma1_stream3, __default_handler
        .weak __irq_dma1_stream4
        .globl __irq_dma1_stream4
        .set __irq_dma1_stream4, __default_handler
        .weak __irq_dma1_stream5
        .globl __irq_dma1_stream5
        .set __irq_dma1_stream5, __default_handler
        .weak __irq_dma1_stream6
        .globl __irq_dma1_stream6
        .set __irq_dma1_stream6, __default_handler
        .weak __irq_adc
        .globl __irq_adc
        .set __irq_adc, __default_handler
        .weak __irq_can1_tx
        .globl __irq_can1_tx
        .set __irq_can1_tx, __default_handler
        .weak __irq_can1_rx0
        .globl __irq_can1_rx0
        .set __irq_can1_rx0, __default_handler
        .weak __irq_can1_rx1
        .globl __irq_can1_rx1
        .set __irq_can1_rx1, __default_handler
        .weak __irq_can1_sce
        .globl __irq_can1_sce
        .set __irq_can1_sce, __default_handler
        .weak __irq_exti9_5
        .globl __irq_exti9_5
        .set __irq_exti9_5, __default_handler
        .weak __irq_tim1_brk_tim9
        .globl __irq_tim1_brk_tim9
        .set __irq_tim1_brk_tim9, __default_handler
        .weak __irq_tim1_up_tim10
        .globl __irq_tim1_up_tim10
        .set __irq_tim1_up_tim10, __default_handler
        .weak __irq_tim1_trg_com_tim11
        .globl __irq_tim1_trg_com_tim11
        .set __irq_tim1_trg_com_tim11, __default_handler
        .weak __irq_tim1_cc
        .globl __irq_tim1_cc
        .set __irq_tim1_cc, __default_handler
        .weak __irq_tim2
        .globl __irq_tim2
        .set __irq_tim2, __default_handler
        .weak __irq_tim3
        .globl __irq_tim3
        .set __irq_tim3, __default_handler
        .weak __irq_tim4
        .globl __irq_tim4
        .set __irq_tim4, __default_handler
        .weak __irq_i2c1_ev
        .globl __irq_i2c1_ev
        .set __irq_i2c1_ev, __default_handler
        .weak __irq_i2c1_er
        .globl __irq_i2c1_er
        .set __irq_i2c1_er, __default_handler
        .weak __irq_i2c2_ev
        .globl __irq_i2c2_ev
        .set __irq_i2c2_ev, __default_handler
        .weak __irq_i2c2_er
        .globl __irq_i2c2_er
        .set __irq_i2c2_er, __default_handler
        .weak __irq_spi1
        .globl __irq_spi1
        .set __irq_spi1, __default_handler
        .weak __irq_spi2
        .globl __irq_spi2
        .set __irq_spi2, __default_handler
        .weak __irq_usart1
        .globl __irq_usart1
        .set __irq_usart1, __default_handler
        .weak __irq_usart2
        .globl __irq_usart2
        .set __irq_usart2, __default_handler
        .weak __irq_usart3
        .globl __irq_usart3
        .set __irq_usart3, __default_handler
        .weak __irq_exti15_10
        .globl __irq_exti15_10
        .set __irq_exti15_10, __default_handler
        .weak __irq_rtc_alarm
        .globl __irq_rtc_alarm
        .set __irq_rtc_alarm, __default_handler
        .weak __irq_otg_fs_wkup
        .globl __irq_otg_fs_wkup
        .set __irq_otg_fs_wkup, __default_handler
        .weak __irq_tim8_brk_tim12
        .globl __irq_tim8_brk_tim12
        .set __irq_tim8_brk_tim12, __default_handler
        .weak __irq_tim8_up_tim13
        .globl __irq_tim8_up_tim13
        .set __irq_tim8_up_tim13, __default_handler
        .weak __irq_tim8_trg_com_tim14
        .globl __irq_tim8_trg_com_tim14
        .set __irq_tim8_trg_com_tim14, __default_handler
        .weak __irq_tim8_cc
        .globl __irq_tim8_cc
        .set __irq_tim8_cc, __default_handler
        .weak __irq_dma1_stream7
        .globl __irq_dma1_stream7
        .set __irq_dma1_stream7, __default_handler
        .weak __irq_fsmc
        .globl __irq_fsmc
        .set __irq_fsmc, __default_handler
        .weak __irq_sdio
        .globl __irq_sdio
        .set __irq_sdio, __default_handler
        .weak __irq_tim5
        .globl __irq_tim5
        .set __irq_tim5, __default_handler
        .weak __irq_spi3
        .globl __irq_spi3
        .set __irq_spi3, __default_handler
        .weak __irq_uart4
        .globl __irq_uart4
        .set __irq_uart4, __default_handler
        .weak __irq_uart5
        .globl __irq_uart5
        .set __irq_uart5, __default_handler
        .weak __irq_tim6_dac
        .globl __irq_tim6_dac
        .set __irq_tim6_dac, __default_handler
        .weak __irq_tim7
        .globl __irq_tim7
        .set __irq_tim7, __default_handler
        .weak __irq_dma2_stream0
        .globl __irq_dma2_stream0
        .set __irq_dma2_stream0, __default_handler
        .weak __irq_dma2_stream1
        .globl __irq_dma2_stream1
        .set __irq_dma2_stream1, __default_handler
        .weak __irq_dma2_stream2
        .globl __irq_dma2_stream2
        .set __irq_dma2_stream2, __default_handler
        .weak __irq_dma2_stream3
        .globl __irq_dma2_stream3
        .set __irq_dma2_stream3, __default_handler
        .weak __irq_dma2_stream4
        .globl __irq_dma2_stream4
        .set __irq_dma2_stream4, __default_handler
        .weak __irq_eth
        .globl __irq_eth
        .set __irq_eth, __default_handler
        .weak __irq_eth_wkup
        .globl __irq_eth_wkup
        .set __irq_eth_wkup, __default_handler
        .weak __irq_can2_tx
        .globl __irq_can2_tx
        .set __irq_can2_tx, __default_handler
        .weak __irq_can2_rx0
        .globl __irq_can2_rx0
        .set __irq_can2_rx0, __default_handler
        .weak __irq_can2_rx1
        .globl __irq_can2_rx1
        .set __irq_can2_rx1, __default_handler
        .weak __irq_can2_sce
        .globl __irq_can2_sce
        .set __irq_can2_sce, __default_handler
        .weak __irq_otg_fs
        .globl __irq_otg_fs
        .set __irq_otg_fs, __default_handler
        .weak __irq_dma2_stream5
        .globl __irq_dma2_stream5
        .set __irq_dma2_stream5, __default_handler
        .weak __irq_dma2_stream6
        .globl __irq_dma2_stream6
        .set __irq_dma2_stream6, __default_handler
        .weak __irq_dma2_stream7
        .globl __irq_dma2_stream7
        .set __irq_dma2_stream7, __default_handler
        .weak __irq_usart6
        .globl __irq_usart6
        .set __irq_usart6, __default_handler
        .weak __irq_i2c3_ev
        .globl __irq_i2c3_ev
        .set __irq_i2c3_ev, __default_handler
        .weak __irq_i2c3_er
        .globl __irq_i2c3_er
        .set __irq_i2c3_er, __default_handler
        .weak __irq_otg_hs_ep1_out
        .globl __irq_otg_hs_ep1_out
        .set __irq_otg_hs_ep1_out, __default_handler
        .weak __irq_otg_hs_ep1_in
        .globl __irq_otg_hs_ep1_in
        .set __irq_otg_hs_ep1_in, __default_handler
        .weak __irq_otg_hs_wkup
        .globl __irq_otg_hs_wkup
        .set __irq_otg_hs_wkup, __default_handler
        .weak __irq_otg_hs
        .globl __irq_otg_hs
        .set __irq_otg_hs, __default_handler
        .weak __irq_dcmi
        .globl __irq_dcmi
        .set __irq_dcmi, __default_handler
        .weak __irq_cryp
        .globl __irq_cryp
        .set __irq_cryp, __default_handler
        .weak __irq_hash_rng
        .globl __irq_hash_rng
        .set __irq_hash_rng, __default_handler
