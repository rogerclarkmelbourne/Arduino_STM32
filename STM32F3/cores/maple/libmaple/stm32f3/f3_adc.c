/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file libmaple/stm32f3/adc.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>,
 *         F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 ADC support.
 */

#include <libmaple/adc.h>
#include <libmaple/gpio.h>
#include <libmaple/nvic.h>

/*
 * Devices
 */

static adc_private_data adc1_priv;
static adc_dev adc1 = {
    .regs   = ADC1_BASE,
    .clk_id = RCC_ADC12,
		.priv = &adc1_priv
};
/** ADC1 device. */
const adc_dev *ADC1 = &adc1;

static adc_private_data adc2_priv;
static adc_dev adc2 = {
    .regs   = ADC2_BASE,
    .clk_id = RCC_ADC12,
		.priv = &adc2_priv
};
/** ADC2 device. */
const adc_dev *ADC2 = &adc2;

#if STM32_F3_LINE == STM32_F3_LINE_303
static adc_private_data adc3_priv;
static adc_dev adc3 = {
    .regs   = ADC3_BASE,
    .clk_id = RCC_ADC34,
		.priv = &adc3_priv
};
/** ADC3 device. */
const adc_dev *ADC3 = &adc3;

static adc_private_data adc4_priv;
static adc_dev adc4 = {
    .regs   = ADC4_BASE,
    .clk_id = RCC_ADC34,
		.priv = &adc4_priv
};
/** ADC4 device. */
const adc_dev *ADC4 = &adc4;
#endif

/*
 * STM32F3 routines
 */

static inline void adc_check_regular_notongoing(const adc_dev *dev) {
    adc_reg_map *regs = dev->regs;
		if(regs->CR & ADC_CR_ADSTART) /* check for ongoing regular channel conversion */
		{
			regs->CR |= ADC_CR_ADSTP;	/* stop regular channel conversion */
			while(regs->CR & ADC_CR_ADSTP)
				; /* wait for conversion to stop */
		}
}

static inline void adc_check_injected_notongoing(const adc_dev *dev) {
    adc_reg_map *regs = dev->regs;
		if(regs->CR & ADC_CR_JADSTART) /* check for ongoing injected channel conversion */
		{
			regs->CR |= ADC_CR_JADSTP; /* stop injected channel conversion */
			while(regs->CR & ADC_CR_JADSTP)
				; /* wait for conversion to stop */
		}
}

void adc_set_extsel(const adc_dev *dev, adc_extsel_event event) {
		adc_check_regular_notongoing(dev);

    uint32 cfgr = dev->regs->CFGR;
    cfgr &= ~ADC_CFGR_EXTSEL;
    cfgr |= event;
    dev->regs->CFGR = cfgr;
}

void adc_set_resolution(const adc_dev *dev, adc_resolution res) {
		adc_check_regular_notongoing(dev);
		adc_check_injected_notongoing(dev);

    uint32 cfgr = dev->regs->CFGR;
    cfgr &= ~ADC_CFGR_RES;
    cfgr |= res;
    dev->regs->CFGR = cfgr;
}

void adc_set_sample_rate(const adc_dev *dev, adc_smp_rate smp_rate) {
		adc_check_regular_notongoing(dev);
		adc_check_injected_notongoing(dev);

    uint32 adc_smpr1_val = 0, adc_smpr2_val = 0;
    int i;

    for(i = 0; i < 9; i++) {
			/* ADC_SMPR1 determines sample time for channels [1,9] */
			adc_smpr1_val |= smp_rate << (i * 3 + 3);
			/* ADC_SMPR2 determines sample time for channels [10,18] */
			adc_smpr2_val |= smp_rate << (i * 3);
		}

    dev->regs->SMPR1 = adc_smpr1_val;
    dev->regs->SMPR2 = adc_smpr2_val;
}

void adc_enable_scan(const adc_dev *dev) {
		/* FIXME nonexistent in F3 series */
}

void adc_disable_scan(const adc_dev *dev) {
		/* FIXME nonexistent in F3 series*/
}

void adc_enable_continuous(const adc_dev *dev) {
    bb_peri_set_bit(&dev->regs->CFGR, ADC_CFGR_CONT_BIT, 1);
}

void adc_disable_continuous(const adc_dev *dev) {
    bb_peri_set_bit(&dev->regs->CFGR, ADC_CFGR_CONT_BIT, 0);
}

