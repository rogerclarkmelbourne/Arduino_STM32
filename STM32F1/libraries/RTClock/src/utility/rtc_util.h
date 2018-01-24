/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 Visible Assets Inc.
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
 * @file   rtc.h
 * @author Rod Gilchrist <rod@visibleassets.com>
 * @brief  Real Time Clock interface.
 *
 */

#ifndef _RTC_UTIL_H
#define _RTC_UTIL_H

#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>
#include <libmaple/bitband.h>
#include <libmaple/pwr.h>
#include <libmaple/bkp.h>
#include <libmaple/exti.h>

#define EXTI_RTC_ALARM_BIT		17						// the extra exti interrupts (16,17,18,19) should be defined in exti.h (BUG)

//#define RCC_BDCR_RTCSEL_LSI   (0x2 << 8)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct rtc_reg_map {
	__IO uint32 CRH;		/**< Control register high */
	__IO uint32 CRL;		/**< Control register high */
	__IO uint32 PRLH;		/**< Prescaler load register high */
	__IO uint32 PRLL;		/**< Prescaler load register low */
	__IO uint32 DIVH;		/**< Prescaler divider register high */
	__IO uint32 DIVL;		/**< Prescaler divider register low */
	__IO uint32 CNTH;		/**< Counter register high */
	__IO uint32 CNTL;		/**< Counter register low */
	__IO uint32 ALRH;		/**< Alarm register high */
	__IO uint32 ALRL;		/**< Alarm register low */
} rtc_reg_map;
		
/** RTC register map base pointer */
#define RTC_BASE        ((struct rtc_reg_map*)0x40002800)

/** rtc device type */
typedef struct rtc_dev {
	rtc_reg_map *regs;			/**< Register map */
	voidFuncPtr handlers[];     /**< User IRQ handlers */
} rtc_dev;

extern rtc_dev *RTC;


/*
 * Register bit definitions
 */
	
/* Control register high (CRH) */
	
#define RTC_CRH_OWIE_BIT	2
#define RTC_CRH_ALRIE_BIT	1
#define RTC_CRH_SECIE_BIT	0

#define RTC_CRH_OWIE		BIT(RTC_CRH_OWIE_BIT)
#define RTC_CRH_ALRIE		BIT(RTC_CRH_ALRIE_BIT)
#define RTC_CRH_SECIE		BIT(RTC_CRH_SECIE_BIT)
	
/* Control register low (CRL) */

#define RTC_CRL_RTOFF_BIT	5
#define RTC_CRL_CNF_BIT		4
#define RTC_CRL_RSF_BIT		3
#define RTC_CRL_OWF_BIT		2
#define RTC_CRL_ALRF_BIT	1
#define RTC_CRL_SECF_BIT	0
	
#define RTC_CRL_RTOFF	BIT(RTC_CRL_RTOFF_BIT)
#define RTC_CRL_CNF		BIT(RTC_CRL_CNF_BIT)
#define RTC_CRL_RSF		BIT(RTC_CRL_RSF_BIT)
#define RTC_CRL_OWF		BIT(RTC_CRL_OWF_BIT)
#define RTC_CRL_ALRF	BIT(RTC_CRL_ALRF_BIT)
#define RTC_CRL_SECF	BIT(RTC_CRL_SECF_BIT)

/**
 * @brief RTC interrupt number.
 *
 */
typedef enum rtc_interrupt_id {
	RTC_SECONDS_INTERRUPT			= 0,	/**< Counter seconds interrupt */
	RTC_ALARM_GLOBAL_INTERRUPT		= 1,	/**< RTC alarm global interrupt (i.e. __irq_rtc()) */
	RTC_OVERFLOW_INTERRUPT			= 2,	/**< Counter overflow interrupt */
	RTC_ALARM_SPECIFIC_INTERRUPT	= 3		/**< RTC alarm specific interrupt (i.e. __irq_rtcalarm(), wake up from halt/sleep) */
} rtc_interrupt_id;

void rtc_attach_interrupt(	uint8 interrupt,
							voidFuncPtr handler);
void rtc_detach_interrupt(	uint8 interrupt);

/**
 * @brief RTC clock source.
 *
 */
typedef enum rtc_clk_src {
	RTCSEL_DEFAULT	= 0,
	RTCSEL_NONE		= 0x10,
	RTCSEL_LSE		= 0x11,
	RTCSEL_LSI		= 0x12,
	RTCSEL_HSE		= 0x13,
} rtc_clk_src;


