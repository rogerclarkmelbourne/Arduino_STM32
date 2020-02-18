/* STM32 ISR weak declarations */

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
	.weak	__irq_dma1_stream0
	.globl	__irq_dma1_stream0
	.set	__irq_dma1_stream0, __default_handler
	.weak	__irq_dma1_stream1
	.globl	__irq_dma1_stream1
	.set	__irq_dma1_stream1, __default_handler
	.weak	__irq_dma1_stream2
	.globl	__irq_dma1_stream2
	.set	__irq_dma1_stream2, __default_handler
	.weak	__irq_dma1_stream3
	.globl	__irq_dma1_stream3
	.set	__irq_dma1_stream3, __default_handler
	.weak	__irq_dma1_stream4
	.globl	__irq_dma1_stream4
	.set	__irq_dma1_stream4, __default_handler
	.weak	__irq_dma1_stream5
	.globl	__irq_dma1_stream5
	.set	__irq_dma1_stream5, __default_handler
	.weak	__irq_dma1_stream6
	.globl	__irq_dma1_stream6
	.set	__irq_dma1_stream6, __default_handler
	.weak	__irq_adc
	.globl	__irq_adc
	.set	__irq_adc, __default_handler
	.weak	__irq_can1_tx
	.globl	__irq_can1_tx
	.set	__irq_can1_tx, __default_handler
	.weak	__irq_can1_rx0
	.globl	__irq_can1_rx0
	.set	__irq_can1_rx0, __default_handler
	.weak	__irq_can1_rx1
	.globl	__irq_can1_rx1
	.set	__irq_can1_rx1, __default_handler
	.weak	__irq_can1_sce
	.globl	__irq_can1_sce
	.set	__irq_can1_sce, __default_handler
	.weak	__irq_exti9_5
	.globl	__irq_exti9_5
	.set	__irq_exti9_5, __default_handler
	.weak	__irq_tim1_brk
	.globl	__irq_tim1_brk
	.set	__irq_tim1_brk, __default_handler
	.weak	__irq_tim1_up
	.globl	__irq_tim1_up
	.set	__irq_tim1_up, __default_handler
	.weak	__irq_tim1_trg_com
	.globl	__irq_tim1_trg_com
	.set	__irq_tim1_trg_com, __default_handler
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
	.weak	__irq_rtcalarm
	.globl	__irq_rtcalarm
	.set	__irq_rtcalarm, __default_handler
	.weak	__irq_usbwakeup
	.globl	__irq_usbwakeup
	.set	__irq_usbwakeup, __default_handler
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
	.weak	__irq_dma1_stream7
	.globl	__irq_dma1_stream7
	.set	__irq_dma1_stream7, __default_handler
	.weak	__irq_fsmc
	.globl	__irq_fsmc
	.set	__irq_fsmc, __default_handler
	.weak	__irq_sdio
	.globl	__irq_sdio
	.set	__irq_sdio, __default_handler
	.weak	__irq_tim5
	.globl	__irq_tim5
	.set	__irq_tim5, __default_handler
	.weak	__irq_spi3
	.globl	__irq_spi3
	.set	__irq_spi3, __default_handler
	.weak	__irq_uart4
	.globl	__irq_uart4
	.set	__irq_uart4, __default_handler
	.weak	__irq_uart5
	.globl	__irq_uart5
	.set	__irq_uart5, __default_handler
	.weak	__irq_tim6_dac
	.globl	__irq_tim6_dac
	.set	__irq_tim6_dac, __default_handler
	.weak	__irq_tim7
	.globl	__irq_tim7
	.set	__irq_tim7, __default_handler
	.weak	__irq_dma2_stream0
	.globl	__irq_dma2_stream0
	.set	__irq_dma2_stream0, __default_handler
	.weak	__irq_dma2_stream1
	.globl	__irq_dma2_stream1
	.set	__irq_dma2_stream1, __default_handler
	.weak	__irq_dma2_stream2
	.globl	__irq_dma2_stream2
	.set	__irq_dma2_stream2, __default_handler
	.weak	__irq_dma2_stream3
	.globl	__irq_dma2_stream3
	.set	__irq_dma2_stream3, __default_handler
	.weak	__irq_dma2_stream4
	.globl	__irq_dma2_stream4
	.set	__irq_dma2_stream4, __default_handler
    .weak	__irq_eth
    .globl	__irq_eth
    .set	__irq_eth, __default_handler
    .weak	__irq_eth_wkup
    .globl	__irq_eth_wkup
    .set	__irq_eth_wkup, __default_handler
    .weak	__irq_can2_tx
    .globl	__irq_can2_tx
    .set	__irq_can2_tx, __default_handler
    .weak	__irq_can2_rx0
    .globl	__irq_can2_rx0
    .set	__irq_can2_rx0, __default_handler
    .weak	__irq_can2_rx1
    .globl	__irq_can2_rx1
    .set	__irq_can2_rx1, __default_handler
    .weak	__irq_can2_sce
    .globl	__irq_can2_sce
    .set	__irq_can2_sce, __default_handler
    .weak	__irq_usb_fs
    .globl	__irq_usb_fs
    .set	__irq_usb_fs, __default_handler
    .weak	__irq_dma2_stream5
    .globl	__irq_dma2_stream5
    .set	__irq_dma2_stream5, __default_handler              
    .weak	__irq_dma2_stream6
    .globl	__irq_dma2_stream6
    .set	__irq_dma2_stream6, __default_handler
    .weak	__irq_dma2_stream7
    .globl	__irq_dma2_stream7
    .set	__irq_dma2_stream7, __default_handler
    .weak	__irq_usart6
    .globl	__irq_usart6
    .set	__irq_usart6, __default_handler                    
    .weak	__irq_i2c3_ev
    .globl	__irq_i2c3_ev
    .set	__irq_i2c3_ev, __default_handler
    .weak	__irq_i2c3_er
    .globl	__irq_i2c3_er
    .set	__irq_i2c3_er, __default_handler
    .weak	__irq_usb_hs_ep1_out
    .globl	__irq_usb_hs_ep1_out
    .set	__irq_usb_hs_ep1_out, __default_handler
    .weak	__irq_usb_hs_ep1_in
    .globl	__irq_usb_hs_ep1_in
    .set	__irq_usb_hs_ep1_in, __default_handler
    .weak	__irq_usb_hs_wkup
    .globl	__irq_usb_hs_wkup
    .set	__irq_usb_hs_wkup, __default_handler
    .weak	__irq_usb_hs
    .globl	__irq_usb_hs
    .set	__irq_usb_hs, __default_handler
    .weak	__irq_dcmi
    .globl	__irq_dcmi
    .set	__irq_dcmi, __default_handler
    .weak	__irq_cryp
    .globl	__irq_cryp
    .set	__irq_cryp, __default_handler
    .weak	__irq_hash_rng
    .globl	__irq_hash_rng
    .set	__irq_hash_rng, __default_handler
    .weak	__irq_fpu
    .globl	__irq_fpu
    .set	__irq_fpu, __default_handler
