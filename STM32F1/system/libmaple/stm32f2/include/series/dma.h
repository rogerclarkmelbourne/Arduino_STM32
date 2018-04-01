/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC
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
 * @file libmaple/stm32f2/include/series/dma.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 DMA series header
 */

#ifndef _LIBMAPLE_STM32F2_DMA_H_
#define _LIBMAPLE_STM32F2_DMA_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/dma_common.h>
#include <libmaple/libmaple_types.h>

/*
 * Register map and base pointers
 */

/**
 * @brief STM32F2 DMA register map type.
 */
typedef struct dma_reg_map {
    /* Isn't it nice how on F1, it's CCR1, but on F2, it's S1CR? */

    /* Global DMA registers */
    __IO uint32 LISR;   /**< Low interrupt status register */
    __IO uint32 HISR;   /**< High interrupt status register */
    __IO uint32 LIFCR;  /**< Low interrupt flag clear register */
    __IO uint32 HIFCR;  /**< High interrupt flag clear register */
    /* Stream 0 registers */
    __IO uint32 S0CR;   /**< Stream 0 control register */
    __IO uint32 S0NDTR; /**< Stream 0 number of data register */
    __IO uint32 S0PAR;  /**< Stream 0 peripheral address register */
    __IO uint32 S0M0AR; /**< Stream 0 memory 0 address register */
    __IO uint32 S0M1AR; /**< Stream 0 memory 1 address register */
    __IO uint32 S0FCR;  /**< Stream 0 FIFO control register */
    /* Stream 1 registers */
    __IO uint32 S1CR;   /**< Stream 1 control register */
    __IO uint32 S1NDTR; /**< Stream 1 number of data register */
    __IO uint32 S1PAR;  /**< Stream 1 peripheral address register */
    __IO uint32 S1M0AR; /**< Stream 1 memory 0 address register */
    __IO uint32 S1M1AR; /**< Stream 1 memory 1 address register */
    __IO uint32 S1FCR;  /**< Stream 1 FIFO control register */
    /* Stream 2 registers */
    __IO uint32 S2CR;   /**< Stream 2 control register */
    __IO uint32 S2NDTR; /**< Stream 2 number of data register */
    __IO uint32 S2PAR;  /**< Stream 2 peripheral address register */
    __IO uint32 S2M0AR; /**< Stream 2 memory 0 address register */
    __IO uint32 S2M1AR; /**< Stream 2 memory 1 address register */
    __IO uint32 S2FCR;  /**< Stream 2 FIFO control register */
    /* Stream 3 registers */
    __IO uint32 S3CR;   /**< Stream 3 control register */
    __IO uint32 S3NDTR; /**< Stream 3 number of data register */
    __IO uint32 S3PAR;  /**< Stream 3 peripheral address register */
    __IO uint32 S3M0AR; /**< Stream 3 memory 0 address register */
    __IO uint32 S3M1AR; /**< Stream 3 memory 1 address register */
    __IO uint32 S3FCR;  /**< Stream 3 FIFO control register */
    /* Stream 4 registers */
    __IO uint32 S4CR;   /**< Stream 4 control register */
    __IO uint32 S4NDTR; /**< Stream 4 number of data register */
    __IO uint32 S4PAR;  /**< Stream 4 peripheral address register */
    __IO uint32 S4M0AR; /**< Stream 4 memory 0 address register */
    __IO uint32 S4M1AR; /**< Stream 4 memory 1 address register */
    __IO uint32 S4FCR;  /**< Stream 4 FIFO control register */
    /* Stream 5 registers */
    __IO uint32 S5CR;   /**< Stream 5 control register */
    __IO uint32 S5NDTR; /**< Stream 5 number of data register */
    __IO uint32 S5PAR;  /**< Stream 5 peripheral address register */
    __IO uint32 S5M0AR; /**< Stream 5 memory 0 address register */
    __IO uint32 S5M1AR; /**< Stream 5 memory 1 address register */
    __IO uint32 S5FCR;  /**< Stream 5 FIFO control register */
    /* Stream 6 registers */
    __IO uint32 S6CR;   /**< Stream 6 control register */
    __IO uint32 S6NDTR; /**< Stream 6 number of data register */
    __IO uint32 S6PAR;  /**< Stream 6 peripheral address register */
    __IO uint32 S6M0AR; /**< Stream 6 memory 0 address register */
    __IO uint32 S6M1AR; /**< Stream 6 memory 1 address register */
    __IO uint32 S6FCR;  /**< Stream 6 FIFO control register */
    /* Stream 7 registers */
    __IO uint32 S7CR;   /**< Stream 7 control register */
    __IO uint32 S7NDTR; /**< Stream 7 number of data register */
    __IO uint32 S7PAR;  /**< Stream 7 peripheral address register */
    __IO uint32 S7M0AR; /**< Stream 7 memory 0 address register */
    __IO uint32 S7M1AR; /**< Stream 7 memory 1 address register */
    __IO uint32 S7FCR;  /**< Stream 7 FIFO control register */
} dma_reg_map;

/** DMA controller 1 register map base pointer */
#define DMA1_BASE                       ((struct dma_reg_map*)0x40026000)
/** DMA controller 2 register map base pointer */
#define DMA2_BASE                       ((struct dma_reg_map*)0x40026400)

/**
 * @brief STM32F2 DMA stream (i.e. tube) register map type.
 * Provides access to an individual stream's registers.
 * @see dma_tube_regs()
 */
typedef struct dma_tube_reg_map {
    __IO uint32 SCR;   /**< Stream configuration register */
    __IO uint32 SNDTR; /**< Stream number of data register */
    __IO uint32 SPAR;  /**< Stream peripheral address register */
    __IO uint32 SM0AR; /**< Stream memory 0 address register */
    __IO uint32 SM1AR; /**< Stream memory 1 address register */
    __IO uint32 SFCR;  /**< Stream FIFO control register */
} dma_tube_reg_map;