#define BITS_PER_SQ 6
#define SQs_PER_SQR 5
void adc_set_conv_seq(const adc_dev *dev, const uint8 *channels, uint8 len) {
    ASSERT( (0 < len) && (len <= 16) );
		uint8 i;
		uint32 val = 0;
		uint8 lshift;
    __io uint32 *sqr = &dev->regs->SQR1;

		for (i=0; i<len; i++)
		{
			lshift = (i+1) % SQs_PER_SQR;
			if (!lshift)
			{
				*sqr = val;
				sqr++;
				val = 0;
			}
			val |= channels[i] << (lshift * BITS_PER_SQ);
		}
		*sqr = val;

		adc_set_reg_seqlen(dev, len);
}

uint16 adc_read(const adc_dev *dev, uint8 channel) {
    adc_reg_map *regs = dev->regs;

		adc_set_conv_seq(dev, &channel, 1);

    regs->CR |= ADC_CR_ADSTART; /* start conversion */
    while (!(regs->ISR & ADC_ISR_EOC))
        ; /* wait until done */

    return (uint16)(regs->DR & ADC_DR_RDATA);
}

void adc_attach_interrupt(const adc_dev *dev, uint32 interrupt_flags,
                          void (*handler)(adc_callback_data*), void *arg) {
    adc_private_data *priv = dev->priv;
    priv->handler = handler;
    priv->handler_flags = interrupt_flags;
    priv->cb_data.arg = arg;
    adc_enable_interrupts(dev, interrupt_flags);
}

void adc_detach_interrupt(const adc_dev *dev) {
    adc_private_data *priv;
    adc_disable_interrupts(dev, ADC_ALL_INTERRUPTS);
    priv = dev->priv;
    priv->handler = NULL;
    priv->handler_flags = 0;
}

void adc_enable_interrupts(const adc_dev *dev, uint32 interrupt_flags) {
    uint32 ier = dev->regs->IER;
    ier |= interrupt_flags;
    dev->regs->IER = ier;
    _adc_enable_dev_irq(dev);
}

void adc_disable_interrupts(const adc_dev *dev, uint32 interrupt_flags) {
    /* Don't use nvic_irq_disable()! IRQs are shared among ADCs. */
    uint32 ier = dev->regs->IER;
    ier &= ~interrupt_flags;
    dev->regs->IER = ier;
}

void adc_calibrate(const adc_dev *dev) {
		adc_reg_map *regs = dev->regs;

		if( (regs->CR & ADC_CR_ADVREGEN) != ADC_CR_ADVREGEN_ENABLE)
			adc_regulator_enable(dev); /* ensure that voltage regulator is enabled */

		if(regs->CR & ADC_CR_ADEN)
			adc_disable(dev); /* ensure that ADC is disabled */

		regs->CR &= ~ADC_CR_ADCALDIF; /* calibrate in single-ended mode */
		regs->CR |= ADC_CR_ADCAL; /* start calibration */
		while (regs->CR & ADC_CR_ADCAL)
			; /* wait until done */

		regs->CR |= ADC_CR_ADCALDIF; /* calibrate in differential mode */
		regs->CR |= ADC_CR_ADCAL; /* start calibration */
		while (regs->CR & ADC_CR_ADCAL)
			; /* wait until done */
}

void adc_set_exttrig(const adc_dev *dev, adc_exttrig_mode mode) {
		adc_reg_map *regs = dev->regs;
		regs->CFGR &= ~ADC_CFGR_EXTEN;
		regs->CFGR |= mode;
}

void adc_set_prescaler(adc_prescaler pre) {
		if (pre & 0x10) { /* PLL is used as clock source */
			ADC12_BASE->CCR &= ~ADC_CCR_CKMODE;
#if STM32_F3_LINE == STM32_F3_LINE_303
			ADC34_BASE->CCR &= ~ADC_CCR_CKMODE;
#endif

			uint32 cfgr2 = RCC_BASE->CFGR2;

			cfgr2 &= ~RCC_CFGR2_ADC12PRES; // clear register
			cfgr2 |= (uint32)pre << RCC_CFGR2_ADC12PRES_SHIFT; // set register

#if STM32_F3_LINE == STM32_F3_LINE_303
			cfgr2 &= ~RCC_CFGR2_ADC34PRES; // clear register
			cfgr2 |= (uint32)pre << RCC_CFGR2_ADC34PRES_SHIFT; // set register
#endif

			RCC_BASE->CFGR2 = cfgr2;
		} else { /* AHB bus is used as clock source */
			/* FIXME does not work with current wirish booting routine */
			uint32 tmp;

			tmp = ADC12_BASE->CCR;
			tmp &= ~ADC_CCR_CKMODE;
			tmp |= pre << ADC_CCR_CKMODE_SHIFT;
			ADC12_BASE->CCR = tmp;

#if STM32_F3_LINE == STM32_F3_LINE_303
			tmp = ADC34_BASE->CCR;
			tmp &= ~ADC_CCR_CKMODE;
			tmp |= pre << ADC_CCR_CKMODE_SHIFT;
			ADC34_BASE->CCR = tmp;
#endif
		}
}

