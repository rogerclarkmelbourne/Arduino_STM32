/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Michael Hope.
 * Copyright (c) 2012 LeafLabs, LLC.
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file libmaple/stm32f3/dma.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Original implementation by Michael Hope,
 *				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 DMA support.
 */

#include <libmaple/dma.h>
#include <libmaple/bitband.h>

/* Hack to ensure inlining in dma_irq_handler() */
#define DMA_GET_HANDLER(dev, tube) (dev->handlers[tube - 1].handler)
#include "dma_private.h"

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
                 { .handler = NULL, .irq_line = NVIC_DMA_CH7 }},
};
/** STM32F3 DMA1 device */
dma_dev *DMA1 = &dma1;

static dma_dev dma2 = {
    .regs     = DMA2_BASE,
    .clk_id   = RCC_DMA2,
    .handlers = {{ .handler = NULL, .irq_line = NVIC_DMA2_CH1 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH2 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH3 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH4 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_CH5 }},
};
/** STM32F3 DMA2 device */
dma_dev *DMA2 = &dma2;

/*
 * Auxiliary routines
 */

/* Can channel serve cfg->tube_req_src? */
static int cfg_req_ok(dma_channel channel, dma_tube_config *cfg) {
    return (cfg->tube_req_src & 0x7) == channel;
}

/* Can dev serve cfg->tube_req_src? */
static int cfg_dev_ok(dma_dev *dev, dma_tube_config *cfg) {
    return (rcc_clk_id)(cfg->tube_req_src >> 3) == dev->clk_id;
}

/* Is addr acceptable for use as DMA src/dst? */
static int cfg_mem_ok(__io void *addr) {
    enum dma_atype atype = _dma_addr_type(addr);
    return atype == DMA_ATYPE_MEM || atype == DMA_ATYPE_PER;
}

/* Is the direction implied by src->dst supported? */
static int cfg_dir_ok(dma_tube_config *cfg) {
    /* We can't do peripheral->peripheral transfers. */
    return ((_dma_addr_type(cfg->tube_src) == DMA_ATYPE_MEM) ||
            (_dma_addr_type(cfg->tube_dst) == DMA_ATYPE_MEM));
}

static int preconfig_check(dma_dev *dev, dma_channel channel,
                           dma_tube_config *cfg) {
    if (!cfg_req_ok(channel, cfg)) {
        return -DMA_TUBE_CFG_EREQ;
    }
    if (cfg->tube_nr_xfers > 65535) {
        return -DMA_TUBE_CFG_ENDATA;
    }
    if (!cfg_dev_ok(dev, cfg)) {
        return -DMA_TUBE_CFG_EDEV;
    }
    if (!cfg_mem_ok(cfg->tube_src)) {
        return -DMA_TUBE_CFG_ESRC;
    }
    if (!cfg_mem_ok(cfg->tube_dst)) {
        return -DMA_TUBE_CFG_EDST;
    }
    if (!cfg_dir_ok(cfg)) {
        return -DMA_TUBE_CFG_EDIR;
    }
    return DMA_TUBE_CFG_SUCCESS;
}

static inline void set_ccr(dma_tube_reg_map *chregs,
                           dma_xfer_size msize, int minc,
                           dma_xfer_size psize, int pinc,
                           uint32 other_flags) {
    chregs->CCR = ((msize << 10) | (psize << 8) |
                   (minc ? DMA_CCR_MINC : 0) | (pinc ? DMA_CCR_PINC : 0) |
                   other_flags);
}

static inline uint32 cfg_ccr_flags(unsigned tube_flags) {
    /* DMA_CFG_SRC_INC and DMA_CFG_DST_INC are special */
    return tube_flags & ~(DMA_CFG_SRC_INC | DMA_CFG_DST_INC);
}

