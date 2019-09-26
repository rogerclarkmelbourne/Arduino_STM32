/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Michael Hope.
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
 * @file libmaple/stm32f1/include/series/dma.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>;
 *         Original implementation by Michael Hope
 * @brief STM32F1 DMA series header.
 */

/*
 * See /notes/dma-stm32f1.txt for more information.
 */

#ifndef _LIBMAPLE_STM32F1_DMA_H_
#define _LIBMAPLE_STM32F1_DMA_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/dma_common.h>

/*
 * Register maps and base pointers
 */

/**
 * @brief STM32F1 DMA register map type.
 *
 * Note that DMA controller 2 (register map base pointer DMA2_BASE)
 * only supports channels 1--5.
 */
typedef struct dma_reg_map {
    __IO uint32 ISR;            /**< Interrupt status register */
    __IO uint32 IFCR;           /**< Interrupt flag clear register */
    __IO uint32 CCR1;           /**< Channel 1 configuration register */
    __IO uint32 CNDTR1;         /**< Channel 1 number of data register */
    __IO uint32 CPAR1;          /**< Channel 1 peripheral address register */
    __IO uint32 CMAR1;          /**< Channel 1 memory address register */
    const uint32 RESERVED1;     /**< Reserved. */
    __IO uint32 CCR2;           /**< Channel 2 configuration register */
    __IO uint32 CNDTR2;         /**< Channel 2 number of data register */
    __IO uint32 CPAR2;          /**< Channel 2 peripheral address register */
    __IO uint32 CMAR2;          /**< Channel 2 memory address register */
    const uint32 RESERVED2;     /**< Reserved. */
    __IO uint32 CCR3;           /**< Channel 3 configuration register */
    __IO uint32 CNDTR3;         /**< Channel 3 number of data register */
    __IO uint32 CPAR3;          /**< Channel 3 peripheral address register */
    __IO uint32 CMAR3;          /**< Channel 3 memory address register */
    const uint32 RESERVED3;     /**< Reserved. */
    __IO uint32 CCR4;           /**< Channel 4 configuration register */
    __IO uint32 CNDTR4;         /**< Channel 4 number of data register */
    __IO uint32 CPAR4;          /**< Channel 4 peripheral address register */
    __IO uint32 CMAR4;          /**< Channel 4 memory address register */
    const uint32 RESERVED4;     /**< Reserved. */
    __IO uint32 CCR5;           /**< Channel 5 configuration register */
    __IO uint32 CNDTR5;         /**< Channel 5 number of data register */
    __IO uint32 CPAR5;          /**< Channel 5 peripheral address register */
    __IO uint32 CMAR5;          /**< Channel 5 memory address register */
    const uint32 RESERVED5;     /**< Reserved. */
    __IO uint32 CCR6;           /**< Channel 6 configuration register */
    __IO uint32 CNDTR6;         /**< Channel 6 number of data register */
    __IO uint32 CPAR6;          /**< Channel 6 peripheral address register */
    __IO uint32 CMAR6;          /**< Channel 6 memory address register */
    const uint32 RESERVED6;     /**< Reserved. */
    __IO uint32 CCR7;           /**< Channel 7 configuration register */
    __IO uint32 CNDTR7;         /**< Channel 7 number of data register */
    __IO uint32 CPAR7;          /**< Channel 7 peripheral address register */
    __IO uint32 CMAR7;          /**< Channel 7 memory address register */
    const uint32 RESERVED7;     /**< Reserved. */
} dma_reg_map;

/** DMA controller 1 register map base pointer */
#define DMA1_BASE                       ((struct dma_reg_map*)0x40020000)
/** DMA controller 2 register map base pointer */
#define DMA2_BASE                       ((struct dma_reg_map*)0x40020400)

/**
 * @brief STM32F1 DMA channel (i.e. tube) register map type.
 * Provides access to an individual channel's registers.
 * @see dma_tube_regs()
 */
typedef struct dma_tube_reg_map {
    __IO uint32 CCR;           /**< Channel configuration register */
    __IO uint32 CNDTR;         /**< Channel number of data register */
    __IO uint32 CPAR;          /**< Channel peripheral address register */
    __IO uint32 CMAR;          /**< Channel memory address register */
} dma_tube_reg_map;

