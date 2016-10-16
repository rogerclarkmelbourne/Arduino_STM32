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
 * @file libmaple/stm32f1/include/series/adc.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief STM32F1 ADC header.
 */

#ifndef _LIBMAPLE_STM32F1_ADC_H_
#define _LIBMAPLE_STM32F1_ADC_H_

#include <libmaple/bitband.h>
#include <libmaple/libmaple_types.h>
#include <libmaple/rcc.h>       /* For the prescalers */

/*
 * Devices
 */
extern adc_dev adc1;
extern struct adc_dev *ADC1;
extern adc_dev adc2;
extern struct adc_dev *ADC2;
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
extern adc_dev adc3;
extern struct adc_dev *ADC3;
#endif

/*
 * Register map base pointers
 */

/** STM32F1 ADC1 register map base pointer. */
#define ADC1_BASE                       ((struct adc_reg_map*)0x40012400)
/** STM32F1 ADC2 register map base pointer. */
#define ADC2_BASE                       ((struct adc_reg_map*)0x40012800)
/** STM32F1 ADC3 register map base pointer. */
#define ADC3_BASE                       ((struct adc_reg_map*)0x40013C00)

/*
 * Register bit definitions
 */

/* Control register 2 */

#define ADC_CR2_ADON_BIT                0
#define ADC_CR2_CONT_BIT                1
#define ADC_CR2_CAL_BIT                 2
#define ADC_CR2_RSTCAL_BIT              3
#define ADC_CR2_DMA_BIT                 8
#define ADC_CR2_ALIGN_BIT               11
#define ADC_CR2_JEXTTRIG_BIT            15
#define ADC_CR2_EXTTRIG_BIT             20
#define ADC_CR2_JSWSTART_BIT            21
#define ADC_CR2_SWSTART_BIT             22
#define ADC_CR2_TSVREFE_BIT             23

#define ADC_CR2_ADON                    (1U << ADC_CR2_ADON_BIT)
#define ADC_CR2_CONT                    (1U << ADC_CR2_CONT_BIT)
#define ADC_CR2_CAL                     (1U << ADC_CR2_CAL_BIT)
#define ADC_CR2_RSTCAL                  (1U << ADC_CR2_RSTCAL_BIT)
#define ADC_CR2_DMA                     (1U << ADC_CR2_DMA_BIT)
#define ADC_CR2_ALIGN                   (1U << ADC_CR2_ALIGN_BIT)
#define ADC_CR2_JEXTSEL                 0x7000
#define ADC_CR2_JEXTSEL_TIM1_TRGO       (0x0 << 12)
#define ADC_CR2_JEXTSEL_TIM1_CC4        (0x1 << 12)
#define ADC_CR2_JEXTSEL_TIM2_TRGO       (0x2 << 12)
#define ADC_CR2_JEXTSEL_TIM2_CC1        (0x3 << 12)
#define ADC_CR2_JEXTSEL_TIM3_CC4        (0x4 << 12)
#define ADC_CR2_JEXTSEL_TIM4_TRGO       (0x5 << 12)
#define ADC_CR2_JEXTSEL_EXTI15          (0x6 << 12)
#define ADC_CR2_JEXTSEL_JSWSTART        (0x7 << 12)
#define ADC_CR2_JEXTTRIG                (1U << ADC_CR2_JEXTTRIG_BIT)
#define ADC_CR2_EXTSEL                  0xE0000
#define ADC_CR2_EXTSEL_TIM1_CC1         (0x0 << 17)
#define ADC_CR2_EXTSEL_TIM1_CC2         (0x1 << 17)
#define ADC_CR2_EXTSEL_TIM1_CC3         (0x2 << 17)
#define ADC_CR2_EXTSEL_TIM2_CC2         (0x3 << 17)
#define ADC_CR2_EXTSEL_TIM3_TRGO        (0x4 << 17)
#define ADC_CR2_EXTSEL_TIM4_CC4         (0x5 << 17)
#define ADC_CR2_EXTSEL_EXTI11           (0x6 << 17)
#define ADC_CR2_EXTSEL_SWSTART          (0x7 << 17)
#define ADC_CR2_EXTTRIG                 (1U << ADC_CR2_EXTTRIG_BIT)
#define ADC_CR2_JSWSTART                (1U << ADC_CR2_JSWSTART_BIT)
#define ADC_CR2_SWSTART                 (1U << ADC_CR2_SWSTART_BIT)
#define ADC_CR2_TSVREFE                 (1U << ADC_CR2_TSVREFE_BIT)

