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

#ifdef STM32F1

/**
 * @file dma.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>;
 *         Original implementation by Michael Hope
 * @brief Direct Memory Access peripheral support
 */

#include "dma.h"
#include "bitband.h"
#include "util.h"

/*
 * Devices
 */

static dma_dev dma1 = {
    .regs     = DMA1_BASE,
    .clk_id   = RCC_DMA1,
    .handlers = {{ .handler = NULL, .irq_line = NVIC_DMA_CH1 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH2 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH3 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH4 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH5 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH6 },
                 { .handler = NULL, .irq_line = NVIC_DMA_CH7 }}
};
/** DMA1 device */
dma_dev *DMA1 = &dma1;

#ifdef STM32_HIGH_DENSITY
static dma_dev dma2 = {
    .regs     = DMA2_BASE,
    .clk_id   = RCC_DMA2,
    .handlers = {{ .handler = NULL, .irq_line = NVIC_DMA2_CH1   },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH2   },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH3   },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH_4_5 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH_4_5 }} /* !@#$ */
};
/** DMA2 device */
dma_dev *DMA2 = &dma2;
#endif

/*
 * Convenience routines
 */

/**
 * @brief Initialize a DMA device.
 * @param dev Device to initialize.
 */
void dma_init(dma_dev *dev) {
    rcc_clk_enable(dev->clk_id);
}

/**
 * @brief Set up a DMA transfer.
 *
 * The channel will be disabled before being reconfigured.  The
 * transfer will have low priority by default.  You may choose another
 * priority before the transfer begins using dma_set_priority(), as
 * well as performing any other configuration you desire.  When the
 * channel is configured to your liking, enable it using dma_enable().
 *
 * @param dev DMA device.
 * @param channel DMA channel.
 * @param peripheral_address Base address of peripheral data register
 *                           involved in the transfer.
 * @param peripheral_size Peripheral data transfer size.
 * @param memory_address Base memory address involved in the transfer.
 * @param memory_size Memory data transfer size.
 * @param mode Logical OR of dma_mode_flags
 * @sideeffect Disables the given DMA channel.
 * @see dma_xfer_size
 * @see dma_mode_flags
 * @see dma_set_num_transfers()
 * @see dma_set_priority()
 * @see dma_attach_interrupt()
 * @see dma_enable()
 */
void dma_setup_transfer(dma_dev       *dev,
                        dma_channel    channel,
                        __io void     *peripheral_address,
                        dma_xfer_size  peripheral_size,
                        __io void     *memory_address,
                        dma_xfer_size  memory_size,
                        uint32         mode) {
    dma_channel_reg_map *channel_regs = dma_channel_regs(dev, channel);

    dma_disable(dev, channel);  /* can't write to CMAR/CPAR otherwise */
    channel_regs->CCR = (memory_size << 10) | (peripheral_size << 8) | mode;
    channel_regs->CMAR = (uint32)memory_address;
    channel_regs->CPAR = (uint32)peripheral_address;
}

/**
 * @brief Set the number of data to be transferred on a DMA channel.
 *
 * You may not call this function while the channel is enabled.
 *
 * @param dev DMA device
 * @param channel Channel through which the transfer occurs.
 * @param num_transfers
 */
void dma_set_num_transfers(dma_dev *dev,
                           dma_channel channel,
                           uint16 num_transfers) {
    dma_channel_reg_map *channel_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    channel_regs = dma_channel_regs(dev, channel);
    channel_regs->CNDTR = num_transfers;
}

/**
 * @brief Set the priority of a DMA transfer.
 *
 * You may not call this function while the channel is enabled.
 *
 * @param dev DMA device
 * @param channel DMA channel
 * @param priority priority to set.
 */
void dma_set_priority(dma_dev *dev,
                      dma_channel channel,
                      dma_priority priority) {
    dma_channel_reg_map *channel_regs;
    uint32 ccr;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    channel_regs = dma_channel_regs(dev, channel);
    ccr = channel_regs->CCR;
    ccr &= ~DMA_CCR_PL;
    ccr |= priority;
    channel_regs->CCR = ccr;
}

/**
 * @brief Attach an interrupt to a DMA transfer.
 *
 * Interrupts are enabled using appropriate mode flags in
 * dma_setup_transfer().
 *
 * @param dev DMA device
 * @param channel Channel to attach handler to
 * @param handler Interrupt handler to call when channel interrupt fires.
 * @see dma_setup_transfer()
 * @see dma_get_irq_cause()
 * @see dma_detach_interrupt()
 */
void dma_attach_interrupt(dma_dev *dev,
                          dma_channel channel,
                          void (*handler)(void)) {
    dev->handlers[channel - 1].handler = handler;
    nvic_irq_enable(dev->handlers[channel - 1].irq_line);
}

/**
 * @brief Detach a DMA transfer interrupt handler.
 *
 * After calling this function, the given channel's interrupts will be
 * disabled.
 *
 * @param dev DMA device
 * @param channel Channel whose handler to detach
 * @sideeffect Clears interrupt enable bits in the channel's CCR register.
 * @see dma_attach_interrupt()
 */
