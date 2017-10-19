/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/adc.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief Analog to digital converter routines
 */

#include <libmaple/adc.h>
#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>

/**
 * @brief Initialize an ADC peripheral.
 *
 * Initializes the RCC clock line for the given peripheral.  Resets
 * ADC device registers.
 *
 * @param dev ADC peripheral to initialize
 */
void adc_init(adc_dev *dev) {
    rcc_clk_enable(dev->clk_id);
    rcc_reset_dev(dev->clk_id);
}

/**
 * @brief Set external event select for regular group
 * @param dev ADC device
 * @param event Event used to trigger the start of conversion.
 * @see adc_extsel_event
 */
void adc_set_extsel(adc_dev *dev, adc_extsel_event event) {
    uint32 cr2 = dev->regs->CR2;
    cr2 &= ~ADC_CR2_EXTSEL;
    cr2 |= event;
    cr2 |= ADC_CR2_EXTTRIG;
    dev->regs->CR2 = cr2;
}

/**
 * @brief Set the sample rate for all channels on an ADC device.
 *
 * Don't call this during conversion.
 *
 * @param dev adc device
 * @param smp_rate sample rate to set
 * @see adc_smp_rate
 */
void adc_set_sample_rate(adc_dev *dev, adc_smp_rate smp_rate) {
    uint32 adc_smpr1_val = 0, adc_smpr2_val = 0;
    int i;

    for (i = 0; i < 10; i++) {
        if (i < 8) {
            /* ADC_SMPR1 determines sample time for channels [10,17] */
            adc_smpr1_val |= smp_rate << (i * 3);
        }
        /* ADC_SMPR2 determines sample time for channels [0,9] */
        adc_smpr2_val |= smp_rate << (i * 3);
    }

    dev->regs->SMPR1 = adc_smpr1_val;
    dev->regs->SMPR2 = adc_smpr2_val;
}

/**
 * @brief Perform a single synchronous software triggered conversion on a
 *        channel.
 * @param dev ADC device to use for reading.
 * @param channel channel to convert
 * @return conversion result
 */
uint16 adc_read(adc_dev *dev, uint8 channel) {
    adc_reg_map *regs = dev->regs;

    adc_set_reg_seqlen(dev, 1);

    regs->SQR3 = channel;
    regs->CR2 |= ADC_CR2_SWSTART;
    while (!(regs->SR & ADC_SR_EOC))
        ;

    return (uint16)(regs->DR & ADC_DR_DATA);
}
