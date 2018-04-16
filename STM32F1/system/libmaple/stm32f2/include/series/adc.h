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
 * @file libmaple/stm32f2/include/series/adc.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 * @brief STM32F2 ADC support.
 */

#ifndef _LIBMAPLE_STM32F2_ADC_H_
#define _LIBMAPLE_STM32F2_ADC_H_

#include <libmaple/libmaple_types.h>

/*
 * Devices
 */

extern const struct adc_dev *ADC1;
extern const struct adc_dev *ADC2;
extern const struct adc_dev *ADC3;

/*
 * Common register map
 */

/** ADC common register map type */
typedef struct adc_common_reg_map {
    __IO uint32 CSR;            /**< Common status register */
    __IO uint32 CCR;            /**< Common control register */
    __IO uint32 CDR;            /**<
                                 * @brief Common regular data register
                                 *        for dual and triple modes */
} adc_common_reg_map;

/*
 * Register map base pointers
 */

/** ADC1 register map base pointer. */
#define ADC1_BASE       ((struct adc_reg_map*)0x40012000)
/** ADC2 register map base pointer. */
#define ADC2_BASE       ((struct adc_reg_map*)0x40012100)
/** ADC3 register map base pointer. */
#define ADC3_BASE       ((struct adc_reg_map*)0x40012200)
/** ADC common register map base pointer. */
#define ADC_COMMON_BASE ((struct adc_common_reg_map*)0x40012300)

/*
 * Register bit definitions
 */

/* Status register */

/** Overrun bit. */
#define ADC_SR_OVR_BIT                  5
/** Overrun. */
#define ADC_SR_OVR                      (1U << ADC_SR_OVR_BIT)

/* Control register 1 */

/** Overrun interrupt enable bit. */
#define ADC_CR1_OVRIE_BIT               26

/** Overrun interrupt error enable. */
#define ADC_CR1_OVRIE                   (1U << ADC_CR1_OVRIE_BIT)
/** Conversion resolution. */
#define ADC_CR1_RES                     (0x3U << 24)
/** Conversion resolution: 12 bit (at least 15 ADCCLK cycles). */
#define ADC_CR1_RES_12BIT               (0x0U << 24)
/** Conversion resolution: 10 bit (at least 13 ADCCLK cycles). */
#define ADC_CR1_RES_10BIT               (0x1U << 24)
/** Conversion resolution: 8 bit (at least 11 ADCCLK cycles). */
#define ADC_CR1_RES_8BIT                (0x2U << 24)
/** Conversion resolution: 6 bit (at least 9 ADCCLK cycles). */
#define ADC_CR1_RES_6BIT                (0x3U << 24)

/* Control register 2 */

#define ADC_CR2_SWSTART_BIT             30
#define ADC_CR2_JSWSTART_BIT            22
#define ADC_CR2_ALIGN_BIT               11
#define ADC_CR2_EOCS_BIT                10
#define ADC_CR2_DDS_BIT                 9
#define ADC_CR2_DMA_BIT                 8
#define ADC_CR2_CONT_BIT                1
#define ADC_CR2_ADON_BIT                0

