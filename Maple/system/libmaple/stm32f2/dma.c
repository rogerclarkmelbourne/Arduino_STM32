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
 * @file libmaple/stm32f2/dma.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 DMA support.
 */

#include <libmaple/dma.h>
#include <libmaple/bitband.h>
#include <libmaple/util.h>

/* Hack to ensure inlining in dma_irq_handler() */
#define DMA_GET_HANDLER(dev, tube) (dev->handlers[tube].handler)
#include "dma_private.h"

/*
 * Devices
 */

static dma_dev dma1 = {
    .regs = DMA1_BASE,
    .clk_id = RCC_DMA1,
    .handlers = {{ .handler = NULL, .irq_line = NVIC_DMA1_STREAM0 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM1 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM2 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM3 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM4 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM5 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM6 },
                 { .handler = NULL, .irq_line = NVIC_DMA1_STREAM7 }},
};
dma_dev *DMA1 = &dma1;

static dma_dev dma2 = {
    .regs = DMA2_BASE,
    .clk_id = RCC_DMA2,
    .handlers = {{ .handler = NULL, .irq_line = NVIC_DMA2_STREAM0 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM1 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM2 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM3 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM4 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM5 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM6 },
                 { .handler = NULL, .irq_line = NVIC_DMA2_STREAM7 }},
};
dma_dev *DMA2 = &dma2;

/*
 * Helpers for dealing with dma_request_src's bit encoding (see the
 * comments in the dma_request_src definition).
 */

/* rcc_clk_id of dma_dev which supports src. */
static __always_inline rcc_clk_id src_clk_id(dma_request_src src) {
    return (rcc_clk_id)(((uint32)src >> 3) & 0x3F);
}

/* Bit vector of streams supporting src (e.g., bit 0 set => DMA_S0 support). */
static __always_inline uint32 src_stream_mask(dma_request_src src) {
    return ((uint32)src >> 10) & 0xFF;
}

/* Channel corresponding to src. */
static __always_inline dma_channel src_channel(dma_request_src src) {
    return (dma_channel)(src & 0x7);
}

/*
 * Routines
 */

/* For convenience */
#define ASSERT_NOT_ENABLED(dev, tube) ASSERT(!dma_is_enabled(dev, tube))

/* Helpers for dma_tube_cfg() */
static int preconfig_check(dma_dev *dev, dma_tube tube, dma_tube_config *cfg);
static int postconfig_check(dma_tube_reg_map *dummy, dma_tube_config *cfg);
static int config_fifo(dma_tube_reg_map *dummy, dma_tube_config *cfg);
static int config_src_dst(dma_tube_reg_map *dummy, dma_tube_config *cfg);
static void copy_regs(dma_tube_reg_map *src, dma_tube_reg_map *dst);

int dma_tube_cfg(dma_dev *dev, dma_tube tube, dma_tube_config *cfg) {
    dma_tube_reg_map dummy_regs;
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    int ret;

    /* Initial error checking. */
    ret = preconfig_check(dev, tube, cfg);
    if (ret < 0) {
        return ret;
    }

    /* Disable `tube' as per RM0033. */
    dma_disable(dev, tube);
    dma_clear_isr_bits(dev, tube);

    /* Don't write to tregs until we've decided `cfg' is really OK,
     * so as not to make a half-formed mess if we have to error out. */
    copy_regs(tregs, &dummy_regs);

    /* Try to reconfigure `tube', bailing on error. */
    ret = config_fifo(&dummy_regs, cfg);
    if (ret < 0) {
        return ret;
    }
    ret = config_src_dst(&dummy_regs, cfg);
    if (ret < 0) {
        return ret;
    }
    dummy_regs.SNDTR = cfg->tube_nr_xfers;
    ret = postconfig_check(&dummy_regs, cfg);
    if (ret < 0) {
        return ret;
    }

    /* Ok, we're good. Commit to the new configuration. */
    copy_regs(&dummy_regs, tregs);
    return ret;
}

void dma_set_priority(dma_dev *dev, dma_stream stream, dma_priority priority) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, stream);
    uint32 scr;
    ASSERT_NOT_ENABLED(dev, stream);
    scr = tregs->SCR;
    scr &= ~DMA_SCR_PL;
    scr |= (priority << 16);
    tregs->SCR = scr;
}

void dma_set_num_transfers(dma_dev *dev, dma_tube tube, uint16 num_transfers) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    ASSERT_NOT_ENABLED(dev, tube);
    tregs->SNDTR = num_transfers;
}