void dma_detach_interrupt(dma_dev *dev, dma_channel channel) {
    /* Don't use nvic_irq_disable()! Think about DMA2 channels 4 and 5. */
    dma_channel_regs(dev, channel)->CCR &= ~0xF;
    dev->handlers[channel - 1].handler = NULL;
}

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
 * @param channel Channel whose interrupt is being handled.
 * @return Reason why the interrupt fired.
 * @sideeffect Clears channel status flags in dev->regs->ISR.
 * @see dma_attach_interrupt()
 * @see dma_irq_cause
 */
dma_irq_cause dma_get_irq_cause(dma_dev *dev, dma_channel channel) {
    uint8 status_bits = dma_get_isr_bits(dev, channel);

    /* If the channel global interrupt flag is cleared, then
     * something's very wrong. */
    ASSERT(status_bits & BIT(0));

    dma_clear_isr_bits(dev, channel);

    /* ISR flags get set even if the corresponding interrupt enable
     * bits in the channel's configuration register are cleared, so we
     * can't use a switch here.
     *
     * Don't change the order of these if statements. */
    if (status_bits & BIT(3)) {
        return DMA_TRANSFER_ERROR;
    } else if (status_bits & BIT(1)) {
        return DMA_TRANSFER_COMPLETE;
    } else if (status_bits & BIT(2)) {
        return DMA_TRANSFER_HALF_COMPLETE;
    } else if (status_bits & BIT(0)) {
        /* Shouldn't happen (unless someone messed up an IFCR write). */
        throb();
    }
#if DEBUG_LEVEL < DEBUG_ALL
    else {
        /* We shouldn't have been called, but the debug level is too
         * low for the above ASSERT() to have had any effect.  In
         * order to fail fast, mimic the DMA controller's behavior
         * when an error occurs. */
        dma_disable(dev, channel);
    }
#endif
    return DMA_TRANSFER_ERROR;
}

/**
 * @brief Enable a DMA channel.
 * @param dev DMA device
 * @param channel Channel to enable
 */
void dma_enable(dma_dev *dev, dma_channel channel) {
    dma_channel_reg_map *chan_regs = dma_channel_regs(dev, channel);
    bb_peri_set_bit(&chan_regs->CCR, DMA_CCR_EN_BIT, 1);
}

/**
 * @brief Disable a DMA channel.
 * @param dev DMA device
 * @param channel Channel to disable
 */
void dma_disable(dma_dev *dev, dma_channel channel) {
    dma_channel_reg_map *chan_regs = dma_channel_regs(dev, channel);
    bb_peri_set_bit(&chan_regs->CCR, DMA_CCR_EN_BIT, 0);
}

/**
 * @brief Set the base memory address where data will be read from or
 *        written to.
 *
 * You must not call this function while the channel is enabled.
 *
 * If the DMA memory size is 16 bits, the address is automatically
 * aligned to a half-word.  If the DMA memory size is 32 bits, the
 * address is aligned to a word.
 *
 * @param dev DMA Device
 * @param channel Channel whose base memory address to set.
 * @param addr Memory base address to use.
 */
void dma_set_mem_addr(dma_dev *dev, dma_channel channel, __io void *addr) {
    dma_channel_reg_map *chan_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    chan_regs = dma_channel_regs(dev, channel);
    chan_regs->CMAR = (uint32)addr;
}

/**
 * @brief Set the base peripheral address where data will be read from
 *        or written to.
 *
 * You must not call this function while the channel is enabled.
 *
 * If the DMA peripheral size is 16 bits, the address is automatically
 * aligned to a half-word.  If the DMA peripheral size is 32 bits, the
 * address is aligned to a word.
 *
 * @param dev DMA Device
 * @param channel Channel whose peripheral data register base address to set.
 * @param addr Peripheral memory base address to use.
 */
void dma_set_per_addr(dma_dev *dev, dma_channel channel, __io void *addr) {
    dma_channel_reg_map *chan_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    chan_regs = dma_channel_regs(dev, channel);
    chan_regs->CPAR = (uint32)addr;
}

/*
 * IRQ handlers
 */

static inline void dispatch_handler(dma_dev *dev, dma_channel channel) {
    void (*handler)(void) = dev->handlers[channel - 1].handler;
    if (handler) {
        handler();
        dma_clear_isr_bits(dev, channel); /* in case handler doesn't */
    }
}

void __irq_dma1_channel1(void) {
    dispatch_handler(DMA1, DMA_CH1);
}

void __irq_dma1_channel2(void) {
    dispatch_handler(DMA1, DMA_CH2);
}

void __irq_dma1_channel3(void) {
    dispatch_handler(DMA1, DMA_CH3);
}

void __irq_dma1_channel4(void) {
    dispatch_handler(DMA1, DMA_CH4);
}

void __irq_dma1_channel5(void) {
    dispatch_handler(DMA1, DMA_CH5);
}

void __irq_dma1_channel6(void) {
    dispatch_handler(DMA1, DMA_CH6);
}

void __irq_dma1_channel7(void) {
    dispatch_handler(DMA1, DMA_CH7);
}

#ifdef STM32_HIGH_DENSITY
void __irq_dma2_channel1(void) {
    dispatch_handler(DMA2, DMA_CH1);
}

void __irq_dma2_channel2(void) {
    dispatch_handler(DMA2, DMA_CH2);
}

void __irq_dma2_channel3(void) {
    dispatch_handler(DMA2, DMA_CH3);
}

void __irq_dma2_channel4_5(void) {
    dispatch_handler(DMA2, DMA_CH4);
    dispatch_handler(DMA2, DMA_CH5);
}
#endif

#endif