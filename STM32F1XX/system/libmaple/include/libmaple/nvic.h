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
 * @file libmaple/include/libmaple/nvic.h
 * @brief Nested vectored interrupt controller support.
 *
 * Basic usage:
 *
 * @code
 *   // Initialise the interrupt controller and point to the vector
 *   // table at the start of flash.
 *   nvic_init(0x08000000, 0);
 *   // Bind in a timer interrupt handler
 *   timer_attach_interrupt(TIMER_CC1_INTERRUPT, handler);
 *   // Optionally set the priority
 *   nvic_irq_set_priority(NVIC_TIMER1_CC, 5);
 *   // All done, enable all interrupts
 *   nvic_globalirq_enable();
 * @endcode
 */

#ifndef _LIBMAPLE_NVIC_H_
#define _LIBMAPLE_NVIC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/util.h>

/** NVIC register map type. */
typedef struct nvic_reg_map {
    __io uint32 ISER[8];      /**< Interrupt Set Enable Registers */
    /** Reserved */
    uint32 RESERVED0[24];

    __io uint32 ICER[8];      /**< Interrupt Clear Enable Registers */
    /** Reserved */
    uint32 RESERVED1[24];

    __io uint32 ISPR[8];      /**< Interrupt Set Pending Registers */
    /** Reserved */
    uint32 RESERVED2[24];

    __io uint32 ICPR[8];      /**< Interrupt Clear Pending Registers */
    /** Reserved */
    uint32 RESERVED3[24];

    __io uint32 IABR[8];      /**< Interrupt Active bit Registers */
    /** Reserved */
    uint32 RESERVED4[56];

    __io uint8  IP[240];      /**< Interrupt Priority Registers */
    /** Reserved */
    uint32 RESERVED5[644];

    __io uint32 STIR;         /**< Software Trigger Interrupt Registers */
} nvic_reg_map;

/** NVIC register map base pointer. */
#define NVIC_BASE                       ((struct nvic_reg_map*)0xE000E100)

/*
 * Note: The series header must define enum nvic_irq_num, which gives
 * descriptive names to the interrupts and exceptions from NMI (-14)
 * to the largest interrupt available in the series, where the value
 * for nonnegative enumerators corresponds to its position in the
 * vector table.
 *
 * It also must define a static inline nvic_irq_disable_all(), which
 * writes 0xFFFFFFFF to all ICE registers available in the series. (We
 * place the include here to give the series header access to
 * NVIC_BASE, in order to let it do so).
 */
#include <series/nvic.h>

void nvic_init(uint32 address, uint32 offset);
void nvic_set_vector_table(uint32 address, uint32 offset);
void nvic_irq_set_priority(nvic_irq_num irqn, uint8 priority);
void nvic_sys_reset();

/**
 * Enables interrupts and configurable fault handlers (clear PRIMASK).
 */
static __always_inline void nvic_globalirq_enable() {
    asm volatile("cpsie i");
}

/**
 * Disable interrupts and configurable fault handlers (set PRIMASK).
 */
static __always_inline void nvic_globalirq_disable() {
    asm volatile("cpsid i");
}

/**
 * @brief Enable interrupt irq_num
 * @param irq_num Interrupt to enable
 */
static inline void nvic_irq_enable(nvic_irq_num irq_num) {
    if (irq_num < 0) {
        return;
    }
    NVIC_BASE->ISER[irq_num / 32] = BIT(irq_num % 32);
}

/**
 * @brief Disable interrupt irq_num
 * @param irq_num Interrupt to disable
 */
static inline void nvic_irq_disable(nvic_irq_num irq_num) {
    if (irq_num < 0) {
        return;
    }
    NVIC_BASE->ICER[irq_num / 32] = BIT(irq_num % 32);
}

/**
 * @brief Quickly disable all interrupts.
 *
 * Calling this function is significantly faster than calling
 * nvic_irq_disable() in a loop.
 */
static inline void nvic_irq_disable_all(void);

#ifdef __cplusplus
}
#endif

#endif