/** DMA1 channel 1 register map base pointer */
#define DMA1CH1_BASE                ((struct dma_tube_reg_map*)0x40020008)
/** DMA1 channel 2 register map base pointer */
#define DMA1CH2_BASE                ((struct dma_tube_reg_map*)0x4002001C)
/** DMA1 channel 3 register map base pointer */
#define DMA1CH3_BASE                ((struct dma_tube_reg_map*)0x40020030)
/** DMA1 channel 4 register map base pointer */
#define DMA1CH4_BASE                ((struct dma_tube_reg_map*)0x40020044)
/** DMA1 channel 5 register map base pointer */
#define DMA1CH5_BASE                ((struct dma_tube_reg_map*)0x40020058)
/** DMA1 channel 6 register map base pointer */
#define DMA1CH6_BASE                ((struct dma_tube_reg_map*)0x4002006C)
/** DMA1 channel 7 register map base pointer */
#define DMA1CH7_BASE                ((struct dma_tube_reg_map*)0x40020080)

/** DMA2 channel 1 register map base pointer */
#define DMA2CH1_BASE                ((struct dma_tube_reg_map*)0x40020408)
/** DMA2 channel 2 register map base pointer */
#define DMA2CH2_BASE                ((struct dma_tube_reg_map*)0x4002041C)
/** DMA2 channel 3 register map base pointer */
#define DMA2CH3_BASE                ((struct dma_tube_reg_map*)0x40020430)
/** DMA2 channel 4 register map base pointer */
#define DMA2CH4_BASE                ((struct dma_tube_reg_map*)0x40020444)
/** DMA2 channel 5 register map base pointer */
#define DMA2CH5_BASE                ((struct dma_tube_reg_map*)0x40020458)

/*
 * Register bit definitions
 */

/* Interrupt status register */

#define DMA_ISR_TEIF_BIT 3
#define DMA_ISR_HTIF_BIT 2
#define DMA_ISR_TCIF_BIT 1
#define DMA_ISR_GIF_BIT  0

#define DMA_ISR_TEIF (1 << DMA_ISR_TEIF_BIT)
#define DMA_ISR_HTIF (1 << DMA_ISR_HTIF_BIT)
#define DMA_ISR_TCIF (1 << DMA_ISR_TCIF_BIT)
#define DMA_ISR_GIF  (1 << DMA_ISR_GIF_BIT)

#define DMA_ISR_TEIF7_BIT               27
#define DMA_ISR_HTIF7_BIT               26
#define DMA_ISR_TCIF7_BIT               25
#define DMA_ISR_GIF7_BIT                24
#define DMA_ISR_TEIF6_BIT               23
#define DMA_ISR_HTIF6_BIT               22
#define DMA_ISR_TCIF6_BIT               21
#define DMA_ISR_GIF6_BIT                20
#define DMA_ISR_TEIF5_BIT               19
#define DMA_ISR_HTIF5_BIT               18
#define DMA_ISR_TCIF5_BIT               17
#define DMA_ISR_GIF5_BIT                16
#define DMA_ISR_TEIF4_BIT               15
#define DMA_ISR_HTIF4_BIT               14
#define DMA_ISR_TCIF4_BIT               13
#define DMA_ISR_GIF4_BIT                12
#define DMA_ISR_TEIF3_BIT               11
#define DMA_ISR_HTIF3_BIT               10
#define DMA_ISR_TCIF3_BIT               9
#define DMA_ISR_GIF3_BIT                8
#define DMA_ISR_TEIF2_BIT               7
#define DMA_ISR_HTIF2_BIT               6
#define DMA_ISR_TCIF2_BIT               5
#define DMA_ISR_GIF2_BIT                4
#define DMA_ISR_TEIF1_BIT               3
#define DMA_ISR_HTIF1_BIT               2
#define DMA_ISR_TCIF1_BIT               1
#define DMA_ISR_GIF1_BIT                0

