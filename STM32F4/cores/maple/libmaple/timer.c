/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
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
 * @file   timer.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  New-style timer interface
 */

#include "timer.h"

/* Just like the corresponding DIER bits:
 * [0] = Update handler;
 * [1,2,3,4] = capture/compare 1,2,3,4 handlers, respectively;
 * [5] = COM;
 * [6] = TRG;
 * [7] = BRK. */
#define NR_ADV_HANDLERS                 8
/* Update, capture/compare 1,2,3,4; <junk>; trigger. */
#define NR_GEN_HANDLERS                 7
/* Update only. */
#define NR_BAS_HANDLERS                 1

timer_dev timer1 = {
    .regs         = { .adv = TIMER1_BASE },
    .clk_id       = RCC_TIMER1,
    .type         = TIMER_ADVANCED,
    .handlers     = { [NR_ADV_HANDLERS - 1] = 0 },
};
/** Timer 1 device (advanced) */
timer_dev *TIMER1 = &timer1;

timer_dev timer2 = {
    .regs         = { .gen = TIMER2_BASE },
    .clk_id       = RCC_TIMER2,
    .type         = TIMER_GENERAL,
    .handlers     = { [NR_GEN_HANDLERS - 1] = 0 },
};
/** Timer 2 device (general-purpose) */
timer_dev *TIMER2 = &timer2;

timer_dev timer3 = {
    .regs         = { .gen = TIMER3_BASE },
    .clk_id       = RCC_TIMER3,
    .type         = TIMER_GENERAL,
    .handlers     = { [NR_GEN_HANDLERS - 1] = 0 },
};
/** Timer 3 device (general-purpose) */
timer_dev *TIMER3 = &timer3;

timer_dev timer4 = {
    .regs         = { .gen = TIMER4_BASE },
    .clk_id       = RCC_TIMER4,
    .type         = TIMER_GENERAL,
    .handlers     = { [NR_GEN_HANDLERS - 1] = 0 },
};
/** Timer 4 device (general-purpose) */
timer_dev *TIMER4 = &timer4;

#ifdef STM32_HIGH_DENSITY
timer_dev timer5 = {
    .regs         = { .gen = TIMER5_BASE },
    .clk_id       = RCC_TIMER5,
    .type         = TIMER_GENERAL,
    .handlers     = { [NR_GEN_HANDLERS - 1] = 0 },
};
/** Timer 5 device (general-purpose) */
timer_dev *TIMER5 = &timer5;

timer_dev timer6 = {
    .regs         = { .bas = TIMER6_BASE },
    .clk_id       = RCC_TIMER6,
    .type         = TIMER_BASIC,
    .handlers     = { [NR_BAS_HANDLERS - 1] = 0 },
};
/** Timer 6 device (basic) */
timer_dev *TIMER6 = &timer6;

timer_dev timer7 = {
    .regs         = { .bas = TIMER7_BASE },
    .clk_id       = RCC_TIMER7,
    .type         = TIMER_BASIC,
    .handlers     = { [NR_BAS_HANDLERS - 1] = 0 },
};
/** Timer 7 device (basic) */
timer_dev *TIMER7 = &timer7;

timer_dev timer8 = {
    .regs         = { .adv = TIMER8_BASE },
    .clk_id       = RCC_TIMER8,
    .type         = TIMER_ADVANCED,
    .handlers     = { [NR_ADV_HANDLERS - 1] = 0 },
};
/** Timer 8 device (advanced) */
timer_dev *TIMER8 = &timer8;
#endif

/*
 * Convenience routines
 */

static void disable_channel(timer_dev *dev, uint8 channel);
static void pwm_mode(timer_dev *dev, uint8 channel);
static void output_compare_mode(timer_dev *dev, uint8 channel);

static inline void enable_irq(timer_dev *dev, uint8 interrupt);

/**
 * Initialize a timer, and reset its register map.
 * @param dev Timer to initialize
 */
