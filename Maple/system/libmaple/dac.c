/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file libmaple/dac.c
 * @brief Digital to analog converter support.
 */

#include <libmaple/dac.h>
#include <libmaple/libmaple.h>
#include <libmaple/gpio.h>

#if STM32_HAVE_DAC
dac_dev dac = {
    .regs = DAC_BASE,
};
const dac_dev *DAC = &dac;
#endif

/**
 * @brief Initialize the digital to analog converter
 * @param dev DAC device
 * @param flags Flags:
 *      DAC_CH1: Enable channel 1
 *      DAC_CH2: Enable channel 2
 * @sideeffect May set PA4 or PA5 to INPUT_ANALOG
 */
void dac_init(const dac_dev *dev, uint32 flags) {
    /* First turn on the clock */
    rcc_clk_enable(RCC_DAC);
    rcc_reset_dev(RCC_DAC);

    if (flags & DAC_CH1) {
        dac_enable_channel(dev, 1);
    }

    if (flags & DAC_CH2) {
        dac_enable_channel(dev, 2);
    }
}

/**
 * @brief Write a 12-bit value to the DAC to output
 * @param dev DAC device
 * @param channel channel to select (1 or 2)
 * @param val value to write
 */
void dac_write_channel(const dac_dev *dev, uint8 channel, uint16 val) {
    switch(channel) {
    case 1:
        dev->regs->DHR12R1 = DAC_DHR12R1_DACC1DHR & val;
        break;
    case 2:
        dev->regs->DHR12R2 = DAC_DHR12R2_DACC2DHR & val;
        break;
    }
}

/**
 * @brief Enable a DAC channel
 * @param dev DAC device
 * @param channel channel to enable, either 1 or 2
 * @sideeffect May change pin mode of PA4 or PA5
 */
void dac_enable_channel(const dac_dev *dev, uint8 channel) {
    /*
     * Setup ANALOG mode on PA4 and PA5. This mapping is consistent
     * across all supported STM32s with a DAC.
     */
    switch (channel) {
    case 1:
        gpio_set_mode(GPIOA, 4, GPIO_MODE_ANALOG);
        dev->regs->CR |= DAC_CR_EN1;
        break;
    case 2:
        gpio_set_mode(GPIOA, 5, GPIO_MODE_ANALOG);
        dev->regs->CR |= DAC_CR_EN2;
        break;
    }
}

/**
 * @brief Disable a DAC channel
 * @param dev DAC device
 * @param channel channel to disable, either 1 or 2
 */
void dac_disable_channel(const dac_dev *dev, uint8 channel) {
    switch (channel) {
    case 1:
        dev->regs->CR &= ~DAC_CR_EN1;
        break;
    case 2:
        dev->regs->CR &= ~DAC_CR_EN2;
        break;
    }
}