/** DMA1 stream 0 register map base pointer */
#define DMA1S0_BASE ((struct dma_tube_reg_map*)0x40026010)
/** DMA1 stream 1 register map base pointer */
#define DMA1S1_BASE ((struct dma_tube_reg_map*)0x40026028)
/** DMA1 stream 2 register map base pointer */
#define DMA1S2_BASE ((struct dma_tube_reg_map*)0x40026040)
/** DMA1 stream 3 register map base pointer */
#define DMA1S3_BASE ((struct dma_tube_reg_map*)0x40026058)
/** DMA1 stream 4 register map base pointer */
#define DMA1S4_BASE ((struct dma_tube_reg_map*)0x40026070)
/** DMA1 stream 5 register map base pointer */
#define DMA1S5_BASE ((struct dma_tube_reg_map*)0x40026088)
/** DMA1 stream 6 register map base pointer */
#define DMA1S6_BASE ((struct dma_tube_reg_map*)0x400260A0)
/** DMA1 stream 7 register map base pointer */
#define DMA1S7_BASE ((struct dma_tube_reg_map*)0x400260B8)

/** DMA2 stream 0 register map base pointer */
#define DMA2S0_BASE ((struct dma_tube_reg_map*)0x40026410)
/** DMA2 stream 1 register map base pointer */
#define DMA2S1_BASE ((struct dma_tube_reg_map*)0x40026028)
/** DMA2 stream 2 register map base pointer */
#define DMA2S2_BASE ((struct dma_tube_reg_map*)0x40026040)
/** DMA2 stream 3 register map base pointer */
#define DMA2S3_BASE ((struct dma_tube_reg_map*)0x40026058)
/** DMA2 stream 4 register map base pointer */
#define DMA2S4_BASE ((struct dma_tube_reg_map*)0x40026070)
/** DMA2 stream 5 register map base pointer */
#define DMA2S5_BASE ((struct dma_tube_reg_map*)0x40026088)
/** DMA2 stream 6 register map base pointer */
#define DMA2S6_BASE ((struct dma_tube_reg_map*)0x400260A0)
/** DMA2 stream 7 register map base pointer */
#define DMA2S7_BASE ((struct dma_tube_reg_map*)0x400260B8)

/*
 * Register bit definitions
 */

/* Low interrupt status register */

#define DMA_LISR_TCIF3_BIT           27
#define DMA_LISR_HTIF3_BIT           26
#define DMA_LISR_TEIF3_BIT           25
#define DMA_LISR_DMEIF3_BIT          24
#define DMA_LISR_FEIF3_BIT           22
#define DMA_LISR_TCIF2_BIT           21
#define DMA_LISR_HTIF2_BIT           20
#define DMA_LISR_TEIF2_BIT           19
#define DMA_LISR_DMEIF2_BIT          18
#define DMA_LISR_FEIF2_BIT           16
#define DMA_LISR_TCIF1_BIT           11
#define DMA_LISR_HTIF1_BIT           10
#define DMA_LISR_TEIF1_BIT            9
#define DMA_LISR_DMEIF1_BIT           8
#define DMA_LISR_FEIF1_BIT            6
#define DMA_LISR_TCIF0_BIT            5
#define DMA_LISR_HTIF0_BIT            4
#define DMA_LISR_TEIF0_BIT            3
#define DMA_LISR_DMEIF0_BIT           2
#define DMA_LISR_FEIF0_BIT            0

#define DMA_LISR_TCIF3               (1U << DMA_LISR_TCIF3_BIT)
#define DMA_LISR_HTIF3               (1U << DMA_LISR_HTIF3_BIT)
#define DMA_LISR_TEIF3               (1U << DMA_LISR_TEIF3_BIT)
#define DMA_LISR_DMEIF3              (1U << DMA_LISR_DMEIF3_BIT)
#define DMA_LISR_FEIF3               (1U << DMA_LISR_FEIF3_BIT)
#define DMA_LISR_TCIF2               (1U << DMA_LISR_TCIF2_BIT)
#define DMA_LISR_HTIF2               (1U << DMA_LISR_HTIF2_BIT)
#define DMA_LISR_TEIF2               (1U << DMA_LISR_TEIF2_BIT)
#define DMA_LISR_DMEIF2              (1U << DMA_LISR_DMEIF2_BIT)
#define DMA_LISR_FEIF2               (1U << DMA_LISR_FEIF2_BIT)
#define DMA_LISR_TCIF1               (1U << DMA_LISR_TCIF1_BIT)
#define DMA_LISR_HTIF1               (1U << DMA_LISR_HTIF1_BIT)
#define DMA_LISR_TEIF1               (1U << DMA_LISR_TEIF1_BIT)
#define DMA_LISR_DMEIF1              (1U << DMA_LISR_DMEIF1_BIT)
#define DMA_LISR_FEIF1               (1U << DMA_LISR_FEIF1_BIT)
#define DMA_LISR_TCIF0               (1U << DMA_LISR_TCIF0_BIT)
#define DMA_LISR_HTIF0               (1U << DMA_LISR_HTIF0_BIT)
#define DMA_LISR_TEIF0               (1U << DMA_LISR_TEIF0_BIT)
#define DMA_LISR_DMEIF0              (1U << DMA_LISR_DMEIF0_BIT)
#define DMA_LISR_FEIF0               (1U << DMA_LISR_FEIF0_BIT)

/* High interrupt status register */

#define DMA_HISR_TCIF7_BIT           27
#define DMA_HISR_HTIF7_BIT           26
#define DMA_HISR_TEIF7_BIT           25
#define DMA_HISR_DMEIF7_BIT          24
#define DMA_HISR_FEIF7_BIT           22
#define DMA_HISR_TCIF6_BIT           21
#define DMA_HISR_HTIF6_BIT           20
#define DMA_HISR_TEIF6_BIT           19
#define DMA_HISR_DMEIF6_BIT          18
#define DMA_HISR_FEIF6_BIT           16
#define DMA_HISR_TCIF5_BIT           11
#define DMA_HISR_HTIF5_BIT           10
#define DMA_HISR_TEIF5_BIT            9
#define DMA_HISR_DMEIF5_BIT           8
#define DMA_HISR_FEIF5_BIT            6
#define DMA_HISR_TCIF4_BIT            5
#define DMA_HISR_HTIF4_BIT            4
#define DMA_HISR_TEIF4_BIT            3
#define DMA_HISR_DMEIF4_BIT           2
#define DMA_HISR_FEIF4_BIT            0

