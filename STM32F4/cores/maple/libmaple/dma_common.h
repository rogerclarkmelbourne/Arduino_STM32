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
 * @file libmaple/include/libmaple/dma_common.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief Common DMA sub-header for <series/dma.h> and <libmaple/dma.h>.
 *
 * CONTENTS UNSTABLE. The existence of this file is an implementation
 * detail.  Never include it directly.  If you need something from
 * here, include <libmaple/dma.h> instead.
 */

/*
 * There's a fair amount of common DMA functionality needed by each
 * <series/dma.h> and <libmaple/dma.h>.  This header exists in order
 * to provide it to both, avoiding some hacks and circular
 * dependencies.
 */

#ifndef _LIBMAPLE_DMA_COMMON_H_
#define _LIBMAPLE_DMA_COMMON_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/nvic.h>
#include <libmaple/rcc.h>

/*
 * Devices
 */

struct dma_reg_map;

/* Encapsulates state related to user interrupt handlers. You
 * shouldn't touch these directly; use dma_attach_interrupt() and
 * dma_detach_interupt() instead. */
//typedef struct dma_handler_config {
//    void (*handler)(void);     /* User handler */
//    nvic_irq_num irq_line;     /* IRQ line for interrupt */
//} dma_handler_config;

/** DMA device type */
//typedef struct dma_dev {
//    struct dma_reg_map        *regs;       /**< Register map */
//    rcc_clk_id                 clk_id;     /**< Clock ID */
//    struct dma_handler_config  handlers[]; /**< For internal use */
//} dma_dev;

/**
 * @brief DMA channels
 *
 * Notes:
 * - This is also the dma_tube type for STM32F1.
 * - Channel 0 is not available on all STM32 series.
 *
 * @see dma_tube
 */
typedef enum dma_channel {
    DMA_CH0 = 0,                /**< Channel 0 */
    DMA_CH1 = 1,                /**< Channel 1 */
    DMA_CH2 = 2,                /**< Channel 2 */
    DMA_CH3 = 3,                /**< Channel 3 */
    DMA_CH4 = 4,                /**< Channel 4 */
    DMA_CH5 = 5,                /**< Channel 5 */
    DMA_CH6 = 6,                /**< Channel 6 */
    DMA_CH7 = 7,                /**< Channel 7 */
} dma_channel;

/**
 * @brief Source and destination transfer sizes.
 * Use these when initializing a struct dma_tube_config.
 * @see struct dma_tube_config
 * @see dma_tube_cfg
 */
//typedef enum dma_xfer_size {
//    DMA_SIZE_8BITS  = 0,        /**< 8-bit transfers */
//    DMA_SIZE_16BITS = 1,        /**< 16-bit transfers */
//    DMA_SIZE_32BITS = 2,        /**< 32-bit transfers */
//} dma_xfer_size;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
