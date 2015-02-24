/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f2/include/series/pwr.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 Power control (PWR) support.
 */

#ifndef _LIBMAPLE_STM32F2_PWR_H_
#define _LIBMAPLE_STM32F2_PWR_H_

/*
 * Additional register bits
 */

/* Control register */

/**
 * @brief Flash power down in stop mode bit.
 * Availability: STM32F2 */
#define PWR_CR_FPDS_BIT                 9
/**
 * @brief Flash power down in stop mode.
 * Availability: STM32F2 */
#define PWR_CR_FPDS                     (1U << PWR_CR_FPDS_BIT)

/* PVD level selection */
#define PWR_CR_PLS_2_0V                 (0x0 << 5)
#define PWR_CR_PLS_2_1V                 (0x1 << 5)
#define PWR_CR_PLS_2_3V                 (0x2 << 5)
#define PWR_CR_PLS_2_5V                 (0x3 << 5)
#define PWR_CR_PLS_2_6V                 (0x4 << 5)
#define PWR_CR_PLS_2_7V                 (0x5 << 5)
#define PWR_CR_PLS_2_8V                 (0x6 << 5)
#define PWR_CR_PLS_2_9V                 (0x7 << 5)

/* Control/Status register */

/** Backup regulator enable bit. */
#define PWR_CSR_BRE_BIT                 9
/** Backup regulator ready bit. */
#define PWR_CSR_BRR_BIT                 3

/** Backup regulator enable. */
#define PWR_CSR_BRE                     (1U << PWR_CSR_BRE_BIT)
/** Backup regulator ready. */
#define PWR_CSR_BRR                     (1U << PWR_CSR_BRR_BIT)

#endif