void rtc_init(rtc_clk_src src);
void rtc_attach_interrupt(uint8 interrupt, voidFuncPtr handler);
void rtc_detach_interrupt(uint8 interrupt);
uint32 rtc_get_count();
void rtc_set_count(uint32 value);
void rtc_set_prescaler_load(uint32 value);
uint32 rtc_get_divider();
void rtc_set_alarm(uint32 value);


/**
 * @brief Check (wait if necessary) to see the previous write operation has completed.
 */
static inline void rtc_wait_finished() {
	while (*bb_perip(&(RTC->regs)->CRL, RTC_CRL_RTOFF_BIT) == 0);
}

	/**
 * @brief Clear the register synchronized flag. The flag is then set by hardware after a write to PRL/DIV or CNT.
 */
static inline void rtc_clear_sync() {
	rtc_wait_finished();
	*bb_perip(&(RTC->regs)->CRL, RTC_CRL_RSF_BIT) = 0;
}

/**
 * @brief Check (wait if necessary) to see RTC registers are synchronized.
 */
static inline void rtc_wait_sync() {
	while (*bb_perip(&(RTC->regs)->CRL, RTC_CRL_RSF_BIT) == 0);
}

/**
 * @brief Enter configuration mode.
 */
static inline void rtc_enter_config_mode() {
	rtc_wait_finished();
	*bb_perip(&(RTC->regs)->CRL, RTC_CRL_CNF_BIT) = 1;
}

/**
 * @brief Exit configuration mode.
 */
static inline void rtc_exit_config_mode() {
	rtc_wait_finished();
	*bb_perip(&(RTC->regs)->CRL, RTC_CRL_CNF_BIT) = 0;
}

/**
 * @brief Enable an RTC interrupt.
 * @param interrupt Interrupt number to enable; this may be any rtc_interrupt_id.
 * @see rtc_interrupt_id
 */
static inline void rtc_enable_irq(uint8 interrupt) {
	rtc_wait_finished();
	if (interrupt == RTC_ALARM_SPECIFIC_INTERRUPT) { // Enabling this interrupt allows waking up from deep sleep via WFI.
		*bb_perip(&EXTI_BASE->IMR, EXTI_RTC_ALARM_BIT) = 1;
		*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 1;
	}
	else *bb_perip(&(RTC->regs)->CRH, interrupt) = 1;
}

/**
 * @brief Disable an RTC interrupt.
 * @param interrupt Interrupt number to disable; this may be any rtc_interrupt_id value.
 * @see rtc_interrupt_id
 */
static inline void rtc_disable_irq(uint8 interrupt) {
	rtc_wait_finished();
	if (interrupt == RTC_ALARM_SPECIFIC_INTERRUPT) {
		*bb_perip(&EXTI_BASE->IMR, EXTI_RTC_ALARM_BIT) = 0;
		*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 0;
	}
	else *bb_perip(&(RTC->regs)->CRH, interrupt) = 0;
}

/**
 * @brief Enable an RTC alarm event. Enabling this event allows waking up from deep sleep via WFE.
 * @see rtc_interrupt_id
 */
static inline void rtc_enable_alarm_event() {
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_ALARM_BIT) = 1;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 1;
}

/**
 * @brief Disable the RTC alarm event.
 * @see rtc_interrupt_id
 */
static inline void rtc_disable_alarm_event() {
	*bb_perip(&EXTI_BASE->EMR, EXTI_RTC_ALARM_BIT) = 0;
	*bb_perip(&EXTI_BASE->RTSR, EXTI_RTC_ALARM_BIT) = 0;
}

/**
 * @brief Test for global interrupt second type.
 * @see rtc_interrupt_id
 */
static inline int rtc_is_second() {
	return *bb_perip(&(RTC->regs)->CRL, RTC_CRL_SECF_BIT);
}

/**
 * @brief Test for global interrupt alarm type.
 * @see rtc_interrupt_id
 */
static inline int rtc_is_alarm() {
	return *bb_perip(&(RTC->regs)->CRL, RTC_CRL_ALRF_BIT);
}

/**
 * @brief Test for global interrupt overflow type.
 * @see rtc_interrupt_id
 */
static inline int rtc_is_overflow() {
	return *bb_perip(&(RTC->regs)->CRL, RTC_CRL_OWF_BIT);
}


	
#ifdef __cplusplus
}
#endif

#endif /* _RTC_H */