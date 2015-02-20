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
 * @file   libmaple/timer.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Portable timer routines.
 */

#include <libmaple/timer.h>
#include <libmaple/stm32.h>
#include "timer_private.h"

static void disable_channel(timer_dev *dev, uint8 channel);
static void pwm_mode(timer_dev *dev, uint8 channel);
static void output_compare_mode(timer_dev *dev, uint8 channel);

static inline void enable_irq(timer_dev *dev, uint8 interrupt);

/*
 * Devices
 *
 * Defer to the timer_private API for declaring these.
 */

#if STM32_HAVE_TIMER(1)
static timer_dev timer1 = ADVANCED_TIMER(1);
/** Timer 1 device (advanced) */
timer_dev *TIMER1 = &timer1;
#endif
#if STM32_HAVE_TIMER(2)
static timer_dev timer2 = GENERAL_TIMER(2);
/** Timer 2 device (general-purpose) */
timer_dev *TIMER2 = &timer2;
#endif
#if STM32_HAVE_TIMER(3)
static timer_dev timer3 = GENERAL_TIMER(3);
/** Timer 3 device (general-purpose) */
timer_dev *TIMER3 = &timer3;
#endif
#if STM32_HAVE_TIMER(4)
static timer_dev timer4 = GENERAL_TIMER(4);
/** Timer 4 device (general-purpose) */
timer_dev *TIMER4 = &timer4;
#endif
#if STM32_HAVE_TIMER(5)
static timer_dev timer5 = GENERAL_TIMER(5);
/** Timer 5 device (general-purpose) */
timer_dev *TIMER5 = &timer5;
#endif
#if STM32_HAVE_TIMER(6)
static timer_dev timer6 = BASIC_TIMER(6);
/** Timer 6 device (basic) */
timer_dev *TIMER6 = &timer6;
#endif
#if STM32_HAVE_TIMER(7)
static timer_dev timer7 = BASIC_TIMER(7);
/** Timer 7 device (basic) */
timer_dev *TIMER7 = &timer7;
#endif
#if STM32_HAVE_TIMER(8)
static timer_dev timer8 = ADVANCED_TIMER(8);
/** Timer 8 device (advanced) */
timer_dev *TIMER8 = &timer8;
#endif
#if STM32_HAVE_TIMER(9)
static timer_dev timer9 = RESTRICTED_GENERAL_TIMER(9, TIMER_DIER_TIE_BIT);
/** Timer 9 device (general-purpose) */
timer_dev *TIMER9 = &timer9;
#endif
#if STM32_HAVE_TIMER(10)
static timer_dev timer10 = RESTRICTED_GENERAL_TIMER(10, TIMER_DIER_CC1IE_BIT);
/** Timer 10 device (general-purpose) */
timer_dev *TIMER10 = &timer10;
#endif
#if STM32_HAVE_TIMER(11)
static timer_dev timer11 = RESTRICTED_GENERAL_TIMER(11, TIMER_DIER_CC1IE_BIT);
/** Timer 11 device (general-purpose) */
timer_dev *TIMER11 = &timer11;
#endif
#if STM32_HAVE_TIMER(12)
static timer_dev timer12 = RESTRICTED_GENERAL_TIMER(12, TIMER_DIER_TIE_BIT);
/** Timer 12 device (general-purpose) */
timer_dev *TIMER12 = &timer12;
#endif
#if STM32_HAVE_TIMER(13)
static timer_dev timer13 = RESTRICTED_GENERAL_TIMER(13, TIMER_DIER_CC1IE_BIT);
/** Timer 13 device (general-purpose) */
timer_dev *TIMER13 = &timer13;
#endif
#if STM32_HAVE_TIMER(14)
static timer_dev timer14 = RESTRICTED_GENERAL_TIMER(14, TIMER_DIER_CC1IE_BIT);
/** Timer 14 device (general-purpose) */
timer_dev *TIMER14 = &timer14;
#endif
#if STM32_HAVE_TIMER(15)
static timer_dev timer15 = RESTRICTED_GENERAL_TIMER(15, TIMER_DIER_TIE_BIT); //XXX
/** Timer 15 device (general-purpose) */
timer_dev *TIMER15 = &timer15;
#endif
#if STM32_HAVE_TIMER(16)
static timer_dev timer16 = RESTRICTED_GENERAL_TIMER(16, TIMER_DIER_CC1IE_BIT); //XXX
/** Timer 16 device (general-purpose) */
timer_dev *TIMER16 = &timer16;
#endif
#if STM32_HAVE_TIMER(17)
static timer_dev timer17 = RESTRICTED_GENERAL_TIMER(17, TIMER_DIER_CC1IE_BIT); //XXX
/** Timer 17 device (general-purpose) */
timer_dev *TIMER17 = &timer17;
#endif