#define DMA_HISR_TCIF7               (1U << DMA_HISR_TCIF7_BIT)
#define DMA_HISR_HTIF7               (1U << DMA_HISR_HTIF7_BIT)
#define DMA_HISR_TEIF7               (1U << DMA_HISR_TEIF7_BIT)
#define DMA_HISR_DMEIF7              (1U << DMA_HISR_DMEIF7_BIT)
#define DMA_HISR_FEIF7               (1U << DMA_HISR_FEIF7_BIT)
#define DMA_HISR_TCIF6               (1U << DMA_HISR_TCIF6_BIT)
#define DMA_HISR_HTIF6               (1U << DMA_HISR_HTIF6_BIT)
#define DMA_HISR_TEIF6               (1U << DMA_HISR_TEIF6_BIT)
#define DMA_HISR_DMEIF6              (1U << DMA_HISR_DMEIF6_BIT)
#define DMA_HISR_FEIF6               (1U << DMA_HISR_FEIF6_BIT)
#define DMA_HISR_TCIF5               (1U << DMA_HISR_TCIF5_BIT)
#define DMA_HISR_HTIF5               (1U << DMA_HISR_HTIF5_BIT)
#define DMA_HISR_TEIF5               (1U << DMA_HISR_TEIF5_BIT)
#define DMA_HISR_DMEIF5              (1U << DMA_HISR_DMEIF5_BIT)
#define DMA_HISR_FEIF5               (1U << DMA_HISR_FEIF5_BIT)
#define DMA_HISR_TCIF4               (1U << DMA_HISR_TCIF4_BIT)
#define DMA_HISR_HTIF4               (1U << DMA_HISR_HTIF4_BIT)
#define DMA_HISR_TEIF4               (1U << DMA_HISR_TEIF4_BIT)
#define DMA_HISR_DMEIF4              (1U << DMA_HISR_DMEIF4_BIT)
#define DMA_HISR_FEIF4               (1U << DMA_HISR_FEIF4_BIT)

/* Low interrupt flag clear register */

#define DMA_LIFCR_CTCIF3_BIT             27
#define DMA_LIFCR_CHTIF3_BIT             26
#define DMA_LIFCR_CTEIF3_BIT             25
#define DMA_LIFCR_CDMEIF3_BIT            24
#define DMA_LIFCR_CFEIF3_BIT             22
#define DMA_LIFCR_CTCIF2_BIT             21
#define DMA_LIFCR_CHTIF2_BIT             20
#define DMA_LIFCR_CTEIF2_BIT             19
#define DMA_LIFCR_CDMEIF2_BIT            18
#define DMA_LIFCR_CFEIF2_BIT             16
#define DMA_LIFCR_CTCIF1_BIT             11
#define DMA_LIFCR_CHTIF1_BIT             10
#define DMA_LIFCR_CTEIF1_BIT              9
#define DMA_LIFCR_CDMEIF1_BIT             8
#define DMA_LIFCR_CFEIF1_BIT              6
#define DMA_LIFCR_CTCIF0_BIT              5
#define DMA_LIFCR_CHTIF0_BIT              4
#define DMA_LIFCR_CTEIF0_BIT              3
#define DMA_LIFCR_CDMEIF0_BIT             2
#define DMA_LIFCR_CFEIF0_BIT              0

#define DMA_LIFCR_CTCIF3                 (1U << DMA_LIFCR_CTCIF3_BIT)
#define DMA_LIFCR_CHTIF3                 (1U << DMA_LIFCR_CHTIF3_BIT)
#define DMA_LIFCR_CTEIF3                 (1U << DMA_LIFCR_CTEIF3_BIT)
#define DMA_LIFCR_CDMEIF3                (1U << DMA_LIFCR_CDMEIF3_BIT)
#define DMA_LIFCR_CFEIF3                 (1U << DMA_LIFCR_CFEIF3_BIT)
#define DMA_LIFCR_CTCIF2                 (1U << DMA_LIFCR_CTCIF2_BIT)
#define DMA_LIFCR_CHTIF2                 (1U << DMA_LIFCR_CHTIF2_BIT)
#define DMA_LIFCR_CTEIF2                 (1U << DMA_LIFCR_CTEIF2_BIT)
#define DMA_LIFCR_CDMEIF2                (1U << DMA_LIFCR_CDMEIF2_BIT)
#define DMA_LIFCR_CFEIF2                 (1U << DMA_LIFCR_CFEIF2_BIT)
#define DMA_LIFCR_CTCIF1                 (1U << DMA_LIFCR_CTCIF1_BIT)
#define DMA_LIFCR_CHTIF1                 (1U << DMA_LIFCR_CHTIF1_BIT)
#define DMA_LIFCR_CTEIF1                 (1U << DMA_LIFCR_CTEIF1_BIT)
#define DMA_LIFCR_CDMEIF1                (1U << DMA_LIFCR_CDMEIF1_BIT)
#define DMA_LIFCR_CFEIF1                 (1U << DMA_LIFCR_CFEIF1_BIT)
#define DMA_LIFCR_CTCIF0                 (1U << DMA_LIFCR_CTCIF0_BIT)
#define DMA_LIFCR_CHTIF0                 (1U << DMA_LIFCR_CHTIF0_BIT)
#define DMA_LIFCR_CTEIF0                 (1U << DMA_LIFCR_CTEIF0_BIT)
#define DMA_LIFCR_CDMEIF0                (1U << DMA_LIFCR_CDMEIF0_BIT)
#define DMA_LIFCR_CFEIF0                 (1U << DMA_LIFCR_CFEIF0_BIT)

