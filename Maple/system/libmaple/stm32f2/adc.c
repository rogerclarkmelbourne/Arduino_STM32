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
 * @file libmaple/stm32f2/adc.c
 * @brief STM32F2 ADC.
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

static adc_dev adc3 = {
    .regs   = ADC3_BASE,
    .clk_id = RCC_ADC3,
};
/** ADC3 device. */
const adc_dev *ADC3 = &adc3;

/*
 * Common routines
 */

void adc_set_prescaler(adc_prescaler pre) {
    uint32 ccr = ADC_COMMON_BASE->CCR;
    ccr &= ~ADC_CCR_ADCPRE;
    ccr |= (uint32)pre;
    ADC_COMMON_BASE->CCR = ccr;
}

void adc_foreach(void (*fn)(const adc_dev*)) {
    fn(ADC1);
    fn(ADC2);
    fn(ADC3);
}

void adc_config_gpio(const adc_dev *ignored, gpio_dev *gdev, uint8 bit) {
    gpio_set_modef(gdev, bit, GPIO_MODE_ANALOG, GPIO_MODEF_PUPD_NONE);
}

void adc_enable_single_swstart(const adc_dev *dev) {
    adc_init(dev);
    adc_enable(dev);
}
