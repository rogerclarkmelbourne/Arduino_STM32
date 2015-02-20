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
 * @file libmaple/stm32f3/include/series/dac.h
 * @brief STM32F3 DAC support
 */

#ifndef _LIBMAPLE_STM32F3_DAC_H_
#define _LIBMAPLE_STM32F3_DAC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/** STM32F3 DAC register map type. */
typedef struct dac_reg_map {
    __io uint32 CR;      /**< Control register */
    __io uint32 SWTRIGR; /**< Software trigger register */
    __io uint32 DHR12R1; /**< Channel 1 12-bit right-aligned data
                              holding register */
    __io uint32 DHR12L1; /**< Channel 1 12-bit left-aligned data
                              holding register */
    __io uint32 DHR8R1;  /**< Channel 1 8-bit left-aligned data
                              holding register */
    __io uint32 DHR12R2; /**< Channel 2 12-bit right-aligned data
                              holding register */
    __io uint32 DHR12L2; /**< Channel 2 12-bit left-aligned data
                              holding register */
    __io uint32 DHR8R2;  /**< Channel 2 8-bit left-aligned data
                              holding register */
    __io uint32 DHR12RD; /**< Dual DAC 12-bit right-aligned data
                              holding register */
    __io uint32 DHR12LD; /**< Dual DAC 12-bit left-aligned data
                              holding register */
    __io uint32 DHR8RD;  /**< Dual DAC 8-bit right-aligned data holding
                              register */
    __io uint32 DOR1;    /**< Channel 1 data output register */
    __io uint32 DOR2;    /**< Channel 2 data output register */
} dac_reg_map;

//#define DAC1_BASE				((struct dac_reg_map*)0x40007400)
//#define DAC2_BASE				((struct dac_reg_map*)0x40009800) //TODO STM32F37/38xx

#ifdef __cplusplus
}
#endif

#endif
