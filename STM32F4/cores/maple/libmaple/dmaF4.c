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

#ifdef STM32F4
 
/**
 * @file dmaF4.c
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
    .handlers = {{ .handler = NULL, .irq_line = 11 },
                 { .handler = NULL, .irq_line = 12 },
                 { .handler = NULL, .irq_line = 13 },
                 { .handler = NULL, .irq_line = 14 },
                 { .handler = NULL, .irq_line = 15 },
                 { .handler = NULL, .irq_line = 16 },
                 { .handler = NULL, .irq_line = 17 },
                 { .handler = NULL, .irq_line = 47 }}
};
/** DMA1 device */
dma_dev *DMA1 = &dma1;

static dma_dev dma2 = {
    .regs     = DMA2_BASE,
    .clk_id   = RCC_DMA2,
    .handlers = {{ .handler = NULL, .irq_line = 56 },
                 { .handler = NULL, .irq_line = 57 },
                 { .handler = NULL, .irq_line = 58 },
                 { .handler = NULL, .irq_line = 59 },
                 { .handler = NULL, .irq_line = 60 },
                 { .handler = NULL, .irq_line = 68 },
                 { .handler = NULL, .irq_line = 69 },
                 { .handler = NULL, .irq_line = 70 }} /* !@#$ */
};
/** DMA2 device */
dma_dev *DMA2 = &dma2;


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
 * @brief Attach an interrupt to a DMA transfer.
 *
 * Interrupts are enabled using appropriate mode flags in
 * dma_setup_transfer().
 *
 * @param dev DMA device
 * @param stream Stream to attach handler to
 * @param handler Interrupt handler to call when channel interrupt fires.
 * @see dma_setup_transfer()
 * @see dma_detach_interrupt()
 */
void dma_attach_interrupt(dma_dev *dev,
                          dma_stream stream,
                          void (*handler)(void)) {
    dev->handlers[stream].handler = handler;
    nvic_irq_enable(dev->handlers[stream].irq_line);
}

/**
 * @brief Detach a DMA transfer interrupt handler.
 *
 * After calling this function, the given channel's interrupts will be
 * disabled.
 *
 * @param dev DMA device
 * @param stream Stream whose handler to detach
 * @sideeffect Clears interrupt enable bits in the channel's CCR register.
 * @see dma_attach_interrupt()
 */
void dma_detach_interrupt(dma_dev *dev, dma_stream stream) {
    nvic_irq_disable(dev->handlers[stream].irq_line);
    dev->handlers[stream].handler = NULL;
}

const uint8 dma_isr_bits_shift[] = { 0, 6, 16, 22};

uint8 dma_get_isr_bit(dma_dev *dev, dma_stream stream, uint8_t mask) {
	if ( stream&0xFC )	return ((dev->regs->HISR)>>dma_isr_bits_shift[stream&0x03]) & mask;
	else				return ((dev->regs->LISR)>>dma_isr_bits_shift[stream&0x03]) & mask;
}

void dma_clear_isr_bit(dma_dev *dev, dma_stream stream, uint8_t mask) {
	if ( stream&0xFC )	dev->regs->HIFCR = (uint32)mask << dma_isr_bits_shift[stream&0x03];
	else				dev->regs->LIFCR = (uint32)mask << dma_isr_bits_shift[stream&0x03];
}

/*
 * IRQ handlers
 */

static inline void dispatch_handler(dma_dev *dev, dma_stream stream) {
    void (*handler)(void) = dev->handlers[stream].handler;
    if (handler) {
        handler();
        dma_clear_isr_bits(dev, stream); /* in case handler doesn't */
    }
}

//void __irq_dma1_stream0(void) {
void __irq_dma1_channel1(void) {
    dispatch_handler(DMA1, DMA_STREAM0);
}

//void __irq_dma1_stream1(void) {
void __irq_dma1_channel2(void) {
    dispatch_handler(DMA1, DMA_STREAM1);
}

//void __irq_dma1_stream2(void) {
void __irq_dma1_channel3(void) {
    dispatch_handler(DMA1, DMA_STREAM2);
}

//void __irq_dma1_stream3(void) {
void __irq_dma1_channel4(void) {
    dispatch_handler(DMA1, DMA_STREAM3);
}

//void __irq_dma1_stream4(void) {
void __irq_dma1_channel5(void) {
    dispatch_handler(DMA1, DMA_STREAM4);
}

//void __irq_dma1_stream5(void) {
void __irq_dma1_channel6(void) {
    dispatch_handler(DMA1, DMA_STREAM5);
}

//void __irq_dma1_stream6(void) {
void __irq_dma1_channel7(void) {
    dispatch_handler(DMA1, DMA_STREAM6);
}

//void __irq_dma1_stream7(void) {
void __irq_adc3(void) {
    dispatch_handler(DMA1, DMA_STREAM7);
}

//void __irq_dma2_stream0(void) {
void __irq_dma2_channel1(void) {
    dispatch_handler(DMA2, DMA_STREAM0);
}

//void __irq_dma2_stream1(void) {
void __irq_dma2_channel2(void) {
    dispatch_handler(DMA2, DMA_STREAM1);
}

//void __irq_dma2_stream2(void) {
void __irq_dma2_channel3(void) {
    dispatch_handler(DMA2, DMA_STREAM2);
}

//void __irq_dma2_stream3(void) {
void __irq_dma2_channel4_5(void) {
    dispatch_handler(DMA2, DMA_STREAM3);
}

//void __irq_dma2_stream4(void) {
void __irq_DMA2_Stream4_IRQHandler(void) {
    dispatch_handler(DMA2, DMA_STREAM4);
}

//void __irq_dma2_stream5(void) {
void __irq_DMA2_Stream5_IRQHandler(void) {
    dispatch_handler(DMA2, DMA_STREAM5);
}

//void __irq_dma2_stream6(void) {
void __irq_DMA2_Stream6_IRQHandler(void) {
    dispatch_handler(DMA2, DMA_STREAM6);
}

//void __irq_dma2_stream7(void) {
void __irq_DMA2_Stream7_IRQHandler(void) {
    dispatch_handler(DMA2, DMA_STREAM7);
}

#endif