void timer_init(timer_dev *dev) {
    rcc_clk_enable(dev->clk_id);
    rcc_reset_dev(dev->clk_id);
}

/**
 * @brief Disable a timer.
 *
 * The timer will stop counting, all DMA requests and interrupts will
 * be disabled, and no state changes will be output.
 *
 * @param dev Timer to disable.
 */
void timer_disable(timer_dev *dev) {
    (dev->regs).bas->CR1 = 0;
    (dev->regs).bas->DIER = 0;
    switch (dev->type) {
    case TIMER_ADVANCED:        /* fall-through */
    case TIMER_GENERAL:
        (dev->regs).gen->CCER = 0;
        break;
    case TIMER_BASIC:
        break;
    }
}

/**
 * Sets the mode of an individual timer channel.
 *
 * Note that not all timers can be configured in every mode.  For
 * example, basic timers cannot be configured to output compare mode.
 * Be sure to use a timer which is appropriate for the mode you want.
 *
 * @param dev Timer whose channel mode to set
 * @param channel Relevant channel
 * @param mode New timer mode for channel
 */
void timer_set_mode(timer_dev *dev, uint8 channel, timer_mode mode) {
    ASSERT_FAULT(channel > 0 && channel <= 4);

    /* TODO decide about the basic timers */
    ASSERT(dev->type != TIMER_BASIC);
    if (dev->type == TIMER_BASIC)
        return;

    switch (mode) {
    case TIMER_DISABLED:
        disable_channel(dev, channel);
        break;
    case TIMER_PWM:
        pwm_mode(dev, channel);
        break;
    case TIMER_OUTPUT_COMPARE:
        output_compare_mode(dev, channel);
        break;
    }
}

/**
 * @brief Call a function on timer devices.
 * @param fn Function to call on each timer device.
 */
void timer_foreach(void (*fn)(timer_dev*)) {
    fn(TIMER1);
    fn(TIMER2);
    fn(TIMER3);
    fn(TIMER4);
#ifdef STM32_HIGH_DENSITY
    fn(TIMER5);
    fn(TIMER6);
    fn(TIMER7);
    fn(TIMER8);
#endif
}

/**
 * @brief Attach a timer interrupt.
 * @param dev Timer device
 * @param interrupt Interrupt number to attach to; this may be any
 *                  timer_interrupt_id or timer_channel value appropriate
 *                  for the timer.
 * @param handler Handler to attach to the given interrupt.
 * @see timer_interrupt_id
 * @see timer_channel
 */
void timer_attach_interrupt(timer_dev *dev,
                            uint8 interrupt,
                            voidFuncPtr handler) {
    dev->handlers[interrupt] = handler;
    timer_enable_irq(dev, interrupt);
    enable_irq(dev, interrupt);
}

/**
 * @brief Detach a timer interrupt.
 * @param dev Timer device
 * @param interrupt Interrupt number to detach; this may be any
 *                  timer_interrupt_id or timer_channel value appropriate
 *                  for the timer.
 * @see timer_interrupt_id
 * @see timer_channel
 */
void timer_detach_interrupt(timer_dev *dev, uint8 interrupt) {
    timer_disable_irq(dev, interrupt);
    dev->handlers[interrupt] = NULL;
}

/*
 * IRQ handlers
 */

static inline void dispatch_adv_brk(timer_dev *dev);
static inline void dispatch_adv_up(timer_dev *dev);
static inline void dispatch_adv_trg_com(timer_dev *dev);
static inline void dispatch_adv_cc(timer_dev *dev);
static inline void dispatch_general(timer_dev *dev);
static inline void dispatch_basic(timer_dev *dev);

void __irq_tim1_brk(void) {
    dispatch_adv_brk(TIMER1);
}

void __irq_tim1_up(void) {
    dispatch_adv_up(TIMER1);
}

void __irq_tim1_trg_com(void) {
    dispatch_adv_trg_com(TIMER1);
}

void __irq_tim1_cc(void) {
    dispatch_adv_cc(TIMER1);
}

