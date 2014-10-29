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
 * @file libmaple/stm32f2/syscfg.c
 * @brief SYSCFG routines.
 */

#include <libmaple/syscfg.h>
#include <libmaple/bitband.h>
#include <libmaple/rcc.h>

/**
 * @brief Initialize the SYSCFG peripheral.
 */
void syscfg_init(void) {
    rcc_clk_enable(RCC_SYSCFG);
    rcc_reset_dev(RCC_SYSCFG);
}

/**
 * @brief Turn on the I/O compensation cell.
 *
 * It's only safe to do this when the supply voltage is between 2.4 V
 * and 3.6 V.
 */
void syscfg_enable_io_compensation(void) {
    bb_peri_set_bit(&SYSCFG_BASE->CMPCR, SYSCFG_CMPCR_CMP_PD_BIT, 1);
    while (!(SYSCFG_BASE->CMPCR & SYSCFG_CMPCR_READY))
        ;
}

/**
 * @brief Turn off the I/O compensation cell.
 */
void syscfg_disable_io_compensation(void) {
    bb_peri_set_bit(&SYSCFG_BASE->CMPCR, SYSCFG_CMPCR_CMP_PD_BIT, 0);
}

/**
 * @brief Set the memory to be mapped at address 0x00000000.
 *
 * This function can be used to override the BOOT pin
 * configuration. Some restrictions apply; see your chip's reference
 * manual for the details.
 *
 * @param mode Mode to set
 * @see syscfg_mem_mode
 */
void syscfg_set_mem_mode(syscfg_mem_mode mode) {
    uint32 memrmp = SYSCFG_BASE->MEMRMP;
    memrmp &= ~SYSCFG_MEMRMP_MEM_MODE;
    memrmp |= (uint32)mode;
    SYSCFG_BASE->MEMRMP = memrmp;
}
