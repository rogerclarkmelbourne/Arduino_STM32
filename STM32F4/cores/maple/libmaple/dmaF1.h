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
 * @file dma.h
 *
 * @author Marti Bolivar <mbolivar@leaflabs.com>;
 *         Original implementation by Michael Hope
 *
 * @brief Direct Memory Access peripheral support
 */

/*
 * See /notes/dma.txt for more information.
 */

#ifndef _DMA_H_
#define _DMA_H_

#include "libmaple_types.h"
#include "rcc.h"
#include "nvic.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Register maps
 */

/**
 * @brief DMA register map type.
 *
 * Note that DMA controller 2 (register map base pointer DMA2_BASE)
 * only supports channels 1--5.
 */
typedef struct dma_reg_map {
    __io uint32 ISR;            /**< Interrupt status register */
    __io uint32 IFCR;           /**< Interrupt flag clear register */
    __io uint32 CCR1;           /**< Channel 1 configuration register */
    __io uint32 CNDTR1;         /**< Channel 1 number of data register */
    __io uint32 CPAR1;          /**< Channel 1 peripheral address register */
    __io uint32 CMAR1;          /**< Channel 1 memory address register */
    const uint32 RESERVED1;     /**< Reserved. */
    __io uint32 CCR2;           /**< Channel 2 configuration register */
    __io uint32 CNDTR2;         /**< Channel 2 number of data register */
    __io uint32 CPAR2;          /**< Channel 2 peripheral address register */
    __io uint32 CMAR2;          /**< Channel 2 memory address register */
    const uint32 RESERVED2;     /**< Reserved. */
    __io uint32 CCR3;           /**< Channel 3 configuration register */
    __io uint32 CNDTR3;         /**< Channel 3 number of data register */
    __io uint32 CPAR3;          /**< Channel 3 peripheral address register */
    __io uint32 CMAR3;          /**< Channel 3 memory address register */
    const uint32 RESERVED3;     /**< Reserved. */
    __io uint32 CCR4;           /**< Channel 4 configuration register */
    __io uint32 CNDTR4;         /**< Channel 4 number of data register */
    __io uint32 CPAR4;          /**< Channel 4 peripheral address register */
    __io uint32 CMAR4;          /**< Channel 4 memory address register */
    const uint32 RESERVED4;     /**< Reserved. */
    __io uint32 CCR5;           /**< Channel 5 configuration register */
    __io uint32 CNDTR5;         /**< Channel 5 number of data register */
    __io uint32 CPAR5;          /**< Channel 5 peripheral address register */
    __io uint32 CMAR5;          /**< Channel 5 memory address register */
    const uint32 RESERVED5;     /**< Reserved. */
    __io uint32 CCR6;           /**< Channel 6 configuration register */
    __io uint32 CNDTR6;         /**< Channel 6 number of data register */
    __io uint32 CPAR6;          /**< Channel 6 peripheral address register */
    __io uint32 CMAR6;          /**< Channel 6 memory address register */
    const uint32 RESERVED6;     /**< Reserved. */
    __io uint32 CCR7;           /**< Channel 7 configuration register */
    __io uint32 CNDTR7;         /**< Channel 7 number of data register */
    __io uint32 CPAR7;          /**< Channel 7 peripheral address register */
    __io uint32 CMAR7;          /**< Channel 7 memory address register */
    const uint32 RESERVED7;     /**< Reserved. */
} dma_reg_map;

/** DMA controller 1 register map base pointer */
#define DMA1_BASE                       ((struct dma_reg_map*)0x40020000)

#ifdef STM32_HIGH_DENSITY
/** DMA controller 2 register map base pointer */
#define DMA2_BASE                       ((struct dma_reg_map*)0x40020400)
#endif

/*
 * Register bit definitions
 */

/* Interrupt status register */

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