/* High interrupt flag clear regsister */

#define DMA_HIFCR_CTCIF7_BIT             27
#define DMA_HIFCR_CHTIF7_BIT             26
#define DMA_HIFCR_CTEIF7_BIT             25
#define DMA_HIFCR_CDMEIF7_BIT            24
#define DMA_HIFCR_CFEIF7_BIT             22
#define DMA_HIFCR_CTCIF6_BIT             21
#define DMA_HIFCR_CHTIF6_BIT             20
#define DMA_HIFCR_CTEIF6_BIT             19
#define DMA_HIFCR_CDMEIF6_BIT            18
#define DMA_HIFCR_CFEIF6_BIT             16
#define DMA_HIFCR_CTCIF5_BIT             11
#define DMA_HIFCR_CHTIF5_BIT             10
#define DMA_HIFCR_CTEIF5_BIT              9
#define DMA_HIFCR_CDMEIF5_BIT             8
#define DMA_HIFCR_CFEIF5_BIT              6
#define DMA_HIFCR_CTCIF4_BIT              5
#define DMA_HIFCR_CHTIF4_BIT              4
#define DMA_HIFCR_CTEIF4_BIT              3
#define DMA_HIFCR_CDMEIF4_BIT             2
#define DMA_HIFCR_CFEIF4_BIT              0

#define DMA_HIFCR_CTCIF7                 (1U << DMA_HIFCR_CTCIF7_BIT)
#define DMA_HIFCR_CHTIF7                 (1U << DMA_HIFCR_CHTIF7_BIT)
#define DMA_HIFCR_CTEIF7                 (1U << DMA_HIFCR_CTEIF7_BIT)
#define DMA_HIFCR_CDMEIF7                (1U << DMA_HIFCR_CDMEIF7_BIT)
#define DMA_HIFCR_CFEIF7                 (1U << DMA_HIFCR_CFEIF7_BIT)
#define DMA_HIFCR_CTCIF6                 (1U << DMA_HIFCR_CTCIF6_BIT)
#define DMA_HIFCR_CHTIF6                 (1U << DMA_HIFCR_CHTIF6_BIT)
#define DMA_HIFCR_CTEIF6                 (1U << DMA_HIFCR_CTEIF6_BIT)
#define DMA_HIFCR_CDMEIF6                (1U << DMA_HIFCR_CDMEIF6_BIT)
#define DMA_HIFCR_CFEIF6                 (1U << DMA_HIFCR_CFEIF6_BIT)
#define DMA_HIFCR_CTCIF5                 (1U << DMA_HIFCR_CTCIF5_BIT)
#define DMA_HIFCR_CHTIF5                 (1U << DMA_HIFCR_CHTIF5_BIT)
#define DMA_HIFCR_CTEIF5                 (1U << DMA_HIFCR_CTEIF5_BIT)
#define DMA_HIFCR_CDMEIF5                (1U << DMA_HIFCR_CDMEIF5_BIT)
#define DMA_HIFCR_CFEIF5                 (1U << DMA_HIFCR_CFEIF5_BIT)
#define DMA_HIFCR_CTCIF4                 (1U << DMA_HIFCR_CTCIF4_BIT)
#define DMA_HIFCR_CHTIF4                 (1U << DMA_HIFCR_CHTIF4_BIT)
#define DMA_HIFCR_CTEIF4                 (1U << DMA_HIFCR_CTEIF4_BIT)
#define DMA_HIFCR_CDMEIF4                (1U << DMA_HIFCR_CDMEIF4_BIT)
#define DMA_HIFCR_CFEIF4                 (1U << DMA_HIFCR_CFEIF4_BIT)

/* Stream configuration register */

#define DMA_SCR_CT_BIT                   19
#define DMA_SCR_DBM_BIT                  18
#define DMA_SCR_PINCOS_BIT               15
#define DMA_SCR_MINC_BIT                 10
#define DMA_SCR_PINC_BIT                  9
#define DMA_SCR_CIRC_BIT                  8
#define DMA_SCR_PFCTRL_BIT                5
#define DMA_SCR_TCIE_BIT                  4
#define DMA_SCR_HTIE_BIT                  3
#define DMA_SCR_TEIE_BIT                  2
#define DMA_SCR_DMEIE_BIT                 1
#define DMA_SCR_EN_BIT                    0

