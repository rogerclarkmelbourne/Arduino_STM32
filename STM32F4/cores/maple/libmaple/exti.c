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
 * @file exti.c
 * @brief External interrupt control routines
 */

#include "exti.h"
#include "libmaple.h"
#include "nvic.h"
#include "bitband.h"

static inline void dispatch_single_exti(uint32 exti_num);
static inline void dispatch_extis(uint32 start, uint32 stop);

/*
 * Internal state
 */

typedef struct exti_channel {
    void (*handler)(void);
    uint32 irq_line;
} exti_channel;

static exti_channel exti_channels[] = {
    { .handler = NULL, .irq_line = NVIC_EXTI0      },  // EXTI0
    { .handler = NULL, .irq_line = NVIC_EXTI1      },  // EXTI1
    { .handler = NULL, .irq_line = NVIC_EXTI2      },  // EXTI2
    { .handler = NULL, .irq_line = NVIC_EXTI3      },  // EXTI3
    { .handler = NULL, .irq_line = NVIC_EXTI4      },  // EXTI4
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI5
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI6
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI7
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI8
    { .handler = NULL, .irq_line = NVIC_EXTI_9_5   },  // EXTI9
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI10
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI11
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI12
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI13
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI14
    { .handler = NULL, .irq_line = NVIC_EXTI_15_10 },  // EXTI15
};

/*
 * Convenience routines
 */

/**
 * @brief Register a handler to run upon external interrupt.
 *
 * This function assumes that the interrupt request corresponding to
 * the given external interrupt is masked.
 *
 * @param num     External interrupt line number.
 * @param port    Port to use as source input for external interrupt.
 * @param handler Function handler to execute when interrupt is triggered.
 * @param mode    Type of transition to trigger on, one of:
 *                EXTI_RISING, EXTI_FALLING, EXTI_RISING_FALLING.
 * @see afio_exti_num
 * @see afio_exti_port
 * @see voidFuncPtr
 * @see exti_trigger_mode
 */
void exti_attach_interrupt(afio_exti_num num,
                           afio_exti_port port,
                           voidFuncPtr handler,
                           exti_trigger_mode mode) {
    ASSERT(handler);

    /* Register the handler */
    exti_channels[num].handler = handler;

    /* Set trigger mode */
    switch (mode) {
    case EXTI_RISING:
        bb_peri_set_bit(&EXTI_BASE->RTSR, num, 1);
        break;
    case EXTI_FALLING:
        bb_peri_set_bit(&EXTI_BASE->FTSR, num, 1);
        break;
    case EXTI_RISING_FALLING:
        bb_peri_set_bit(&EXTI_BASE->RTSR, num, 1);
        bb_peri_set_bit(&EXTI_BASE->FTSR, num, 1);
        break;
    }

    /* Map num to port */
    afio_exti_select(num, port);

    /* Unmask external interrupt request */
    bb_peri_set_bit(&EXTI_BASE->IMR, num, 1);

    /* Enable the interrupt line */
    nvic_irq_enable(exti_channels[num].irq_line);
}

/**
 * @brief Unregister an external interrupt handler
 * @param num Number of the external interrupt line to disable.
 * @see afio_exti_num
 */
void exti_detach_interrupt(afio_exti_num num) {
    /* First, mask the interrupt request */
    bb_peri_set_bit(&EXTI_BASE->IMR, num, 0);

    /* Then, clear the trigger selection registers */
    bb_peri_set_bit(&EXTI_BASE->FTSR, num, 0);
    bb_peri_set_bit(&EXTI_BASE->RTSR, num, 0);

    /* Finally, unregister the user's handler */
    exti_channels[num].handler = NULL;
}

/*
 * Interrupt handlers
 */

void __irq_exti0(void) {
    dispatch_single_exti(AFIO_EXTI_0);
}

void __irq_exti1(void) {
    dispatch_single_exti(AFIO_EXTI_1);
}

void __irq_exti2(void) {
    dispatch_single_exti(AFIO_EXTI_2);
}

void __irq_exti3(void) {
    dispatch_single_exti(AFIO_EXTI_3);
}

void __irq_exti4(void) {
    dispatch_single_exti(AFIO_EXTI_4);
}

void __irq_exti9_5(void) {
    dispatch_extis(5, 9);
}

void __irq_exti15_10(void) {
    dispatch_extis(10, 15);
}

/*
 * Auxiliary functions
 */

/* Clear the pending bits for EXTIs whose bits are set in exti_msk.
 *
 * If a pending bit is cleared as the last instruction in an ISR, it
 * won't actually be cleared in time and the ISR will fire again.  To
 * compensate, this function NOPs for 2 cycles after clearing the
 * pending bits to ensure it takes effect. */
static inline void clear_pending_msk(uint32 exti_msk) {
    EXTI_BASE->PR = exti_msk;
    asm volatile("nop");
    asm volatile("nop");
}

/* This dispatch routine is for non-multiplexed EXTI lines only; i.e.,
 * it doesn't check EXTI_PR. */
static inline void dispatch_single_exti(uint32 exti) {
    voidFuncPtr handler = exti_channels[exti].handler;

    if (!handler) {
        return;
    }

    handler();
    clear_pending_msk(BIT(exti));
}

/* Dispatch routine for EXTIs which share an IRQ. */
static inline void dispatch_extis(uint32 start, uint32 stop) {
    uint32 pr = EXTI_BASE->PR;
    uint32 handled_msk = 0;
    uint32 exti;

    /* Dispatch user handlers for pending EXTIs. */
    for (exti = start; exti <= stop; exti++) {
        uint32 eb = BIT(exti);
        if (pr & eb) {
            voidFuncPtr handler = exti_channels[exti].handler;
            if (handler) {
                handler();
                handled_msk |= eb;
            }
        }
    }

    /* Clear the pending bits for handled EXTIs. */
    clear_pending_msk(handled_msk);
}
