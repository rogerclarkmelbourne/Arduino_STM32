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
 *  @file adc.h
 *
 *  @brief Analog-to-Digital Conversion (ADC) header.
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "libmaple.h"
#include "bitband.h"
#include "rcc.h"
#include "dma.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"{
#endif

/** ADC register map type. */
typedef struct adc_reg_map {
    __IO uint32 SR;             ///< Status register
    __IO uint32 CR1;            ///< Control register 1
    __IO uint32 CR2;            ///< Control register 2
    __IO uint32 SMPR1;          ///< Sample time register 1
    __IO uint32 SMPR2;          ///< Sample time register 2
    __IO uint32 JOFR1;          ///< Injected channel data offset register 1
    __IO uint32 JOFR2;          ///< Injected channel data offset register 2
    __IO uint32 JOFR3;          ///< Injected channel data offset register 3
    __IO uint32 JOFR4;          ///< Injected channel data offset register 4
    __IO uint32 HTR;            ///< Watchdog high threshold register
    __IO uint32 LTR;            ///< Watchdog low threshold register
    __IO uint32 SQR1;           ///< Regular sequence register 1
    __IO uint32 SQR2;           ///< Regular sequence register 2
    __IO uint32 SQR3;           ///< Regular sequence register 3
    __IO uint32 JSQR;           ///< Injected sequence register
    __IO uint32 JDR1;           ///< Injected data register 1
    __IO uint32 JDR2;           ///< Injected data register 2
    __IO uint32 JDR3;           ///< Injected data register 3
    __IO uint32 JDR4;           ///< Injected data register 4
    __IO uint32 DR;             ///< Regular data register
} adc_reg_map;

//Added by bubulindo - Interrupt ID's for ADC
typedef enum {
    ADC_EOC,     // End Of Conversion interrupt.
    ADC_AWD ,    // Analog WatchDog interrupt
    ADC_JEOC,    // Injected End Of Conversion interrupt.
	ADC_LAST_IRQ_ID
} adc_irq_id;

extern voidFuncPtr adc1Handlers[ADC_LAST_IRQ_ID]; // EOC, JEOC, AWD Interrupts
extern voidFuncPtr adc2Handlers[ADC_LAST_IRQ_ID]; // EOC, JEOC, AWD Interrupts
extern voidFuncPtr adc3Handlers[ADC_LAST_IRQ_ID]; // EOC, JEOC, AWD Interrupts

/** ADC device type. */
typedef struct adc_dev
{
    adc_reg_map *regs; /**< Register map */
    rcc_clk_id clk_id; /**< RCC clock information */
	dma_stream dmaStream;
	dma_channel dmaChannel;
	voidFuncPtr (*handler_p)[];
} adc_dev;

extern const adc_dev adc1;
extern const adc_dev adc2;
extern const adc_dev adc3;
#define ADC1 (&adc1)
#define ADC2 (&adc2)
#define ADC3 (&adc3)

typedef struct adc_common_reg_map {
	__IO uint32 CSR;            ///< Common status register
	__IO uint32 CCR;            ///< Common control register
	__IO uint32 CDR;            ///< Common regular data register for dual AND triple modes
} adc_common_reg_map;

/*
 * Register map base pointers
 */

/** ADC1 register map base pointer. */
#define ADC1_BASE                       ((struct adc_reg_map*)0x40012000)
/** ADC2 register map base pointer. */
#define ADC2_BASE                       ((struct adc_reg_map*)0x40012100)
/** ADC3 register map base pointer. */
#define ADC3_BASE                       ((struct adc_reg_map*)0x40012200)
/** ADC common register map base pointer. */
#define ADC_COMMON_BASE                 ((struct adc_common_reg_map*)0x40012300)

extern adc_common_reg_map* const ADC_COMMON;

/*
 * Register bit definitions
 */

/* Status register */