void adc_foreach(void (*fn)(const adc_dev*)) {
    fn(ADC1);
    fn(ADC2);
#if STM32_F3_LINE == STM32_F3_LINE_303
    fn(ADC3);
    fn(ADC4);
#endif
}

void adc_config_gpio(const adc_dev *ignored, gpio_dev *gdev, uint8 bit) {
    gpio_set_modef(gdev, bit, GPIO_MODE_ANALOG, GPIO_MODEF_PUPD_NONE);
}

void adc_enable_single_swstart(const adc_dev *dev) {
    int check_dev_adc = dev == ADC1;
#if STM32_F3_LINE == STM32_F3_LINE_303
		check_dev_adc = (check_dev_adc || dev == ADC3);
#endif
		if (check_dev_adc)
			adc_init(dev); /* FIXME hack needed for wirish, as master and slave ADC share the same reset register */
    adc_set_exttrig(dev, ADC_EXTTRIG_MODE_SOFTWARE);
		adc_regulator_enable(dev);
    adc_calibrate(dev);
    adc_enable(dev);
}

void adc_set_reg_seqlen(const adc_dev *dev, uint8 length) {
		adc_check_regular_notongoing(dev);

    uint32 tmp = dev->regs->SQR1;
    tmp &= ~ADC_SQR1_L;
    tmp |= (length - 1) & ADC_SQR1_L;
    dev->regs->SQR1 = tmp;
}

void adc_enable(const adc_dev *dev) {
		adc_reg_map *regs = dev->regs;

		while( (regs->CR & ADC_CR_ADVREGEN) != ADC_CR_ADVREGEN_ENABLE)
			adc_regulator_enable(dev); /* ensure that voltage regulator is enabled */

		regs->CR |= ADC_CR_ADEN; /* enable ADC */
		while (!(regs->ISR & ADC_ISR_ADRDY))
			; /* wait until ADC is ready */
}

void adc_disable(const adc_dev *dev) {
		adc_reg_map *regs = dev->regs;

		adc_check_regular_notongoing(dev);
		adc_check_injected_notongoing(dev);

		regs->CR |= ADC_CR_ADDIS; /* disable ADC */ 
		while(regs->CR & ADC_CR_ADEN)
			; /* wait until ADC is effectively disabled */
}

void adc_regulator_enable(const adc_dev *dev) {
		adc_reg_map *regs = dev->regs;

		regs->CR &= ~ADC_CR_ADVREGEN;
		regs->CR |= ADC_CR_ADVREGEN_ENABLE;

		delay_us(10); /* 10us are worst case */
}

void adc_regulator_disable(const adc_dev *dev) {
		adc_reg_map *regs = dev->regs;

		regs->CR &= ~ADC_CR_ADVREGEN;
		regs->CR |= ADC_CR_ADVREGEN_DISABLE;
}

/*
 * Private API
 */

void _adc_enable_dev_irq(const adc_dev *dev) {
		if ( (dev == ADC1) || (dev == ADC2) )
			nvic_irq_enable(NVIC_ADC1_2);
#if STM32_F3_LINE == STM32_F3_LINE_303
		else {
			if (dev == ADC3)
				nvic_irq_enable(NVIC_ADC3);
			else // dev == ADC4
				nvic_irq_enable(NVIC_ADC4);
		}
#endif
}

/* IRQ handler for adc_attach_interrupt() */
static __always_inline void adc_irq(const adc_dev *dev) {
    adc_private_data *priv = dev->priv;
    uint32 irq_flags = dev->regs->ISR & priv->handler_flags;

    if (!irq_flags) {
        /* The user isn't interested in this IRQ. */
        return;
    } else if (priv->handler) {
        priv->cb_data.irq_flags = irq_flags;
        priv->handler(&priv->cb_data);
    }
}

/*
 * IRQ handlers for adc_attach_interrupt()
 */
/*
void __irq_adc1_2(void) {
    adc_irq(ADC1);
    adc_irq(ADC2);
}

#if STM32_F3_LINE == STM32_F3_LINE_303
void __irq_adc3(void) {
		adc_irq(ADC3);
}

void __irq_adc4(void) {
		adc_irq(ADC4);
}
#endif
*/