#define DMA_SCR_CHSEL                    (0x7 << 25)
#define DMA_SCR_CHSEL_CH_0               (0x0 << 25)
#define DMA_SCR_CHSEL_CH_1               (0x1 << 25)
#define DMA_SCR_CHSEL_CH_2               (0x2 << 25)
#define DMA_SCR_CHSEL_CH_3               (0x3 << 25)
#define DMA_SCR_CHSEL_CH_4               (0x4 << 25)
#define DMA_SCR_CHSEL_CH_5               (0x5 << 25)
#define DMA_SCR_CHSEL_CH_6               (0x6 << 25)
#define DMA_SCR_CHSEL_CH_7               (0x7 << 25)
#define DMA_SCR_MBURST                   (0x3 << 23)
#define DMA_SCR_MBURST_SINGLE            (0x0 << 23)
#define DMA_SCR_MBURST_INCR4             (0x1 << 23)
#define DMA_SCR_MBURST_INCR8             (0x2 << 23)
#define DMA_SCR_MBURST_INCR16            (0x3 << 23)
#define DMA_SCR_PBURST                   (0x3 << 21)
#define DMA_SCR_PBURST_SINGLE            (0x0 << 21)
#define DMA_SCR_PBURST_INCR4             (0x1 << 21)
#define DMA_SCR_PBURST_INCR8             (0x2 << 21)
#define DMA_SCR_PBURST_INCR16            (0x3 << 21)
#define DMA_SCR_CT                       (1U << DMA_SCR_CT_BIT)
#define DMA_SCR_DBM                      (1U << DMA_SCR_DBM_BIT)
#define DMA_SCR_PL                       (0x3 << 16)
#define DMA_SCR_PL_LOW                   (0x0 << 16)
#define DMA_SCR_PL_MEDIUM                (0x1 << 16)
#define DMA_SCR_PL_HIGH                  (0x2 << 16)
#define DMA_SCR_VERY_HIGH                (0x3 << 16)
#define DMA_SCR_PINCOS                   (1U << DMA_SCR_PINCOS_BIT)
#define DMA_SCR_MSIZE                    (0x3 << 13)
#define DMA_SCR_MSIZE_8BITS              (0x0 << 13)
#define DMA_SCR_MSIZE_16BITS             (0x1 << 13)
#define DMA_SCR_MSIZE_32BITS             (0x2 << 13)
#define DMA_SCR_PSIZE                    (0x3 << 11)
#define DMA_SCR_PSIZE_8BITS              (0x0 << 11)
#define DMA_SCR_PSIZE_16BITS             (0x1 << 11)
#define DMA_SCR_PSIZE_32BITS             (0x2 << 11)
#define DMA_SCR_MINC                     (1U << DMA_SCR_MINC_BIT)
#define DMA_SCR_PINC                     (1U << DMA_SCR_PINC_BIT)
#define DMA_SCR_CIRC                     (1U << DMA_SCR_CIRC_BIT)
#define DMA_SCR_DIR                      (0x3 << 6)
#define DMA_SCR_DIR_PER_TO_MEM           (0x0 << 6)
#define DMA_SCR_DIR_MEM_TO_PER           (0x1 << 6)
#define DMA_SCR_DIR_MEM_TO_MEM           (0x2 << 6)
#define DMA_SCR_PFCTRL                   (1U << DMA_SCR_PFCTRL_BIT)
#define DMA_SCR_TCIE                     (1U << DMA_SCR_TCIE_BIT)
#define DMA_SCR_HTIE                     (1U << DMA_SCR_HTIE_BIT)
#define DMA_SCR_TEIE                     (1U << DMA_SCR_TEIE_BIT)
#define DMA_SCR_DMEIE                    (1U << DMA_SCR_DMEIE_BIT)
#define DMA_SCR_EN                       (1U << DMA_SCR_EN_BIT)

/* Stream FIFO control register */

#define DMA_SFCR_FEIE_BIT                            7
#define DMA_SFCR_DMDIS_BIT                           2

#define DMA_SFCR_FEIE                                (1U << DMA_SFCR_FEIE_BIT)
#define DMA_SFCR_FS                                  (0x7 << 3)
#define DMA_SFCR_FS_ZERO_TO_QUARTER                  (0x0 << 3)
#define DMA_SFCR_FS_QUARTER_TO_HALF                  (0x1 << 3)
#define DMA_SFCR_FS_HALF_TO_THREE_QUARTERS           (0x2 << 3)
#define DMA_SFCR_FS_THREE_QUARTERS_TO_FULL           (0x3 << 3)
#define DMA_SFCR_FS_EMPTY                            (0x4 << 3)
#define DMA_SFCR_FS_FULL                             (0x5 << 3)
#define DMA_SFCR_DMDIS                               (1U << DMA_SFCR_DMDIS_BIT)
#define DMA_SFCR_FTH                                 (0x3 << 0)
#define DMA_SFCR_FTH_QUARTER_FULL                    (0x0 << 3)
#define DMA_SFCR_FTH_HALF_FULL                       (0x1 << 3)
#define DMA_SFCR_FTH_THREE_QUARTERS_FULL             (0x2 << 3)
#define DMA_SFCR_FTH_FULL                            (0x3 << 3)

/*
 * Devices
 */

extern dma_dev *DMA1;
extern dma_dev *DMA2;

/*
 * Other types needed by, or useful for, <libmaple/dma.h>
 */

/**
 * @brief DMA streams
 * This is also the dma_tube type for STM32F2.
 * @see dma_tube
 */
typedef enum dma_stream {
    DMA_S0 = 0,
    DMA_S1 = 1,
    DMA_S2 = 2,
    DMA_S3 = 3,
    DMA_S4 = 4,
    DMA_S5 = 5,
    DMA_S6 = 6,
    DMA_S7 = 7,
} dma_stream;

/** STM32F2 dma_tube (=dma_stream) */
#define dma_tube dma_stream

/**
 * @brief STM32F2 configuration flags for dma_tube_config.
 * @see struct dma_tube_config
 */
typedef enum dma_cfg_flags {
    /* NB: flags that aren't SCR bits are treated specially. */

    /**
     * Source address increment mode
     *
     * If this flag is set, the source address is incremented (by the
     * source size) after each DMA transfer.
     */
    DMA_CFG_SRC_INC = 1U << 31,

    /**
     * Destination address increment mode
     *
     * If this flag is set, the destination address is incremented (by
     * the destination size) after each DMA transfer.
     */
    DMA_CFG_DST_INC = 1U << 30,

    /**
     * Circular mode
     *
     * This mode is not available for memory-to-memory transfers.
     */
    DMA_CFG_CIRC = DMA_SCR_CIRC,

    /** Transfer complete interrupt enable */
    DMA_CFG_CMPLT_IE      = DMA_SCR_TCIE,
    /** Transfer half-complete interrupt enable  */
    DMA_CFG_HALF_CMPLT_IE = DMA_SCR_HTIE,
    /** Transfer error interrupt enable */
    DMA_CFG_ERR_IE        = DMA_SCR_TEIE,
    /** Direct mode error interrupt enable */
    DMA_CFG_DM_ERR_IE     = DMA_SCR_DMEIE,
    /** FIFO error interrupt enable  */
    DMA_CFG_FIFO_ERR_IE   = (1U << 29),
} dma_cfg_flags;

