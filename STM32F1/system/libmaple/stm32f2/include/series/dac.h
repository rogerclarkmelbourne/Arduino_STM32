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
 * @file libmaple/stm32f2/include/series/dac.h
 * @brief STM32F2 DAC support
 */

#ifndef _LIBMAPLE_STM32F2_DAC_H_
#define _LIBMAPLE_STM32F2_DAC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map type
 */

/** STM32F2 DAC register map type. */
typedef struct dac_reg_map {
    __IO uint32 CR;      /**< Control register */
    __IO uint32 SWTRIGR; /**< Software trigger register */
    __IO uint32 DHR12R1; /**< Channel 1 12-bit right-aligned data
                              holding register */
    __IO uint32 DHR12L1; /**< Channel 1 12-bit left-aligned data
                              holding register */
    __IO uint32 DHR8R1;  /**< Channel 1 8-bit left-aligned data
                              holding register */
    __IO uint32 DHR12R2; /**< Channel 2 12-bit right-aligned data
                              holding register */
    __IO uint32 DHR12L2; /**< Channel 2 12-bit left-aligned data
                              holding register */
    __IO uint32 DHR8R2;  /**< Channel 2 8-bit left-aligned data
                              holding register */
    __IO uint32 DHR12RD; /**< Dual DAC 12-bit right-aligned data
                              holding register */
    __IO uint32 DHR12LD; /**< Dual DAC 12-bit left-aligned data
                              holding register */
    __IO uint32 DHR8RD;  /**< Dual DAC 8-bit right-aligned data holding
                              register */
    __IO uint32 DOR1;    /**< Channel 1 data output register */
    __IO uint32 DOR2;    /**< Channel 2 data output register */
    __IO uint32 SR;      /**< Status register */
} dac_reg_map;

/*
 * Register bit definitions
 */

/* Control register */

#define DAC_CR_DMAUDRIE1            (1U << 13) /* Channel 1 DMA underrun
                                                * interrupt enable */
#define DAC_CR_DMAUDRIE2            (1U << 29) /* Channel 2 DMA underrun
                                                * interrupt enable */

/* Status register */

#define DAC_SR_DMAUDR1              (1U << 13) /* Channel 1 DMA underrun
                                                * occurred */
#define DAC_SR_DMAUDR2              (1U << 29) /* Channel 2 DMA underrun
                                                * ocurred */

#ifdef __cplusplus
}
#endif

#endif