/**
 * @brief Set memory 0 or memory 1 address.
 *
 * This is a general function for setting one of the two memory
 * addresses available on the double-buffered STM32F2 DMA controllers.
 *
 * @param dev     DMA device
 * @param tube    Tube on dev.
 * @param n       If 0, set memory 0 address. If 1, set memory 1 address.
 * @param address Address to set
 */
void dma_set_mem_n_addr(dma_dev *dev, dma_tube tube, int n,
                        __io void *address) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    uint32 addr = (uint32)address;

    ASSERT_NOT_ENABLED(dev, tube);
    if (n) {
        tregs->SM1AR = addr;
    } else {
        tregs->SM0AR = addr;
    }
}

void dma_set_per_addr(dma_dev *dev, dma_tube tube, __io void *address) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    ASSERT_NOT_ENABLED(dev, tube);
    tregs->SPAR = (uint32)address;
}

/**
 * @brief Enable a stream's FIFO.
 *
 * You may only call this function when the stream is disabled.
 *
 * @param dev  DMA device
 * @param tube Stream whose FIFO to enable.
 */
void dma_enable_fifo(dma_dev *dev, dma_tube tube) {
    ASSERT_NOT_ENABLED(dev, tube);
    bb_peri_set_bit(&(dma_tube_regs(dev, tube)->SFCR), DMA_SFCR_DMDIS_BIT, 1);
}

/**
 * @brief Disable a stream's FIFO.
 *
 * You may only call this function when the stream is disabled.
 *
 * @param dev  DMA device
 * @param tube Stream whose FIFO to disable.
 */
void dma_disable_fifo(dma_dev *dev, dma_tube tube) {
    ASSERT_NOT_ENABLED(dev, tube);
    bb_peri_set_bit(&(dma_tube_regs(dev, tube)->SFCR), DMA_SFCR_DMDIS_BIT, 0);
}

void dma_attach_interrupt(dma_dev *dev, dma_tube tube,
                          void (*handler)(void)) {
    dev->handlers[tube].handler = handler;
    nvic_irq_enable(dev->handlers[tube].irq_line);
}

void dma_detach_interrupt(dma_dev *dev, dma_tube tube) {
    nvic_irq_disable(dev->handlers[tube].irq_line);
    dev->handlers[tube].handler = NULL;
}

void dma_enable(dma_dev *dev, dma_tube tube) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    bb_peri_set_bit(&tregs->SCR, DMA_SCR_EN_BIT, 1);
}

void dma_disable(dma_dev *dev, dma_tube tube) {
    dma_tube_reg_map *tregs = dma_tube_regs(dev, tube);
    bb_peri_set_bit(&tregs->SCR, DMA_SCR_EN_BIT, 0);
    /* The stream might not get disabled immediately, so wait. */
    while (tregs->SCR & DMA_SCR_EN)
        ;
}

dma_irq_cause dma_get_irq_cause(dma_dev *dev, dma_tube tube) {
    /* TODO: does it still make sense to have this function? We should
     * probably just be returning the ISR bits, with some defines to
     * pull the flags out. The lack of masked status bits is an
     * annoyance that would require documentation to solve, though. */
    uint8 status_bits = dma_get_isr_bits(dev, tube);
    dma_clear_isr_bits(dev, tube);
    ASSERT(status_bits);        /* Or something's very wrong */
    /* Don't change the order of these if statements. */
    if (status_bits & 0x0) {
        return DMA_TRANSFER_FIFO_ERROR;
    } else if (status_bits & 0x4) {
        return DMA_TRANSFER_DME_ERROR;
    } else if (status_bits & 0x8) {
        return DMA_TRANSFER_ERROR;
    } else if (status_bits & 0x20) {
        return DMA_TRANSFER_COMPLETE;
    } else if (status_bits & 0x10) {
        return DMA_TRANSFER_HALF_COMPLETE;
    }

    /* Something's wrong; one of those bits should have been set. Fail
     * an assert, and mimic the error behavior in case of a high debug
     * level. */
    ASSERT(0);
    dma_disable(dev, tube);
    return DMA_TRANSFER_ERROR;
}

/*
 * IRQ handlers
 */

void __irq_dma1_stream0(void) {
    dma_irq_handler(DMA1, DMA_S0);
}

void __irq_dma1_stream1(void) {
    dma_irq_handler(DMA1, DMA_S1);
}

void __irq_dma1_stream2(void) {
    dma_irq_handler(DMA1, DMA_S2);
}

void __irq_dma1_stream3(void) {
    dma_irq_handler(DMA1, DMA_S3);
}

void __irq_dma1_stream4(void) {
    dma_irq_handler(DMA1, DMA_S4);
}

void __irq_dma1_stream5(void) {
    dma_irq_handler(DMA1, DMA_S5);
}