/*
 * Other types
 */

/**
 * @brief STM32F1 external event selectors for regular group
 *        conversion.
 *
 * Some external events are only available on ADCs 1 and 2, others
 * only on ADC3, while others are available on all three ADCs.
 * Additionally, some events are only available on high- and
 * XL-density STM32F1 MCUs, as they use peripherals only available on
 * those MCU densities.
 *
 * For ease of use, each event selector is given along with the ADCs
 * it's available on, along with any other availability restrictions.
 *
 * @see adc_set_extsel()
 */
typedef enum adc_extsel_event {
    /* TODO: Smarten this up a bit, as follows.
     *
     * The EXTSEL bits on F1 are a little brain-damaged in that the
     * TIM8 TRGO event has different bits depending on whether you're
     * using ADC1/2 or ADC3.  We route around this by declaring two
     * enumerators, ADC_EXT_EV_ADC12_TIM8_TRGO and
     * ADC_EXT_EV_ADC3_TIM8_TRGO.
     *
     * The right thing to do is to provide a single
     * ADC_EXT_EV_TIM8_TRGO enumerator and override adc_set_extsel on
     * STM32F1 to handle this situation correctly. We can do that
     * later, though, and change the per-ADC enumerator values to
     * ADC_EXT_EV_TIM8_TRGO to preserve compatibility. */

    /* ADC1 and ADC2 only: */
    ADC_EXT_EV_TIM1_CC1  = 0x00000, /**< ADC1, ADC2: Timer 1 CC1 event */
    ADC_EXT_EV_TIM1_CC2  = 0x20000, /**< ADC1, ADC2: Timer 1 CC2 event */
    ADC_EXT_EV_TIM2_CC2  = 0x60000, /**< ADC1, ADC2: Timer 2 CC2 event */
    ADC_EXT_EV_TIM3_TRGO = 0x80000, /**< ADC1, ADC2: Timer 3 TRGO event */
    ADC_EXT_EV_TIM4_CC4  = 0xA0000, /**< ADC1, ADC2: Timer 4 CC4 event */
    ADC_EXT_EV_EXTI11    = 0xC0000, /**< ADC1, ADC2: EXTI11 event */

    /* Common: */
    ADC_EXT_EV_TIM1_CC3  = 0x40000, /**< ADC1, ADC2, ADC3: Timer 1 CC3 event */
    ADC_EXT_EV_SWSTART   = 0xE0000, /**< ADC1, ADC2, ADC3: Software start */

    /* HD only: */
    ADC_EXT_EV_TIM3_CC1  = 0x00000, /**<
                                     * ADC3: Timer 3 CC1 event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_TIM2_CC3  = 0x20000, /**<
                                     * ADC3: Timer 2 CC3 event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_TIM8_CC1  = 0x60000, /**<
                                     * ADC3: Timer 8 CC1 event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_ADC3_TIM8_TRGO = 0x80000, /**<
                                     * ADC3: Timer 8 TRGO event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_TIM5_CC1  = 0xA0000, /**<
                                     * ADC3: Timer 5 CC1 event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_ADC12_TIM8_TRGO = 0xC0000, /**<
                                     * ADC1, ADC2: Timer 8 TRGO event
                                     * Availability: high- and XL-density. */
    ADC_EXT_EV_TIM5_CC3  = 0xC0000, /**<
                                     * ADC3: Timer 5 CC3 event
                                     * Availability: high- and XL-density. */
} adc_extsel_event;

/* We'll keep these old adc_extsel_event enumerators around for a
 * while, for backwards compatibility: */