/* Configure chregs according to cfg, where cfg->tube_dst is peripheral. */
static int config_to_per(dma_tube_reg_map *chregs, dma_tube_config *cfg) {
    /* Check that ->tube_src is memory (if it's anything else, we
     * shouldn't have been called). */
    ASSERT(_dma_addr_type(cfg->tube_src) == DMA_ATYPE_MEM);

    set_ccr(chregs,
            cfg->tube_src_size, cfg->tube_flags & DMA_CFG_SRC_INC,
            cfg->tube_dst_size, cfg->tube_flags & DMA_CFG_DST_INC,
            (cfg_ccr_flags(cfg->tube_flags) | DMA_CCR_DIR_FROM_MEM));
    chregs->CNDTR = cfg->tube_nr_xfers;
    chregs->CMAR = (uint32)cfg->tube_src;
    chregs->CPAR = (uint32)cfg->tube_dst;
    return DMA_TUBE_CFG_SUCCESS;
}

/* Configure chregs according to cfg, where cfg->tube_dst is memory. */
static int config_to_mem(dma_tube_reg_map *chregs, dma_tube_config *cfg) {
    uint32 mem2mem;

    if ((_dma_addr_type(cfg->tube_src) == DMA_ATYPE_MEM) &&
        (cfg->tube_flags & DMA_CFG_CIRC)) {
        /* Can't do mem-to-mem and circular mode */
        return -DMA_TUBE_CFG_ECFG;
    }

    mem2mem = (_dma_addr_type(cfg->tube_src) == DMA_ATYPE_MEM ?
               DMA_CCR_MEM2MEM : 0);
    set_ccr(chregs,
            cfg->tube_dst_size, cfg->tube_flags & DMA_CFG_DST_INC,
            cfg->tube_src_size, cfg->tube_flags & DMA_CFG_SRC_INC,
            (cfg_ccr_flags(cfg->tube_flags) |
             DMA_CCR_DIR_FROM_PER |
             mem2mem));
    chregs->CNDTR = cfg->tube_nr_xfers;
    chregs->CMAR = (uint32)cfg->tube_dst;
    chregs->CPAR = (uint32)cfg->tube_src;
    return DMA_TUBE_CFG_SUCCESS;
}

/*
 * Routines
 */

int dma_tube_cfg(dma_dev *dev, dma_channel channel, dma_tube_config *cfg) {
    dma_tube_reg_map *chregs;
    int ret = preconfig_check(dev, channel, cfg);

    if (ret < 0) {
        return ret;
    }

    dma_disable(dev, channel);        /* Must disable before reconfiguring */
    dma_clear_isr_bits(dev, channel); /* For sanity and consistency
                                       * with STM32F2. */

    chregs = dma_tube_regs(dev, channel);
    switch (_dma_addr_type(cfg->tube_dst)) {
    case DMA_ATYPE_PER:
        ret = config_to_per(chregs, cfg);
        break;
    case DMA_ATYPE_MEM:
        ret = config_to_mem(chregs, cfg);
        break;
    default:
        /* Can't happen */
        ASSERT(0);
        return -DMA_TUBE_CFG_ECFG;
    }
    if (ret < 0) {
        return ret;
    }
    chregs->CNDTR = cfg->tube_nr_xfers;
    return DMA_TUBE_CFG_SUCCESS;
}

void dma_set_priority(dma_dev *dev,
                      dma_channel channel,
                      dma_priority priority) {
    dma_channel_reg_map *channel_regs;
    uint32 ccr;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    channel_regs = dma_channel_regs(dev, channel);
    ccr = channel_regs->CCR;
    ccr &= ~DMA_CCR_PL;
    ccr |= (priority << 12);
    channel_regs->CCR = ccr;
}

void dma_set_num_transfers(dma_dev *dev,
                           dma_channel channel,
                           uint16 num_transfers) {
    dma_channel_reg_map *channel_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    channel_regs = dma_channel_regs(dev, channel);
    channel_regs->CNDTR = num_transfers;
}

void dma_attach_interrupt(dma_dev *dev, dma_channel channel,
                          void (*handler)(void)) {
    DMA_GET_HANDLER(dev, channel) = handler;
    nvic_irq_enable(dev->handlers[channel - 1].irq_line);
}

