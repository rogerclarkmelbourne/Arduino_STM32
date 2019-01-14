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
 * @file dmaF4.h
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
 * @brief DMA stream type.
 *
 */
typedef struct dma_stream_t {
    __IO uint32 CR;           /**< Stream configuration register */
    __IO uint32 NDTR;         /**< Stream number of data register */
    __IO uint32 PAR;          /**< Stream peripheral address register */
    __IO uint32 M0AR;         /**< Stream memory address register 0 */
    __IO uint32 M1AR;         /**< Stream memory address register 1 */
    __IO uint32 FCR;          /**< Stream FIFO configuration register */
} dma_stream_t;
/**
 * @brief DMA register map type.
 *
 */
typedef struct dma_reg_map {
    __IO uint32  LISR;         /**< Low interrupt status register */
    __IO uint32  HISR;         /**< High interrupt status register */
    __IO uint32  LIFCR;        /**< Low interrupt flag clear register */
    __IO uint32  HIFCR;        /**< High interrupt flag clear register */
    dma_stream_t STREAM[8];
} dma_reg_map;

/** DMA controller register map base pointers */
#define DMA1_BASE                       ((struct dma_reg_map*)0x40026000)
#define DMA2_BASE                       ((struct dma_reg_map*)0x40026400)


/*
 * Register bit definitions
 */

// Stream configuration register
#define DMA_CR_CH0                      (0x0 << 25)
#define DMA_CR_CH1                      (0x1 << 25)
#define DMA_CR_CH2                      (0x2 << 25)
#define DMA_CR_CH3                      (0x3 << 25)
#define DMA_CR_CH4                      (0x4 << 25)
#define DMA_CR_CH5                      (0x5 << 25)
#define DMA_CR_CH6                      (0x6 << 25)
#define DMA_CR_CH7                      (0x7 << 25)
#define DMA_CR_MBURST0                  (0x0 << 23)
#define DMA_CR_MBURST4                  (0x1 << 23)
#define DMA_CR_MBURST8                  (0x2 << 23)
#define DMA_CR_MBURST16                 (0x3 << 23)
#define DMA_CR_PBURST0                  (0x0 << 21)
#define DMA_CR_PBURST4                  (0x1 << 21)
#define DMA_CR_PBURST8                  (0x2 << 21)
#define DMA_CR_PBURST16                 (0x3 << 21)
#define DMA_CR_CT0                      (0x0 << 19)
#define DMA_CR_CT1                      (0x1 << 19)
#define DMA_CR_DBM                      (0x1 << 18)

#define DMA_CR_PL_LOW                   (0x0 << 16)
#define DMA_CR_PL_MEDIUM                (0x1 << 16)
#define DMA_CR_PL_HIGH                  (0x2 << 16)
#define DMA_CR_PL_VERY_HIGH             (0x3 << 16)
#define DMA_CR_PL_MASK                  (0x3 << 16)

#define DMA_CR_PINCOS                   (0x1 << 15)

#define DMA_CR_MSIZE_8BITS              (0x0 << 13)
#define DMA_CR_MSIZE_16BITS             (0x1 << 13)
#define DMA_CR_MSIZE_32BITS             (0x2 << 13)

#define DMA_CR_PSIZE_8BITS              (0x0 << 11)
#define DMA_CR_PSIZE_16BITS             (0x1 << 11)
#define DMA_CR_PSIZE_32BITS             (0x2 << 11)

#define DMA_CR_MINC                     (0x1 << 10)
#define DMA_CR_PINC                     (0x1 << 9)
#define DMA_CR_CIRC                     (0x1 << 8)
#define DMA_CR_DIR_P2M                  (0x0 << 6)
#define DMA_CR_DIR_M2P                  (0x1 << 6)
#define DMA_CR_DIR_M2M                  (0x2 << 6)

#define DMA_CR_PFCTRL                   (0x1 << 5)
#define DMA_CR_TCIE                     (0x1 << 4)
#define DMA_CR_HTIE                     (0x1 << 3)
#define DMA_CR_TEIE                     (0x1 << 2)
#define DMA_CR_DMEIE                    (0x1 << 1)
#define DMA_CR_EN                       (0x1 << 0)

// Device interrupt status register flags
#define DMA_ISR_TCIF    (1 << 5)
#define DMA_ISR_HTIF    (1 << 4)
#define DMA_ISR_TEIF    (1 << 3)
#define DMA_ISR_DMEIF   (1 << 2)
#define DMA_ISR_FEIF    (1 << 0)

#define DMA_ISR_ERROR_BITS (DMA_ISR_TEIF | DMA_ISR_DMEIF | DMA_ISR_FEIF)
#define DMA_ISR_BIT_MASK 0x3D