#define ADC_CR2_SWSTART                 (1U << ADC_CR2_SWSTART_BIT)
#define ADC_CR2_EXTEN                   (0x3 << 28)
#define ADC_CR2_EXTEN_DISABLED          (0x0 << 28)
#define ADC_CR2_EXTEN_RISE              (0x1 << 28)
#define ADC_CR2_EXTEN_FALL              (0x2 << 28)
#define ADC_CR2_EXTEN_RISE_FALL         (0x3 << 28)
#define ADC_CR2_EXTSEL                  (0xF << 24)
#define ADC_CR2_EXTSEL_TIM1_CC1         (0x0 << 24)
#define ADC_CR2_EXTSEL_TIM1_CC2         (0x1 << 24)
#define ADC_CR2_EXTSEL_TIM1_CC3         (0x2 << 24)
#define ADC_CR2_EXTSEL_TIM2_CC2         (0x3 << 24)
#define ADC_CR2_EXTSEL_TIM2_CC3         (0x4 << 24)
#define ADC_CR2_EXTSEL_TIM2_CC4         (0x5 << 24)
#define ADC_CR2_EXTSEL_TIM1_TRGO        (0x6 << 24)
#define ADC_CR2_EXTSEL_TIM3_CC1         (0x7 << 24)
#define ADC_CR2_EXTSEL_TIM3_TRGO        (0x8 << 24)
#define ADC_CR2_EXTSEL_TIM4_CC4         (0x9 << 24)
#define ADC_CR2_EXTSEL_TIM5_CC1         (0xA << 24)
#define ADC_CR2_EXTSEL_TIM5_CC2         (0xB << 24)
#define ADC_CR2_EXTSEL_TIM5_CC3         (0xC << 24)
#define ADC_CR2_EXTSEL_TIM8_CC1         (0xD << 24)
#define ADC_CR2_EXTSEL_TIM8_TRGO        (0xE << 24)
#define ADC_CR2_EXTSEL_TIM1_EXTI11      (0xF << 24)
#define ADC_CR2_JSWSTART                (1U << ADC_CR2_JSWSTART_BIT)
#define ADC_CR2_JEXTEN                  (0x3 << 20)
#define ADC_CR2_JEXTEN_DISABLED         (0x0 << 20)
#define ADC_CR2_JEXTEN_RISE             (0x1 << 20)
#define ADC_CR2_JEXTEN_FALL             (0x2 << 20)
#define ADC_CR2_JEXTEN_RISE_FALL        (0x3 << 20)
#define ADC_CR2_JEXTSEL                 (0xF << 16)
#define ADC_CR2_JEXTSEL_TIM1_CC4        (0x0 << 16)
#define ADC_CR2_JEXTSEL_TIM1_TRGO       (0x1 << 16)
#define ADC_CR2_JEXTSEL_TIM2_CC1        (0x2 << 16)
#define ADC_CR2_JEXTSEL_TIM2_TRGO       (0x3 << 16)
#define ADC_CR2_JEXTSEL_TIM3_CC2        (0x4 << 16)
#define ADC_CR2_JEXTSEL_TIM3_CC4        (0x5 << 16)
#define ADC_CR2_JEXTSEL_TIM4_CC1        (0x6 << 16)
#define ADC_CR2_JEXTSEL_TIM4_CC2        (0x7 << 16)
#define ADC_CR2_JEXTSEL_TIM4_CC3        (0x8 << 16)
#define ADC_CR2_JEXTSEL_TIM4_TRGO       (0x9 << 16)
#define ADC_CR2_JEXTSEL_TIM5_CC4        (0xA << 16)
#define ADC_CR2_JEXTSEL_TIM5_TRGO       (0xB << 16)
#define ADC_CR2_JEXTSEL_TIM8_CC2        (0xC << 16)
#define ADC_CR2_JEXTSEL_TIM8_CC3        (0xD << 16)
#define ADC_CR2_JEXTSEL_TIM8_CC4        (0xE << 16)
#define ADC_CR2_JEXTSEL_TIM1_EXTI15     (0xF << 16)
#define ADC_CR2_ALIGN                   (1U << ADC_CR2_ALIGN_BIT)
#define ADC_CR2_ALIGN_RIGHT             (0U << ADC_CR2_ALIGN_BIT)
#define ADC_CR2_ALIGN_LEFT              (1U << ADC_CR2_ALIGN_BIT)
#define ADC_CR2_EOCS                    (1U << ADC_CR2_EOCS_BIT)
#define ADC_CR2_EOCS_SEQUENCE           (0U << ADC_CR2_EOCS_BIT)
#define ADC_CR2_EOCS_CONVERSION         (1U << ADC_CR2_EOCS_BIT)
#define ADC_CR2_DDS                     (1U << ADC_CR2_DDS_BIT)
#define ADC_CR2_DMA                     (1U << ADC_CR2_DMA_BIT)
#define ADC_CR2_CONT                    (1U << ADC_CR2_CONT_BIT)
#define ADC_CR2_ADON                    (1U << ADC_CR2_ADON_BIT)

/* Common status register */

#define ADC_CSR_OVR3_BIT                21
#define ADC_CSR_STRT3_BIT               20
#define ADC_CSR_JSTRT3_BIT              19
#define ADC_CSR_JEOC3_BIT               18
#define ADC_CSR_EOC3_BIT                17
#define ADC_CSR_AWD3_BIT                16
#define ADC_CSR_OVR2_BIT                13
#define ADC_CSR_STRT2_BIT               12
#define ADC_CSR_JSTRT2_BIT              11
#define ADC_CSR_JEOC2_BIT               10
#define ADC_CSR_EOC2_BIT                9
#define ADC_CSR_AWD2_BIT                8
#define ADC_CSR_OVR1_BIT                5
#define ADC_CSR_STRT1_BIT               4
#define ADC_CSR_JSTRT1_BIT              3
#define ADC_CSR_JEOC1_BIT               2
#define ADC_CSR_EOC1_BIT                1
#define ADC_CSR_AWD1_BIT                0