/**
 * @brief STM32F2 DMA request sources.
 *
 * IMPORTANT:
 *
 * 1. On STM32F2, a particular dma_request_src is always tied to a
 * single DMA controller, but often can be supported by multiple
 * streams. For example, DMA requests from ADC1 (DMA_REQ_SRC_ADC1) can
 * only be handled by DMA2, but they can go to either stream 0 or
 * stream 4 (though not any other stream). If you try to use a request
 * source with the wrong DMA controller or the wrong stream on
 * STM32F2, dma_tube_cfg() will fail.
 *
 * 2. A single stream can only handle a single request source at a
 * time. If you change a stream's request source later, it will stop
 * serving requests from the old source. However, for some streams,
 * some sources conflict with one another (when they correspond to the
 * same channel on that stream), and on STM32F2, Terrible Super-Bad
 * Things will happen if two conflicting request sources are active at
 * the same time.
 *
 * @see struct dma_tube_config
 * @see dma_tube_cfg()
 */
typedef enum dma_request_src {
    /* These are constructed like so (though this may change, so user
     * code shouldn't depend on it):
     *
     * Bits 0--2: Channel associated with request source
     *
     * Bits 3--9: rcc_clk_id of DMA controller associated with request source
     *
     * Bits 10--17: Bit mask of streams which can handle that request
     *              source.  (E.g., bit 10 set means stream 0 can
     *              handle the source, bit 11 set means stream 1 can,
     *              etc.)
     *
     * Among other things, this is used for error checking in
     * dma_tube_cfg(). If you change this bit encoding, you need to
     * update the helper functions in stm32f2/dma.c.
     */
#define _DMA_STM32F2_REQ_SRC(stream_mask, clk_id, channel) \
    (((stream_mask) << 10) | ((clk_id) << 3) | (channel))
#define _DMA_S(n) (1U << (n))

    /* DMA1 request sources */
#define _DMA_1_REQ_SRC(stream_mask, channel) \
    _DMA_STM32F2_REQ_SRC(stream_mask, RCC_DMA1, channel)

    /* Channel 0 */
    DMA_REQ_SRC_SPI3_RX = _DMA_1_REQ_SRC(_DMA_S(0) | _DMA_S(2), 0),
    DMA_REQ_SRC_SPI2_RX = _DMA_1_REQ_SRC(_DMA_S(3), 0),
    DMA_REQ_SRC_SPI2_TX = _DMA_1_REQ_SRC(_DMA_S(4), 0),
    DMA_REQ_SRC_SPI3_TX = _DMA_1_REQ_SRC(_DMA_S(5) | _DMA_S(7), 0),

    /* Channel 1 */
    DMA_REQ_SRC_I2C1_RX = _DMA_1_REQ_SRC(_DMA_S(0) | _DMA_S(5), 1),
    DMA_REQ_SRC_TIM7_UP = _DMA_1_REQ_SRC(_DMA_S(2) | _DMA_S(4), 1),
    DMA_REQ_SRC_I2C1_TX = _DMA_1_REQ_SRC(_DMA_S(6) | _DMA_S(7), 1),

    /* Channel 2 */
    DMA_REQ_SRC_TIM4_CH1 = _DMA_1_REQ_SRC(_DMA_S(0), 2),
    DMA_REQ_SRC_TIM4_CH2 = _DMA_1_REQ_SRC(_DMA_S(3), 2),
    DMA_REQ_SRC_TIM4_UP  = _DMA_1_REQ_SRC(_DMA_S(6), 2),
    DMA_REQ_SRC_TIM4_CH3 = _DMA_1_REQ_SRC(_DMA_S(7), 2),

    /* Channel 3 */
    DMA_REQ_SRC_TIM2_UP  = _DMA_1_REQ_SRC(_DMA_S(1) | _DMA_S(7), 3),
    DMA_REQ_SRC_TIM2_CH3 = _DMA_1_REQ_SRC(_DMA_S(1), 3),
    DMA_REQ_SRC_I2C3_RX  = _DMA_1_REQ_SRC(_DMA_S(2), 3),
    DMA_REQ_SRC_I2C3_TX  = _DMA_1_REQ_SRC(_DMA_S(4), 3),
    DMA_REQ_SRC_TIM2_CH1 = _DMA_1_REQ_SRC(_DMA_S(5), 3),
    DMA_REQ_SRC_TIM2_CH2 = _DMA_1_REQ_SRC(_DMA_S(6), 3),
    DMA_REQ_SRC_TIM2_CH4 = _DMA_1_REQ_SRC(_DMA_S(6) | _DMA_S(7), 3),

    /* Channel 4 */
    DMA_REQ_SRC_UART5_RX  = _DMA_1_REQ_SRC(_DMA_S(0), 4),
    DMA_REQ_SRC_USART3_RX = _DMA_1_REQ_SRC(_DMA_S(1), 4),
    DMA_REQ_SRC_UART4_RX  = _DMA_1_REQ_SRC(_DMA_S(2), 4),
    DMA_REQ_SRC_USART3_TX = _DMA_1_REQ_SRC(_DMA_S(3), 4),
    DMA_REQ_SRC_UART4_TX  = _DMA_1_REQ_SRC(_DMA_S(4), 4),
    DMA_REQ_SRC_USART2_RX = _DMA_1_REQ_SRC(_DMA_S(5), 4),
    DMA_REQ_SRC_USART2_TX = _DMA_1_REQ_SRC(_DMA_S(6), 4),
    DMA_REQ_SRC_UART5_TX  = _DMA_1_REQ_SRC(_DMA_S(7), 4),

    /* Channel 5 */
    DMA_REQ_SRC_TIM3_CH4  = _DMA_1_REQ_SRC(_DMA_S(2), 5),
    DMA_REQ_SRC_TIM3_UP   = _DMA_1_REQ_SRC(_DMA_S(2), 5),
    DMA_REQ_SRC_TIM3_CH1  = _DMA_1_REQ_SRC(_DMA_S(4), 5),
    DMA_REQ_SRC_TIM3_TRIG = _DMA_1_REQ_SRC(_DMA_S(4), 5),
    DMA_REQ_SRC_TIM3_CH2  = _DMA_1_REQ_SRC(_DMA_S(5), 5),
    DMA_REQ_SRC_TIM3_CH3  = _DMA_1_REQ_SRC(_DMA_S(7), 5),

    /* Channel 6 */
    DMA_REQ_SRC_TIM5_CH3  = _DMA_1_REQ_SRC(_DMA_S(0), 6),
    DMA_REQ_SRC_TIM5_UP   = _DMA_1_REQ_SRC(_DMA_S(0) | _DMA_S(6), 6),
    DMA_REQ_SRC_TIM5_CH4  = _DMA_1_REQ_SRC(_DMA_S(1) | _DMA_S(3), 6),
    DMA_REQ_SRC_TIM5_TRIG = _DMA_1_REQ_SRC(_DMA_S(1) | _DMA_S(3), 6),
    DMA_REQ_SRC_TIM5_CH1  = _DMA_1_REQ_SRC(_DMA_S(2), 6),
    DMA_REQ_SRC_TIM5_CH2  = _DMA_1_REQ_SRC(_DMA_S(4), 6),

    /* Channel 7 */
    DMA_REQ_SRC_TIM6_UP   = _DMA_1_REQ_SRC(_DMA_S(1), 7),
    DMA_REQ_SRC_I2C2_RX   = _DMA_1_REQ_SRC(_DMA_S(2) | _DMA_S(3), 7),
    DMA_REQ_SRC_USART3_TX_ALTERNATE = _DMA_1_REQ_SRC(_DMA_S(4), 7),
    DMA_REQ_SRC_DAC1      = _DMA_1_REQ_SRC(_DMA_S(5), 7),
    DMA_REQ_SRC_DAC2      = _DMA_1_REQ_SRC(_DMA_S(6), 7),
    DMA_REQ_SRC_I2C2_TX   = _DMA_1_REQ_SRC(_DMA_S(7), 7),
#undef _DMA_1_REQ_SRC

    /* DMA2 request sources */
#define _DMA_2_REQ_SRC(stream_mask, channel) \
    _DMA_STM32F2_REQ_SRC(stream_mask, RCC_DMA2, channel)

    /* Channel 0 */
    DMA_REQ_SRC_ADC1     = _DMA_2_REQ_SRC(_DMA_S(0) | _DMA_S(4), 0),
    /* You can use these "DMA_REQ_SRC_TIMx_CHx_ALTERNATE" if you know
     * what you're doing, but the other ones (for channels 6 and 7),
     * are better, in that they don't conflict with one another. */
    DMA_REQ_SRC_TIM8_CH1_ALTERNATE = _DMA_2_REQ_SRC(_DMA_S(2), 0),
    DMA_REQ_SRC_TIM8_CH2_ALTERNATE = _DMA_2_REQ_SRC(_DMA_S(2), 0),
    DMA_REQ_SRC_TIM8_CH3_ALTERNATE = _DMA_2_REQ_SRC(_DMA_S(2), 0),
    DMA_REQ_SRC_TIM1_CH1_ALTERNATE = _DMA_2_REQ_SRC(_DMA_S(6), 0),
    DMA_REQ_SRC_TIM1_CH2_ALTERNATE = _DMA_2_REQ_SRC(_DMA_S(6), 0),
    DMA_REQ_SRC_TIM1_CH3_ALTENRATE = _DMA_2_REQ_SRC(_DMA_S(6), 0),

    /* Channel 1 */
    DMA_REQ_SRC_DCMI = _DMA_2_REQ_SRC(_DMA_S(1) | _DMA_S(7), 1),
    DMA_REQ_SRC_ADC2 = _DMA_2_REQ_SRC(_DMA_S(2) | _DMA_S(3), 1),

    /* Channel 2 */
    DMA_REQ_SRC_ADC3     = _DMA_2_REQ_SRC(_DMA_S(0) | _DMA_S(1), 2),
    DMA_REQ_SRC_CRYP_OUT = _DMA_2_REQ_SRC(_DMA_S(5), 2),
    DMA_REQ_SRC_CRYP_IN  = _DMA_2_REQ_SRC(_DMA_S(6), 2),
    DMA_REQ_SRC_HASH_IN  = _DMA_2_REQ_SRC(_DMA_S(7), 2),

    /* Channel 3 */
    DMA_REQ_SRC_SPI1_RX = _DMA_2_REQ_SRC(_DMA_S(0) | _DMA_S(2), 3),
    DMA_REQ_SRC_SPI1_TX = _DMA_2_REQ_SRC(_DMA_S(3) | _DMA_S(5), 3),

    /* Channel 4 */
    DMA_REQ_SRC_USART1_RX = _DMA_2_REQ_SRC(_DMA_S(2) | _DMA_S(5), 4),
    DMA_REQ_SRC_SDIO      = _DMA_2_REQ_SRC(_DMA_S(3) | _DMA_S(6), 4),
    DMA_REQ_SRC_USART1_TX = _DMA_2_REQ_SRC(_DMA_S(7), 4),

    /* Channel 5 */
    DMA_REQ_SRC_USART6_RX = _DMA_2_REQ_SRC(_DMA_S(1) | _DMA_S(2), 5),
    DMA_REQ_SRC_USART6_TX = _DMA_2_REQ_SRC(_DMA_S(6) | _DMA_S(7), 5),

    /* Channel 6 */
    DMA_REQ_SRC_TIM1_TRIG = _DMA_2_REQ_SRC(_DMA_S(0) | _DMA_S(4), 6),
    DMA_REQ_SRC_TIM1_CH1  = _DMA_2_REQ_SRC(_DMA_S(1) | _DMA_S(3), 6),
    DMA_REQ_SRC_TIM1_CH2  = _DMA_2_REQ_SRC(_DMA_S(3), 6),
    DMA_REQ_SRC_TIM1_CH4  = _DMA_2_REQ_SRC(_DMA_S(4), 6),
    DMA_REQ_SRC_TIM1_COM  = _DMA_2_REQ_SRC(_DMA_S(4), 6),
    DMA_REQ_SRC_TIM1_UP   = _DMA_2_REQ_SRC(_DMA_S(5), 6),
    DMA_REQ_SRC_TIM1_CH3  = _DMA_2_REQ_SRC(_DMA_S(6), 6),

    /* Channel 7 */
    DMA_REQ_SRC_TIM8_UP   = _DMA_2_REQ_SRC(_DMA_S(1), 7),
    DMA_REQ_SRC_TIM8_CH1  = _DMA_2_REQ_SRC(_DMA_S(2), 7),
    DMA_REQ_SRC_TIM8_CH2  = _DMA_2_REQ_SRC(_DMA_S(3), 7),
    DMA_REQ_SRC_TIM8_CH3  = _DMA_2_REQ_SRC(_DMA_S(4), 7),
    DMA_REQ_SRC_TIM8_CH4  = _DMA_2_REQ_SRC(_DMA_S(7), 7),
    DMA_REQ_SRC_TIM8_TRIG = _DMA_2_REQ_SRC(_DMA_S(7), 7),
    DMA_REQ_SRC_TIM8_COM  = _DMA_2_REQ_SRC(_DMA_S(7), 7),
#undef _DMA_2_REQ_SRC
#undef _DMA_S
} dma_request_src;

