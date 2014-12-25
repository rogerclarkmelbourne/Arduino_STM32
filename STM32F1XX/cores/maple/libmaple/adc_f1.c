/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/stm32f1/adc.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief STM32F1 ADC support.
 */

#include <libmaple/adc.h>
#include <libmaple/gpio.h>

/*
 * Devices
 */

static adc_dev adc1 = {
    .regs   = ADC1_BASE,
    .clk_id = RCC_ADC1,
};
/** ADC1 device. */
const adc_dev *ADC1 = &adc1;

static adc_dev adc2 = {
    .regs   = ADC2_BASE,
    .clk_id = RCC_ADC2,
};
/** ADC2 device. */
const adc_dev *ADC2 = &adc2;

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
static adc_dev adc3 = {
    .regs   = ADC3_BASE,
    .clk_id = RCC_ADC3,
};
/** ADC3 device. */
const adc_dev *ADC3 = &adc3;
#endif

/*
 * STM32F1 routines
 */

/**
 * @brief Calibrate an ADC peripheral
 *
 * Availability: STM32F1.
 *
 * @param dev adc device
 */
void adc_calibrate(const adc_dev *dev) {
    __io uint32 *rstcal_bit = bb_perip(&(dev->regs->CR2), 3);
    __io uint32 *cal_bit = bb_perip(&(dev->regs->CR2), 2);

    *rstcal_bit = 1;
    while (*rstcal_bit)
        ;

    *cal_bit = 1;
    while (*cal_bit)
        ;
}

/*
 * Common routines
 */

void adc_set_prescaler(adc_prescaler pre) {
    rcc_set_prescaler(RCC_PRESCALER_ADC, (uint32)pre);
}

void adc_foreach(void (*fn)(const adc_dev*)) {
    fn(ADC1);
    fn(ADC2);
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
    fn(ADC3);
#endif
}

void adc_config_gpio(const adc_dev *ignored, gpio_dev *gdev, uint8 bit) {
    gpio_set_mode(gdev, bit, GPIO_INPUT_ANALOG);
}

void adc_enable_single_swstart(const adc_dev *dev) {
    adc_init(dev);
    adc_set_extsel(dev, ADC_SWSTART);
    adc_set_exttrig(dev, 1);
    adc_enable(dev);
    adc_calibrate(dev);
}
