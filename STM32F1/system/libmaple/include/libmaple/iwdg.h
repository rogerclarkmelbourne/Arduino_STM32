/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Michael Hope.
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
 * @file libmaple/include/libmaple/iwdg.h
 * @author Michael Hope, Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Independent watchdog support.
 *
 * To use the independent watchdog, first call iwdg_init() with the
 * appropriate prescaler and IWDG counter reload values for your
 * application.  Afterwards, you must periodically call iwdg_feed()
 * before the IWDG counter reaches 0 to reset the counter to its
 * reload value.  If you do not, the chip will reset.
 *
 * Once started, the independent watchdog cannot be turned off.
 */

#ifndef _LIBMAPLE_IWDG_H_
#define _LIBMAPLE_IWDG_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/** Independent watchdog register map type. */
typedef struct iwdg_reg_map {
    __IO uint32 KR;             /**< Key register. */
    __IO uint32 PR;             /**< Prescaler register. */
    __IO uint32 RLR;            /**< Reload register. */
    __IO uint32 SR;             /**< Status register */
} iwdg_reg_map;

/** Independent watchdog base pointer */
#define IWDG_BASE                       ((struct iwdg_reg_map*)0x40003000)

/*
 * Register bit definitions
 */

/* Key register */

#define IWDG_KR_UNLOCK                  0x5555
#define IWDG_KR_FEED                    0xAAAA
#define IWDG_KR_START                   0xCCCC

/* Prescaler register */

#define IWDG_PR_DIV_4                   0x0
#define IWDG_PR_DIV_8                   0x1
#define IWDG_PR_DIV_16                  0x2
#define IWDG_PR_DIV_32                  0x3
#define IWDG_PR_DIV_64                  0x4
#define IWDG_PR_DIV_128                 0x5
#define IWDG_PR_DIV_256                 0x6

/* Status register */

#define IWDG_SR_RVU_BIT                 1
#define IWDG_SR_PVU_BIT                 0

#define IWDG_SR_RVU                     (1U << IWDG_SR_RVU_BIT)
#define IWDG_SR_PVU                     (1U << IWDG_SR_PVU_BIT)

/**
 * @brief Independent watchdog prescalers.
 *
 * These divide the 40 kHz IWDG clock.
 */
typedef enum iwdg_prescaler {
    IWDG_PRE_4 = IWDG_PR_DIV_4,     /**< Divide by 4 */
    IWDG_PRE_8 = IWDG_PR_DIV_8,     /**< Divide by 8 */
    IWDG_PRE_16 = IWDG_PR_DIV_16,   /**< Divide by 16 */
    IWDG_PRE_32 = IWDG_PR_DIV_32,   /**< Divide by 32 */
    IWDG_PRE_64 = IWDG_PR_DIV_64,   /**< Divide by 64 */
    IWDG_PRE_128 = IWDG_PR_DIV_128, /**< Divide by 128 */
    IWDG_PRE_256 = IWDG_PR_DIV_256  /**< Divide by 256 */
} iwdg_prescaler;

void iwdg_init(iwdg_prescaler prescaler, uint16 reload);
void iwdg_feed(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