#define ADC_SR_OVR                      BIT(5)
#define ADC_SR_STRT                     BIT(4)
#define ADC_SR_JSTRT                    BIT(3)
#define ADC_SR_JEOC                     BIT(2)
#define ADC_SR_EOC                      BIT(1)
#define ADC_SR_AWD                      BIT(0)

/* Control register 1 */

#define ADC_CR1_OVRIE_BIT               26
#define ADC_CR1_AWDEN_BIT               23
#define ADC_CR1_JAWDEN_BIT              22
#define ADC_CR1_DISCNUM_SHIFT           13
#define ADC_CR1_JDISCEN_BIT             12
#define ADC_CR1_DISCEN_BIT              11
#define ADC_CR1_JAUTO_BIT               10
#define ADC_CR1_AWDSGL_BIT              9
#define ADC_CR1_SCAN_BIT                8
#define ADC_CR1_JEOCIE_BIT              7
#define ADC_CR1_AWDIE_BIT               6
#define ADC_CR1_EOCIE_BIT               5
#define ADC_CR1_AWDCH_SHIFT             0

#define ADC_CR1_OVRIE                   (0x1 << ADC_CR1_OVRIE_BIT)
#define ADC_CR1_AWDEN                   (0x1 << ADC_CR1_AWDEN_BIT)
#define ADC_CR1_JAWDEN                  (0x1 << ADC_CR1_JAWDEN_BIT)
#define ADC_CR1_DISCNUM                 (0x3 << ADC_CR1_DISCNUM_SHIFT)
#define ADC_CR1_JDISCEN                 (0x1 << ADC_CR1_JDISCEN_BIT)
#define ADC_CR1_DISCEN                  (0x1 << ADC_CR1_DISCEN_BIT)
#define ADC_CR1_JAUTO                   (0x1 << ADC_CR1_JAUTO_BIT)
#define ADC_CR1_AWDSGL                  (0x1 << ADC_CR1_AWDSGL_BIT)
#define ADC_CR1_SCAN                    (0x1 << ADC_CR1_SCAN_BIT)
#define ADC_CR1_JEOCIE                  (0x1 << ADC_CR1_JEOCIE_BIT)
#define ADC_CR1_AWDIE                   (0x1 << ADC_CR1_AWDIE_BIT)
#define ADC_CR1_EOCIE                   (0x1 << ADC_CR1_EOCIE_BIT)
#define ADC_CR1_AWDCH                   (0x1F << ADC_CR1_AWDCH_SHIFT)

/* Control register 2 */

#define ADC_CR2_SWSTART_BIT             30
#define ADC_CR2_EXTEN_SHIFT             28
#define ADC_CR2_EXTSEL_SHIFT            24
#define ADC_CR2_JSWSTART_BIT            22
#define ADC_CR2_JEXTEN_SHIFT            20
#define ADC_CR2_JEXTSEL_SHIFT           16
#define ADC_CR2_ALIGN_BIT               11
#define ADC_CR2_EOCS_BIT                10
#define ADC_CR2_DDS_BIT                 9
#define ADC_CR2_DMA_BIT                 8
#define ADC_CR2_CONT_BIT                1
#define ADC_CR2_ADON_BIT                0

#define ADC_CR2_SWSTART                 (0x1 << ADC_CR2_SWSTART_BIT)
#define ADC_CR2_EXTEN                   (0x3 << ADC_CR2_EXTEN_SHIFT)
#define ADC_CR2_EXTSEL                  (0xF << ADC_CR2_EXTSEL_SHIFT)
#define ADC_CR2_JSWSTART                (0x1 << ADC_CR2_JSWSTART_BIT)
#define ADC_CR2_JEXTEN                  (0x3 << ADC_CR2_JEXTEN_SHIFT)
#define ADC_CR2_JEXTSEL                 (0xF << ADC_CR2_JEXTSEL_SHIFT)
#define ADC_CR2_ALIGN                   (0x1 << ADC_CR2_ALIGN_BIT)
#define ADC_CR2_EOCS                    (0x1 << ADC_CR2_EOCS_BIT)
#define ADC_CR2_DDS                     (0x1 << ADC_CR2_DDS_BIT)
#define ADC_CR2_DMA                     (0x1 << ADC_CR2_DMA_BIT)
#define ADC_CR2_CONT                    (0x1 << ADC_CR2_CONT_BIT)
#define ADC_CR2_ADON                    (0x1 << ADC_CR2_ADON_BIT)

