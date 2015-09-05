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
 * @file libmaple/include/libmaple/rcc.h
 * @brief Reset and Clock Control (RCC) interface.
 */

#ifndef _LIBMAPLE_RCC_H_
#define _LIBMAPLE_RCC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/* Put the SYSCLK sources before the series header is included, as it
 * might need them. */
/**
 * @brief SYSCLK sources
 * @see rcc_switch_sysclk()
 */
typedef enum rcc_sysclk_src {
    RCC_CLKSRC_HSI = 0x0,
    RCC_CLKSRC_HSE = 0x1,
    RCC_CLKSRC_PLL = 0x2,
} rcc_sysclk_src;

/* Roger clark. Replaced with line below #include <series/rcc.h>*/
#include "stm32f1/include/series/rcc.h"

/* Note: Beyond the usual (registers, etc.), it's up to the series
 * header to define the following types:
 *
 * - enum rcc_clk: Available system and secondary clock sources,
 *   e.g. RCC_CLK_HSE, RCC_CLK_PLL, RCC_CLK_LSE.
 *
 *   Note that the inclusion of secondary clock sources (like LSI and
 *   LSE) makes enum rcc_clk different from the SYSCLK sources, which
 *   are defined in this header as enum rcc_sysclk_src.
 *
 *   IMPORTANT NOTE TO IMPLEMENTORS: If you are adding support for a
 *   new STM32 series, see the comment near rcc_clk_reg() in
 *   libmaple/rcc.c for information on how to choose these values so
 *   that rcc_turn_on_clk() etc. will work on your series.
 *
 * - enum rcc_clk_id: For each available peripheral. These are widely used
 *   as unique IDs (TODO extricate from RCC?). Peripherals which are
 *   common across STM32 series should use the same token for their
 *   rcc_clk_id in each series header.
 *
 * - enum rcc_clk_domain: For each clock domain. This is returned by
 *   rcc_dev_clk(). For instance, each AHB and APB is a clock domain.
 *
 * - enum rcc_prescaler: And a suitable set of dividers for
 *   rcc_set_prescaler().
 *
 * - enum rcc_pllsrc: For each PLL source. Same source, same token.
 *
 * - A target-dependent type to be pointed to by the data field in a
 *   struct rcc_pll_cfg.
 */

#ifdef __DOXYGEN__
/** RCC register map base pointer */
#define RCC_BASE
#endif

/* Clock prescaler management. */

/**
 * @brief Set the divider on a peripheral prescaler
 * @param prescaler prescaler to set
 * @param divider prescaler divider
 */
extern void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider);

/* SYSCLK. */

void rcc_switch_sysclk(rcc_sysclk_src sysclk_src);

/* PLL configuration */

/**
 * @brief Specifies a configuration for the main PLL.
 */
typedef struct rcc_pll_cfg {
    rcc_pllsrc  pllsrc;     /**< PLL source */

    /** Series-specific configuration data. */
    void       *data;
} rcc_pll_cfg;

/**
 * @brief Configure the main PLL.
 *
 * You may only call this function while the PLL is disabled.
 *
 * @param pll_cfg Desired PLL configuration. The contents of this
 *                struct depend entirely on the target.
 */
extern void rcc_configure_pll(rcc_pll_cfg *pll_cfg);

/* System and secondary clock sources. */

void rcc_turn_on_clk(rcc_clk clock);
void rcc_turn_off_clk(rcc_clk clock);
int rcc_is_clk_on(rcc_clk clock);
int rcc_is_clk_ready(rcc_clk clock);

/* Peripheral clock lines and clock domains. */

/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
extern void rcc_clk_enable(rcc_clk_id id);

/**
 * @brief Reset a peripheral.
 *
 * Caution: not all rcc_clk_id values refer to a peripheral which can
 * be reset. (Only rcc_clk_ids for peripherals with bits in an RCC
 * reset register can be used here.)
 *
 * @param id Clock ID of the peripheral to reset.
 */
extern void rcc_reset_dev(rcc_clk_id id);

rcc_clk_domain rcc_dev_clk(rcc_clk_id id);

/* Clock security system */

/**
 * @brief Enable the clock security system (CSS).
 */
static inline void rcc_enable_css() {
    RCC_BASE->CR |= RCC_CR_CSSON;
}

/**
 * @brief Disable the clock security system (CSS).
 */
static inline void rcc_disable_css() {
    RCC_BASE->CR &= ~RCC_CR_CSSON;
}

/**
 * @brief Turn off the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
extern void rcc_clk_disable(rcc_clk_id id);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
