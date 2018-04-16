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
 * @file libmaple/rcc.c
 * @brief Portable RCC routines.
 */

#include <libmaple/rcc.h>

#include "rcc_private.h"

/**
 * @brief Get a peripheral's clock domain
 * @param id Clock ID of the peripheral whose clock domain to return
 * @return Clock source for the given clock ID
 */
rcc_clk_domain rcc_dev_clk(rcc_clk_id id) {
    return rcc_dev_table[id].clk_domain;
}

/**
 * @brief Switch the clock used as the source of the system clock.
 *
 * After switching the source, this function blocks until the new
 * clock source is in use.
 *
 * @param sysclk_src New system clock source.
 * @see rcc_sysclk_src
 */
void rcc_switch_sysclk(rcc_sysclk_src sysclk_src) {
    uint32 cfgr = RCC_BASE->CFGR;
    cfgr &= ~RCC_CFGR_SW;
    cfgr |= sysclk_src;

    /* Switch SYSCLK source. */
    RCC_BASE->CFGR = cfgr;

    /* Wait for new source to come into use. */
    while ((RCC_BASE->CFGR & RCC_CFGR_SWS) != (sysclk_src << 2))
        ;
}

/*
 * Turning clocks off and on, querying their status.
 */

/* IMPORTANT NOTE FOR IMPLEMENTORS:
 *
 * libmaple assumes that enum rcc_clk enumerators are two-byte
 * values, stored in a uint16, in the following way:
 *
 *  - The high-order byte is the byte offset (from RCC_BASE) of the register
 *    to touch when turning on or off the given clock.
 *
 *  - The low-order byte is the bit in that register that turns the
 *    clock on or off.
 *
 * Example for STM32F1: Turning on the high-speed external clock (HSE)
 * involves setting HSEON, bit 16, of RCC_CR. The high-order byte is
 * then offsetof(struct rcc_reg_map, CR) = 0, and the low-order byte
 * is 16.
 *
 * The corresponding value of RCC_CLK_HSE is thus (0 << 8) | 16 = 16.
 *
 * On all known STM32 series, this encoding has the property that
 * adding one to the low byte also gives the bit to check to determine
 * if the clock is ready. For example, on STM32F1, RCC_CR_HSERDY is
 * bit 17. If that's not the case on your series, rcc_is_clk_ready()
 * won't work for you. */

/* Returns the RCC register which controls the clock source. */
static inline __IO uint32* rcc_clk_reg(rcc_clk clock) {
    return (__IO uint32*)((__IO uint8*)RCC_BASE + (clock >> 8));
}

/* Returns a mask in rcc_clk_reg(clock) to be used for turning the
 * clock on and off */
static inline uint32 rcc_clk_on_mask(rcc_clk clock) {
    return 1 << (clock & 0xFF);
}

/* Returns a mask in rcc_clk_reg(clock) to be used when checking the
 * readiness of the clock. */
static inline uint32 rcc_clk_ready_mask(rcc_clk clock) {
    return rcc_clk_on_mask(clock) << 1;
}

/**
 * @brief Turn on a clock source.
 *
 * After this routine exits, callers should ensure that the clock
 * source is ready by waiting until rcc_is_clk_ready(clock) returns
 * true.
 *
 * @param clock Clock to turn on.
 * @see rcc_turn_off_clk()
 * @see rcc_is_clk_ready()
 */
void rcc_turn_on_clk(rcc_clk clock) {
    *rcc_clk_reg(clock) |= rcc_clk_on_mask(clock);
}

/**
 * @brief Turn off a clock source.
 *
 * In certain configurations, certain clock sources cannot be safely
 * turned off. (For example, the main PLL on STM32F1 devices cannot be
 * turned off if it has been selected as the SYSCLK source). Consult
 * the reference material for your MCU to ensure it is safe to call
 * this function.
 *
 * @param clock Clock to turn off.
 * @see rcc_turn_on_clk()
 * @see rcc_is_clk_ready()
 */
void rcc_turn_off_clk(rcc_clk clock) {
    *rcc_clk_reg(clock) &= ~rcc_clk_on_mask(clock);
}

/**
 * @brief Check if a clock is on.
 * @param clock Clock to check.
 * @return 1 if the clock is on, 0 if the clock is off.
 */
int rcc_is_clk_on(rcc_clk clock) {
    return !!(*rcc_clk_reg(clock) & rcc_clk_on_mask(clock));
}

/**
 * @brief Check if a clock source is ready.
 *
 * In general, it is not safe to rely on a clock source unless this
 * function returns nonzero. Also note that this function may return
 * nonzero for a short period of time after a clock has been turned
 * off. Consult the reference material for your MCU for more details.
 *
 * @param clock Clock whose readiness to check for.
 * @return Nonzero if the clock is ready, zero otherwise.
 * @see rcc_turn_on_clk()
 * @see rcc_turn_off_clk()
 */
int rcc_is_clk_ready(rcc_clk clock) {
    return (int)(*rcc_clk_reg(clock) & rcc_clk_ready_mask(clock));
}