/* Sample time register 1 */

#define ADC_SMPR1_SMP18                 (0x7 << 24)
#define ADC_SMPR1_SMP17                 (0x7 << 21)
#define ADC_SMPR1_SMP16                 (0x7 << 18)
#define ADC_SMPR1_SMP15                 (0x7 << 15)
#define ADC_SMPR1_SMP14                 (0x7 << 12)
#define ADC_SMPR1_SMP13                 (0x7 << 9)
#define ADC_SMPR1_SMP12                 (0x7 << 6)
#define ADC_SMPR1_SMP11                 (0x7 << 3)
#define ADC_SMPR1_SMP10                 0x7

/* Sample time register 2 */

#define ADC_SMPR2_SMP9                  (0x7 << 27)
#define ADC_SMPR2_SMP8                  (0x7 << 24)
#define ADC_SMPR2_SMP7                  (0x7 << 21)
#define ADC_SMPR2_SMP6                  (0x7 << 18)
#define ADC_SMPR2_SMP5                  (0x7 << 15)
#define ADC_SMPR2_SMP4                  (0x7 << 12)
#define ADC_SMPR2_SMP3                  (0x7 << 9)
#define ADC_SMPR2_SMP2                  (0x7 << 6)
#define ADC_SMPR2_SMP1                  (0x7 << 3)
#define ADC_SMPR2_SMP0                  0x7

/* Injected channel data offset register */

#define ADC_JOFR_JOFFSET                0x3FF

/* Watchdog high threshold register */

#define ADC_HTR_HT                      0x3FF

/* Watchdog low threshold register */

#define ADC_LTR_LT                      0x3FF

/* Regular sequence register 1 */

#define ADC_SQR1_L                      (0x1F << 20)
#define ADC_SQR1_SQ16                   (0x1F << 15)
#define ADC_SQR1_SQ15                   (0x1F << 10)
#define ADC_SQR1_SQ14                   (0x1F << 5)
#define ADC_SQR1_SQ13                   0x1F

/* Regular sequence register 2 */

#define ADC_SQR2_SQ12                   (0x1F << 25)
#define ADC_SQR2_SQ11                   (0x1F << 20)
#define ADC_SQR2_SQ10                   (0x1F << 16)
#define ADC_SQR2_SQ9                    (0x1F << 10)
#define ADC_SQR2_SQ8                    (0x1F << 5)
#define ADC_SQR2_SQ7                    0x1F

/* Regular sequence register 3 */

#define ADC_SQR3_SQ6                    (0x1F << 25)
#define ADC_SQR3_SQ5                    (0x1F << 20)
#define ADC_SQR3_SQ4                    (0x1F << 16)
#define ADC_SQR3_SQ3                    (0x1F << 10)
#define ADC_SQR3_SQ2                    (0x1F << 5)
#define ADC_SQR3_SQ1                    0x1F

/* Injected sequence register */

#define ADC_JSQR_JL                     (0x3 << 20)
#define ADC_JSQR_JL_1CONV               (0x0 << 20)
#define ADC_JSQR_JL_2CONV               (0x1 << 20)
#define ADC_JSQR_JL_3CONV               (0x2 << 20)
#define ADC_JSQR_JL_4CONV               (0x3 << 20)
#define ADC_JSQR_JSQ4                   (0x1F << 15)
#define ADC_JSQR_JSQ3                   (0x1F << 10)
#define ADC_JSQR_JSQ2                   (0x1F << 5)
#define ADC_JSQR_JSQ1                   0x1F

/* Injected data registers */

#define ADC_JDR_JDATA                   0xFFFF