#define DMA_ISR_TEIF7                   (1U << DMA_ISR_TEIF7_BIT)
#define DMA_ISR_HTIF7                   (1U << DMA_ISR_HTIF7_BIT)
#define DMA_ISR_TCIF7                   (1U << DMA_ISR_TCIF7_BIT)
#define DMA_ISR_GIF7                    (1U << DMA_ISR_GIF7_BIT)
#define DMA_ISR_TEIF6                   (1U << DMA_ISR_TEIF6_BIT)
#define DMA_ISR_HTIF6                   (1U << DMA_ISR_HTIF6_BIT)
#define DMA_ISR_TCIF6                   (1U << DMA_ISR_TCIF6_BIT)
#define DMA_ISR_GIF6                    (1U << DMA_ISR_GIF6_BIT)
#define DMA_ISR_TEIF5                   (1U << DMA_ISR_TEIF5_BIT)
#define DMA_ISR_HTIF5                   (1U << DMA_ISR_HTIF5_BIT)
#define DMA_ISR_TCIF5                   (1U << DMA_ISR_TCIF5_BIT)
#define DMA_ISR_GIF5                    (1U << DMA_ISR_GIF5_BIT)
#define DMA_ISR_TEIF4                   (1U << DMA_ISR_TEIF4_BIT)
#define DMA_ISR_HTIF4                   (1U << DMA_ISR_HTIF4_BIT)
#define DMA_ISR_TCIF4                   (1U << DMA_ISR_TCIF4_BIT)
#define DMA_ISR_GIF4                    (1U << DMA_ISR_GIF4_BIT)
#define DMA_ISR_TEIF3                   (1U << DMA_ISR_TEIF3_BIT)
#define DMA_ISR_HTIF3                   (1U << DMA_ISR_HTIF3_BIT)
#define DMA_ISR_TCIF3                   (1U << DMA_ISR_TCIF3_BIT)
#define DMA_ISR_GIF3                    (1U << DMA_ISR_GIF3_BIT)
#define DMA_ISR_TEIF2                   (1U << DMA_ISR_TEIF2_BIT)
#define DMA_ISR_HTIF2                   (1U << DMA_ISR_HTIF2_BIT)
#define DMA_ISR_TCIF2                   (1U << DMA_ISR_TCIF2_BIT)
#define DMA_ISR_GIF2                    (1U << DMA_ISR_GIF2_BIT)
#define DMA_ISR_TEIF1                   (1U << DMA_ISR_TEIF1_BIT)
#define DMA_ISR_HTIF1                   (1U << DMA_ISR_HTIF1_BIT)
#define DMA_ISR_TCIF1                   (1U << DMA_ISR_TCIF1_BIT)
#define DMA_ISR_GIF1                    (1U << DMA_ISR_GIF1_BIT)

/* Interrupt flag clear register */

#define DMA_IFCR_CTEIF7_BIT             27
#define DMA_IFCR_CHTIF7_BIT             26
#define DMA_IFCR_CTCIF7_BIT             25
#define DMA_IFCR_CGIF7_BIT              24
#define DMA_IFCR_CTEIF6_BIT             23
#define DMA_IFCR_CHTIF6_BIT             22
#define DMA_IFCR_CTCIF6_BIT             21
#define DMA_IFCR_CGIF6_BIT              20
#define DMA_IFCR_CTEIF5_BIT             19
#define DMA_IFCR_CHTIF5_BIT             18
#define DMA_IFCR_CTCIF5_BIT             17
#define DMA_IFCR_CGIF5_BIT              16
#define DMA_IFCR_CTEIF4_BIT             15
#define DMA_IFCR_CHTIF4_BIT             14
#define DMA_IFCR_CTCIF4_BIT             13
#define DMA_IFCR_CGIF4_BIT              12
#define DMA_IFCR_CTEIF3_BIT             11
#define DMA_IFCR_CHTIF3_BIT             10
#define DMA_IFCR_CTCIF3_BIT             9
#define DMA_IFCR_CGIF3_BIT              8
#define DMA_IFCR_CTEIF2_BIT             7
#define DMA_IFCR_CHTIF2_BIT             6
#define DMA_IFCR_CTCIF2_BIT             5
#define DMA_IFCR_CGIF2_BIT              4
#define DMA_IFCR_CTEIF1_BIT             3
#define DMA_IFCR_CHTIF1_BIT             2
#define DMA_IFCR_CTCIF1_BIT             1
#define DMA_IFCR_CGIF1_BIT              0