void __irq_tim2(void) {
    dispatch_general(TIMER2);
}

void __irq_tim3(void) {
    dispatch_general(TIMER3);
}

void __irq_tim4(void) {
    dispatch_general(TIMER4);
}

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)

void __irq_tim5(void) {
    dispatch_general(TIMER5);
}

void __irq_tim6(void) {
    dispatch_basic(TIMER6);
}

void __irq_tim7(void) {
    dispatch_basic(TIMER7);
}

void __irq_tim8_brk(void) {
    dispatch_adv_brk(TIMER8);
}

void __irq_tim8_up(void) {
    dispatch_adv_up(TIMER8);
}

void __irq_tim8_trg_com(void) {
    dispatch_adv_trg_com(TIMER8);
}

void __irq_tim8_cc(void) {
    dispatch_adv_cc(TIMER8);
}
#endif

/* Note: the following dispatch routines make use of the fact that
 * DIER interrupt enable bits and SR interrupt flags have common bit
 * positions.  Thus, ANDing DIER and SR lets us check if an interrupt
 * is enabled and if it has occurred simultaneously.
 */

/* A special-case dispatch routine for single-interrupt NVIC lines.
 * This function assumes that the interrupt corresponding to `iid' has
 * in fact occurred (i.e., it doesn't check DIER & SR). */
static inline void dispatch_single_irq(timer_dev *dev,
                                       timer_interrupt_id iid,
                                       uint32 irq_mask) {
    timer_bas_reg_map *regs = (dev->regs).bas;
    void (*handler)(void) = dev->handlers[iid];
    if (handler) {
        handler();
        regs->SR &= ~irq_mask;
    }
}

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

static inline void dispatch_adv_brk(timer_dev *dev) {
    dispatch_single_irq(dev, TIMER_BREAK_INTERRUPT, TIMER_SR_BIF);
}

static inline void dispatch_adv_up(timer_dev *dev) {
    dispatch_single_irq(dev, TIMER_UPDATE_INTERRUPT, TIMER_SR_UIF);
}

static inline void dispatch_adv_trg_com(timer_dev *dev) {
    timer_adv_reg_map *regs = (dev->regs).adv;
    uint32 dsr = regs->DIER & regs->SR;
    void (**hs)(void) = dev->handlers;
    uint32 handled = 0; /* Logical OR of SR interrupt flags we end up
                         * handling.  We clear these.  User handlers
                         * must clear overcapture flags, to avoid
                         * wasting time in output mode. */

    handle_irq(dsr, TIMER_SR_TIF,   hs, TIMER_TRG_INTERRUPT, handled);
    handle_irq(dsr, TIMER_SR_COMIF, hs, TIMER_COM_INTERRUPT, handled);

    regs->SR &= ~handled;
}

static inline void dispatch_adv_cc(timer_dev *dev) {
    timer_adv_reg_map *regs = (dev->regs).adv;
    uint32 dsr = regs->DIER & regs->SR;
    void (**hs)(void) = dev->handlers;
    uint32 handled = 0;

    handle_irq(dsr, TIMER_SR_CC4IF, hs, TIMER_CC4_INTERRUPT, handled);
    handle_irq(dsr, TIMER_SR_CC3IF, hs, TIMER_CC3_INTERRUPT, handled);
    handle_irq(dsr, TIMER_SR_CC2IF, hs, TIMER_CC2_INTERRUPT, handled);
    handle_irq(dsr, TIMER_SR_CC1IF, hs, TIMER_CC1_INTERRUPT, handled);

    regs->SR &= ~handled;
}