/* Regular data register */

#define ADC_DR_ADC2DATA                 (0xFFFF << 16)
#define ADC_DR_DATA                     0xFFFF

/* Common status register */

#define ADC_CSR_OVR3                    BIT(21)
#define ADC_CSR_STRT3                   BIT(20)
#define ADC_CSR_JSTRT3                  BIT(19)
#define ADC_CSR_JEOC3                   BIT(18)
#define ADC_CSR_EOC3                    BIT(17)
#define ADC_CSR_AWD3                    BIT(16)
#define ADC_CSR_OVR2                    BIT(13)
#define ADC_CSR_STRT2                   BIT(12)
#define ADC_CSR_JSTRT2                  BIT(11)
#define ADC_CSR_JEOC2                   BIT(10)
#define ADC_CSR_EOC2                    BIT(9)
#define ADC_CSR_AWD2                    BIT(8)
#define ADC_CSR_OVR1                    BIT(5)
#define ADC_CSR_STRT1                   BIT(4)
#define ADC_CSR_JSTRT1                  BIT(3)
#define ADC_CSR_JEOC1                   BIT(2)
#define ADC_CSR_EOC1                    BIT(1)
#define ADC_CSR_AWD1                    BIT(0)

/* Common control register */

#define ADC_CCR_TSVREFE_BIT             23
#define ADC_CCR_VBATE_BIT               22
#define ADC_CCR_ADCPRE_SHIFT            16
#define ADC_CCR_DMA_SHIFT               14
#define ADC_CCR_DDS_BIT                 13
#define ADC_CCR_DELAY_SHIFT             8
#define ADC_CCR_MULTI_SHIFT             0

#define ADC_CCR_TSVREFE                 (0x1 << ADC_CCR_TSVREFE_BIT)
#define ADC_CCR_VBATE                   (0x1 << ADC_CCR_VBATE_BIT)
#define ADC_CCR_ADCPRE                  (0x3 << ADC_CCR_ADCPRE_SHIFT)
#define ADC_CCR_DMA                     (0x3 << ADC_CCR_DMA_SHIFT)
#define ADC_CCR_DDS                     (0x1 << ADC_CCR_DDS_BIT)
#define ADC_CCR_DELAY                   (0xF << ADC_CCR_DELAY_SHIFT)
#define ADC_CCR_MULTI                   (0x1F << ADC_CCR_MULTI_SHIFT)

/* Common regular data register */

#define ADC_CDR_DATA2                   (0xFFFF << 16)
#define ADC_CDR_DATA1                   (0xFFFF)


void adc_init(const adc_dev *dev);

typedef enum {
    ADC_EXT_TRIGGER_DISABLE         = 0,
    ADC_EXT_TRIGGER_ON_RISING_EDGE  = 1,
    ADC_EXT_TRIGGER_ON_FALLING_EDGE = 2,
    ADC_EXT_TRIGGER_ON_BOTH_EDGES   = 3,
} adc_ext_trigger;

/**
 * @brief External event selector for regular group conversion.
 * @see adc_set_extsel
 */
typedef enum {
    ADC_EXTSEL_TIM1_CC1  =  0, /**< Timer 1 CC1 event */
    ADC_EXTSEL_TIM1_CC2  =  1, /**< Timer 1 CC2 event */
    ADC_EXTSEL_TIM1_CC3  =  2, /**< Timer 1 CC3 event */
    ADC_EXTSEL_TIM2_CC2  =  3, /**< Timer 2 CC2 event */
    ADC_EXTSEL_TIM2_CC3  =  4, /**< Timer 2 CC3 event */
    ADC_EXTSEL_TIM2_CC4  =  5, /**< Timer 2 CC4 event */
    ADC_EXTSEL_TIM2_TRGO =  6, /**< Timer 2 TRGO event */
    ADC_EXTSEL_TIM3_CC1  =  7, /**< Timer 3 CC1 event */
    ADC_EXTSEL_TIM3_TRGO =  8, /**< Timer 3 TRGO event */
    ADC_EXTSEL_TIM4_CC4  =  9, /**< Timer 4 CC4 event */
    ADC_EXTSEL_TIM5_CC1  = 10, /**< Timer 5 CC1 event */
    ADC_EXTSEL_TIM5_CC2  = 11, /**< Timer 5 CC2 event */
    ADC_EXTSEL_TIM5_CC3  = 12, /**< Timer 5 CC3 event */
    ADC_EXTSEL_TIM8_CC1  = 13, /**< Timer 8 CC1 event */
    ADC_EXTSEL_TIM8_TRGO = 14, /**< Timer 8 TRGO event */
    ADC_EXTSEL_EXTI11    = 15, /**< EXTI11 event */
} adc_extsel_event;

