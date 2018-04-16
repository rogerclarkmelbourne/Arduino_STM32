/******************************************************************************
 * The MIT License
 *
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
 * @file adc.c
 *
 * @brief Analog to digital converter routines
 *
 * IMPORTANT: maximum external impedance must be below 0.4kOhms for 1.5
 * sample conversion time.
 *
 * At 55.5 cycles/sample, the external input impedance < 50kOhms.
 *
 * See STM32 manual RM0008 for how to calculate this.
 */

#include "libmaple.h"
#include "rcc.h"
#include "adc.h"

/** ADC1 device. */
const adc_dev ADC1 = {
    .regs   = ADC1_BASE,
    .clk_id = RCC_ADC1
};

/** ADC2 device. */
const adc_dev ADC2 = {
    .regs   = ADC2_BASE,
    .clk_id = RCC_ADC2
};

#ifdef STM32_HIGH_DENSITY
/** ADC3 device. */
const adc_dev ADC3 = {
    .regs   = ADC3_BASE,
    .clk_id = RCC_ADC3
};
#endif

/**
 * @brief Initialize an ADC peripheral.
 *
 * Initializes the RCC clock line for the given peripheral.  Resets
 * ADC device registers.
 *
 * @param dev ADC peripheral to initialize
 */
void adc_init(const adc_dev *dev) {
    rcc_clk_enable(dev->clk_id);
#ifdef STM32F4
    if(dev->clk_id == RCC_ADC1) {
    	rcc_reset_dev(dev->clk_id);
    }
#else
    rcc_reset_dev(dev->clk_id);
#endif
}

/**
 * @brief Set external event select for regular group
 * @param dev ADC device
 * @param event Event used to trigger the start of conversion.
 * @see adc_extsel_event
 */
void adc_set_extsel(const adc_dev *dev, adc_extsel_event event) {
    uint32 cr2 = dev->regs->CR2;
    cr2 &= ~ADC_CR2_EXTSEL;
    cr2 |= event;
    dev->regs->CR2 = cr2;
}

/**
 * @brief Call a function on all ADC devices.
 * @param fn Function to call on each ADC device.
 */
void adc_foreach(void (*fn)(const adc_dev*)) {
    fn(&ADC1);
    fn(&ADC2);
#ifdef STM32_HIGH_DENSITY
    fn(&ADC3);
#endif
}

/**
 * @brief Turn the given sample rate into values for ADC_SMPRx. Don't
 * call this during conversion.
 * @param dev adc device
 * @param smp_rate sample rate to set
 * @see adc_smp_rate
 */
void adc_set_sample_rate(const adc_dev *dev, adc_smp_rate smp_rate) {
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
 * @brief Calibrate an ADC peripheral
 * @param dev adc device
 */
void adc_calibrate(const adc_dev *dev)
{
/*
#ifndef STM32F2
    __IO uint32 *rstcal_bit = bb_perip(&(dev->regs->CR2), 3);
    __IO uint32 *cal_bit = bb_perip(&(dev->regs->CR2), 2);

    *rstcal_bit = 1;
    while (*rstcal_bit)
        ;

    *cal_bit = 1;
    while (*cal_bit)
        ;
#endif
*/
}

/**
 * @brief Perform a single synchronous software triggered conversion on a
 * channel.
 * @param dev ADC device to use for reading.
 * @param channel channel to convert
 * @return conversion result
 */
uint16 adc_read(const adc_dev *dev, uint8 channel) {
    adc_reg_map *regs = dev->regs;

    adc_set_reg_seqlen(dev, 1);

    regs->SQR3 = channel;
    regs->CR2 |= ADC_CR2_SWSTART;
    while(!(regs->SR & ADC_SR_EOC))
        ;

    return (uint16)(regs->DR & ADC_DR_DATA);
}

void setupADC_F4(void)
{
		  uint32 tmpreg1 = 0;

		  tmpreg1 = ADC_COMMON->CCR;

		  /* Clear MULTI, DELAY, DMA and ADCPRE bits */
#define CR_CLEAR_MASK  ((uint32)0xFFFC30E0)
		  tmpreg1 &= CR_CLEAR_MASK;

		  /* Configure ADCx: Multi mode, Delay between two sampling time, ADC prescaler,
		     and DMA access mode for multimode */
		  /* Set MULTI bits according to ADC_Mode value */
		  /* Set ADCPRE bits according to ADC_Prescaler value */
		  /* Set DMA bits according to ADC_DMAAccessMode value */
		  /* Set DELAY bits according to ADC_TwoSamplingDelay value */
#define ADC_Mode_Independent            0
#define ADC_Prescaler_Div2              0
#define ADC_DMAAccessMode_Disabled      0     /* DMA mode disabled */
#define ADC_TwoSamplingDelay_5Cycles    0

		  tmpreg1 |= ADC_Mode_Independent | ADC_Prescaler_Div2 | ADC_DMAAccessMode_Disabled | ADC_TwoSamplingDelay_5Cycles;

		  /* Write to ADC CCR */
		  ADC_COMMON->CCR = tmpreg1;
}