// FIFO control register flags
#define DMA_FCR_FEIE                   (0x1 << 7) // FIFO error interrupt enable
#define DMA_FCR_FS                     (0x7 << 3) // FIFO status (READ_ONLY):
#define DMA_FCR_FS_LEVEL_1             (0x0 << 3) //  000: 0 < fifo_level < 1/4
#define DMA_FCR_FS_LEVEL_2             (0x1 << 3) //  001: 1/4 ≤ fifo_level < 1/2
#define DMA_FCR_FS_LEVEL_3             (0x2 << 3) //  010: 1/2 ≤ fifo_level < 3/4
#define DMA_FCR_FS_LEVEL_4             (0x3 << 3) //  011: 3/4 ≤ fifo_level < full
#define DMA_FCR_FS_EMPTY               (0x4 << 3) //  100: FIFO is empty
#define DMA_FCR_FS_FULL                (0x5 << 3) //  101: FIFO is full
#define DMA_FCR_DMDIS                  (0x1 << 2) // Direct mode disable
#define DMA_FCR_FTH                    (0x3 << 0) // FIFO threshold selection
#define DMA_FCR_FTH_1_4                (0x0 << 0) //  1/4 full FIFO
#define DMA_FCR_FTH_2_4                (0x1 << 0) //  2/4 full FIFO
#define DMA_FCR_FTH_3_4                (0x2 << 0) //  3/4 full FIFO
#define DMA_FCR_FTH_FULL               (0x3 << 0) //  full FIFO


typedef enum dma_channel {
    DMA_CH0 = DMA_CR_CH0,                /**< Channel 0 */
    DMA_CH1 = DMA_CR_CH1,                /**< Channel 1 */
    DMA_CH2 = DMA_CR_CH2,                /**< Channel 2 */
    DMA_CH3 = DMA_CR_CH3,                /**< Channel 3 */
    DMA_CH4 = DMA_CR_CH4,                /**< Channel 4 */
    DMA_CH5 = DMA_CR_CH5,                /**< Channel 5 */
    DMA_CH6 = DMA_CR_CH6,                /**< Channel 6 */
    DMA_CH7 = DMA_CR_CH7,                /**< Channel 7 */
} dma_channel;

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
                                    * @see dma_detach_interrupt()
                                    */
} dma_dev;

/*
 * Devices
 */
extern dma_dev *DMA1;
extern dma_dev *DMA2;

/** Flags for DMA transfer configuration. */
typedef enum dma_mode_flags {
    DMA_MEM_BUF_0      = DMA_CR_CT0, /**< Current memory target buffer 0 */
    DMA_MEM_BUF_1      = DMA_CR_CT1, /**< Current memory target buffer 1 */
    DMA_DBL_BUF_MODE   = DMA_CR_DBM,  /**< Current memory double buffer mode */
    DMA_PINC_OFFSET    = DMA_CR_PINCOS,  /**< Peripheral increment offset size */
    DMA_MINC_MODE      = DMA_CR_MINC,  /**< Memory increment mode */
    DMA_PINC_MODE      = DMA_CR_PINC,  /**< Peripheral increment mode */
    DMA_CIRC_MODE      = DMA_CR_CIRC,  /**< Memory Circular mode */
    DMA_FROM_PER       = DMA_CR_DIR_P2M,  /**< Read from memory to peripheral */
    DMA_FROM_MEM       = DMA_CR_DIR_M2P,  /**< Read from memory to peripheral */
    DMA_MEM_TO_MEM     = DMA_CR_DIR_M2M,  /**< Read from memory to memory */
    DMA_PERIF_CTRL     = DMA_CR_PFCTRL,  /**< Peripheral flow controller */
    DMA_PRIO_MEDIUM    = DMA_CR_PL_MEDIUM,    /**< Medium priority */
    DMA_PRIO_HIGH      = DMA_CR_PL_HIGH,      /**< High priority */
    DMA_PRIO_VERY_HIGH = DMA_CR_PL_VERY_HIGH, /**< Very high priority */
    DMA_TRNS_CMPLT     = DMA_CR_TCIE,   /**< Interrupt on transfer completion */
    DMA_TRNS_HALF      = DMA_CR_HTIE,  /**< Interrupt on half-transfer */
    DMA_TRNS_ERR       = DMA_CR_TEIE,  /**< Interrupt on transfer error */
    DMA_DIR_MODE_ERR   = DMA_CR_DMEIE  /**< Interrupt on direct mode error */
} dma_mode_flags;

// Source and destination transfer sizes.
typedef enum dma_xfer_size {
    DMA_SIZE_8BITS  = ( DMA_CR_MSIZE_8BITS|DMA_CR_PSIZE_8BITS ),  // 8-bit transfers
    DMA_SIZE_16BITS = (DMA_CR_MSIZE_16BITS|DMA_CR_PSIZE_16BITS),  // 16-bit transfers
    DMA_SIZE_32BITS = (DMA_CR_MSIZE_32BITS|DMA_CR_PSIZE_32BITS)   // 32-bit transfers
} dma_xfer_size;

