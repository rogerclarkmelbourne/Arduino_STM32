/******************************************************************************
 * The MIT License
 *
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
 * @file dac.h
 * @brief Digital to analog converter support.
 */

/* See notes/dac.txt for more info */

#ifndef _DAC_H_
#define _DAC_H_

#include "rcc.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Register maps
 */

/** DAC register map. */
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

/** DAC register map base address */
#define DAC_BASE                ((struct dac_reg_map*)0x40007400)

/*
 * Devices
 */

/** DAC device type. */
typedef struct dac_dev_t {
    dac_reg_map *regs; /**< Register map */
} dac_dev_t;
extern dac_dev_t const dac_dev;
#define DAC (&dac_dev)

/*
 * Register bit definitions
 */

/* Control register */
#define DAC_CR_TSEL1_SHIFT               3
#define DAC_CR_WAVE1_SHIFT               6
#define DAC_CR_MAMP1_SHIFT               8
#define DAC_CR_TSEL2_SHIFT              19
#define DAC_CR_WAVE2_SHIFT              22
#define DAC_CR_MAMP2_SHIFT              24
#define DAC_CR_TSEL_MASK               0x7
#define DAC_CR_WAVE_MASK               0x3
#define DAC_CR_MAMP_MASK               0xF
/* Channel 1 control */
#define DAC_CR_EN1                   BIT(0) /* Enable */
#define DAC_CR_BOFF1                 BIT(1) /* Output buffer disable */
#define DAC_CR_TEN1                  BIT(2) /* Trigger enable */
#define DAC_CR_TSEL1             (0x7 << 3) /* Trigger selection */
#define DAC_CR_WAVE1             (0x3 << 6) /* Noise/triangle wave enable */
#define DAC_CR_MAMP1             (0xF << 8) /* Mask/amplitude selector */
#define DAC_CR_DMAEN1               BIT(12) /* DMA enable */
#define DAC_CR_DMAUDRIE1            BIT(13) /* DMA underrun interrupt enable */
/* Channel 2 control */
#define DAC_CR_EN2                  BIT(16) /* Enable */
#define DAC_CR_BOFF2                BIT(17) /* Output buffer disable */
#define DAC_CR_TEN2                 BIT(18) /* Trigger enable */
#define DAC_CR_TSEL2            (0x7 << 19) /* Trigger selection */
#define DAC_CR_WAVE2            (0x3 << 22) /* Noise/triangle wave generation*/
#define DAC_CR_MAMP2            (0xF << 24) /* Mask/amplitude selector */
#define DAC_CR_DMAEN2               BIT(28) /* DMA enable */
#define DAC_CR_DMAUDRIE2            BIT(29) /* DMA underrun interrupt enable */

typedef enum dac_wave_t {
DAC_WAVE_DISABLED = 0,
DAC_WAVE_NOISE    = 1,
DAC_WAVE_TRIANGLE = 2,
} dac_wave_t;

typedef enum dac_trigger_t {
DAC_TRG_TIMER6TRGO = 0,
DAC_TRG_TIMER8TRGO = 1,
DAC_TRG_TIMER7TRGO = 2,
DAC_TRG_TIMER5TRGO = 3,
DAC_TRG_TIMER2TRGO = 4,
DAC_TRG_TIMER4TRGO = 5,
DAC_TRG_EXT9       = 6,
DAC_TRG_SW         = 7,
} dac_trigger_t;

/* Software trigger register */
#define DAC_SWTRIGR_SWTRIG1          BIT(0) /* Channel 1 software trigger */
#define DAC_SWTRIGR_SWTRIG2          BIT(1) /* Channel 2 software trigger */

// 12-bit right-aligned data mask
#define DAC_DHR12R_MASK     0x00000FFF

// 12-bit left-aligned data mask
#define DAC_DHR12L_MASK     0x0000FFF0

// 8-bit right-aligned data mask
#define DAC_DHR8R_MASK      0x000000FF

// Dual DAC 12-bit right-aligned data masks
#define DAC_DHR12RD_CH1_MASK     0x00000FFF
#define DAC_DHR12RD_CH2_MASK     0x0FFF0000

// Dual DAC 12-bit left-aligned data masks
#define DAC_DHR12LD_CH1_MASK     0x0000FFF0
#define DAC_DHR12LD_CH2_MASK     0xFFF00000

// Dual DAC 8-bit right-aligned data masks
#define DAC_DHR8RD_CH1_MASK      0x000000FF
#define DAC_DHR8RD_CH2_MASK      0x0000FF00

// Data output register
#define DAC_DOR_DACCDOR          0x00000FFF


/*
 * Convenience functions
 */

/* We take the dev argument in these convenience functions for
 * future-proofing */
typedef enum {
DAC_CH1 = 1,
DAC_CH2 = 2,
} dac_channel_t;

extern void dac_init();
extern void dac_write_channel(dac_channel_t channel, uint16 val);
extern void dac_set_mask_amplitude(dac_channel_t channel, uint16 val);
extern void dac_set_wave(dac_channel_t channel, dac_wave_t val);
extern void dac_set_trigger(dac_channel_t channel, dac_trigger_t val);
extern void dac_sw_trigger(dac_channel_t channel);
extern void dac_enable_buffer(dac_channel_t channel);
extern void dac_disable_buffer(dac_channel_t channel);
extern void dac_enable_dma(dac_channel_t channel);
extern void dac_disable_dma(dac_channel_t channel);
extern void dac_enable(dac_channel_t channel);
extern void dac_disable(dac_channel_t channel);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