/*
 * Routines
 */

/**
 * @brief Call a function on timer devices.
 * @param fn Function to call on each timer device.
 */
void timer_foreach(void (*fn)(timer_dev*)) {
#if STM32_HAVE_TIMER(1)
    fn(TIMER1);
#endif
#if STM32_HAVE_TIMER(2)
    fn(TIMER2);
#endif
#if STM32_HAVE_TIMER(3)
    fn(TIMER3);
#endif
#if STM32_HAVE_TIMER(4)
    fn(TIMER4);
#endif
#if STM32_HAVE_TIMER(5)
    fn(TIMER5);
#endif
#if STM32_HAVE_TIMER(6)
    fn(TIMER6);
#endif
#if STM32_HAVE_TIMER(7)
    fn(TIMER7);
#endif
#if STM32_HAVE_TIMER(8)
    fn(TIMER8);
#endif
#if STM32_HAVE_TIMER(9)
    fn(TIMER9);
#endif
#if STM32_HAVE_TIMER(10)
    fn(TIMER10);
#endif
#if STM32_HAVE_TIMER(11)
    fn(TIMER11);
#endif
#if STM32_HAVE_TIMER(12)
    fn(TIMER12);
#endif
#if STM32_HAVE_TIMER(13)
    fn(TIMER13);
#endif
#if STM32_HAVE_TIMER(14)
    fn(TIMER14);
#endif
#if STM32_HAVE_TIMER(15)
    fn(TIMER15);
#endif
#if STM32_HAVE_TIMER(16)
    fn(TIMER16);
#endif
#if STM32_HAVE_TIMER(17)
    fn(TIMER17);
#endif
}

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
 * @brief Determine whether a timer has a particular capture/compare channel.
 *
 * Different timers have different numbers of capture/compare channels
 * (and some have none at all). Use this function to test whether a
 * given timer/channel combination will work.
 *
 * @param dev Timer device
 * @param channel Capture/compare channel, from 1 to 4
 * @return Nonzero if dev has channel, zero otherwise.
 */
