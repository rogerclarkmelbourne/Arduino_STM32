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
 * @file libmaple/dma.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>;
 *         Original implementation by Michael Hope
 * @brief Portable DMA routines.
 */

#include <libmaple/dma.h>
#include "dma_private.h"
#include "stm32_private.h"

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

/*
 * Private API
 */

enum dma_atype _dma_addr_type(__io void *addr) {
    switch (stm32_block_purpose((void*)addr)) {
    /* Notice we're treating the code block as memory here.  That's
     * correct for addresses in Flash and in [0x0, 0x7FFFFFF]
     * (provided that those addresses are aliased to Flash, SRAM, or
     * FSMC, depending on BOOT[01] and possibly SYSCFG_MEMRMP). It's
     * not correct for other addresses in the code block, but those
     * will (hopefully) just fail-fast with transfer or bus errors. If
     * lots of people get confused, it might be worth being more
     * careful here. */
    case STM32_BLOCK_CODE:      /* Fall through */
    case STM32_BLOCK_SRAM:      /* ... */
    case STM32_BLOCK_FSMC_1_2:  /* ... */
    case STM32_BLOCK_FSMC_3_4:
        return DMA_ATYPE_MEM;
    case STM32_BLOCK_PERIPH:
        return DMA_ATYPE_PER;
    case STM32_BLOCK_FSMC_REG:        /* Fall through */
        /* Is this right? I can't think of a reason to DMA into or out
         * of the FSMC registers. [mbolivar]  */
    case STM32_BLOCK_UNUSED:          /* ... */
    case STM32_BLOCK_CORTEX_INTERNAL: /* ... */
        return DMA_ATYPE_OTHER;
    default:
        ASSERT(0);              /* Can't happen */
        return DMA_ATYPE_OTHER;
    }
}