static inline void dispatch_general(timer_dev *dev) {
    timer_gen_reg_map *regs = (dev->regs).gen;
    uint32 dsr = regs->DIER & regs->SR;
    void (**hs)(void) = dev->handlers;
    uint32 handled = 0;

    handle_irq(dsr, TIMER_SR_TIF,   hs, TIMER_TRG_INTERRUPT,    handled);
    handle_irq(dsr, TIMER_SR_CC4IF, hs, TIMER_CC4_INTERRUPT,    handled);
    handle_irq(dsr, TIMER_SR_CC3IF, hs, TIMER_CC3_INTERRUPT,    handled);
    handle_irq(dsr, TIMER_SR_CC2IF, hs, TIMER_CC2_INTERRUPT,    handled);
    handle_irq(dsr, TIMER_SR_CC1IF, hs, TIMER_CC1_INTERRUPT,    handled);
    handle_irq(dsr, TIMER_SR_UIF,   hs, TIMER_UPDATE_INTERRUPT, handled);

    regs->SR &= ~handled;
}

static inline void dispatch_basic(timer_dev *dev) {
    dispatch_single_irq(dev, TIMER_UPDATE_INTERRUPT, TIMER_SR_UIF);
}

/*
 * Utilities
 */

static void disable_channel(timer_dev *dev, uint8 channel) {
    timer_detach_interrupt(dev, channel);
    timer_cc_disable(dev, channel);
}

static void pwm_mode(timer_dev *dev, uint8 channel) {
    timer_disable_irq(dev, channel);
    timer_oc_set_mode(dev, channel, TIMER_OC_MODE_PWM_1, TIMER_OC_PE);
    timer_cc_enable(dev, channel);
}

static void output_compare_mode(timer_dev *dev, uint8 channel) {
    timer_oc_set_mode(dev, channel, TIMER_OC_MODE_ACTIVE_ON_MATCH, 0);
    timer_cc_enable(dev, channel);
}

static void enable_advanced_irq(timer_dev *dev, timer_interrupt_id id);
static void enable_nonmuxed_irq(timer_dev *dev);

static inline void enable_irq(timer_dev *dev, timer_interrupt_id iid) {
    if (dev->type == TIMER_ADVANCED) {
        enable_advanced_irq(dev, iid);
    } else {
        enable_nonmuxed_irq(dev);
    }
}

static void enable_advanced_irq(timer_dev *dev, timer_interrupt_id id) {
    uint8 is_timer1 = dev->clk_id == RCC_TIMER1;

    switch (id) {
    case TIMER_UPDATE_INTERRUPT:
        nvic_irq_enable(is_timer1 ? NVIC_TIMER1_UP : NVIC_TIMER8_UP);
        break;
    case TIMER_CC1_INTERRUPT:
    case TIMER_CC2_INTERRUPT:
    case TIMER_CC3_INTERRUPT:
    case TIMER_CC4_INTERRUPT:
        nvic_irq_enable(is_timer1 ? NVIC_TIMER1_CC : NVIC_TIMER8_CC);
        break;
    case TIMER_COM_INTERRUPT:
    case TIMER_TRG_INTERRUPT:
        nvic_irq_enable(is_timer1 ? NVIC_TIMER1_TRG_COM : NVIC_TIMER8_TRG_COM);
        break;
    case TIMER_BREAK_INTERRUPT:
        nvic_irq_enable(is_timer1 ? NVIC_TIMER1_BRK : NVIC_TIMER8_BRK);
        break;
    }
}

static void enable_nonmuxed_irq(timer_dev *dev) {
    switch (dev->clk_id) {
    case RCC_TIMER2:
        nvic_irq_enable(NVIC_TIMER2);
        break;
    case RCC_TIMER3:
        nvic_irq_enable(NVIC_TIMER3);
        break;
    case RCC_TIMER4:
        nvic_irq_enable(NVIC_TIMER4);
        break;
#ifdef STM32_HIGH_DENSITY
    case RCC_TIMER5:
        nvic_irq_enable(NVIC_TIMER5);
        break;
    case RCC_TIMER6:
        nvic_irq_enable(NVIC_TIMER6);
        break;
    case RCC_TIMER7:
        nvic_irq_enable(NVIC_TIMER7);
        break;
#endif
    default:
        ASSERT_FAULT(0);
        break;
    }
}