typedef enum {
    ADC_JEXTSEL_TIM1_CC4  =  0, /**< Timer 1 CC4 event */
    ADC_JEXTSEL_TIM1_TRGO =  1, /**< Timer 1 TRGO event */
    ADC_JEXTSEL_TIM2_CC1  =  2, /**< Timer 2 CC1 event */
    ADC_JEXTSEL_TIM2_TRGO =  3, /**< Timer 2 TRGO event */
    ADC_JEXTSEL_TIM3_CC2  =  4, /**< Timer 3 CC2 event */
    ADC_JEXTSEL_TIM3_CC4  =  5, /**< Timer 3 CC4 event */
    ADC_JEXTSEL_TIM4_CC1  =  6, /**< Timer 4 CC1 event */
    ADC_JEXTSEL_TIM4_CC2  =  7, /**< Timer 4 CC2 event */
    ADC_JEXTSEL_TIM4_CC3  =  8, /**< Timer 4 CC3 event */
    ADC_JEXTSEL_TIM4_TRGO =  9, /**< Timer 4 TRGO event */
    ADC_JEXTSEL_TIM5_CC4  = 10, /**< Timer 5 CC4 event */
    ADC_JEXTSEL_TIM5_TRGO = 11, /**< Timer 5 TRGO event */
    ADC_JEXTSEL_TIM8_CC2  = 12, /**< Timer 8 CC2 event */
    ADC_JEXTSEL_TIM8_CC3  = 13, /**< Timer 8 CC3 event */
    ADC_JEXTSEL_TIM8_CC4  = 14, /**< Timer 8 CC4 event */
    ADC_JEXTSEL_EXTI15    = 15, /**< EXTI15 event */
} adc_jextsel_event;

/**
 * @brief ADC sample times, in ADC clock cycles
 *
 * These control the amount of time spent sampling the input voltage.
 */
typedef enum {
    ADC_SMPR_3 = 0,            /**<  3 ADC cycles */
    ADC_SMPR_15,               /**< 15 ADC cycles */
    ADC_SMPR_28,               /**< 28 ADC cycles */
    ADC_SMPR_56,               /**< 56 ADC cycles */
    ADC_SMPR_84,               /**< 84 ADC cycles */
    ADC_SMPR_112,              /**< 112 ADC cycles */
    ADC_SMPR_144,              /**< 144 ADC cycles */
    ADC_SMPR_480,              /**< 480 ADC cycles */
} adc_smp_rate;

void adc_set_sampling_time(const adc_dev *dev, adc_smp_rate smp_rate);
void adc_set_exttrig(const adc_dev *dev, adc_ext_trigger trigger);
void adc_set_jextrig(const adc_dev *dev, adc_ext_trigger trigger);
void adc_set_extsel(const adc_dev *dev, adc_extsel_event event);
void adc_set_jextsel(const adc_dev *dev, adc_jextsel_event event);
void adc_foreach(void (*fn)(const adc_dev*));
void adc_enable_irq(const adc_dev* dev);
void adc_disable_irq(const adc_dev* dev);
void adc_attach_interrupt(const adc_dev *dev, adc_irq_id irq_id, voidFuncPtr handler);
void adc_set_reg_sequence(const adc_dev * dev, uint8 * channels, uint8 len);
void adc_enable_tsvref(void);
void adc_enable_vbat(void);

