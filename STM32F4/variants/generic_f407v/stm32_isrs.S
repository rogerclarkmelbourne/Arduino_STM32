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
	.weak	__irq_dma1_channel1
	.globl	__irq_dma1_channel1
	.set	__irq_dma1_channel1, __default_handler
	.weak	__irq_dma1_channel2
	.globl	__irq_dma1_channel2
	.set	__irq_dma1_channel2, __default_handler
	.weak	__irq_dma1_channel3
	.globl	__irq_dma1_channel3
	.set	__irq_dma1_channel3, __default_handler
	.weak	__irq_dma1_channel4
	.globl	__irq_dma1_channel4
	.set	__irq_dma1_channel4, __default_handler
	.weak	__irq_dma1_channel5
	.globl	__irq_dma1_channel5
	.set	__irq_dma1_channel5, __default_handler
	.weak	__irq_dma1_channel6
	.globl	__irq_dma1_channel6
	.set	__irq_dma1_channel6, __default_handler
	.weak	__irq_dma1_channel7
	.globl	__irq_dma1_channel7
	.set	__irq_dma1_channel7, __default_handler
	.weak	__irq_adc
	.globl	__irq_adc
	.set	__irq_adc, __default_handler
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
#if defined (STM32_HIGH_DENSITY)
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
	.weak	__irq_tim6
	.globl	__irq_tim6
	.set	__irq_tim6, __default_handler
	.weak	__irq_tim7
	.globl	__irq_tim7
	.set	__irq_tim7, __default_handler
	.weak	__irq_dma2_channel1
	.globl	__irq_dma2_channel1
	.set	__irq_dma2_channel1, __default_handler
	.weak	__irq_dma2_channel2
	.globl	__irq_dma2_channel2
	.set	__irq_dma2_channel2, __default_handler
	.weak	__irq_dma2_channel3
	.globl	__irq_dma2_channel3
	.set	__irq_dma2_channel3, __default_handler
	.weak	__irq_dma2_channel4_5
	.globl	__irq_dma2_channel4_5
	.set	__irq_dma2_channel4_5, __default_handler
#endif /* STM32_HIGH_DENSITY */

	.weak	__irq_DMA2_Stream4_IRQHandler              
	.globl	__irq_DMA2_Stream4_IRQHandler              
	.set	__irq_DMA2_Stream4_IRQHandler, __default_handler

  .weak	__irq_ETH_IRQHandler                       
  .globl	__irq_ETH_IRQHandler                       
  .set	__irq_ETH_IRQHandler, __default_handler

  .weak	__irq_ETH_WKUP_IRQHandler                         
  .globl	__irq_ETH_WKUP_IRQHandler                         
  .set	__irq_ETH_WKUP_IRQHandler, __default_handler  

  .weak	__irq_CAN2_TX_IRQHandler                          
  .globl	__irq_CAN2_TX_IRQHandler                          
  .set	__irq_CAN2_TX_IRQHandler, __default_handler    

  .weak	__irq_CAN2_RX0_IRQHandler                         
  .globl	__irq_CAN2_RX0_IRQHandler                         
  .set	__irq_CAN2_RX0_IRQHandler, __default_handler      

  .weak	__irq_CAN2_RX1_IRQHandler                         
  .globl	__irq_CAN2_RX1_IRQHandler                         
  .set	__irq_CAN2_RX1_IRQHandler, __default_handler        

  .weak	__irq_CAN2_SCE_IRQHandler                         
  .globl	__irq_CAN2_SCE_IRQHandler                         
  .set	__irq_CAN2_SCE_IRQHandler, __default_handler          

  .weak	__irq_OTG_FS_IRQHandler                    
  .globl	__irq_OTG_FS_IRQHandler                    
  .set	__irq_OTG_FS_IRQHandler, __default_handler            

  .weak	__irq_DMA2_Stream5_IRQHandler              
  .globl	__irq_DMA2_Stream5_IRQHandler              
  .set	__irq_DMA2_Stream5_IRQHandler, __default_handler              

  .weak	__irq_DMA2_Stream6_IRQHandler              
  .globl	__irq_DMA2_Stream6_IRQHandler              
  .set	__irq_DMA2_Stream6_IRQHandler, __default_handler                

  .weak	__irq_DMA2_Stream7_IRQHandler              
  .globl	__irq_DMA2_Stream7_IRQHandler              
  .set	__irq_DMA2_Stream7_IRQHandler, __default_handler                  

  .weak	__irq_USART6_IRQHandler                     
  .globl	__irq_USART6_IRQHandler                     
  .set	__irq_USART6_IRQHandler, __default_handler                    

  .weak	__irq_I2C3_EV_IRQHandler                          
  .globl	__irq_I2C3_EV_IRQHandler                          
  .set	__irq_I2C3_EV_IRQHandler, __default_handler                      

  .weak	__irq_I2C3_ER_IRQHandler                          
  .globl	__irq_I2C3_ER_IRQHandler                          
  .set	__irq_I2C3_ER_IRQHandler, __default_handler                        

  .weak	__irq_OTG_HS_EP1_OUT_IRQHandler              
  .globl	__irq_OTG_HS_EP1_OUT_IRQHandler              
  .set	__irq_OTG_HS_EP1_OUT_IRQHandler, __default_handler                          

  .weak	__irq_OTG_HS_EP1_IN_IRQHandler              
  .globl	__irq_OTG_HS_EP1_IN_IRQHandler              
  .set	__irq_OTG_HS_EP1_IN_IRQHandler, __default_handler                            

  .weak	__irq_OTG_HS_WKUP_IRQHandler                       
  .globl	__irq_OTG_HS_WKUP_IRQHandler                       
  .set	__irq_OTG_HS_WKUP_IRQHandler, __default_handler                              

  .weak	__irq_OTG_HS_IRQHandler                    
  .globl	__irq_OTG_HS_IRQHandler                    
  .set	__irq_OTG_HS_IRQHandler, __default_handler                                

  .weak	__irq_DCMI_IRQHandler                      
  .globl	__irq_DCMI_IRQHandler                      
  .set	__irq_DCMI_IRQHandler, __default_handler                                  

  .weak	__irq_CRYP_IRQHandler                      
  .globl	__irq_CRYP_IRQHandler                      
  .set	__irq_CRYP_IRQHandler, __default_handler                                    

  .weak	__irq_HASH_RNG_IRQHandler    
  .globl	__irq_HASH_RNG_IRQHandler    
  .set	__irq_HASH_RNG_IRQHandler, __default_handler                                      

  .weak	__irq_FPU_IRQHandler         
  .globl	__irq_FPU_IRQHandler         
  .set	__irq_FPU_IRQHandler, __default_handler                                        