#define DMA_ISR_TEIF7                   BIT(DMA_ISR_TEIF7_BIT)
#define DMA_ISR_HTIF7                   BIT(DMA_ISR_HTIF7_BIT)
#define DMA_ISR_TCIF7                   BIT(DMA_ISR_TCIF7_BIT)
#define DMA_ISR_GIF7                    BIT(DMA_ISR_GIF7_BIT)
#define DMA_ISR_TEIF6                   BIT(DMA_ISR_TEIF6_BIT)
#define DMA_ISR_HTIF6                   BIT(DMA_ISR_HTIF6_BIT)
#define DMA_ISR_TCIF6                   BIT(DMA_ISR_TCIF6_BIT)
#define DMA_ISR_GIF6                    BIT(DMA_ISR_GIF6_BIT)
#define DMA_ISR_TEIF5                   BIT(DMA_ISR_TEIF5_BIT)
#define DMA_ISR_HTIF5                   BIT(DMA_ISR_HTIF5_BIT)
#define DMA_ISR_TCIF5                   BIT(DMA_ISR_TCIF5_BIT)
#define DMA_ISR_GIF5                    BIT(DMA_ISR_GIF5_BIT)
#define DMA_ISR_TEIF4                   BIT(DMA_ISR_TEIF4_BIT)
#define DMA_ISR_HTIF4                   BIT(DMA_ISR_HTIF4_BIT)
#define DMA_ISR_TCIF4                   BIT(DMA_ISR_TCIF4_BIT)
#define DMA_ISR_GIF4                    BIT(DMA_ISR_GIF4_BIT)
#define DMA_ISR_TEIF3                   BIT(DMA_ISR_TEIF3_BIT)
#define DMA_ISR_HTIF3                   BIT(DMA_ISR_HTIF3_BIT)
#define DMA_ISR_TCIF3                   BIT(DMA_ISR_TCIF3_BIT)
#define DMA_ISR_GIF3                    BIT(DMA_ISR_GIF3_BIT)
#define DMA_ISR_TEIF2                   BIT(DMA_ISR_TEIF2_BIT)
#define DMA_ISR_HTIF2                   BIT(DMA_ISR_HTIF2_BIT)
#define DMA_ISR_TCIF2                   BIT(DMA_ISR_TCIF2_BIT)
#define DMA_ISR_GIF2                    BIT(DMA_ISR_GIF2_BIT)
#define DMA_ISR_TEIF1                   BIT(DMA_ISR_TEIF1_BIT)
#define DMA_ISR_HTIF1                   BIT(DMA_ISR_HTIF1_BIT)
#define DMA_ISR_TCIF1                   BIT(DMA_ISR_TCIF1_BIT)
#define DMA_ISR_GIF1                    BIT(DMA_ISR_GIF1_BIT)

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

#define DMA_IFCR_CTEIF7                 BIT(DMA_IFCR_CTEIF7_BIT)
#define DMA_IFCR_CHTIF7                 BIT(DMA_IFCR_CHTIF7_BIT)
#define DMA_IFCR_CTCIF7                 BIT(DMA_IFCR_CTCIF7_BIT)
#define DMA_IFCR_CGIF7                  BIT(DMA_IFCR_CGIF7_BIT)
#define DMA_IFCR_CTEIF6                 BIT(DMA_IFCR_CTEIF6_BIT)
#define DMA_IFCR_CHTIF6                 BIT(DMA_IFCR_CHTIF6_BIT)
#define DMA_IFCR_CTCIF6                 BIT(DMA_IFCR_CTCIF6_BIT)
#define DMA_IFCR_CGIF6                  BIT(DMA_IFCR_CGIF6_BIT)
#define DMA_IFCR_CTEIF5                 BIT(DMA_IFCR_CTEIF5_BIT)
#define DMA_IFCR_CHTIF5                 BIT(DMA_IFCR_CHTIF5_BIT)
#define DMA_IFCR_CTCIF5                 BIT(DMA_IFCR_CTCIF5_BIT)
#define DMA_IFCR_CGIF5                  BIT(DMA_IFCR_CGIF5_BIT)
#define DMA_IFCR_CTEIF4                 BIT(DMA_IFCR_CTEIF4_BIT)
#define DMA_IFCR_CHTIF4                 BIT(DMA_IFCR_CHTIF4_BIT)
#define DMA_IFCR_CTCIF4                 BIT(DMA_IFCR_CTCIF4_BIT)
#define DMA_IFCR_CGIF4                  BIT(DMA_IFCR_CGIF4_BIT)
#define DMA_IFCR_CTEIF3                 BIT(DMA_IFCR_CTEIF3_BIT)
#define DMA_IFCR_CHTIF3                 BIT(DMA_IFCR_CHTIF3_BIT)
#define DMA_IFCR_CTCIF3                 BIT(DMA_IFCR_CTCIF3_BIT)
#define DMA_IFCR_CGIF3                  BIT(DMA_IFCR_CGIF3_BIT)
#define DMA_IFCR_CTEIF2                 BIT(DMA_IFCR_CTEIF2_BIT)
#define DMA_IFCR_CHTIF2                 BIT(DMA_IFCR_CHTIF2_BIT)
#define DMA_IFCR_CTCIF2                 BIT(DMA_IFCR_CTCIF2_BIT)
#define DMA_IFCR_CGIF2                  BIT(DMA_IFCR_CGIF2_BIT)
#define DMA_IFCR_CTEIF1                 BIT(DMA_IFCR_CTEIF1_BIT)
#define DMA_IFCR_CHTIF1                 BIT(DMA_IFCR_CHTIF1_BIT)
#define DMA_IFCR_CTCIF1                 BIT(DMA_IFCR_CTCIF1_BIT)
#define DMA_IFCR_CGIF1                  BIT(DMA_IFCR_CGIF1_BIT)

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