#define ADC_CSR_OVR3                    (1U << ADC_CSR_OVR3_BIT)
#define ADC_CSR_STRT3                   (1U << ADC_CSR_STRT3_BIT)
#define ADC_CSR_JSTRT3                  (1U << ADC_CSR_JSTRT3_BIT)
#define ADC_CSR_JEOC3                   (1U << ADC_CSR_JEOC3_BIT)
#define ADC_CSR_EOC3                    (1U << ADC_CSR_EOC3_BIT)
#define ADC_CSR_AWD3                    (1U << ADC_CSR_AWD3_BIT)
#define ADC_CSR_OVR2                    (1U << ADC_CSR_OVR2_BIT)
#define ADC_CSR_STRT2                   (1U << ADC_CSR_STRT2_BIT)
#define ADC_CSR_JSTRT2                  (1U << ADC_CSR_JSTRT2_BIT)
#define ADC_CSR_JEOC2                   (1U << ADC_CSR_JEOC2_BIT)
#define ADC_CSR_EOC2                    (1U << ADC_CSR_EOC2_BIT)
#define ADC_CSR_AWD2                    (1U << ADC_CSR_AWD2_BIT)
#define ADC_CSR_OVR1                    (1U << ADC_CSR_OVR1_BIT)
#define ADC_CSR_STRT1                   (1U << ADC_CSR_STRT1_BIT)
#define ADC_CSR_JSTRT1                  (1U << ADC_CSR_JSTRT1_BIT)
#define ADC_CSR_JEOC1                   (1U << ADC_CSR_JEOC1_BIT)
#define ADC_CSR_EOC1                    (1U << ADC_CSR_EOC1_BIT)
#define ADC_CSR_AWD1                    (1U << ADC_CSR_AWD1_BIT)

/* Common control register */

#define ADC_CCR_TSVREFE_BIT             23
#define ADC_CCR_VBATE_BIT               22
#define ADC_CCR_DDS_BIT                 13

#define ADC_CCR_TSVREFE                       (1U << ADC_CCR_TSVREFE_BIT)
#define ADC_CCR_VBATE                         (1U << ADC_CCR_VBATE_BIT)
#define ADC_CCR_ADCPRE                        (0x3 << 16)
#define ADC_CCR_ADCPRE_PCLK2_DIV_2            (0x0 << 16)
#define ADC_CCR_ADCPRE_PCLK2_DIV_4            (0x1 << 16)
#define ADC_CCR_ADCPRE_PCLK2_DIV_6            (0x2 << 16)
#define ADC_CCR_ADCPRE_PCLK2_DIV_8            (0x3 << 16)
#define ADC_CCR_DMA                           (0x3 << 14)
#define ADC_CCR_DMA_DIS                       (0x0 << 14)
#define ADC_CCR_DMA_MODE_1                    (0x1 << 14)
#define ADC_CCR_DMA_MODE_2                    (0x2 << 14)
#define ADC_CCR_DMA_MODE_3                    (0x3 << 14)
#define ADC_CCR_DDS                           (1U << ADC_CCR_DDS_BIT)
#define ADC_CCR_DELAY                         (0xF << 8)
#define ADC_CCR_DELAY_5                       (0x0 << 8)
#define ADC_CCR_DELAY_6                       (0x1 << 8)
#define ADC_CCR_DELAY_7                       (0x2 << 8)
#define ADC_CCR_DELAY_8                       (0x3 << 8)
#define ADC_CCR_DELAY_9                       (0x4 << 8)
#define ADC_CCR_DELAY_10                      (0x5 << 8)
#define ADC_CCR_DELAY_11                      (0x6 << 8)
#define ADC_CCR_DELAY_12                      (0x7 << 8)
#define ADC_CCR_DELAY_13                      (0x8 << 8)
#define ADC_CCR_DELAY_14                      (0x9 << 8)
#define ADC_CCR_DELAY_15                      (0xA << 8)
#define ADC_CCR_DELAY_16                      (0xB << 8)
#define ADC_CCR_DELAY_17                      (0xC << 8)
#define ADC_CCR_DELAY_18                      (0xD << 8)
#define ADC_CCR_DELAY_19                      (0xE << 8)
#define ADC_CCR_DELAY_20                      (0xF << 8)
/** Multi ADC mode selection. */
#define ADC_CCR_MULTI                         0x1F
/** All ADCs independent. */
#define ADC_CCR_MULTI_INDEPENDENT             0x0
/** Dual mode: combined regular simultaneous/injected simultaneous. */
#define ADC_CCR_MULTI_DUAL_REG_SIM_INJ_SIM    0x1
/** Dual mode: combined regular simultaneous/alternate trigger. */
#define ADC_CCR_MULTI_DUAL_REG_SIM_ALT_TRIG   0x2
/** Dual mode: injected simultaneous mode only. */
#define ADC_CCR_MULTI_DUAL_INJ_SIM            0x5
/** Dual mode: regular simultaneous mode only. */
#define ADC_CCR_MULTI_DUAL_REG_SIM            0x6
/** Dual mode: interleaved mode only. */
#define ADC_CCR_MULTI_DUAL_INTER              0x7
/** Dual mode: alternate trigger mode only. */
#define ADC_CCR_MULTI_DUAL_ALT_TRIG           0x9
/** Triple mode: combined regular simultaneous/injected simultaneous. */
#define ADC_CCR_MULTI_TRIPLE_REG_SIM_INJ_SIM  0x10
/** Triple mode: combined regular simultaneous/alternate trigger. */
#define ADC_CCR_MULTI_TRIPLE_REG_SIM_ALT_TRIG 0x11
/** Triple mode: injected simultaneous mode only. */
#define ADC_CCR_MULTI_TRIPLE_INJ_SIM          0x12
/** Triple mode: regular simultaneous mode only. */
#define ADC_CCR_MULTI_TRIPLE_REG_SIM          0x15
/** Triple mode: interleaved mode only. */
#define ADC_CCR_MULTI_TRIPLE_INTER            0x17
/** Triple mode: alternate trigger mode only. */
#define ADC_CCR_MULTI_TRIPLE_ALT_TRIG         0x19

