/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Michael Hope.
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
 * @file libmaple/include/libmaple/dma.h
 *
 * @author Marti Bolivar <mbolivar@leaflabs.com>;
 *         Original implementation by Michael Hope
 *
 * @brief Direct Memory Access peripheral support
 */

#ifndef _LIBMAPLE_DMA_H_
#define _LIBMAPLE_DMA_H_

#ifdef __cplusplus
extern "C"{
#endif

/* <series/dma.h> provides:
 *
 * - An opaque dma_tube type, and predefined rvalues for each tube
 *   supported by the series.
 *
 *   A "DMA tube" is a series-specific (hopefully integer) datatype
 *   that abstracts the conduit through which DMA-ed data flow.
 *
 *   Examples: On STM32F1, dma_tube is just an alias for dma_channel,
 *   and the tube values are just DMA_CH1 (=1), DMA_CH2 (=2), etc.
 *
 *   Note that a dma_tube doesn't have to be an enum, and its values
 *   don't have to be integral. They _do_ need to be cheap to pass as
 *   arguments, though.
 *
 * - struct dma_tube_reg_map (and typedef to dma_tube_reg_map). DMA
 *   register maps tend to be split into global registers and per-tube
 *   registers. It's convenient to pass around pointers to a tube's
 *   registers, since that makes it possible to configure or otherwise
 *   mess with a tube without knowing which one you're dealing with.
 *
 * - Base pointers to the various dma_tube_reg_maps.
 *
 *   Examples: On STM32F1, these are DMAxCHy_BASE. You can access
 *   registers like DMAxCHy_BASE->CPAR, etc.
 *
 * - enum dma_request_src (and typedef to dma_request_src). This
 *   specifies the peripheral DMA request sources (e.g. USART TX DMA
 *   requests, etc.).
 *
 * - enum dma_mode_flags (and typedef to dma_mode_flags). Used in
 *   dma_tube_config. If two series both support the same mode flags,
 *   they must use the same enumerator names for those flags (the
 *   values of those enumerators are of course allowed to differ).
 *
 * - Normal stuff: dma_reg_map and base pointers, register bit
 *   definitions, dma_dev pointer declarations, and any other
 *   convenience functions useful for the series. */

 /* Roger clark. Replaced with line below #include <series/dma.h>*/
#include "stm32f1/include/series/dma.h"

/* <libmaple/dma_common.h> buys us dma_dev and other necessities. */
#include <libmaple/dma_common.h>
#include <libmaple/libmaple_types.h>

/*
 * Declarations/documentation for some of the series-provided types.
 */

/**
 * @brief (Series-dependent) DMA request sources.
 *
 * These specify the various pieces of peripheral functionality which
 * may make DMA requests.  Use them to set up a DMA transfer (see
 * struct dma_tube_config, dma_tube_cfg()).
 */
enum dma_request_src;

/**
 * @brief (Series-dependent) DMA tube configuration flags.
 * These specify miscellaneous bits of configuration for a DMA tube.
 * @see struct dma_mode_config
 */
enum dma_cfg_flags;

/**
 * @brief (Series-dependent) DMA tube register map type.
 * This allows you to access a tube's registers as a group.
 * @see dma_tube_regs()
 */
struct dma_tube_reg_map;

/*
 * Convenience functions
 */

/* Initialization */

void dma_init(dma_dev *dev);

/* dma_tube configuration
 *
 * Use these types and functions to set up DMA transfers, handle
 * interrupts, etc. The main function of interest is dma_tube_cfg(),
 * which the various series implement separately. */

/**
 * @brief Specifies a DMA tube configuration.
 *
 * Use one of these to set up a DMA transfer by passing it to
 * dma_tube_cfg().
 *
 * @see dma_tube_cfg()
 * @see dma_xfer_size
 */
typedef struct dma_tube_config {
    /** Source of data */
    __IO void     *tube_src;
    /** Source transfer size */
    dma_xfer_size  tube_src_size;

    /** Destination of data */
    __IO void     *tube_dst;
    /** Destination transfer size */
    dma_xfer_size  tube_dst_size;

    /**
     * Number of data to transfer (0 to 65,535).
     *
     * Note that this is NOT measured in bytes; it's measured in
     * number of data, which occur in multiples of tube_src_size. For
     * example, if tube_src_size is DMA_SIZE_32BITS and tube_nr_xfers
     * is 2, then 8 total bytes will be transferred.
     */
    unsigned tube_nr_xfers;

    /**
     * Target-specific configuration flags.
     *
     * These are an OR of series-specific enum dma_mode_flags values.
     * Consult the documentation for your target for what flags you
     * can use here.
     *
     * Typical flag examples: DMA_CFG_SRC_INC, DMA_CFG_DST_INC,
     * DMA_CFG_CIRC, DMA_CFG_CMPLT_IE, etc.
     */
    unsigned tube_flags;

    /**
     * Currently unused. You must set this to 0 or something valid for
     * your target. */
    void *target_data;

    /**
     * Hardware DMA request source.
     *
     * This is ignored for memory-to-memory transfers.
     */
    enum dma_request_src tube_req_src;
} dma_tube_config;

#define DMA_TUBE_CFG_SUCCESS 0
#define DMA_TUBE_CFG_EREQ    1
#define DMA_TUBE_CFG_ENDATA  2
#define DMA_TUBE_CFG_EDEV    3
#define DMA_TUBE_CFG_ESRC    4
#define DMA_TUBE_CFG_EDST    5
#define DMA_TUBE_CFG_EDIR    6
#define DMA_TUBE_CFG_ESIZE   7
#define DMA_TUBE_CFG_ECFG    0xFF
/**
 * @brief Configure a DMA tube.
 *
 * Use this function to set up a DMA transfer. The tube will be
 * disabled before being reconfigured. The transfer will have low
 * priority by default. You can choose another priority before the
 * transfer begins using dma_set_priority(). You can manage your
 * interrupt handlers for the tube using dma_attach_interrupt() and
 * dma_detach_interrupt().
 *
 * After calling dma_tube_cfg() and performing any other desired
 * configuration, start the transfer using dma_enable().
 *
 * @param dev  DMA device.
 * @param tube DMA tube to configure.
 * @param cfg  Configuration to apply to tube.
 *
 * @return DMA_TUBE_CFG_SUCCESS (0) on success, <0 on failure. On
 *         failure, returned value will be the opposite (-) of one of:
 *
 *         - DMA_TUBE_CFG_EREQ: tube doesn't work with cfg->tube_req_src
 *         - DMA_TUBE_CFG_ENDATA: cfg->tube_[src,dst]_size are
 *           incompatible with cfg->tube_nr_xfers, or cfg->tube_nr_xfers
 *           is out of bounds.
 *         - DMA_TUBE_CFG_EDEV: dev does not support cfg
 *         - DMA_TUBE_CFG_ESRC: bad cfg->tube_src
 *         - DMA_TUBE_CFG_EDST: bad cfg->tube_dst
 *         - DMA_TUBE_CFG_EDIR: dev can't transfer from cfg->tube_src to
 *           cfg->tube_dst
 *         - DMA_TUBE_CFG_ESIZE: something ended up wrong due to MSIZE/PSIZE
 *         - DMA_TUBE_CFG_ECFG: generic "something's wrong"
 *
 * @sideeffect Disables tube. May alter tube's registers even when an
 *             error occurs.
 * @see struct dma_tube_config
 * @see dma_attach_interrupt()
 * @see dma_detach_interrupt()
 * @see dma_enable()
 */
extern int dma_tube_cfg(dma_dev *dev, dma_tube tube, dma_tube_config *cfg);

/* Other tube configuration functions. You can use these if
 * dma_tube_cfg() isn't enough, or to adjust parts of an existing tube
 * configuration. */

/** DMA transfer priority. */
typedef enum dma_priority {
    DMA_PRIORITY_LOW       = 0, /**< Low priority */
    DMA_PRIORITY_MEDIUM    = 1, /**< Medium priority */
    DMA_PRIORITY_HIGH      = 2, /**< High priority */
    DMA_PRIORITY_VERY_HIGH = 3, /**< Very high priority */
} dma_priority;

/**
 * @brief Set the priority of a DMA transfer.
 *
 * You may not call this function while the tube is enabled.
 *
 * @param dev DMA device
 * @param tube DMA tube
 * @param priority priority to set.
 */
extern void dma_set_priority(dma_dev *dev, dma_tube tube,
                             dma_priority priority);

/**
 * @brief Set the number of data transfers on a DMA tube.
 *
 * You may not call this function while the tube is enabled.
 *
 * @param dev DMA device
 * @param tube Tube through which the transfer will occur.
 * @param num_transfers Number of DMA transactions to set.
 */
extern void dma_set_num_transfers(dma_dev *dev, dma_tube tube,
                                  uint16 num_transfers);

/**
 * @brief Set the base memory address where data will be read from or
 *        written to.
 *
 * You must not call this function while the tube is enabled.
 *
 * If the DMA memory size is 16 bits, the address is automatically
 * aligned to a half-word.  If the DMA memory size is 32 bits, the
 * address is aligned to a word.
 *
 * @param dev DMA Device
 * @param tube Tube whose base memory address to set.
 * @param address Memory base address to use.
 */
extern void dma_set_mem_addr(dma_dev *dev, dma_tube tube, __IO void *address);

/**
 * @brief Set the base peripheral address where data will be read from
 *        or written to.
 *
 * You must not call this function while the channel is enabled.
 *
 * If the DMA peripheral size is 16 bits, the address is automatically
 * aligned to a half-word. If the DMA peripheral size is 32 bits, the
 * address is aligned to a word.
 *
 * @param dev DMA Device
 * @param tube Tube whose peripheral data register base address to set.
 * @param address Peripheral memory base address to use.
 */
extern void dma_set_per_addr(dma_dev *dev, dma_tube tube, __IO void *address);

/* Interrupt handling */

/**
 * @brief Attach an interrupt to a DMA transfer.
 *
 * Interrupts are enabled using series-specific mode flags in
 * dma_tube_cfg().
 *
 * @param dev DMA device
 * @param tube Tube to attach handler to
 * @param handler Interrupt handler to call when tube interrupt fires.
 * @see dma_tube_cfg()
 * @see dma_get_irq_cause()
 * @see dma_detach_interrupt()
 */
extern void dma_attach_interrupt(dma_dev *dev, dma_tube tube,
                                 void (*handler)(void));


/**
 * @brief Detach a DMA transfer interrupt handler.
 *
 * After calling this function, the given tube's interrupts will be
 * disabled.
 *
 * @param dev DMA device
 * @param tube Tube whose handler to detach
 * @sideeffect Clears the tube's interrupt enable bits.
 * @see dma_attach_interrupt()
 */
extern void dma_detach_interrupt(dma_dev *dev, dma_tube tube);

/* Tube enable/disable */

/**
 * @brief Enable a DMA tube.
 *
 * If the tube has been properly configured, calling this function
 * allows it to start serving DMA requests.
 *
 * @param dev DMA device
 * @param tube Tube to enable
 * @see dma_tube_cfg()
 */
extern void dma_enable(dma_dev *dev, dma_tube tube);

/**
 * @brief Disable a DMA channel.
 *
 * Calling this function makes the tube stop serving DMA requests.
 *
 * @param dev DMA device
 * @param tube Tube to disable
 */
extern void dma_disable(dma_dev *dev, dma_tube tube);

/**
 * @brief Check if a DMA tube is enabled.
 * @param dev DMA device.
 * @param tube Tube to check.
 * @return 0 if the tube is disabled, >0 if it is enabled.
 */
static inline uint8 dma_is_enabled(dma_dev *dev, dma_tube tube);

/* Other conveniences */

/**
 * @brief Obtain a pointer to an individual DMA tube's registers.
 *
 * Examples:
 *
 * - On STM32F1, dma_channel_regs(DMA1, DMA_CH1)->CCR is DMA1_BASE->CCR1.
 *
 * @param dev DMA device.
 * @param tube DMA tube whose register map to obtain.
 * @return (Series-specific) tube register map.
 */
static inline dma_tube_reg_map* dma_tube_regs(dma_dev *dev, dma_tube tube);

/**
 * Encodes the reason why a DMA interrupt was called.
 * @see dma_get_irq_cause()
 */
typedef enum dma_irq_cause {
    DMA_TRANSFER_COMPLETE,      /**< Transfer is complete. */
    DMA_TRANSFER_HALF_COMPLETE, /**< Transfer is half complete. */
    DMA_TRANSFER_ERROR,         /**< Error occurred during transfer. */
    DMA_TRANSFER_DME_ERROR,     /**<
                                 * @brief Direct mode error occurred during
                                 *        transfer. */
    DMA_TRANSFER_FIFO_ERROR,    /**< FIFO error occurred during transfer. */
} dma_irq_cause;

/**
 * @brief Discover the reason why a DMA interrupt was called.
 *
 * You may only call this function within an attached interrupt
 * handler for the given channel.
 *
 * This function resets the internal DMA register state which encodes
 * the cause of the interrupt; consequently, it can only be called
 * once per interrupt handler invocation.
 *
 * @param dev DMA device
 * @param tube Tube whose interrupt is being handled.
 * @return Reason why the interrupt fired.
 * @sideeffect Clears flags in dev's interrupt status registers.
 * @see dma_attach_interrupt()
 * @see dma_irq_cause
 */
extern dma_irq_cause dma_get_irq_cause(dma_dev *dev, dma_tube tube);

/**
 * @brief Get the ISR status bits for a DMA channel.
 *
 * The bits are returned right-aligned, in the order they appear in
 * the corresponding ISR register.
 *
 * If you're trying to figure out why a DMA interrupt fired, you may
 * find dma_get_irq_cause() more convenient.
 *
 * @param dev DMA device
 * @param tube Tube whose ISR bits to return.
 * @see dma_get_irq_cause().
 */
static inline uint8 dma_get_isr_bits(dma_dev *dev, dma_tube tube);

/**
 * @brief Clear the ISR status bits for a given DMA tube.
 *
 * If you're trying to clean up after yourself in a DMA interrupt, you
 * may find dma_get_irq_cause() more convenient.
 *
 * @param dev DMA device
 * @param tube Tube whose ISR bits to clear.
 * @see dma_get_irq_cause()
 */
static inline void dma_clear_isr_bits(dma_dev *dev, dma_tube tube);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
