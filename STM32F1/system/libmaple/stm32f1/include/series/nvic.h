/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/stm32f1/include/series/nvic.h
 * @brief STM32F1 Nested Vectored Interrupt Controller (NVIC) support.
 */

#ifndef _LIBMAPLE_STM32F1_NVIC_H_
#define _LIBMAPLE_STM32F1_NVIC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/stm32.h>

/**
 * @brief STM32F1 interrupt vector table interrupt numbers.
 * @see <libmaple/scb.h>
 */
typedef enum nvic_irq_num {
    NVIC_NMI            = -14,  /**< Non-maskable interrupt */
    NVIC_HARDFAULT      = -13,  /**< Hard fault (all class of fault) */
    NVIC_MEM_MANAGE     = -12,  /**< Memory management */
    NVIC_BUS_FAULT      = -11,  /**< Bus fault: prefetch fault, memory
                                     access fault. */
    NVIC_USAGE_FAULT    = -10,  /**< Usage fault: Undefined instruction or
                                     illegal state. */
    NVIC_SVC            = -5,   /**< System service call via SWI insruction */
    NVIC_DEBUG_MON      = -4,   /**< Debug monitor */
    NVIC_PEND_SVC       = -2,   /**< Pendable request for system service */
    NVIC_SYSTICK        = -1,   /**< System tick timer */
    NVIC_WWDG           = 0,    /**< Window watchdog interrupt */
    NVIC_PVD            = 1,    /**< PVD through EXTI line detection */
    NVIC_TAMPER         = 2,    /**< Tamper */
    NVIC_RTC            = 3,    /**< Real-time clock */
    NVIC_FLASH          = 4,    /**< Flash */
    NVIC_RCC            = 5,    /**< Reset and clock control */
    NVIC_EXTI0          = 6,    /**< EXTI line 0 */
    NVIC_EXTI1          = 7,    /**< EXTI line 1 */
    NVIC_EXTI2          = 8,    /**< EXTI line 2 */
    NVIC_EXTI3          = 9,    /**< EXTI line 3 */
    NVIC_EXTI4          = 10,   /**< EXTI line 4 */
    NVIC_DMA_CH1        = 11,   /**< DMA1 channel 1 */
    NVIC_DMA_CH2        = 12,   /**< DMA1 channel 2 */
    NVIC_DMA_CH3        = 13,   /**< DMA1 channel 3 */
    NVIC_DMA_CH4        = 14,   /**< DMA1 channel 4 */
    NVIC_DMA_CH5        = 15,   /**< DMA1 channel 5 */
    NVIC_DMA_CH6        = 16,   /**< DMA1 channel 6 */
    NVIC_DMA_CH7        = 17,   /**< DMA1 channel 7 */
    NVIC_ADC_1_2        = 18,   /**< ADC1 and ADC2 */
    NVIC_USB_HP_CAN_TX  = 19,   /**< USB high priority or CAN TX */
    NVIC_USB_LP_CAN_RX0 = 20,   /**< USB low priority or CAN RX0 */
    NVIC_CAN_RX1        = 21,   /**< CAN RX1 */
    NVIC_CAN_SCE        = 22,   /**< CAN SCE */
    NVIC_EXTI_9_5       = 23,   /**< EXTI line [9:5] */
    NVIC_TIMER1_BRK_TIMER9 = 24, /**< Timer 1 break, Timer 9. */
    NVIC_TIMER1_UP_TIMER10 = 25, /**< Timer 1 update, Timer 10. */
    NVIC_TIMER1_TRG_COM_TIMER11 = 26, /**<
                                       * Timer 1 trigger and commutation,
                                       * Timer 11. */
    NVIC_TIMER1_CC      = 27,   /**< Timer 1 capture/compare */
    NVIC_TIMER2         = 28,   /**< Timer 2 */
    NVIC_TIMER3         = 29,   /**< Timer 3 */
    NVIC_TIMER4         = 30,   /**< Timer 4 */
    NVIC_I2C1_EV        = 31,   /**< I2C1 event */
    NVIC_I2C1_ER        = 32,   /**< I2C1 error */
    NVIC_I2C2_EV        = 33,   /**< I2C2 event */
    NVIC_I2C2_ER        = 34,   /**< I2C2 error */
    NVIC_SPI1           = 35,   /**< SPI1 */
    NVIC_SPI2           = 36,   /**< SPI2 */
    NVIC_USART1         = 37,   /**< USART1 */
    NVIC_USART2         = 38,   /**< USART2 */
    NVIC_USART3         = 39,   /**< USART3 */
    NVIC_EXTI_15_10     = 40,   /**< EXTI line [15:10] */
    NVIC_RTCALARM       = 41,   /**< RTC alarm through EXTI line */
    NVIC_USBWAKEUP      = 42,   /**< USB wakeup from suspend through
                                     EXTI line */
    NVIC_TIMER8_BRK_TIMER12 = 43, /**< Timer 8 break, timer 12  */
    NVIC_TIMER8_UP_TIMER13 = 44, /**< Timer 8 update, timer 13 */
    NVIC_TIMER8_TRG_COM_TIMER14 = 45, /**<
                                       * Timer 8 trigger and commutation,
                                       * Timer 14. */
    NVIC_TIMER8_CC      = 46,   /**< Timer 8 capture/compare */
    NVIC_ADC3           = 47,   /**< ADC3 */
    NVIC_FSMC           = 48,   /**< FSMC */
    NVIC_SDIO           = 49,   /**< SDIO */
    NVIC_TIMER5         = 50,   /**< Timer 5 */
    NVIC_SPI3           = 51,   /**< SPI3 */
    NVIC_UART4          = 52,   /**< UART4 */
    NVIC_UART5          = 53,   /**< UART5 */
    NVIC_TIMER6         = 54,   /**< Timer 6 */
    NVIC_TIMER7         = 55,   /**< Timer 7 */
    NVIC_DMA2_CH1       = 56,   /**< DMA2 channel 1 */
    NVIC_DMA2_CH2       = 57,   /**< DMA2 channel 2 */
    NVIC_DMA2_CH3       = 58,   /**< DMA2 channel 3 */
    NVIC_DMA2_CH_4_5    = 59,   /**< DMA2 channels 4 and 5 */

    /* Old enumerators kept around for backwards compatibility: */
    NVIC_TIMER1_BRK =
        NVIC_TIMER1_BRK_TIMER9, /**< @brief (Deprecated) Timer 1 break
                                  *
                                  * For backwards compatibility only.
                                  * Use NVIC_TIMER1_BRK_TIMER9 instead. */
    NVIC_TIMER1_UP =
        NVIC_TIMER1_UP_TIMER10, /**< @brief (Deprecated) Timer 1 update.
                                  *
                                  * For backwards compatibility only.
                                  * Use NVIC_TIMER1_UP_TIMER10 instead. */
    NVIC_TIMER1_TRG_COM =
        NVIC_TIMER1_TRG_COM_TIMER11, /**< @brief (deprecated) Timer 1 trigger
                                      *          and commutation.
                                      *
                                      * For backwards compatibility only.
                                      * Use NVIC_TIMER1_TRG_COM_TIMER11
                                      * instead. */
    NVIC_TIMER8_BRK =
        NVIC_TIMER8_BRK_TIMER12, /**< @brief (deprecated) Timer 8 break
                                  *
                                  * For backwards compatibility only.
                                  * Use NVIC_TIMER8_BRK_TIMER12 instead. */
    NVIC_TIMER8_UP =
        NVIC_TIMER8_UP_TIMER13, /**< @brief (deprecated) Timer 8 update
                                  * For backwards compatibility only.
                                  * Use NVIC_TIMER8_UP_TIMER13 instead. */
    NVIC_TIMER8_TRG_COM =
        NVIC_TIMER8_TRG_COM_TIMER14, /**< @brief (deprecated) Timer 8 trigger
                                      *          and commutation.
                                      * For backwards compatibility only.
                                      * Use NVIC_TIMER8_TRG_COM_TIMER14
                                      * instead. */
} nvic_irq_num;

static inline void nvic_irq_disable_all(void) {
    /* Even low-density devices have over 32 interrupt lines. */
    NVIC_BASE->ICER[0] = 0xFFFFFFFF;
    NVIC_BASE->ICER[1] = 0xFFFFFFFF;
#if STM32_NR_INTERRUPTS > 64
    /* Only some have over 64; e.g. connectivity line MCUs. */
    NVIC_BASE->ICER[2] = 0xFFFFFFFF;
#endif
}

#ifdef __cplusplus
}
#endif

#endif