#define DMA_IFCR_CTEIF7                 (1U << DMA_IFCR_CTEIF7_BIT)
#define DMA_IFCR_CHTIF7                 (1U << DMA_IFCR_CHTIF7_BIT)
#define DMA_IFCR_CTCIF7                 (1U << DMA_IFCR_CTCIF7_BIT)
#define DMA_IFCR_CGIF7                  (1U << DMA_IFCR_CGIF7_BIT)
#define DMA_IFCR_CTEIF6                 (1U << DMA_IFCR_CTEIF6_BIT)
#define DMA_IFCR_CHTIF6                 (1U << DMA_IFCR_CHTIF6_BIT)
#define DMA_IFCR_CTCIF6                 (1U << DMA_IFCR_CTCIF6_BIT)
#define DMA_IFCR_CGIF6                  (1U << DMA_IFCR_CGIF6_BIT)
#define DMA_IFCR_CTEIF5                 (1U << DMA_IFCR_CTEIF5_BIT)
#define DMA_IFCR_CHTIF5                 (1U << DMA_IFCR_CHTIF5_BIT)
#define DMA_IFCR_CTCIF5                 (1U << DMA_IFCR_CTCIF5_BIT)
#define DMA_IFCR_CGIF5                  (1U << DMA_IFCR_CGIF5_BIT)
#define DMA_IFCR_CTEIF4                 (1U << DMA_IFCR_CTEIF4_BIT)
#define DMA_IFCR_CHTIF4                 (1U << DMA_IFCR_CHTIF4_BIT)
#define DMA_IFCR_CTCIF4                 (1U << DMA_IFCR_CTCIF4_BIT)
#define DMA_IFCR_CGIF4                  (1U << DMA_IFCR_CGIF4_BIT)
#define DMA_IFCR_CTEIF3                 (1U << DMA_IFCR_CTEIF3_BIT)
#define DMA_IFCR_CHTIF3                 (1U << DMA_IFCR_CHTIF3_BIT)
#define DMA_IFCR_CTCIF3                 (1U << DMA_IFCR_CTCIF3_BIT)
#define DMA_IFCR_CGIF3                  (1U << DMA_IFCR_CGIF3_BIT)
#define DMA_IFCR_CTEIF2                 (1U << DMA_IFCR_CTEIF2_BIT)
#define DMA_IFCR_CHTIF2                 (1U << DMA_IFCR_CHTIF2_BIT)
#define DMA_IFCR_CTCIF2                 (1U << DMA_IFCR_CTCIF2_BIT)
#define DMA_IFCR_CGIF2                  (1U << DMA_IFCR_CGIF2_BIT)
#define DMA_IFCR_CTEIF1                 (1U << DMA_IFCR_CTEIF1_BIT)
#define DMA_IFCR_CHTIF1                 (1U << DMA_IFCR_CHTIF1_BIT)
#define DMA_IFCR_CTCIF1                 (1U << DMA_IFCR_CTCIF1_BIT)
#define DMA_IFCR_CGIF1                  (1U << DMA_IFCR_CGIF1_BIT)

/* Channel configuration register */

#define DMA_CCR_MEM2MEM_BIT             14
#define DMA_CCR_MINC_BIT                7
#define DMA_CCR_PINC_BIT                6
#define DMA_CCR_CIRC_BIT                5
#define DMA_CCR_DIR_BIT                 4
#define DMA_CCR_TEIE_BIT                3
#define DMA_CCR_HTIE_BIT                2
#define DMA_CCR_TCIE_BIT                1
#define DMA_CCR_EN_BIT                  0

