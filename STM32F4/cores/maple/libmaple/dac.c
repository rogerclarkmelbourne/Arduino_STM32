/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
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
 * @file dac.c
 * @brief Digital to analog converter support.
 */

#include "libmaple.h"
#include "gpio.h"
#include "dac.h"


/**
 * @brief DAC peripheral routines.
 */

/** DAC device. */
dac_dev_t const dac_dev = {
    .regs = DAC_BASE,
};

/**
 * @brief Initialize the digital to analog converter
 */
void dac_init()
{
    rcc_clk_enable(RCC_DAC);
    rcc_reset_dev(RCC_DAC);
}

/**
 * @brief Write a 12-bit value to the DAC to output
 * @param channel channel to select (1 or 2)
 * @param val value to write
 */
void dac_write_channel(dac_channel_t channel, uint16 val)
{
    if (channel & DAC_CH1) {
        DAC->regs->DHR12R1 = DAC_DHR12R_MASK & val;
    }
    if (channel & DAC_CH2) {
        DAC->regs->DHR12R2 = DAC_DHR12R_MASK & val;
    }
}

void dac_set_mask_amplitude(dac_channel_t channel, uint16 val)
{
	val &= DAC_CR_MAMP_MASK;
    if (channel & DAC_CH1) {
        DAC->regs->CR = (DAC->regs->CR&(~DAC_CR_MAMP1)) | (val<<DAC_CR_MAMP1_SHIFT);
    }
    if (channel & DAC_CH2) {
        DAC->regs->CR = (DAC->regs->CR&(~DAC_CR_MAMP2)) | (val<<DAC_CR_MAMP2_SHIFT);
    }
}

void dac_set_wave(dac_channel_t channel, dac_wave_t val)
{
    val &= DAC_CR_WAVE_MASK;
    if (channel & DAC_CH1) {
        DAC->regs->CR = (DAC->regs->CR&(~DAC_CR_WAVE1)) | (val<<DAC_CR_WAVE1_SHIFT);
    }
    if (channel & DAC_CH2) {
        DAC->regs->CR = (DAC->regs->CR&(~DAC_CR_WAVE2)) | (val<<DAC_CR_WAVE2_SHIFT);
    }
}

void dac_set_trigger(dac_channel_t channel, dac_trigger_t val)
{
    val &= DAC_CR_TSEL_MASK;
    if (channel & DAC_CH1) {
		DAC->regs->CR &= ~DAC_CR_TSEL1;
        DAC->regs->CR |= (val<<DAC_CR_TSEL1_SHIFT) | DAC_CR_TEN1;
    }
    if (channel & DAC_CH2) {
		DAC->regs->CR &= ~DAC_CR_TSEL2;
        DAC->regs->CR |= (val<<DAC_CR_TSEL2_SHIFT) | DAC_CR_TEN2;
    }
}

void dac_sw_trigger(dac_channel_t channel)
{
	// trigger the channels simultanously
	DAC->regs->SWTRIGR = channel & (DAC_CH1 | DAC_CH2);
}

/**
 * @brief Enable/disable the output buffer of the digital to analog converter
 * @param channel Flags:
 *      DAC_CH1: Select channel 1
 *      DAC_CH2: Select channel 2
 */
void dac_enable_buffer(uint8 channel)
{
	uint32 flags = (channel&DAC_CH1)<<1 | (channel&DAC_CH2)<<16;
	DAC->regs->CR |= flags; // enable the buffers simultanously
}

void dac_disable_buffer(uint8 channel)
{
	uint32 flags = (channel&DAC_CH1)<<1 | (channel&DAC_CH2)<<16;
	DAC->regs->CR &= ~flags; // disable the buffers simultanously
}

/**
 * @brief Enable/disable DMA request on trigger occurance
 * @param channel Flags:
 *      DAC_CH1: Select channel 1
 *      DAC_CH2: Select channel 2
 */
void dac_enable_dma(dac_channel_t channel)
{
	uint32 flags = (channel&DAC_CH1)<<12 | (channel&DAC_CH2)<<27;
	DAC->regs->CR |= flags; // enable the channels simultanously
}

void dac_disable_dma(dac_channel_t channel)
{
	uint32 flags = (channel&DAC_CH1)<<12 | (channel&DAC_CH2)<<27;
	DAC->regs->CR &= ~flags; // disable the channels simultanously
}

/**
 * @brief Enable a DAC channel
 * @param channel channel to enable, 1 and/or 2
 * @sideeffect Changes pin mode of PA4 or PA5
 */
void dac_enable(dac_channel_t channel)
{
    // Setup ANALOG mode on PA4 and PA5. This mapping is consistent across
    // all STM32 chips with a DAC. See RM0008 12.2.
    if (channel & DAC_CH1) {
        gpio_set_mode(PA4, GPIO_INPUT_ANALOG);
    }
    if (channel & DAC_CH2) {
        gpio_set_mode(PA5, GPIO_INPUT_ANALOG);
    }
	uint32 flags = (channel&DAC_CH1) | (channel&DAC_CH2)<<15;
	DAC->regs->CR |= flags; // enable the channels simultanously
}

/**
 * @brief Disable a DAC channel
 * @param channel channel to disable, 1 and/or 2
 */
void dac_disable(dac_channel_t channel)
{
	uint32 flags = (channel&DAC_CH1) | (channel&DAC_CH2)<<15;
	DAC->regs->CR &= ~flags; // disable the channels simultanously
}
