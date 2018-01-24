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

#ifndef _LIBMAPLE_DMA_PRIVATE_H_
#define _LIBMAPLE_DMA_PRIVATE_H_

#include <libmaple/dma.h>
#include <libmaple/libmaple_types.h>

/*
 * IRQ handling
 */

/* Wrap this in an ifdef to shut up GCC. (We provide DMA_GET_HANDLER
 * in the series support files, which need dma_irq_handler().) */
#ifdef DMA_GET_HANDLER
static inline __always_inline void dma_irq_handler(dma_dev *dev, dma_tube tube) {

    void (*handler)(void) = DMA_GET_HANDLER(dev, tube);
    if (handler) {
        handler();
	    dma_clear_isr_bits(dev, tube); /* in case handler doesn't */
    }
}
#endif

/*
 * Conveniences for dealing with tube sources/destinations
 */

enum dma_atype {
    DMA_ATYPE_MEM,
    DMA_ATYPE_PER,
    DMA_ATYPE_OTHER,
};

enum dma_atype _dma_addr_type(__IO void *addr);

#endif