/*
 * Tube conveniences
 */

static inline dma_tube_reg_map* dma_tube_regs(dma_dev *dev,
                                              dma_tube tube) {
    ASSERT(DMA_S0 <= tube && tube <= DMA_S7);
    switch (dev->clk_id) {
    case RCC_DMA1:
        return DMA1S0_BASE + (int)tube;
    case RCC_DMA2:
        return DMA2S0_BASE + (int)tube;
    default:
        /* Can't happen */
        ASSERT(0);
        return 0;
    }
}

static inline uint8 dma_is_enabled(dma_dev *dev, dma_tube tube) {
    return dma_tube_regs(dev, tube)->SCR & DMA_SCR_EN;
}

/* F2-only; available because of double-buffering. */
void dma_set_mem_n_addr(dma_dev *dev, dma_tube tube, int n,
                        __IO void *address);

/**
 * @brief Set memory 0 address.
 * Availability: STM32F2.
 *
 * @param dev DMA device
 * @param tube Tube whose memory 0 address to set
 * @param addr Address to use as memory 0
 */
static inline __always_inline void
dma_set_mem0_addr(dma_dev *dev, dma_tube tube, __IO void *addr) {
    dma_set_mem_n_addr(dev, tube, 0, addr);
}

/**
 * @brief Set memory 1 address.
 * Availability: STM32F2.
 *
 * @param dev DMA device
 * @param tube Tube whose memory 1 address to set
 * @param addr Address to use as memory 1
 */
