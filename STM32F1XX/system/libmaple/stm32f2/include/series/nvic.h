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

/**
 * @file libmaple/stm32f2/include/series/nvic.h
 * @brief STM32F2 nested vectored interrupt controller (NVIC) header.
 */

#ifndef _LIBMAPLE_STM32F2_NVIC_H_
#define _LIBMAPLE_STM32F2_NVIC_H_

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief STM32F2 interrupt vector table interrupt numbers.
 */
typedef enum nvic_irq_num {
    NVIC_NMI                    = -14, /**< Non-maskable interrupt */
    NVIC_HARDFAULT              = -13, /**< Hard fault (all class of fault) */
    NVIC_MEM_MANAGE             = -12, /**< Memory management */
    NVIC_BUS_FAULT              = -11, /**< Bus fault: prefetch fault, memory
                                          access fault. */
    NVIC_USAGE_FAULT            = -10, /**< Usage fault: Undefined instruction
                                          or illegal state. */
    NVIC_SVC                    = -5,  /**< System service call via SWI
                                          instruction */
    NVIC_DEBUG_MON              = -4,  /**< Debug monitor */
    NVIC_PEND_SVC               = -2,  /**< Pendable request for system
                                          service */
    NVIC_SYSTICK                = -1,  /**< System tick timer */
    NVIC_WWDG                   = 0,   /**< Window watchdog interrupt */
    NVIC_PVD                    = 1,   /**< PVD through EXTI line detection */
    NVIC_TAMP_STAMP             = 2,   /**< Tamper and TimeStamp */
    NVIC_RTC_WKUP               = 3,   /**< Real-time clock wakeup */
    NVIC_FLASH                  = 4,   /**< Flash */
    NVIC_RCC                    = 5,   /**< Reset and clock control */
    NVIC_EXTI0                  = 6,   /**< EXTI line 0 */
    NVIC_EXTI1                  = 7,   /**< EXTI line 1 */
    NVIC_EXTI2                  = 8,   /**< EXTI line 2 */
    NVIC_EXTI3                  = 9,   /**< EXTI line 3 */
    NVIC_EXTI4                  = 10,  /**< EXTI line 4 */
    NVIC_DMA1_STREAM0           = 11,  /**< DMA1 stream 0 */
    NVIC_DMA1_STREAM1           = 12,  /**< DMA1 stream 1 */
    NVIC_DMA1_STREAM2           = 13,  /**< DMA1 stream 2 */
    NVIC_DMA1_STREAM3           = 14,  /**< DMA1 stream 3 */
    NVIC_DMA1_STREAM4           = 15,  /**< DMA1 stream 4 */
    NVIC_DMA1_STREAM5           = 16,  /**< DMA1 stream 5 */
    NVIC_DMA1_STREAM6           = 17,  /**< DMA1 stream 6 */
    NVIC_ADC                    = 18,  /**< ADC */
    NVIC_CAN1_TX                = 19,  /**< CAN1 TX */
    NVIC_CAN1_RX0               = 20,  /**< CAN1 RX0 */
    NVIC_CAN1_RX1               = 21,  /**< CAN1 RX1 */
    NVIC_CAN1_SCE               = 22,  /**< CAN1 SCE */
    NVIC_EXTI_9_5               = 23,  /**< EXTI lines [9:5] */
    NVIC_TIMER1_BRK_TIMER9      = 24,  /**< Timer 1 break and timer 9 */
    NVIC_TIMER1_UP_TIMER10      = 25,  /**< Timer 1 update and timer 10 */
    NVIC_TIMER1_TRG_COM_TIMER11 = 26,  /**< Timer 1 trigger and commutation and
                                          timer 11.*/
    NVIC_TIMER1_CC              = 27,  /**< Timer 1 capture and compare */
    NVIC_TIMER2                 = 28,  /**< Timer 2 */
    NVIC_TIMER3                 = 29,  /**< Timer 3 */
    NVIC_TIMER4                 = 30,  /**< Timer 4 */
    NVIC_I2C1_EV                = 31,  /**< I2C1 event */
    NVIC_I2C1_ER                = 32,  /**< I2C2 error */
    NVIC_I2C2_EV                = 33,  /**< I2C2 event */
    NVIC_I2C2_ER                = 34,  /**< I2C2 error */
    NVIC_SPI1                   = 35,  /**< SPI1 */
    NVIC_SPI2                   = 36,  /**< SPI2 */
    NVIC_USART1                 = 37,  /**< USART1 */
    NVIC_USART2                 = 38,  /**< USART2 */
    NVIC_USART3                 = 39,  /**< USART3 */
    NVIC_EXTI_15_10             = 40,  /**< EXTI lines [15:10] */
    NVIC_RTCALARM               = 41,  /**< RTC alarms A and B through EXTI */
    NVIC_OTG_FS_WKUP            = 42,  /**< USB on-the-go full-speed wakeup
                                          through EXTI*/
    NVIC_TIMER8_BRK_TIMER12     = 43,  /**< Timer 8 break and timer 12 */
    NVIC_TIMER8_UP_TIMER13      = 44,  /**< Timer 8 update and timer 13 */
    NVIC_TIMER8_TRG_COM_TIMER14 = 45,  /**< Timer 8 trigger and commutation and
                                          timer 14 */
    NVIC_TIMER8_CC              = 46,  /**< Timer 8 capture and compare */
    NVIC_DMA1_STREAM7           = 47,  /**< DMA1 stream 7 */
    NVIC_FSMC                   = 48,  /**< FSMC */
    NVIC_SDIO                   = 49,  /**< SDIO */
    NVIC_TIMER5                 = 50,  /**< Timer 5 */
    NVIC_SPI3                   = 51,  /**< SPI3 */
    NVIC_UART4                  = 52,  /**< UART4 */
    NVIC_UART5                  = 53,  /**< UART5 */
    NVIC_TIMER6_DAC             = 54,  /**< Timer 6 and DAC underrun */
    NVIC_TIMER7                 = 55,  /**< Timer 7 */
    NVIC_DMA2_STREAM0           = 56,  /**< DMA2 stream 0 */
    NVIC_DMA2_STREAM1           = 57,  /**< DMA2 stream 1 */
    NVIC_DMA2_STREAM2           = 58,  /**< DMA2 stream 2 */
    NVIC_DMA2_STREAM3           = 59,  /**< DMA2 stream 3 */
    NVIC_DMA2_STREAM4           = 60,  /**< DMA2 stream 4 */
    NVIC_ETH                    = 61,  /**< Ethernet */
    NVIC_ETH_WKUP               = 62,  /**< Ethernet wakeup through EXTI */
    NVIC_CAN2_TX                = 63,  /**< CAN2 TX */
    NVIC_CAN2_RX0               = 64,  /**< CAN2 RX0 */
    NVIC_CAN2_RX1               = 65,  /**< CAN2 RX1 */
    NVIC_CAN2_SCE               = 66,  /**< CAN2 SCE */
    NVIC_OTG_FS                 = 67,  /**< USB on-the-go full-speed */
    NVIC_DMA2_STREAM5           = 68,  /**< DMA2 stream 5 */
    NVIC_DMA2_STREAM6           = 69,  /**< DMA2 stream 6 */
    NVIC_DMA2_STREAM7           = 70,  /**< DMA2 stream 7 */
    NVIC_USART6                 = 71,  /**< USART6 */
    NVIC_I2C3_EV                = 72,  /**< I2C3 event */
    NVIC_I2C3_ER                = 73,  /**< I2C3 error */
    NVIC_OTG_HS_EP1_OUT         = 74,  /**< USB on-the-go high-speed
                                          endpoint 1 OUT */
    NVIC_OTG_HS_EP1_IN          = 75,  /**< USB on-the-go high-speed
                                          endpoint 1 IN */
    NVIC_OTG_HS_WKUP            = 76,  /**< USB on-the-go high-speed wakeup
                                          through EXTI*/
    NVIC_OTG_HS                 = 77,  /**< USB on-the-go high-speed */
    NVIC_DCMI                   = 78,  /**< DCMI */
    NVIC_CRYP                   = 79,  /**< Cryptographic processor */
    NVIC_HASH_RNG               = 80,  /**< Hash and random number
                                          generation */

    /* Fake enumerator values, for compatiblity with F1.
     * TODO decide if this is actually a good idea. */
    NVIC_TIMER6 = NVIC_TIMER6_DAC, /**< For compatibility with STM32F1. */
} nvic_irq_num;

static inline void nvic_irq_disable_all(void) {
    NVIC_BASE->ICER[0] = 0xFFFFFFFF;
    NVIC_BASE->ICER[1] = 0xFFFFFFFF;
    NVIC_BASE->ICER[2] = 0xFFFFFFFF;
}

#ifdef __cplusplus
}
#endif

#endif