uint16 adc_read(const adc_dev *dev, uint8 channel);

/*
    Starts a single conversion in the specified channel.
    Results must be read through interrupt or polled outside this function.
*/
void adc_start_single_convert(const adc_dev* dev, uint8 channel);

/*
    Starts the continuous mode on one channel of the ADC channel.
    Results must be read through interrupt or polled outside this function.
*/
void adc_start_continuous_convert(const adc_dev* dev, uint8 channel);

void setupADC_F4(void);

/**
 * @brief Set the regular channel sequence length.
 *
 * Defines the total number of conversions in the regular channel
 * conversion sequence.
 *
 * @param dev ADC device.
 * @param length Regular channel sequence length, from 1 to 16.
 */
static inline void adc_set_reg_seqlen(const adc_dev *dev, uint8 length)
{
    uint32 tmp = dev->regs->SQR1;
    tmp &= ~ADC_SQR1_L;
    tmp |= (length - 1) << 20;
    dev->regs->SQR1 = tmp;
}

/**
 * @brief Enable an adc peripheral
 * @param dev ADC device to enable
 */
static inline void adc_enable(const adc_dev *dev)
{
	dev->regs->CR2 |= ADC_CR2_ADON;
}

/**
 * @brief Disable an ADC peripheral
 * @param dev ADC device to disable
 */
static inline void adc_disable(const adc_dev *dev)
{
	dev->regs->CR2 &= ~ADC_CR2_ADON;
}

/**
 * @brief Disable all ADC peripherals.
 */
static inline void adc_disable_all(void)
{
    adc_foreach(adc_disable);
}

static inline void adc_awd_set_low_limit(const adc_dev * dev, uint32 limit)
{
	dev->regs->LTR = limit;
}

static inline void adc_awd_set_high_limit(const adc_dev * dev, uint32 limit)
{
	dev->regs->HTR = limit;
}

static inline void adc_awd_enable_channel(const adc_dev * dev, uint8 awd_channel)
{
    dev->regs->CR1 |= (awd_channel & ADC_CR1_AWDCH);
}

void adc_awd_enable_irq(const adc_dev * dev);

static inline void adc_awd_enable(const adc_dev * dev)
{
	dev->regs->CR1 |= ADC_CR1_AWDEN;
}

static inline void adc_set_scan_mode(const adc_dev * dev)
{
	dev->regs->CR1 |= ADC_CR1_SCAN;
}

static inline void adc_set_continuous(const adc_dev * dev)
{
	dev->regs->CR2 |= ADC_CR2_CONT;
}

static inline void adc_clear_continuous(const adc_dev * dev)
{
	dev->regs->CR2 &= ~ADC_CR2_CONT;
}

static inline void adc_dma_disable(const adc_dev * dev)
{
	dev->regs->CR2 &= ~ADC_CR2_DMA;
}

static inline void adc_dma_enable(const adc_dev * dev)
{
	dev->regs->CR2 |= ADC_CR2_DMA;
}

static inline void adc_dma_single(const adc_dev * dev)
{
	dev->regs->CR2 &= ~ADC_CR2_DDS;
}

static inline void adc_dma_continuous(const adc_dev * dev)
{
	dev->regs->CR2 |= ADC_CR2_DDS;
}

static inline bool adc_is_end_of_convert(const adc_dev * dev)
{
	return (dev->regs->SR&ADC_SR_EOC);
}

static inline uint16 adc_get_data(const adc_dev * dev)
{ 
	return dev->regs->DR;
}

static inline uint16 adc_get_status(const adc_dev * dev)
{ 
	return dev->regs->SR;
}

static inline void adc_start_convert(const adc_dev * dev)
{
	dev->regs->CR2 |= ADC_CR2_SWSTART;
}


#ifdef __cplusplus
} // extern "C"
#endif

#endif