/** Deprecated. Use ADC_EXT_EV_TIM1_CC1 instead. */
#define ADC_ADC12_TIM1_CC1  ADC_EXT_EV_TIM1_CC1
/** Deprecated. Use ADC_EXT_EV_TIM1_CC2 instead. */
#define ADC_ADC12_TIM1_CC2  ADC_EXT_EV_TIM1_CC2
/** Deprecated. Use ADC_EXT_EV_TIM1_CC3 instead. */
#define ADC_ADC12_TIM1_CC3  ADC_EXT_EV_TIM1_CC3
/** Deprecated. Use ADC_EXT_EV_TIM2_CC2 instead. */
#define ADC_ADC12_TIM2_CC2  ADC_EXT_EV_TIM2_CC2
/** Deprecated. Use ADC_EXT_EV_TIM3_TRGO instead. */
#define ADC_ADC12_TIM3_TRGO ADC_EXT_EV_TIM3_TRGO
/** Deprecated. Use ADC_EXT_EV_TIM4_CC4 instead. */
#define ADC_ADC12_TIM4_CC4  ADC_EXT_EV_TIM4_CC4
/** Deprecated. Use ADC_EXT_EV_EXTI11 instead. */
#define ADC_ADC12_EXTI11    ADC_EXT_EV_EXTI11
/** Deprecated. Use ADC_EXT_EV_ADC12_TIM8_TRGO instead. */
#define ADC_ADC12_TIM8_TRGO ADC_EXT_EV_ADC12_TIM8_TRGO
/** Deprecated. Use ADC_EXT_EV_SWSTART instead. */
#define ADC_ADC12_SWSTART   ADC_EXT_EV_SWSTART
/** Deprecated. Use ADC_EXT_EV_TIM1_CC1 instead. */
#define ADC_ADC3_TIM3_CC1   ADC_EXT_EV_TIM1_CC1
/** Deprecated. Use ADC_EXT_EV_TIM1_CC2 instead. */
#define ADC_ADC3_TIM2_CC3   ADC_EXT_EV_TIM1_CC2
/** Deprecated. Use ADC_EXT_EV_TIM1_CC3 instead. */
#define ADC_ADC3_TIM1_CC3   ADC_EXT_EV_TIM1_CC3
/** Deprecated. Use ADC_EXT_EV_TIM2_CC2 instead. */
#define ADC_ADC3_TIM8_CC1   ADC_EXT_EV_TIM2_CC2
/** Deprecated. Use ADC_EXT_EV_TIM3_TRGO instead. */
#define ADC_ADC3_TIM8_TRGO  ADC_EXT_EV_TIM3_TRGO
/** Deprecated. Use ADC_EXT_EV_TIM4_CC4 instead. */
#define ADC_ADC3_TIM5_CC1   ADC_EXT_EV_TIM4_CC4
/** Deprecated. Use ADC_EXT_EV_EXTI11 instead. */
#define ADC_ADC3_TIM5_CC3   ADC_EXT_EV_EXTI11
/** Deprecated. Use ADC_EXT_EV_TIM8_TRGO instead. */
#define ADC_ADC3_SWSTART    ADC_EXT_EV_TIM8_TRGO
/** Deprecated. Use ADC_EXT_EV_SWSTART instead. */
#define ADC_SWSTART         ADC_EXT_EV_SWSTART

/**
 * @brief STM32F1 sample times, in ADC clock cycles.
 *
 * These control the amount of time spent sampling the input voltage.
 */
typedef enum adc_smp_rate {
    ADC_SMPR_1_5,               /**< 1.5 ADC cycles */
    ADC_SMPR_7_5,               /**< 7.5 ADC cycles */
    ADC_SMPR_13_5,              /**< 13.5 ADC cycles */
    ADC_SMPR_28_5,              /**< 28.5 ADC cycles */
    ADC_SMPR_41_5,              /**< 41.5 ADC cycles */
    ADC_SMPR_55_5,              /**< 55.5 ADC cycles */
    ADC_SMPR_71_5,              /**< 71.5 ADC cycles */
    ADC_SMPR_239_5,             /**< 239.5 ADC cycles */
} adc_smp_rate;

/**
 * @brief STM32F1 ADC prescalers, as divisors of PCLK2.
 */
typedef enum adc_prescaler {
    /** PCLK2 divided by 2 */
    ADC_PRE_PCLK2_DIV_2 = RCC_ADCPRE_PCLK_DIV_2,
    /** PCLK2 divided by 4 */
    ADC_PRE_PCLK2_DIV_4 = RCC_ADCPRE_PCLK_DIV_4,
    /** PCLK2 divided by 6 */
    ADC_PRE_PCLK2_DIV_6 = RCC_ADCPRE_PCLK_DIV_6,
    /** PCLK2 divided by 8 */
    ADC_PRE_PCLK2_DIV_8 = RCC_ADCPRE_PCLK_DIV_8,
} adc_prescaler;

/*
 * Routines
 */

void adc_calibrate(adc_dev *dev);

/**
 * @brief Set external trigger conversion mode event for regular channels
 *
 * Availability: STM32F1.
 *
 * @param dev    ADC device
 * @param enable If 1, conversion on external events is enabled; if 0,
 *               disabled.
 */
static inline void adc_set_exttrig(adc_dev *dev, uint8 enable) {
    *bb_perip(&dev->regs->CR2, ADC_CR2_EXTTRIG_BIT) = !!enable;
}

#endif