static inline __always_inline void
dma_set_mem1_addr(dma_dev *dev, dma_tube tube, __IO void *addr) {
    dma_set_mem_n_addr(dev, tube, 1, addr);
}

/* Assume the user means SM0AR in a non-double-buffered configuration. */
static inline __always_inline void
dma_set_mem_addr(dma_dev *dev, dma_tube tube, __IO void *addr) {
    dma_set_mem0_addr(dev, tube, addr);
}

/* SM0AR and SM1AR are treated as though they have the same size */
static inline dma_xfer_size dma_get_mem_size(dma_dev *dev, dma_tube tube) {
    return (dma_xfer_size)(dma_tube_regs(dev, tube)->SCR >> 13);
}

static inline dma_xfer_size dma_get_per_size(dma_dev *dev, dma_tube tube) {
    return (dma_xfer_size)(dma_tube_regs(dev, tube)->SCR >> 11);
}

void dma_enable_fifo(dma_dev *dev, dma_tube tube);
void dma_disable_fifo(dma_dev *dev, dma_tube tube);

static inline __always_inline int dma_is_fifo_enabled(dma_dev *dev, dma_tube tube) {
    return dma_tube_regs(dev, tube)->SFCR & DMA_SFCR_DMDIS;
}

/*
 * TODO:
 * - Double-buffer configuration function
 * - FIFO configuration function
 * - MBURST/PBURST configuration function
 */

/*
 * ISR/IFCR conveniences.
 */

/* (undocumented) helper for reading LISR/HISR and writing
 * LIFCR/HIFCR. For these registers,
 *
 * S0, S4:  bits start at bit 0
 * S1, S5:                    6
 * S2, S6:                   16
 * S3, S7:                   22
 *
 * I can't imagine why ST didn't just use a byte for each group. The
 * bits fit, and it would have made functions like these simpler and
 * faster.  Oh well. */
static inline __always_inline uint32 _dma_sr_fcr_shift(dma_tube tube) {
    switch (tube) {
    case DMA_S0:                /* fall through */
    case DMA_S4:
        return 0;
    case DMA_S1:                /* fall through */
    case DMA_S5:
        return 6;
    case DMA_S2:                /* fall through */
    case DMA_S6:
        return 16;
    case DMA_S3:                /* fall through */
    case DMA_S7:
        return 22;
    }
    /* Can't happen */
    ASSERT(0);
    return 0;
}

static inline uint8 dma_get_isr_bits(dma_dev *dev, dma_tube tube) {
    dma_reg_map *regs = dev->regs;
    __IO uint32 *isr = tube > DMA_S3 ? &regs->HISR : &regs->LISR;
    return (*isr >> _dma_sr_fcr_shift(tube)) & 0x3D;
}

static inline void dma_clear_isr_bits(dma_dev *dev, dma_tube tube) {
    dma_reg_map *regs = dev->regs;
    __IO uint32 *ifcr = tube > DMA_S3 ? &regs->HIFCR : &regs->LIFCR;
    *ifcr = (0x3D << _dma_sr_fcr_shift(tube));
}

#undef _DMA_IRQ_BIT_SHIFT

#ifdef __cplusplus
} // extern "C"
#endif

#endif
