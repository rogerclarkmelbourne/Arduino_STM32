/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/nvic.c
 * @brief Nested vector interrupt controller support.
 */

#include <libmaple/nvic.h>
#include <libmaple/scb.h>
#include <libmaple/stm32.h>

/**
 * @brief Set interrupt priority for an interrupt line
 *
 * Note: The STM32 only implements 4 bits of priority, ignoring the
 * lower 4 bits. This means there are only 16 levels of priority.
 * Bits[3:0] read as zero and ignore writes.
 *
 * @param irqn device to set
 * @param priority Priority to set, 0 being highest priority and 15
 *                 being lowest.
 */
void nvic_irq_set_priority(nvic_irq_num irqn, uint8 priority) {
    if (irqn < 0) {
        /* This interrupt is in the system handler block */
        SCB_BASE->SHP[((uint32)irqn & 0xF) - 4] = (priority & 0xF) << 4;
    } else {
        NVIC_BASE->IP[irqn] = (priority & 0xF) << 4;
    }
}

/**
 * @brief Initialize the NVIC, setting interrupts to a default priority.
 */
void nvic_init(uint32 address, uint32 offset) {
    uint32 i;

    nvic_set_vector_table(address, offset);

    /*
     * Lower priority level for all peripheral interrupts to lowest
     * possible.
     */
    for (i = 0; i < STM32_NR_INTERRUPTS; i++) {
        nvic_irq_set_priority((nvic_irq_num)i, 0xF);
    }

    /* Lower systick interrupt priority to lowest level */
    nvic_irq_set_priority(NVIC_SYSTICK, 0xF);
}

/**
 * @brief Set the vector table base address.
 *
 * For stand-alone products, the vector table base address is normally
 * the start of Flash (0x08000000).
 *
 * @param address Vector table base address.
 * @param offset Offset from address.  Some restrictions apply to the
 *               use of nonzero offsets; see the ARM Cortex M3
 *               Technical Reference Manual.
 */
void nvic_set_vector_table(uint32 address, uint32 offset) {
    SCB_BASE->VTOR = address | (offset & 0x1FFFFF80);
}

/**
 * @brief Force a system reset.
 *
 * Resets all major system components, excluding debug.
 */
void nvic_sys_reset() {
    uint32 prigroup = SCB_BASE->AIRCR & SCB_AIRCR_PRIGROUP;
    SCB_BASE->AIRCR = SCB_AIRCR_VECTKEY | SCB_AIRCR_SYSRESETREQ | prigroup;
    asm volatile("dsb");
    while (1)
        ;
}