void dma_detach_interrupt(dma_dev *dev, dma_channel channel) {
    /* Don't use nvic_irq_disable()! Think about DMA2 channels 4 and 5. */
    dma_channel_regs(dev, channel)->CCR &= ~0xF;
    DMA_GET_HANDLER(dev, channel) = NULL;
}

void dma_enable(dma_dev *dev, dma_channel channel) {
    dma_channel_reg_map *chan_regs = dma_channel_regs(dev, channel);
    bb_peri_set_bit(&chan_regs->CCR, DMA_CCR_EN_BIT, 1);
}

void dma_disable(dma_dev *dev, dma_channel channel) {
    dma_channel_reg_map *chan_regs = dma_channel_regs(dev, channel);
    bb_peri_set_bit(&chan_regs->CCR, DMA_CCR_EN_BIT, 0);
}

dma_irq_cause dma_get_irq_cause(dma_dev *dev, dma_channel channel) {
    /* Grab and clear the ISR bits. */
    uint8 status_bits = dma_get_isr_bits(dev, channel);
    dma_clear_isr_bits(dev, channel);

    /* If the channel global interrupt flag is cleared, then
     * something's very wrong. */
    ASSERT(status_bits & 0x1);
    /* If GIF is set, then some other flag should be set, barring
     * something unexpected (e.g. the user making an unforeseen IFCR
     * write). */
    ASSERT(status_bits != 0x1);

    /* ISR flags get set even if the corresponding interrupt enable
     * bits in the channel's configuration register are cleared, so we
     * can't use a switch here.
     *
     * Don't change the order of these if statements. */
    if (status_bits & 0x8) {
        return DMA_TRANSFER_ERROR;
    } else if (status_bits & 0x2) {
        return DMA_TRANSFER_COMPLETE;
    } else if (status_bits & 0x4) {
        return DMA_TRANSFER_HALF_COMPLETE;
    }

    /* If we get here, one of our assumptions has been violated, but
     * the debug level is too low for the above ASSERTs() to have had
     * any effect. In order to fail fast, mimic the DMA controller's
     * behavior when an error occurs. */
    dma_disable(dev, channel);
    return DMA_TRANSFER_ERROR;
}

void dma_set_mem_addr(dma_dev *dev, dma_channel channel, __io void *addr) {
    dma_channel_reg_map *chan_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    chan_regs = dma_channel_regs(dev, channel);
    chan_regs->CMAR = (uint32)addr;
}

void dma_set_per_addr(dma_dev *dev, dma_channel channel, __io void *addr) {
    dma_channel_reg_map *chan_regs;

    ASSERT_FAULT(!dma_is_channel_enabled(dev, channel));

    chan_regs = dma_channel_regs(dev, channel);
    chan_regs->CPAR = (uint32)addr;
}

/*
 * IRQ handlers
 */

void __irq_dma1_ch1(void) {
    dma_irq_handler(DMA1, DMA_CH1);
}

void __irq_dma1_ch2(void) {
    dma_irq_handler(DMA1, DMA_CH2);
}

void __irq_dma1_ch3(void) {
    dma_irq_handler(DMA1, DMA_CH3);
}

void __irq_dma1_ch4(void) {
    dma_irq_handler(DMA1, DMA_CH4);
}

void __irq_dma1_ch5(void) {
    dma_irq_handler(DMA1, DMA_CH5);
}

void __irq_dma1_ch6(void) {
    dma_irq_handler(DMA1, DMA_CH6);
}

void __irq_dma1_ch7(void) {
    dma_irq_handler(DMA1, DMA_CH7);
}

void __irq_dma2_ch1(void) {
    dma_irq_handler(DMA2, DMA_CH1);
}

void __irq_dma2_ch2(void) {
    dma_irq_handler(DMA2, DMA_CH2);
}

void __irq_dma2_ch3(void) {
    dma_irq_handler(DMA2, DMA_CH3);
}

void __irq_dma2_ch4(void) {
    dma_irq_handler(DMA2, DMA_CH4);
}

void __irq_dma2_ch5(void) {
    dma_irq_handler(DMA2, DMA_CH5);
}