int timer_has_cc_channel(timer_dev *dev, uint8 channel) {
    /* On all currently supported series: advanced and "full-featured"
     * general purpose timers have all four channels. Of the
     * restricted general timers, timers 9, 12 and 15 have channels 1 and
     * 2; the others have channel 1 only. Basic timers have none. */
    rcc_clk_id id = dev->clk_id;
    ASSERT((1 <= channel) && (channel <= 4));
    if (id <= RCC_TIMER5 || id == RCC_TIMER8) {
        return 1;     /* 1 and 8 are advanced, 2-5 are "full" general */
    } else if (id <= RCC_TIMER7) {
        return 0;     /* 6 and 7 are basic */
    }
    /* The rest are restricted general. */
    return (((id == RCC_TIMER9 || id == RCC_TIMER12 || id == RCC_TIMER15) && channel <= 2) ||
            channel == 1);
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

static void enable_adv_irq(timer_dev *dev, timer_interrupt_id id);
static void enable_bas_gen_irq(timer_dev *dev);

static inline void enable_irq(timer_dev *dev, timer_interrupt_id iid) {
    if (dev->type == TIMER_ADVANCED) {
        enable_adv_irq(dev, iid);
    } else {
        enable_bas_gen_irq(dev);
    }
}

/* Advanced control timers have several IRQ lines corresponding to
 * different timer interrupts.
 *
 * Note: This function assumes that the only advanced timers are TIM1
 * and TIM8, and needs the obvious changes if that assumption is
 * violated by a later STM32 series. */
static void enable_adv_irq(timer_dev *dev, timer_interrupt_id id) {
    uint8 is_tim1 = dev->clk_id == RCC_TIMER1;
    nvic_irq_num irq_num;
    switch (id) {
    case TIMER_UPDATE_INTERRUPT:
        irq_num = (is_tim1 ?
                   NVIC_TIMER1_UP_TIMER10 :
                   NVIC_TIMER8_UP_TIMER13);
        break;
    case TIMER_CC1_INTERRUPT:   /* Fall through */
    case TIMER_CC2_INTERRUPT:   /* ... */
    case TIMER_CC3_INTERRUPT:   /* ... */
    case TIMER_CC4_INTERRUPT:
        irq_num = is_tim1 ? NVIC_TIMER1_CC : NVIC_TIMER8_CC;
        break;
    case TIMER_COM_INTERRUPT:   /* Fall through */
    case TIMER_TRG_INTERRUPT:
        irq_num = (is_tim1 ?
                   NVIC_TIMER1_TRG_COM_TIMER11 :
                   NVIC_TIMER8_TRG_COM_TIMER14);
        break;
    case TIMER_BREAK_INTERRUPT:
        irq_num = (is_tim1 ?
                   NVIC_TIMER1_BRK_TIMER9 :
                   NVIC_TIMER8_BRK_TIMER12);
        break;
    default:
        /* Can't happen, but placate the compiler */
        ASSERT(0);
        return;
    }
    nvic_irq_enable(irq_num);
}

/* Basic and general purpose timers have a single IRQ line, which is
 * shared by all interrupts supported by a particular timer. */
static void enable_bas_gen_irq(timer_dev *dev) {
    nvic_irq_num irq_num;
    switch (dev->clk_id) {
    case RCC_TIMER2:
        irq_num = NVIC_TIMER2;
        break;
    case RCC_TIMER3:
        irq_num = NVIC_TIMER3;
        break;
    case RCC_TIMER4:
        irq_num = NVIC_TIMER4;
        break;
#if STM32_HAVE_TIMER(5)
    case RCC_TIMER5:
        irq_num = NVIC_TIMER5;
        break;
#endif
    case RCC_TIMER6:
        irq_num = NVIC_TIMER6;
        break;
    case RCC_TIMER7:
        irq_num = NVIC_TIMER7;
        break;
    case RCC_TIMER9:
        irq_num = NVIC_TIMER1_BRK_TIMER9;
        break;
    case RCC_TIMER10:
        irq_num = NVIC_TIMER1_UP_TIMER10;
        break;
    case RCC_TIMER11:
        irq_num = NVIC_TIMER1_TRG_COM_TIMER11;
        break;
    case RCC_TIMER12:
        irq_num = NVIC_TIMER8_BRK_TIMER12;
        break;
    case RCC_TIMER13:
        irq_num = NVIC_TIMER8_UP_TIMER13;
        break;
    case RCC_TIMER14:
        irq_num = NVIC_TIMER8_TRG_COM_TIMER14;
        break;
#if STM32_HAVE_TIMER(15)
    case RCC_TIMER15:
        //irq_num = NVIC_TIMER1_BRK_TIMER15;
        irq_num = NVIC_TIMER1_BRK_TIMER9;
        break;
#endif
#if STM32_HAVE_TIMER(16)
    case RCC_TIMER16:
        //irq_num = NVIC_TIMER1_UP_TIMER16;
        irq_num = NVIC_TIMER1_UP_TIMER10;
        break;
#endif
#if STM32_HAVE_TIMER(17)
    case RCC_TIMER17:
        //irq_num = NVIC_TIMER1_TRG_COM_TIMER17;
        irq_num = NVIC_TIMER1_TRG_COM_TIMER11;
        break;
#endif
    default:
        ASSERT_FAULT(0);
        return;
    }
    nvic_irq_enable(irq_num);
}