#define DMA_CCR_MEM2MEM                 BIT(DMA_CCR_MEM2MEM_BIT)
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
#define DMA_CCR_MINC                    BIT(DMA_CCR_MINC_BIT)
#define DMA_CCR_PINC                    BIT(DMA_CCR_PINC_BIT)
#define DMA_CCR_CIRC                    BIT(DMA_CCR_CIRC_BIT)
#define DMA_CCR_DIR                     BIT(DMA_CCR_DIR_BIT)
#define DMA_CCR_TEIE                    BIT(DMA_CCR_TEIE_BIT)
#define DMA_CCR_HTIE                    BIT(DMA_CCR_HTIE_BIT)
#define DMA_CCR_TCIE                    BIT(DMA_CCR_TCIE_BIT)
#define DMA_CCR_EN                      BIT(DMA_CCR_EN_BIT)

/*
 * Devices
 */

/** Encapsulates state related to a DMA channel interrupt. */
typedef struct dma_handler_config {
    void (*handler)(void);      /**< User-specified channel interrupt
                                     handler */
    nvic_irq_num irq_line;      /**< Channel's NVIC interrupt number */
} dma_handler_config;

/** DMA device type */
typedef struct dma_dev {
    dma_reg_map *regs;             /**< Register map */
    rcc_clk_id clk_id;             /**< Clock ID */
    dma_handler_config handlers[]; /**<
                                    * @brief IRQ handlers and NVIC numbers.
                                    *
                                    * @see dma_attach_interrupt()
                                    * @see dma_detach_interrupt()
                                    */
} dma_dev;

extern dma_dev *DMA1;
#ifdef STM32_HIGH_DENSITY
extern dma_dev *DMA2;
#endif

/*
 * Convenience functions
 */

void dma_init(dma_dev *dev);

/** Flags for DMA transfer configuration. */
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

/** Source and destination transfer sizes. */
typedef enum dma_xfer_size {
    DMA_SIZE_8BITS  = 0,        /**< 8-bit transfers */
    DMA_SIZE_16BITS = 1,        /**< 16-bit transfers */
    DMA_SIZE_32BITS = 2         /**< 32-bit transfers */
} dma_xfer_size;

/** DMA channel */
typedef enum dma_channel {
    DMA_CH1 = 1,                /**< Channel 1 */
    DMA_CH2 = 2,                /**< Channel 2 */
    DMA_CH3 = 3,                /**< Channel 3 */
    DMA_CH4 = 4,                /**< Channel 4 */
    DMA_CH5 = 5,                /**< Channel 5 */
    DMA_CH6 = 6,                /**< Channel 6 */
    DMA_CH7 = 7,                /**< Channel 7 */
} dma_channel;

void dma_setup_transfer(dma_dev       *dev,
                        dma_channel    channel,
                        __io void     *peripheral_address,
                        dma_xfer_size  peripheral_size,
                        __io void     *memory_address,
                        dma_xfer_size  memory_size,
                        uint32         mode);

void dma_set_num_transfers(dma_dev *dev,
                           dma_channel channel,
                           uint16 num_transfers);