#define DMA_CCR_MEM2MEM                 (1U << DMA_CCR_MEM2MEM_BIT)
#define DMA_CCR_PL                      (0x3 << 12)
#define DMA_CCR_PL_LOW                  (0x0 << 12)
#define DMA_CCR_PL_MEDIUM               (0x1 << 12)
#define DMA_CCR_PL_HIGH                 (0x2 << 12)
#define DMA_CCR_PL_VERY_HIGH            (0x3 << 12)
#define DMA_CCR_MSIZE                   (0x3 << 10)
#define DMA_CCR_MSIZE_8BITS             (0x0 << 10)
#define DMA_CCR_MSIZE_16BITS            (0x1 << 10)
#define DMA_CCR_MSIZE_32BITS            (0x2 << 10)
#define DMA_CCR_PSIZE                   (0x3 << 8)
#define DMA_CCR_PSIZE_8BITS             (0x0 << 8)
#define DMA_CCR_PSIZE_16BITS            (0x1 << 8)
#define DMA_CCR_PSIZE_32BITS            (0x2 << 8)
#define DMA_CCR_MINC                    (1U << DMA_CCR_MINC_BIT)
#define DMA_CCR_PINC                    (1U << DMA_CCR_PINC_BIT)
#define DMA_CCR_CIRC                    (1U << DMA_CCR_CIRC_BIT)
#define DMA_CCR_DIR                     (1U << DMA_CCR_DIR_BIT)
#define DMA_CCR_DIR_FROM_PER            (0U << DMA_CCR_DIR_BIT)
#define DMA_CCR_DIR_FROM_MEM            (1U << DMA_CCR_DIR_BIT)
#define DMA_CCR_TEIE                    (1U << DMA_CCR_TEIE_BIT)
#define DMA_CCR_HTIE                    (1U << DMA_CCR_HTIE_BIT)
#define DMA_CCR_TCIE                    (1U << DMA_CCR_TCIE_BIT)
#define DMA_CCR_EN                      (1U << DMA_CCR_EN_BIT)

/*
 * Devices
 */

extern dma_dev *DMA1;
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
extern dma_dev *DMA2;
#endif

/*
 * Other types needed by, or useful for, <libmaple/dma.h>.
 */

/**
 * @brief STM32F1 dma_tube.
 * On STM32F1, DMA tubes are just channels.
 */
#define dma_tube dma_channel

/**
 * @brief On STM32F1, dma_channel_reg_map is an alias for dma_tube_reg_map.
 * This is for backwards compatibility. */
#define dma_channel_reg_map dma_tube_reg_map

/**
 * @brief STM32F1 configuration flags for dma_tube_config
 * @see struct dma_tube_config
 */
typedef enum dma_cfg_flags {
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
    DMA_CFG_CIRC = DMA_CCR_CIRC,

    /** Transfer complete interrupt enable */
    DMA_CFG_CMPLT_IE      = DMA_CCR_TCIE,
    /** Transfer half-complete interrupt enable  */
    DMA_CFG_HALF_CMPLT_IE = DMA_CCR_HTIE,
    /** Transfer error interrupt enable */
    DMA_CFG_ERR_IE        = DMA_CCR_TEIE,
} dma_cfg_flags;

/**
 * @brief STM32F1 DMA request sources.
 *
 * IMPORTANT:
 *
 * 1. On STM32F1, each dma_request_src can only be used by a
 * particular tube on a particular DMA controller. For example,
 * DMA_REQ_SRC_ADC1 belongs to DMA1, tube 1. DMA2 cannot serve
 * requests from ADC1, nor can DMA1 tube 2, etc. If you try to use a
 * request source with the wrong DMA controller or tube on STM32F1,
 * dma_tube_cfg() will fail.
 *
 * 2. In general, a DMA tube can only serve a single request source at
 * a time, and on STM32F1, Terrible Super-Bad Things will happen if
 * two request sources are active for a single tube.
 *
 * To make all this easier to sort out, these dma_request_src
 * enumerators are grouped by DMA controller and tube.
 *
 * @see struct dma_tube_config
 * @see dma_tube_cfg()
 */
