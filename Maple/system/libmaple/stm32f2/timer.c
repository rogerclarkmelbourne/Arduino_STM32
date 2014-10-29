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
 * @file   libmaple/stm32f2/timer.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  STM32F2 timers.
 */

#include <libmaple/timer.h>
#include "timer_private.h"

/*
 * Routines
 */

/**
 * @brief Get the GPIO alternate function corresponding to a timer.
 *
 * For example, if dev is TIMER1, this function returns
 * GPIO_AF_TIM_1_2. This is useful for e.g. using gpio_set_af() to set
 * a pin's alternate function to a timer.
 *
 * @param dev Timer device, must not be TIMER6 or TIMER7.
 * @return gpio_af corresponding to dev
 * @see gpio_set_af
 * @see gpio_af
 */
gpio_af timer_get_af(timer_dev *dev) {
    rcc_clk_id clk_id = dev->clk_id;
    /* Timers 6 and 7 don't have any capture/compare, so they can't do
     * PWM (and in fact have no AF values). */
    ASSERT(clk_id != RCC_TIMER6 && clk_id != RCC_TIMER7);
    switch(dev->clk_id) {
    case RCC_TIMER1:        // fall-through
    case RCC_TIMER2:
        return GPIO_AF_TIM_1_2;
    case RCC_TIMER3:        // fall-through
    case RCC_TIMER4:        // ...
    case RCC_TIMER5:
        return GPIO_AF_TIM_3_4_5;
    case RCC_TIMER8:        // fall-through
    case RCC_TIMER9:        // ...
    case RCC_TIMER10:       // ...
    case RCC_TIMER11:
        return GPIO_AF_TIM_8_9_10_11;
    case RCC_TIMER12:       // fall-through
    case RCC_TIMER13:       // ...
    case RCC_TIMER14:
        return GPIO_AF_CAN_1_2_TIM_12_13_14;
    default:
        ASSERT(0);          // Can't happen
        return (gpio_af)-1;
    }
}

/*
 * IRQ handlers
 *
 * Defer to the timer_private dispatch API.
 */

void __irq_tim1_brk_tim9(void) {
    dispatch_adv_brk(TIMER1);
    dispatch_tim_9_12(TIMER9);
}

void __irq_tim1_up_tim10(void) {
    dispatch_adv_up(TIMER1);
    dispatch_tim_10_11_13_14(TIMER10);
}

void __irq_tim1_trg_com_tim11(void) {
    dispatch_adv_trg_com(TIMER1);
    dispatch_tim_10_11_13_14(TIMER11);
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

void __irq_tim5(void) {
    dispatch_general(TIMER5);
}

/* FIXME: this is also the DAC DMA underrun interrupt, so it needs a
 * different name (and to be supported?). */
void __irq_tim6(void) {
    dispatch_basic(TIMER6);
}

void __irq_tim7(void) {
    dispatch_basic(TIMER7);
}

void __irq_tim8_brk_tim12(void) {
    dispatch_adv_brk(TIMER8);
    dispatch_tim_9_12(TIMER12);
}

void __irq_tim8_up_tim13(void) {
    dispatch_adv_up(TIMER8);
    dispatch_tim_10_11_13_14(TIMER13);
}

void __irq_tim8_trg_com_tim14(void) {
    dispatch_adv_trg_com(TIMER8);
    dispatch_tim_10_11_13_14(TIMER14);
}

void __irq_tim8_cc(void) {
    dispatch_adv_cc(TIMER8);
}