/** DMA transfer priority. */
typedef enum dma_priority {
    DMA_PRIORITY_LOW       = DMA_CCR_PL_LOW,      /**< Low priority */
    DMA_PRIORITY_MEDIUM    = DMA_CCR_PL_MEDIUM,   /**< Medium priority */
    DMA_PRIORITY_HIGH      = DMA_CCR_PL_HIGH,     /**< High priority */
    DMA_PRIORITY_VERY_HIGH = DMA_CCR_PL_VERY_HIGH /**< Very high priority */
} dma_priority;

void dma_set_priority(dma_dev *dev,
                      dma_channel channel,
                      dma_priority priority);

void dma_attach_interrupt(dma_dev *dev,
                          dma_channel channel,
                          void (*handler)(void));
void dma_detach_interrupt(dma_dev *dev, dma_channel channel);

/**
 * Encodes the reason why a DMA interrupt was called.
 * @see dma_get_irq_cause()
 */
typedef enum dma_irq_cause {
    DMA_TRANSFER_COMPLETE,      /**< Transfer is complete. */
    DMA_TRANSFER_HALF_COMPLETE, /**< Transfer is half complete. */
    DMA_TRANSFER_ERROR,         /**< Error occurred during transfer. */
} dma_irq_cause;

dma_irq_cause dma_get_irq_cause(dma_dev *dev, dma_channel channel);

void dma_enable(dma_dev *dev, dma_channel channel);
void dma_disable(dma_dev *dev, dma_channel channel);

void dma_set_mem_addr(dma_dev *dev, dma_channel channel, __io void *address);
void dma_set_per_addr(dma_dev *dev, dma_channel channel, __io void *address);

/**
 * @brief DMA channel register map type.
 *
 * Provides access to an individual channel's registers.
 */
typedef struct dma_channel_reg_map {
    __io uint32 CCR;           /**< Channel configuration register */
    __io uint32 CNDTR;         /**< Channel number of data register */
    __io uint32 CPAR;          /**< Channel peripheral address register */
    __io uint32 CMAR;          /**< Channel memory address register */
} dma_channel_reg_map;

#define DMA_CHANNEL_NREGS 5

/**
 * @brief Obtain a pointer to an individual DMA channel's registers.
 *
 * For example, dma_channel_regs(DMA1, DMA_CH1)->CCR is DMA1_BASE->CCR1.
 *
 * @param dev DMA device
 * @param channel DMA channel whose channel register map to obtain.
 */
static inline dma_channel_reg_map* dma_channel_regs(dma_dev *dev,
                                                    dma_channel channel) {
    __io uint32 *ccr1 = &dev->regs->CCR1;
    return (dma_channel_reg_map*)(ccr1 + DMA_CHANNEL_NREGS * (channel - 1));
}

/**
 * @brief Check if a DMA channel is enabled
 * @param dev DMA device
 * @param channel Channel whose enabled bit to check.
 */
static inline uint8 dma_is_channel_enabled(dma_dev *dev, dma_channel channel) {
    return (uint8)(dma_channel_regs(dev, channel)->CCR & DMA_CCR_EN);
}

/**
 * @brief Get the ISR status bits for a DMA channel.
 *
 * The bits are returned right-aligned, in the following order:
 * transfer error flag, half-transfer flag, transfer complete flag,
 * global interrupt flag.
 *
 * If you're attempting to figure out why a DMA interrupt fired; you
 * may find dma_get_irq_cause() more convenient.
 *
 * @param dev DMA device
 * @param channel Channel whose ISR bits to return.
 * @see dma_get_irq_cause().
 */
static inline uint8 dma_get_isr_bits(dma_dev *dev, dma_channel channel) {
    uint8 shift = (channel - 1) * 4;
    return (dev->regs->ISR >> shift) & 0xF;
}

/**
 * @brief Clear the ISR status bits for a given DMA channel.
 *
 * If you're attempting to clean up after yourself in a DMA interrupt,
 * you may find dma_get_irq_cause() more convenient.
 *
 * @param dev DMA device
 * @param channel Channel whose ISR bits to clear.
 * @see dma_get_irq_cause()
 */
static inline void dma_clear_isr_bits(dma_dev *dev, dma_channel channel) {
    dev->regs->IFCR = BIT(4 * (channel - 1));
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