typedef enum dma_request_src {
    /* Each request source encodes the DMA controller and channel it
     * belongs to, for error checking in dma_tube_cfg(). */

    /* DMA1 request sources */

    /**@{*/
    /** (DMA1, tube 1) */
    DMA_REQ_SRC_ADC1      = (RCC_DMA1 << 3) | 1,
    DMA_REQ_SRC_TIM2_CH3  = (RCC_DMA1 << 3) | 1,
    DMA_REQ_SRC_TIM4_CH1  = (RCC_DMA1 << 3) | 1,
    /**@}*/

    /**@{*/
    /** (DMA1, tube 2)*/
    DMA_REQ_SRC_SPI1_RX   = (RCC_DMA1 << 3) | 2,
    DMA_REQ_SRC_USART3_TX = (RCC_DMA1 << 3) | 2,
    DMA_REQ_SRC_TIM1_CH1  = (RCC_DMA1 << 3) | 2,
    DMA_REQ_SRC_TIM2_UP   = (RCC_DMA1 << 3) | 2,
    DMA_REQ_SRC_TIM3_CH3  = (RCC_DMA1 << 3) | 2,
    /**@}*/

    /**@{*/
    /** (DMA1, tube 3)*/
    DMA_REQ_SRC_SPI1_TX   = (RCC_DMA1 << 3) | 3,
    DMA_REQ_SRC_USART3_RX = (RCC_DMA1 << 3) | 3,
    DMA_REQ_SRC_TIM1_CH2  = (RCC_DMA1 << 3) | 3,
    DMA_REQ_SRC_TIM3_CH4  = (RCC_DMA1 << 3) | 3,
    DMA_REQ_SRC_TIM3_UP   = (RCC_DMA1 << 3) | 3,
    /**@}*/

    /**@{*/
    /** (DMA1, tube 4)*/
    DMA_REQ_SRC_SPI2_RX   = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_I2S2_RX   = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_USART1_TX = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_I2C2_TX   = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_TIM1_CH4  = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_TIM1_TRIG = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_TIM1_COM  = (RCC_DMA1 << 3) | 4,
    DMA_REQ_SRC_TIM4_CH2  = (RCC_DMA1 << 3) | 4,
    /**@}*/

    /**@{*/
    /** (DMA1, tube 5)*/
    DMA_REQ_SRC_SPI2_TX   = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_I2S2_TX   = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_USART1_RX = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_I2C2_RX   = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_TIM1_UP   = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_TIM2_CH1  = (RCC_DMA1 << 3) | 5,
    DMA_REQ_SRC_TIM4_CH3  = (RCC_DMA1 << 3) | 5,
    /**@}*/

    /**@{*/
    /** (DMA1, tube 6)*/
    DMA_REQ_SRC_USART2_RX = (RCC_DMA1 << 3) | 6,
    DMA_REQ_SRC_I2C1_TX   = (RCC_DMA1 << 3) | 6,
    DMA_REQ_SRC_TIM1_CH3  = (RCC_DMA1 << 3) | 6,
    DMA_REQ_SRC_TIM3_CH1  = (RCC_DMA1 << 3) | 6,
    DMA_REQ_SRC_TIM3_TRIG = (RCC_DMA1 << 3) | 6,
    /**@}*/

    /**@{*/
    /* Tube 7 */
    DMA_REQ_SRC_USART2_TX = (RCC_DMA1 << 3) | 7,
    DMA_REQ_SRC_I2C1_RX   = (RCC_DMA1 << 3) | 7,
    DMA_REQ_SRC_TIM2_CH2  = (RCC_DMA1 << 3) | 7,
    DMA_REQ_SRC_TIM2_CH4  = (RCC_DMA1 << 3) | 7,
    DMA_REQ_SRC_TIM4_UP   = (RCC_DMA1 << 3) | 7,
    /**@}*/

    /* DMA2 request sources */

    /**@{*/
    /** (DMA2, tube 1)*/
    DMA_REQ_SRC_SPI3_RX   = (RCC_DMA2 << 3) | 1,
    DMA_REQ_SRC_I2S3_RX   = (RCC_DMA2 << 3) | 1,
    DMA_REQ_SRC_TIM5_CH4  = (RCC_DMA2 << 3) | 1,
    DMA_REQ_SRC_TIM5_TRIG = (RCC_DMA2 << 3) | 1,
    /**@}*/

    /**@{*/
    /** (DMA2, tube 2)*/
    DMA_REQ_SRC_SPI3_TX   = (RCC_DMA2 << 3) | 2,
    DMA_REQ_SRC_I2S3_TX   = (RCC_DMA2 << 3) | 2,
    DMA_REQ_SRC_TIM5_CH3  = (RCC_DMA2 << 3) | 2,
    DMA_REQ_SRC_TIM5_UP   = (RCC_DMA2 << 3) | 2,
    /**@}*/

    /**@{*/
    /** (DMA2, tube 3)*/
    DMA_REQ_SRC_UART4_RX  = (RCC_DMA2 << 3) | 3,
    DMA_REQ_SRC_TIM6_UP   = (RCC_DMA2 << 3) | 3,
    DMA_REQ_SRC_DAC_CH1   = (RCC_DMA2 << 3) | 3,
    /**@}*/

    /**@{*/
    /** (DMA2, tube 4)*/
    DMA_REQ_SRC_SDIO      = (RCC_DMA2 << 3) | 4,
    DMA_REQ_SRC_TIM5_CH2  = (RCC_DMA2 << 3) | 4,
    /**@}*/

    /**@{*/
    /** (DMA2, tube 5)*/
    DMA_REQ_SRC_ADC3      = (RCC_DMA2 << 3) | 5,
    DMA_REQ_SRC_UART4_TX  = (RCC_DMA2 << 3) | 5,
    DMA_REQ_SRC_TIM5_CH1  = (RCC_DMA2 << 3) | 5,
    /**@}*/
} dma_request_src;

