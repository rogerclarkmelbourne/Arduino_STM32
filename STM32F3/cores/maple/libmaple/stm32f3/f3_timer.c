/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file   libmaple/stm32f3/timer.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 * 				 F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief  STM32F3 timer.
 */

#include <libmaple/timer.h>
#include <libmaple/stm32.h>
#include <libmaple/gpio.h>
#include "timer_private.h"

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
 /* FIXME this has to be inline with the PIN_MAP, but some timers are accessible on different alternate function lanes based on the used pin... */
gpio_af timer_get_af(timer_dev *dev) {
    rcc_clk_id clk_id = dev->clk_id;
    /* Timers 6 and 7 don't have any capture/compare, so they can't do
     * PWM (and in fact have no AF values). */
    ASSERT(clk_id != RCC_TIMER6 && clk_id != RCC_TIMER7);
    switch(dev->clk_id) {
    case RCC_TIMER1:
        return GPIO_AF_6;
    case RCC_TIMER2:	// fall-through
    case RCC_TIMER3:	// fall-through
    case RCC_TIMER4:
        return GPIO_AF_2;
    case RCC_TIMER8:
        return GPIO_AF_4;
    case RCC_TIMER15:
        return GPIO_AF_3;
    case RCC_TIMER16:	// fall-through
    case RCC_TIMER17:
        return GPIO_AF_1;
    default:
        ASSERT(0);          // Can't happen
        return (gpio_af)-1;
    }
}

/*
 * IRQ handlers
 *
 * Defer to the timer_private dispatch API.
 *
 * FIXME: The names of these handlers are inaccurate since XL-density
 * devices came out. Update these to match the STM32F3 names, maybe
 * using some weak symbol magic to preserve backwards compatibility if
 * possible. Once that's done, we can just move the IRQ handlers into
 * the top-level libmaple/timer.c, and there will be no need for this
 * file.
 */

void __irq_tim1_brk_tim15(void) {
    dispatch_adv_brk(TIMER1);
    dispatch_tim_9_12(TIMER15);
}

void __irq_tim1_up_tim16(void) {
    dispatch_adv_up(TIMER1);
    dispatch_tim_10_11_13_14(TIMER16);
}

void __irq_tim1_trg_com_tim17(void) {
    dispatch_adv_trg_com(TIMER1);
    dispatch_tim_10_11_13_14(TIMER17);
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

void __irq_tim6_dacunder(void) {
    dispatch_basic(TIMER6);
		/* FIXME handle DAC12 underrun */
}

#if STM32_F3_LINE == STM32_F3_LINE_303
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
