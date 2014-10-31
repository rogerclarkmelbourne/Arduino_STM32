/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file   libmaple/stm32f1/timer.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  STM32F1 timer.
 */

#include <libmaple/timer.h>
#include <libmaple/stm32.h>
#include "timer_private.h"

/*
 * IRQ handlers
 *
 * Defer to the timer_private dispatch API.
 *
 * FIXME: The names of these handlers are inaccurate since XL-density
 * devices came out. Update these to match the STM32F2 names, maybe
 * using some weak symbol magic to preserve backwards compatibility if
 * possible. Once that's done, we can just move the IRQ handlers into
 * the top-level libmaple/timer.c, and there will be no need for this
 * file.
 */

void __irq_tim1_brk(void) {
    dispatch_adv_brk(TIMER1);
#if STM32_HAVE_TIMER(9)
    dispatch_tim_9_12(TIMER9);
#endif
}

void __irq_tim1_up(void) {
    dispatch_adv_up(TIMER1);
#if STM32_HAVE_TIMER(10)
    dispatch_tim_10_11_13_14(TIMER10);
#endif
}

void __irq_tim1_trg_com(void) {
    dispatch_adv_trg_com(TIMER1);
#if STM32_HAVE_TIMER(11)
    dispatch_tim_10_11_13_14(TIMER11);
#endif
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
#if STM32_HAVE_TIMER(12)
    dispatch_tim_9_12(TIMER12);
#endif
}

void __irq_tim8_up(void) {
    dispatch_adv_up(TIMER8);
#if STM32_HAVE_TIMER(13)
    dispatch_tim_10_11_13_14(TIMER13);
#endif
}

void __irq_tim8_trg_com(void) {
    dispatch_adv_trg_com(TIMER8);
#if STM32_HAVE_TIMER(14)
    dispatch_tim_10_11_13_14(TIMER14);
#endif
}

void __irq_tim8_cc(void) {
    dispatch_adv_cc(TIMER8);
}
#endif  /* defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY) */