/* Common regular data register for dual and triple modes */

#define ADC_CDR_DATA2                   0xFFFF0000
#define ADC_CDR_DATA1                   0xFFFF

/*
 * Other types
 */

/**
 * @brief STM32F2 external event selectors for regular group
 *        conversion.
 * @see adc_set_extsel()
 */
typedef enum adc_extsel_event {
    ADC_EXT_EV_TIM1_CC1    = ADC_CR2_EXTSEL_TIM1_CC1,
    ADC_EXT_EV_TIM1_CC2    = ADC_CR2_EXTSEL_TIM1_CC2,
    ADC_EXT_EV_TIM1_CC3    = ADC_CR2_EXTSEL_TIM1_CC3,
    ADC_EXT_EV_TIM2_CC2    = ADC_CR2_EXTSEL_TIM2_CC2,
    ADC_EXT_EV_TIM2_CC3    = ADC_CR2_EXTSEL_TIM2_CC3,
    ADC_EXT_EV_TIM2_CC4    = ADC_CR2_EXTSEL_TIM2_CC4,
    ADC_EXT_EV_TIM1_TRGO   = ADC_CR2_EXTSEL_TIM1_TRGO,
    ADC_EXT_EV_TIM3_CC1    = ADC_CR2_EXTSEL_TIM3_CC1,
    ADC_EXT_EV_TIM3_TRGO   = ADC_CR2_EXTSEL_TIM3_TRGO,
    ADC_EXT_EV_TIM4_CC4    = ADC_CR2_EXTSEL_TIM4_CC4,
    ADC_EXT_EV_TIM5_CC1    = ADC_CR2_EXTSEL_TIM5_CC1,
    ADC_EXT_EV_TIM5_CC2    = ADC_CR2_EXTSEL_TIM5_CC2,
    ADC_EXT_EV_TIM5_CC3    = ADC_CR2_EXTSEL_TIM5_CC3,
    ADC_EXT_EV_TIM8_CC1    = ADC_CR2_EXTSEL_TIM8_CC1,
    ADC_EXT_EV_TIM8_TRGO   = ADC_CR2_EXTSEL_TIM8_TRGO,
    ADC_EXT_EV_TIM1_EXTI11 = ADC_CR2_EXTSEL_TIM1_EXTI11,
} adc_extsel_event;

/**
 * @brief STM32F2 sample times, in ADC clock cycles.
 */
typedef enum adc_smp_rate {
    ADC_SMPR_3,                 /**< 3 ADC cycles */
    ADC_SMPR_15,                /**< 15 ADC cycles */
    ADC_SMPR_28,                /**< 28 ADC cycles */
    ADC_SMPR_56,                /**< 56 ADC cycles */
    ADC_SMPR_84,                /**< 84 ADC cycles */
    ADC_SMPR_112,               /**< 112 ADC cycles */
    ADC_SMPR_144,               /**< 144 ADC cycles */
    ADC_SMPR_480,               /**< 480 ADC cycles */
} adc_smp_rate;

/**
 * @brief STM32F2 ADC prescalers, as divisors of PCLK2.
 */
typedef enum adc_prescaler {
    /** PCLK2 divided by 2 */
    ADC_PRE_PCLK2_DIV_2 = ADC_CCR_ADCPRE_PCLK2_DIV_2,
    /** PCLK2 divided by 4 */
    ADC_PRE_PCLK2_DIV_4 = ADC_CCR_ADCPRE_PCLK2_DIV_4,
    /** PCLK2 divided by 6 */
    ADC_PRE_PCLK2_DIV_6 = ADC_CCR_ADCPRE_PCLK2_DIV_6,
    /** PCLK2 divided by 8 */
    ADC_PRE_PCLK2_DIV_8 = ADC_CCR_ADCPRE_PCLK2_DIV_8,
} adc_prescaler;

#endif
