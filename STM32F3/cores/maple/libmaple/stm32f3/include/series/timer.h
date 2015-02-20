/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file   libmaple/stm32f3/include/series/timer.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 * 				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief  STM32F3 timer support.
 */

#ifndef _LIBMAPLE_STM32F3_TIMER_H_
#define _LIBMAPLE_STM32F3_TIMER_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/gpio.h>

/*
 * Register maps and base pointers
 */

/** STM32F1 general purpose timer register map type */
typedef struct timer_gen_reg_map {
    __io uint32 CR1;            /**< Control register 1 */
    __io uint32 CR2;            /**< Control register 2 */
    __io uint32 SMCR;           /**< Slave mode control register */
    __io uint32 DIER;           /**< DMA/Interrupt enable register */
    __io uint32 SR;             /**< Status register */
    __io uint32 EGR;            /**< Event generation register  */
    __io uint32 CCMR1;          /**< Capture/compare mode register 1 */
    __io uint32 CCMR2;          /**< Capture/compare mode register 2 */
    __io uint32 CCER;           /**< Capture/compare enable register */
    __io uint32 CNT;            /**< Counter */
    __io uint32 PSC;            /**< Prescaler */
    __io uint32 ARR;            /**< Auto-reload register */
    const uint32 RESERVED1;     /**< Reserved */
    __io uint32 CCR1;           /**< Capture/compare register 1 */
    __io uint32 CCR2;           /**< Capture/compare register 2 */
    __io uint32 CCR3;           /**< Capture/compare register 3 */
    __io uint32 CCR4;           /**< Capture/compare register 4 */
    const uint32 RESERVED2;     /**< Reserved */
    __io uint32 DCR;            /**< DMA control register */
    __io uint32 DMAR;           /**< DMA address for full transfer */
} timer_gen_reg_map;

struct timer_adv_reg_map;
struct timer_bas_reg_map;

/** Timer 1 register map base pointer */
#define TIMER1_BASE                     ((struct timer_adv_reg_map*)0x40012C00)
/** Timer 2 register map base pointer */
#define TIMER2_BASE                     ((struct timer_gen_reg_map*)0x40000000)
/** Timer 3 register map base pointer */
#define TIMER3_BASE                     ((struct timer_gen_reg_map*)0x40000400)
/** Timer 4 register map base pointer */
#define TIMER4_BASE                     ((struct timer_gen_reg_map*)0x40000800)

/** Timer 6 register map base pointer */
#define TIMER6_BASE                     ((struct timer_bas_reg_map*)0x40001000)
/** Timer 7 register map base pointer */
#define TIMER7_BASE                     ((struct timer_bas_reg_map*)0x40001400)
/** Timer 8 register map base pointer */
#define TIMER8_BASE                     ((struct timer_adv_reg_map*)0x40013400)

/** Timer 15 register map base pointer */
#define TIMER15_BASE                    ((struct timer_gen_reg_map*)0x40014000)
/** Timer 16 register map base pointer */
#define TIMER16_BASE                    ((struct timer_gen_reg_map*)0x40014400)
/** Timer 17 register map base pointer */
#define TIMER17_BASE                    ((struct timer_gen_reg_map*)0x40014800)

/*
 * Device pointers
 *
 * We only declare device pointers to timers which actually exist on
 * the target MCU.
 */

struct timer_dev;

extern struct timer_dev *TIMER1;
extern struct timer_dev *TIMER2; /* FIXME 32bit-capable counter */
extern struct timer_dev *TIMER3;
extern struct timer_dev *TIMER4;
extern struct timer_dev *TIMER6;
#if STM32_F3_LINE == STM32_F3_LINE_303
extern struct timer_dev *TIMER7;
extern struct timer_dev *TIMER8;
#endif
extern struct timer_dev *TIMER15;
extern struct timer_dev *TIMER16;
extern struct timer_dev *TIMER17;

/*
 * Routines
 */

gpio_af timer_get_af(struct timer_dev *dev);

#endif