// Source and destination burst sizes.
typedef enum dma_burst_size {
    DMA_BURST_INCR0  = ( DMA_CR_MBURST0|DMA_CR_PBURST0 ),  // single transfer
    DMA_BURST_INCR4  = ( DMA_CR_MBURST4|DMA_CR_PBURST4 ),  // incremental burst of 4 beats
    DMA_BURST_INCR8  = ( DMA_CR_MBURST8|DMA_CR_PBURST8 ),  // incremental burst of 8 beats
    DMA_BURST_INCR16 = (DMA_CR_MBURST16|DMA_CR_PBURST16)   // incremental burst of 16 beats
} dma_burst_size;

/** DMA channel */
typedef enum dma_stream {
    DMA_STREAM0 = 0,                /**< Stream 0 */
    DMA_STREAM1 = 1,                /**< Stream 1 */
    DMA_STREAM2 = 2,                /**< Stream 2 */
    DMA_STREAM3 = 3,                /**< Stream 3 */
    DMA_STREAM4 = 4,                /**< Stream 4 */
    DMA_STREAM5 = 5,                /**< Stream 5 */
    DMA_STREAM6 = 6,                /**< Stream 6 */
    DMA_STREAM7 = 7,                /**< Stream 7 */
} dma_stream;

/*
 * Convenience functions
 */
extern void dma_init(dma_dev *dev);

static inline void dma_setup_transfer(dma_dev       *dev,
                                      dma_stream    stream,
                                      dma_channel   channel,
                                      dma_xfer_size trx_size,
                                      __IO void     *peripheral_address,
                                      __IO void     *memory_address0,
                                      __IO void     *memory_address1,
                                      uint32        flags)
{
    dev->regs->STREAM[stream].CR &= ~DMA_CR_EN; // disable
	while( (dev->regs->STREAM[stream].CR)&DMA_CR_EN ); // wait till enable bit is cleared
    dev->regs->STREAM[stream].PAR = (uint32)peripheral_address;
    dev->regs->STREAM[stream].M0AR = (uint32)memory_address0;
    dev->regs->STREAM[stream].M1AR = (uint32)memory_address1;
    dev->regs->STREAM[stream].CR = (uint32)((flags|channel|trx_size) & 0x0feffffe); // mask out reserved and enable
}
    
static inline void dma_set_num_transfers(dma_dev *dev, dma_stream stream, uint16 num_transfers)
{
    dev->regs->STREAM[stream].NDTR = (uint32)num_transfers;
}

static inline void dma_set_fifo_flags(dma_dev *dev, dma_stream stream, uint8 fifo_flags)
{
    dev->regs->STREAM[stream].FCR = (uint32)(fifo_flags & 0x87); // mask out reserved bits
}

void dma_attach_interrupt(dma_dev *dev,
                          dma_stream stream,
                          void (*handler)(void));

void dma_detach_interrupt(dma_dev *dev, dma_stream stream);

static inline void dma_enable(dma_dev *dev, dma_stream stream)
{
    dev->regs->STREAM[stream].CR |= (uint32)DMA_CR_EN;
}    

static inline void dma_disable(dma_dev *dev, dma_stream stream)
{
    dev->regs->STREAM[stream].CR &= (uint32)(~DMA_CR_EN);
	while (dev->regs->STREAM[stream].CR & DMA_CR_EN); // wait till EN bit is reset, see AN4031, chapter 4.1
}    

/**
 * @brief Check if a DMA stream is enabled
 * @param dev DMA device
 * @param stream Stream whose enabled bit to check.
 */
static inline uint8 dma_is_stream_enabled(dma_dev *dev, dma_stream stream) {
    return (uint8)(dev->regs->STREAM[stream].CR & DMA_CR_EN);
}

/**
 * @brief Get the ISR status bits for a DMA stream.
 *
 * The bits are returned right-aligned, in the following order:
 * transfer error flag, half-transfer flag, transfer complete flag,
 * global interrupt flag.
 *
 * @param dev DMA device
 * @param stream Stream whose ISR bits to return.
 */
uint8 dma_get_isr_bit(dma_dev *dev, dma_stream stream, uint8_t mask);

static inline uint8 dma_get_isr_bits(dma_dev *dev, dma_stream stream) {
	return dma_get_isr_bit(dev, stream, DMA_ISR_BIT_MASK);
}

/**
 * @brief Clear the ISR status bits for a given DMA stream.
 *
 * @param dev DMA device
 * @param stream Stream whose ISR bits to clear.
 */
void dma_clear_isr_bit(dma_dev *dev, dma_stream stream, uint8_t mask);

static inline void dma_clear_isr_bits(dma_dev *dev, dma_stream stream) {
	dma_clear_isr_bit(dev, stream, DMA_ISR_BIT_MASK);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
