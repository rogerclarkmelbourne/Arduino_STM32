/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011,2012 LeafLabs, LLC.
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
 * @file   libmaple/stm32f2/include/series/timer.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  STM32F2 timer support.
 */

#ifndef _LIBMAPLE_STM32F2_TIMER_H_
#define _LIBMAPLE_STM32F2_TIMER_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/gpio.h>      /* for gpio_af */

/*
 * Register maps and base pointers
 */

/**
 * @brief STM32F2 general purpose timer register map type
 *
 * Note that not all general purpose timers have all of these
 * registers. Consult your chip's reference manual for the details.
 */
typedef struct timer_gen_reg_map {
    __IO uint32 CR1;            /**< Control register 1 */
    __IO uint32 CR2;            /**< Control register 2 */
    __IO uint32 SMCR;           /**< Slave mode control register */
    __IO uint32 DIER;           /**< DMA/Interrupt enable register */
    __IO uint32 SR;             /**< Status register */
    __IO uint32 EGR;            /**< Event generation register  */
    __IO uint32 CCMR1;          /**< Capture/compare mode register 1 */
    __IO uint32 CCMR2;          /**< Capture/compare mode register 2 */
    __IO uint32 CCER;           /**< Capture/compare enable register */
    __IO uint32 CNT;            /**< Counter */
    __IO uint32 PSC;            /**< Prescaler */
    __IO uint32 ARR;            /**< Auto-reload register */
    const uint32 RESERVED1;     /**< Reserved */
    __IO uint32 CCR1;           /**< Capture/compare register 1 */
    __IO uint32 CCR2;           /**< Capture/compare register 2 */
    __IO uint32 CCR3;           /**< Capture/compare register 3 */
    __IO uint32 CCR4;           /**< Capture/compare register 4 */
    const uint32 RESERVED2;     /**< Reserved */
    __IO uint32 DCR;            /**< DMA control register */
    __IO uint32 DMAR;           /**< DMA address for full transfer */
    __IO uint32 OR;             /**< Option register. */
} timer_gen_reg_map;

struct timer_adv_reg_map;
struct timer_bas_reg_map;

/** Timer 1 register map base pointer */
#define TIMER1_BASE                     ((struct timer_adv_reg_map*)0x40010000)
/** Timer 2 register map base pointer */
#define TIMER2_BASE                     ((struct timer_gen_reg_map*)0x40000000)
/** Timer 3 register map base pointer */
#define TIMER3_BASE                     ((struct timer_gen_reg_map*)0x40000400)
/** Timer 4 register map base pointer */
#define TIMER4_BASE                     ((struct timer_gen_reg_map*)0x40000800)
/** Timer 5 register map base pointer */
#define TIMER5_BASE                     ((struct timer_gen_reg_map*)0x40000C00)
/** Timer 6 register map base pointer */
#define TIMER6_BASE                     ((struct timer_bas_reg_map*)0x40001000)
/** Timer 7 register map base pointer */
#define TIMER7_BASE                     ((struct timer_bas_reg_map*)0x40001400)
/** Timer 8 register map base pointer */
#define TIMER8_BASE                     ((struct timer_adv_reg_map*)0x40010400)
/** Timer 9 register map base pointer */
#define TIMER9_BASE                     ((struct timer_gen_reg_map*)0x40014000)
/** Timer 10 register map base pointer */
#define TIMER10_BASE                    ((struct timer_gen_reg_map*)0x40014400)
/** Timer 11 register map base pointer */
#define TIMER11_BASE                    ((struct timer_gen_reg_map*)0x40014800)
/** Timer 12 register map base pointer */
#define TIMER12_BASE                    ((struct timer_gen_reg_map*)0x40001800)
/** Timer 13 register map base pointer */
#define TIMER13_BASE                    ((struct timer_gen_reg_map*)0x40001C00)
/** Timer 14 register map base pointer */
#define TIMER14_BASE                    ((struct timer_gen_reg_map*)0x40002000)

/*
 * Register bit definitions
 */

/* TIM2 option register */

/** Timer 2 option register internal trigger 1 remap */
#define TIMER2_OR_ITR1_RMP              (0x3 << 10)
/** Timer 2 OR internal trigger 1: TIM8_TRGOUT */
#define TIMER2_OR_ITR1_RMP_TIM8_TRGOUT  (0x0 << 10)
/** Timer 2 OR internal trigger 1: Ethernet PTP trigger output */
#define TIMER2_OR_ITR1_RMP_PTP_TRGOUT   (0x1 << 10)
/** Timer 2 OR internal trigger 1: USB OTG full speed start of frame */
#define TIMER2_OR_ITR1_RMP_OTG_FS_SOF   (0x2 << 10)
/** Timer 2 OR internal trigger 1: USB OTG high speed start of frame */
#define TIMER2_OR_ITR1_RMP_OTG_HS_SOF   (0x3 << 10)

/* TIM5 option register */

/**
 * Timer 5 option register input 4 remap.
 *
 * These bits control whether TIM5_CH4 is connected to a GPIO or a
 * clock. Connecting to a GPIO is the normal mode, useful for e.g. PWM
 * generation or input pulse duration measurement. Connecting to a
 * clock is useful for calibrating that clock.
 */
#define TIMER5_OR_TI4_RMP               (0x3 << 6)
/**
 * Timer 5 OR input 4: Timer 5 channel 4 connected to GPIO. */
#define TIMER5_OR_TI4_RMP_GPIO          (0x0 << 6)
/**
 * Timer 5 OR input 4: low speed internal clock (LSI) is connected to
 * TIM5_CH4. */
#define TIMER5_OR_TI4_RMP_LSI           (0x1 << 6)
/**
 * Timer 5 OR input 4: low speed external clock (LSE) is connected to
 * TIM5_CH4. */
#define TIMER5_OR_TI4_RMP_LSE           (0x2 << 6)
/**
 * Timer 5 OR input 4: real time clock (RTC) output is connected to
 * TIM5_CH4. */
#define TIMER5_OR_TI4_RMP_RTC           (0x3 << 6)

/*
 * Device pointers
 */

struct timer_dev;

extern struct timer_dev *TIMER1;
extern struct timer_dev *TIMER2;
extern struct timer_dev *TIMER3;
extern struct timer_dev *TIMER4;
extern struct timer_dev *TIMER5;
extern struct timer_dev *TIMER6;
extern struct timer_dev *TIMER7;
extern struct timer_dev *TIMER8;
extern struct timer_dev *TIMER9;
extern struct timer_dev *TIMER10;
extern struct timer_dev *TIMER11;
extern struct timer_dev *TIMER12;
extern struct timer_dev *TIMER13;
extern struct timer_dev *TIMER14;

/*
 * Routines
 */

gpio_af timer_get_af(struct timer_dev *dev);

#endif