void __irq_dma1_stream6(void) {
    dma_irq_handler(DMA1, DMA_S6);
}

void __irq_dma1_stream7(void) {
    dma_irq_handler(DMA1, DMA_S7);
}

void __irq_dma2_stream0(void) {
    dma_irq_handler(DMA2, DMA_S0);
}

void __irq_dma2_stream1(void) {
    dma_irq_handler(DMA2, DMA_S1);
}

void __irq_dma2_stream2(void) {
    dma_irq_handler(DMA2, DMA_S2);
}

void __irq_dma2_stream3(void) {
    dma_irq_handler(DMA2, DMA_S3);
}

void __irq_dma2_stream4(void) {
    dma_irq_handler(DMA2, DMA_S4);
}

void __irq_dma2_stream5(void) {
    dma_irq_handler(DMA2, DMA_S5);
}

void __irq_dma2_stream6(void) {
    dma_irq_handler(DMA2, DMA_S6);
}

void __irq_dma2_stream7(void) {
    dma_irq_handler(DMA2, DMA_S7);
}

/*
 * Auxiliary routines for dma_tube_cfg()
 */

/* Is addr acceptable for use as DMA src/dst? */
static int cfg_mem_ok(__io void *addr) {
    enum dma_atype atype = _dma_addr_type(addr);
    return atype == DMA_ATYPE_MEM || atype == DMA_ATYPE_PER;
}

/* Is src -> dst a reasonable combination of [MEM,PER] -> [MEM,PER]? */
static int cfg_dir_ok(dma_dev *dev, __io void *src, __io void *dst) {
    switch (_dma_addr_type(dst)) {
    case DMA_ATYPE_MEM:
        /* Only DMA2 can do memory-to-memory */
        return ((_dma_addr_type(src) == DMA_ATYPE_PER) ||
                (dev->clk_id == RCC_DMA2));
    case DMA_ATYPE_PER:
        /* Peripheral-to-peripheral is illegal */
        return _dma_addr_type(src) == DMA_ATYPE_PER;
    default: /* Can't happen */
        ASSERT(0);
        return 0;
    }
}

/* Initial sanity check for dma_tube_cfg() */
static int preconfig_check(dma_dev *dev, dma_tube tube,
                           dma_tube_config *cfg) {
    if (!(src_stream_mask(cfg->tube_req_src) & (1U << tube))) {
        /* ->tube_req_src not supported by stream */
        return -DMA_TUBE_CFG_EREQ;
    }
    if (cfg->tube_nr_xfers > 65535) {
        /* That's too many. */
        return -DMA_TUBE_CFG_ENDATA;
    }
    if (src_clk_id(cfg->tube_req_src) != dev->clk_id) {
        /* ->tube_req_src not supported by dev */
        return -DMA_TUBE_CFG_EDEV;
    }
    if (!cfg_mem_ok(cfg->tube_src)) {
        return -DMA_TUBE_CFG_ESRC;
    }
    if (!cfg_mem_ok(cfg->tube_dst)) {
        return -DMA_TUBE_CFG_EDST;
    }
    if (!cfg_dir_ok(dev, cfg->tube_src, cfg->tube_dst)) {
        return -DMA_TUBE_CFG_EDIR;
    }
    return DMA_TUBE_CFG_SUCCESS;
}

static int config_fifo(dma_tube_reg_map *dummy, dma_tube_config *cfg) {
    /* TODO: FIFO configuration based on cfg->target_data */
    uint32 sfcr = dummy->SFCR;
    sfcr &= ~DMA_SFCR_FEIE;
    sfcr |= (cfg->tube_flags & DMA_CFG_FIFO_ERR_IE) ? DMA_SFCR_FEIE : 0;
    dummy->SFCR = sfcr;
    return DMA_TUBE_CFG_SUCCESS;
}

/* Helper for configuring (DMA_SxCR) */
#define BITS_WE_CARE_ABOUT                                              \
    (DMA_SCR_CHSEL | DMA_SCR_MBURST | DMA_SCR_PBURST | DMA_SCR_PINCOS | \
     DMA_SCR_MINC | DMA_SCR_PINC | DMA_SCR_CIRC | DMA_SCR_DIR |         \
     DMA_SCR_PFCTRL | DMA_SCR_TCIE | DMA_SCR_HTIE | DMA_SCR_TEIE |      \
     DMA_SCR_DMEIE)
