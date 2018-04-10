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
 * @file   rtc.c
 * @author Rod Gilchrist <rod@visibleassets.com>
 * @brief  Real Time Clock interface
 */

#include "rtc_util.h"


#define NR_RTC_HANDLERS                 4

static rtc_dev rtc = {
    .regs         = RTC_BASE,
    .handlers     = { [NR_RTC_HANDLERS - 1] = 0 },
};

rtc_dev *RTC = &rtc;


/**
 * Initialize the RTC interface, and enable access to its register map and 
 * the backup registers.
 */
void rtc_init(rtc_clk_src src) {

	bkp_init();		// turn on peripheral clocks to PWR and BKP and reset the backup domain via RCC registers.
					// (we reset the backup domain here because we must in order to change the rtc clock source).

	bkp_enable_writes();	// enable writes to the backup registers and the RTC registers via the DBP bit in the PWR control register

	RCC_BASE->BDCR &= ~RCC_BDCR_RTCSEL;
	switch (src) {
		case RTCSEL_NONE:
			RCC_BASE->BDCR = RCC_BDCR_RTCSEL_NONE;
			break;

		case RTCSEL_LSE:
			rcc_start_lse();
			RCC_BASE->BDCR |= RCC_BDCR_RTCSEL_LSE;

			break;

		case RTCSEL_LSI:
		case RTCSEL_DEFAULT:
			rcc_start_lsi();
			RCC_BASE->BDCR |= RCC_BDCR_RTCSEL_LSI;
			break;

		case RTCSEL_HSE:			// This selection uses HSE/128 as the RTC source (i.e. 64 kHz with an 8 mHz xtal)
			rcc_start_hse();
			RCC_BASE->BDCR |= RCC_BDCR_RTCSEL_HSE;
			break;
	}
	bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_RTCEN_BIT, 1); // Enable the RTC

	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
}

/**
 * @brief Attach a RTC interrupt.
 * @param interrupt Interrupt number to attach to; this may be any rtc_interrupt_id.
 * @param handler Handler to attach to the given interrupt.
 * @see rtc_interrupt_id
 */
void rtc_attach_interrupt(	uint8 interrupt,
                            voidFuncPtr handler) {
    RTC->handlers[interrupt] = handler;
    rtc_enable_irq(interrupt);
	switch (interrupt) {
		case RTC_SECONDS_INTERRUPT: nvic_irq_enable(NVIC_RTC); break;
		case RTC_OVERFLOW_INTERRUPT: nvic_irq_enable(NVIC_RTC); break;
		case RTC_ALARM_GLOBAL_INTERRUPT: nvic_irq_enable(NVIC_RTC); break;
		case RTC_ALARM_SPECIFIC_INTERRUPT: nvic_irq_enable(NVIC_RTCALARM); break; // The alarm specific interrupt can wake us from deep sleep.
	}
}

/**
 * @brief Detach an rtc interrupt.
 * @param interrupt Interrupt number to detach.
 * @see rtc_interrupt_id
 */
void rtc_detach_interrupt(uint8 interrupt) {
	rtc_disable_irq(interrupt);
    RTC->handlers[interrupt] = NULL;
}

/*
 * IRQ handlers
 */

/* For dispatch routines which service multiple interrupts. */
#define handle_irq(dier_sr, irq_mask, handlers, iid, handled_irq) do {  \
	if ((dier_sr) & (irq_mask)) {                                   \
		void (*__handler)(void) = (handlers)[iid];                  \
		if (__handler) {                                            \
			__handler();                                            \
			handled_irq |= (irq_mask);                              \
		}                                                           \
	}                                                               \
} while (0)

static inline void dispatch_multiple_rtc_irq() {
    rtc_reg_map *regs = RTC->regs;
    uint32 dsr = regs->CRH & regs->CRL;
    void (**hs)(void) = RTC->handlers;
    uint32 handled = 0;

    handle_irq(dsr, RTC_CRL_SECF, hs, RTC_SECONDS_INTERRUPT, handled);
    handle_irq(dsr, RTC_CRL_ALRF, hs, RTC_ALARM_GLOBAL_INTERRUPT, handled);
    handle_irq(dsr, RTC_CRL_OWF, hs, RTC_OVERFLOW_INTERRUPT, handled);

    regs->CRL &= ~handled;
}

void __irq_rtc(void) {
    dispatch_multiple_rtc_irq();
}

/* A special-case dispatch routine for single-interrupt NVIC lines.
 * This function assumes that the interrupt corresponding to `RTC_ALARM_INTERRUPT' has
 * in fact occurred (i.e., it doesn't check DIER & SR). */
void __irq_rtcalarm(void) {
    void (*handler)(void) = RTC->handlers[RTC_ALARM_SPECIFIC_INTERRUPT];
    if (handler) {
        handler();
		*bb_perip(&EXTI_BASE->PR, EXTI_RTC_ALARM_BIT) = 1;
		//asm volatile("nop");		// See comment in exti.c. Doesn't seem to be required.
		//asm volatile("nop");
    }
}

/**
 * @brief Returns the rtc's counter (i.e. time/date) value.
 *
 * This value is likely to be inaccurate if the counter is running
 * with a low prescaler.
 */
uint32 rtc_get_count() {
	uint32 h, l;
	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
	do {
		h = RTC->regs->CNTH & 0xffff;
		l = RTC->regs->CNTL & 0xffff;
	} while (h != (RTC->regs->CNTH & 0xffff));
	return (h << 16) | l;
}

/**
 * @brief Sets the counter value (i.e. time/date) for the rtc.
 * @param value New counter value
 */
void rtc_set_count(uint32 value) {
	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
	rtc_enter_config_mode();
	RTC->regs->CNTH = (value >> 16) & 0xffff;
	RTC->regs->CNTL = value & 0xffff;
	rtc_exit_config_mode();
	rtc_wait_finished();
}

/**
 * @brief Sets the prescaler load value for the rtc.
 * @param value New prescaler load value (use 0x7fff to get 1 second period with 32.768 Hz clock).
 */
void rtc_set_prescaler_load(uint32 value) {
	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
	rtc_enter_config_mode();
	RTC->regs->PRLH = (value >> 16) & 0xffff;
	RTC->regs->PRLL = value & 0xffff;
	rtc_exit_config_mode();
	rtc_wait_finished();
}

/**
 * @brief Returns the rtc's prescaler divider (i.e. current divider count) value.
 */
uint32 rtc_get_divider() {
	uint32 h, l;
	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
	do {
		h = RTC->regs->DIVH & 0x000f;
		l = RTC->regs->DIVL & 0xffff;
	} while (h != (RTC->regs->DIVH & 0x000f));
	return (h << 16) | l;
}

/**
 * @brief Sets the alarm value (i.e. time/date) for the rtc.
 * @param value New alarm value
 */
void rtc_set_alarm(uint32 value) {
	rtc_clear_sync();
	rtc_wait_sync();
	rtc_wait_finished();
	rtc_enter_config_mode();
	RTC->regs->ALRH = (value >> 16) & 0xffff;
	RTC->regs->ALRL = value & 0xffff;
	rtc_exit_config_mode();
	rtc_wait_finished();
}