/*
 * Convenience routines.
 */

/**
 * @brief On STM32F1, dma_is_channel_enabled() is an alias for
 *        dma_is_enabled().
 * This is for backwards compatibility.
 */
#define dma_is_channel_enabled dma_is_enabled

#define DMA_CHANNEL_NREGS 5     /* accounts for reserved word */
static inline dma_tube_reg_map* dma_tube_regs(dma_dev *dev, dma_tube tube) {
    __IO uint32 *ccr1 = &dev->regs->CCR1;
    return (dma_channel_reg_map*)(ccr1 + DMA_CHANNEL_NREGS * (tube - 1));
}

/**
 * @brief On STM32F1, dma_channel_regs() is an alias for dma_tube_regs().
 * This is for backwards compatibility. */
#define dma_channel_regs(dev, ch) dma_tube_regs(dev, ch)

static inline uint8 dma_is_enabled(dma_dev *dev, dma_tube tube) {
    return (uint8)(dma_tube_regs(dev, tube)->CCR & DMA_CCR_EN);
}

static inline uint8 dma_get_isr_bits(dma_dev *dev, dma_tube tube) {
    uint8 shift = (tube - 1) * 4;
    return (dev->regs->ISR >> shift) & 0xF;
}

static inline void dma_clear_isr_bits(dma_dev *dev, dma_tube tube) {
    dev->regs->IFCR = (1U << (4 * (tube - 1)));
}

static inline uint16 dma_get_count(dma_dev *dev, dma_tube tube) {
    return dma_channel_regs(dev, tube)->CNDTR;
}

/**
 * @brief Deprecated
 * STM32F1 mode flags for dma_setup_xfer(). Use dma_tube_cfg() instead.
 * @see dma_tube_cfg()
 */
typedef enum dma_mode_flags {
    DMA_MEM_2_MEM  = 1 << 14, /**< Memory to memory mode */
    DMA_MINC_MODE  = 1 << 7,  /**< Auto-increment memory address */
    DMA_PINC_MODE  = 1 << 6,  /**< Auto-increment peripheral address */
    DMA_CIRC_MODE  = 1 << 5,  /**< Circular mode */
    DMA_FROM_MEM   = 1 << 4,  /**< Read from memory to peripheral */
    DMA_TRNS_ERR   = 1 << 3,  /**< Interrupt on transfer error */
    DMA_HALF_TRNS  = 1 << 2,  /**< Interrupt on half-transfer */
    DMA_TRNS_CMPLT = 1 << 1   /**< Interrupt on transfer completion */
} dma_mode_flags;

/* Keep this around for backwards compatibility, but it's deprecated.
 * New code should use dma_tube_cfg() instead.
 *
 * (It's not possible to fully configure a DMA stream on F2 with just
 * this information, so this interface is too tied to the F1.) */
void dma_setup_transfer(dma_dev       *dev,
                        dma_channel    channel,
                        __IO void     *peripheral_address,
                        dma_xfer_size  peripheral_size,
                        __IO void     *memory_address,
                        dma_xfer_size  memory_size,
                        uint32         mode);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
