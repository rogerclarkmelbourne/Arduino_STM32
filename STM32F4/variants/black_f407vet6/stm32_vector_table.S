/* STM32 vector table */

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

  .long	__irq_DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */                   
  .long	__irq_ETH_IRQHandler                    /* Ethernet                     */                   
  .long	__irq_ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */                     
  .long	__irq_CAN2_TX_IRQHandler                /* CAN2 TX                      */                          
  .long	__irq_CAN2_RX0_IRQHandler               /* CAN2 RX0                     */                          
  .long	__irq_CAN2_RX1_IRQHandler               /* CAN2 RX1                     */                          
  .long	__irq_CAN2_SCE_IRQHandler               /* CAN2 SCE                     */                          
  .long	__irq_OTG_FS_IRQHandler                 /* USB OTG FS                   */                   
  .long	__irq_DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */                   
  .long	__irq_DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */                   
  .long	__irq_DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */                   
  .long	__irq_USART6_IRQHandler                 /* USART6                       */                    
  .long	__irq_I2C3_EV_IRQHandler                /* I2C3 event                   */                          
  .long	__irq_I2C3_ER_IRQHandler                /* I2C3 error                   */                          
  .long	__irq_OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */                   
  .long	__irq_OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */                   
  .long	__irq_OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */                         
  .long	__irq_OTG_HS_IRQHandler                 /* USB OTG HS                   */                   
  .long	__irq_DCMI_IRQHandler                   /* DCMI                         */                   
  .long	__irq_CRYP_IRQHandler                   /* CRYP crypto                  */                   
  .long	__irq_HASH_RNG_IRQHandler               /* Hash and Rng                 */
  .long	__irq_FPU_IRQHandler                    /* FPU                          */

	.size	__stm32_vector_table, . - __stm32_vector_table