static inline void config_scr(dma_tube_reg_map *dummy, dma_tube_config *cfg,
                              unsigned src_shift, uint32 src_inc,
                              unsigned dst_shift, uint32 dst_inc,
                              uint32 dir) {
    /* These would go here if we supported them: MBURST, PBURST,
     * PINCOS, PFCTRL. We explicitly choose low priority, and double
     * buffering belongs elsewhere, I think. [mbolivar] */
    uint32 flags = cfg->tube_flags & BITS_WE_CARE_ABOUT;
    uint32 scr = dummy->SCR;
    scr &= ~(BITS_WE_CARE_ABOUT | DMA_SCR_PL);
    scr |= (/* CHSEL */
            (src_channel(cfg->tube_req_src) << 25) |
            /* MSIZE/PSIZE */
            (cfg->tube_src_size << src_shift) |
            (cfg->tube_dst_size << dst_shift) |
            /* MINC/PINC */
            ((cfg->tube_flags & DMA_CFG_SRC_INC) ? src_inc : 0) |
            ((cfg->tube_flags & DMA_CFG_DST_INC) ? dst_inc : 0) |
            /* DIR */
            dir |
            /* Other flags carried by cfg->tube_flags */
            flags);
    dummy->SCR = scr;
}
#undef BITS_WE_CARE_ABOUT

/* Helper for when cfg->tube_dst is memory */
static int config_to_mem(dma_tube_reg_map *dummy, dma_tube_config *cfg) {
    uint32 dir = (_dma_addr_type(cfg->tube_src) == DMA_ATYPE_MEM ?
                  DMA_SCR_DIR_MEM_TO_MEM : DMA_SCR_DIR_PER_TO_MEM);

    if ((dir == DMA_SCR_DIR_MEM_TO_MEM) && (cfg->tube_flags & DMA_CFG_CIRC)) {
        return -DMA_TUBE_CFG_ECFG; /* Can't do DMA_CFG_CIRC and mem->mem. */
    }

    config_scr(dummy, cfg, 11, DMA_SCR_PINC, 13, DMA_SCR_MINC, dir);
    dummy->SPAR = (uint32)cfg->tube_src;
    dummy->SM0AR = (uint32)cfg->tube_dst;
    return DMA_TUBE_CFG_SUCCESS;
}

/* Helper for when cfg->tube_src is peripheral */
static int config_to_per(dma_tube_reg_map *dummy, dma_tube_config *cfg) {
    config_scr(dummy, cfg, 13, DMA_SCR_MINC, 11, DMA_SCR_PINC,
               DMA_SCR_DIR_MEM_TO_PER);
    dummy->SM0AR = (uint32)cfg->tube_src;
    dummy->SPAR = (uint32)cfg->tube_dst;
    return DMA_TUBE_CFG_SUCCESS;
}

/* Configures SCR, SPAR, SM0AR, and checks that the result is OK. */
static int config_src_dst(dma_tube_reg_map *dummy, dma_tube_config *cfg) {
    switch (_dma_addr_type(cfg->tube_dst)) {
    case DMA_ATYPE_MEM:
        return config_to_mem(dummy, cfg);
    case DMA_ATYPE_PER:
        return config_to_per(dummy, cfg);
    case DMA_ATYPE_OTHER:
    default: /* shut up, GCC */
        /* Can't happen */
        ASSERT(0);
        return -DMA_TUBE_CFG_ECFG;
    }
}

/* Final checks we can only perform when fully configured */
static int postconfig_check(dma_tube_reg_map *dummy, dma_tube_config *cfg) {
    /* TODO add dma_get_[mem,per]_size() and use them here */
    /* msize and psize are in bytes here: */
    uint32 scr = dummy->SCR;
    uint32 msize = 1U << ((scr >> 13) & 0x3);
    uint32 psize = 1U << ((scr >> 11) & 0x3);

    /* Ensure NDT will work with PSIZE/MSIZE.
     *
     * RM0033 specifies that PSIZE, MSIZE, and NDT must be such that
     * the last transfer completes; i.e. that if PSIZE < MSIZE, then
     * NDT is a multiple of MSIZE/PSIZE.  See e.g. Table 27. */
    if ((psize < msize) && (cfg->tube_nr_xfers % (msize / psize))) {
        return -DMA_TUBE_CFG_ENDATA;
    }

    /* Direct mode is only possible if MSIZE == PSIZE. */
    if ((msize != psize) && !(dummy->SFCR & DMA_SFCR_DMDIS)) {
        return -DMA_TUBE_CFG_ESIZE;
    }

    return DMA_TUBE_CFG_SUCCESS;
}

/* Convenience for dealing with dummy registers */
static void copy_regs(dma_tube_reg_map *src, dma_tube_reg_map *dst) {
    dst->SCR = src->SCR;
    dst->SNDTR = src->SNDTR;
    dst->SPAR = src->SPAR;
    dst->SM0AR = src->SM0AR;
    dst->SFCR = src->SFCR;
}
