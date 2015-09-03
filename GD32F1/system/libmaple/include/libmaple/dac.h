/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
 * Copyright (c) 2010 Bryan Newbold.
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
 * @file libmaple/include/libmaple/dac.h
 * @brief Digital to analog converter support.
 */

/* See notes/dac.txt for more info */

#ifndef _LIBMAPLE_DAC_H_
#define _LIBMAPLE_DAC_H_

#ifdef __cplusplus
extern "C"{
#endif

 /* Roger clark. Replaced with line below #include <series/dac.h>*/
#include "stm32f1/include/series/dac.h"
#include <libmaple/libmaple_types.h>
#include <libmaple/rcc.h>
#include <libmaple/stm32.h>

/*
 * Register map base and device pointers.
 *
 * The DACs are the same on all supported targets, so it's not worth
 * repeating these in the series headers.
 */

#define DAC_BASE                ((struct dac_reg_map*)0x40007400)

/** DAC device type. */
typedef struct dac_dev {
    dac_reg_map *regs; /**< Register map */
} dac_dev;

#if STM32_HAVE_DAC
extern const dac_dev *DAC;
#endif

/*
 * Register bit definitions
 */

/* Control register */

/* Channel 1 control */
#define DAC_CR_EN1                  (1U << 0)  /* Enable */
#define DAC_CR_BOFF1                (1U << 1)  /* Output buffer disable */
#define DAC_CR_TEN1                 (1U << 2)  /* Trigger enable */
#define DAC_CR_TSEL1                (0x7 << 3) /* Trigger selection */
#define DAC_CR_WAVE1                (0x3 << 6) /* Noise/triangle wave */
#define DAC_CR_MAMP1                (0xF << 8) /* Mask/amplitude selector */
#define DAC_CR_DMAEN1               (1U << 12) /* DMA enable */
/* Channel 2 control */
#define DAC_CR_EN2                  (1U << 16)  /* Enable */
#define DAC_CR_BOFF2                (1U << 17)  /* Output buffer disable */
#define DAC_CR_TEN2                 (1U << 18)  /* Trigger enable */
#define DAC_CR_TSEL2                (0x7 << 19) /* Trigger selection */
#define DAC_CR_WAVE2                (0x3 << 22) /* Noise/triangle wave */
#define DAC_CR_MAMP2                (0xF << 24) /* Mask/amplitude selector */
#define DAC_CR_DMAEN2               (1U << 28)  /* DMA enable */

/* Software trigger register */

#define DAC_SWTRIGR_SWTRIG1         (1U << 0) /* Channel 1 software trigger */
#define DAC_SWTRIGR_SWTRIG2         (1U << 1) /* Channel 2 software trigger */

/* Channel 1 12-bit right-aligned data holding register */

#define DAC_DHR12R1_DACC1DHR        0x00000FFF

/* Channel 1 12-bit left-aligned data holding register */

#define DAC_DHR12L1_DACC1DHR        0x0000FFF0

/* Channel 1 8-bit left-aligned data holding register */

#define DAC_DHR8R1_DACC1DHR         0x000000FF

/* Channel 2 12-bit right-aligned data holding register */

#define DAC_DHR12R2_DACC2DHR        0x00000FFF

/* Channel 2 12-bit left-aligned data holding register */

#define DAC_DHR12L2_DACC2DHR        0x0000FFF0

/* Channel 2 8-bit left-aligned data holding register */

#define DAC_DHR8R2_DACC2DHR         0x000000FF

/* Dual DAC 12-bit right-aligned data holding register */

#define DAC_DHR12RD_DACC1DHR        0x00000FFF
#define DAC_DHR12RD_DACC2DHR        0x0FFF0000

/* Dual DAC 12-bit left-aligned data holding register */

#define DAC_DHR12LD_DACC1DHR        0x0000FFF0
#define DAC_DHR12LD_DACC2DHR        0xFFF00000

/* Dual DAC 8-bit left-aligned data holding register */

#define DAC_DHR8RD_DACC1DHR         0x000000FF
#define DAC_DHR8RD_DACC2DHR         0x0000FF00

/* Channel 1 data output register */

#define DAC_DOR1_DACC1DOR           0x00000FFF

/* Channel 1 data output register */

#define DAC_DOR2_DACC2DOR           0x00000FFF

/*
 * Routines
 */

/* We take the dev argument in these for future-proofing */

#define DAC_CH1 0x1
#define DAC_CH2 0x2
void dac_init(const dac_dev *dev, uint32 flags);

void dac_write_channel(const dac_dev *dev, uint8 channel, uint16 val);
void dac_enable_channel(const dac_dev *dev, uint8 channel);
void dac_disable_channel(const dac_dev *dev, uint8 channel);

#define dac_write_channel1(val) ( DAC->regs->DHR12R1 = DAC_DHR12R1_DACC1DHR & val )
#define dac_write_channel2(val) ( DAC->regs->DHR12R2 = DAC_DHR12R2_DACC2DHR & val )

#ifdef __cplusplus
} // extern "C"
#endif

#endif
