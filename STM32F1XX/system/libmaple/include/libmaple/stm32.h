/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010, 2011, 2012 LeafLabs, LLC.
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
 * @file libmaple/include/libmaple/stm32.h
 * @brief STM32 chip header
 *
 * This header supplies various chip-specific values for the current
 * build target. It's useful both to abstract away hardware details
 * (e.g. through use of STM32_NR_INTERRUPTS) and to decide what to do
 * when you want something nonportable (e.g. by checking
 * STM32_MCU_SERIES).
 */

#ifndef _LIBMAPLE_STM32_H_
#define _LIBMAPLE_STM32_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * STM32 series identifiers.
 *
 * Don't make these into an enum; the preprocessor needs them.
 */

/** STM32F1 series. */
#define STM32_SERIES_F1 0
/** STM32F2 series. */
#define STM32_SERIES_F2 1
/** STM32L1 series. */
#define STM32_SERIES_L1 2
/** STM32F4 series. */
#define STM32_SERIES_F4 3

/* The series header is responsible for defining:
 *
 * - Everything in the following __DOXYGEN__ conditional block.
 *
 * - STM32_HAVE_FSMC: 1 if the MCU has the FSMC peripheral, and 0
 *   otherwise.
 *
 * - STM32_HAVE_USB: 1 if the MCU has a USB peripheral, and 0
 *   otherwise.
 */
 /* roger clark. replaced with line below  #include <series/stm32.h> */
#include "stm32f1/include/series/stm32.h"

/* Ensure the series header isn't broken. */
#if (!defined(STM32_PCLK1)         ||     \
     !defined(STM32_PCLK2)         ||     \
     !defined(STM32_MCU_SERIES)    ||     \
     !defined(STM32_NR_INTERRUPTS) ||     \
     !defined(STM32_NR_GPIO_PORTS) ||     \
     !defined(STM32_TIMER_MASK)    ||     \
     !defined(STM32_DELAY_US_MULT) ||     \
     !defined(STM32_SRAM_END)      ||     \
     !defined(STM32_HAVE_DAC)      ||     \
     !defined(STM32_HAVE_FSMC)     ||     \
     !defined(STM32_HAVE_USB))
#error "Bad STM32F1 configuration. Check <series/stm32.h> header for your MCU."
#endif

/*
 * Derived macros
 */

/* FIXME [0.0.13] add this to ReST API page */
/**
 * @brief Statically determine whether a timer is present.
 *
 * Given a constant timer number n (starting from 1), this macro has a
 * nonzero value exactly when TIMERn is available.
 */
#define STM32_HAVE_TIMER(n) (STM32_TIMER_MASK & (1 << (n)))

/*
 * Doxygen for functionality provided by series header.
 */

#ifdef __DOXYGEN__

/*
 * Clock configuration.
 *
 * These defines depend upon how the MCU is configured.  Because of
 * the potential for a mismatch between them and the actual clock
 * configuration, keep their number to a minimum.
 */

/**
 * @brief APB1 clock speed, in Hz.
 */
#define STM32_PCLK1

/**
 *  @brief APB2 clock speed, in Hz.
 */
#define STM32_PCLK2

/** @brief Deprecated. Use STM32_PCLK1 instead. */
#define PCLK1
/** @brief Deprecated. Use STM32_PCLK2 instead. */
#define PCLK2

/*
 * Series- and MCU-specific values.
 */

/**
 * @brief STM32 series value for the MCU being targeted.
 *
 * At time of writing, allowed values are: STM32_SERIES_F1,
 * STM32_SERIES_F2. This set of values will expand as libmaple adds
 * support for more STM32 series MCUs.
 */
#define STM32_MCU_SERIES

/**
 * @brief Number of interrupts in the vector table.
 *
 * This does not include Cortex-M interrupts (NMI, HardFault, etc.).
 */
#define STM32_NR_INTERRUPTS

/**
 * Number of GPIO ports.
 */
#define STM32_NR_GPIO_PORTS

/* FIXME [0.0.13] add this to ReST API page */
/**
 * @brief Bitmask of timers available on the MCU.
 *
 * That is, if TIMERn is available, then STM32_TIMER_MASK & (1 << n)
 * will be nonzero. For example, a nonzero value of "STM32_TIMER_MASK
 * & 0x2" means TIMER1 is available.
 *
 * A bitmask is necessary as some STM32 MCUs have "holes" in the range
 * of available timers.
 */
#define STM32_TIMER_MASK

/**
 * @brief Multiplier to convert microseconds into loop iterations
 *        in delay_us().
 *
 * @see delay_us()
 */
#define STM32_DELAY_US_MULT

/**
 * @brief Pointer to end of built-in SRAM.
 *
 * Points to the address which is 1 byte past the last valid
 * SRAM address.
 */
#define STM32_SRAM_END

/**
 * @brief 1 if the target MCU has a DAC, and 0 otherwise.
 */
#define STM32_HAVE_DAC

/**
 * @brief 1 if the target MCU has the FSMC peripheral, and 0 otherwise.
 *
 * Note that the feature set of the FSMC peripheral is restricted on
 * some MCUs.
 */
#define STM32_HAVE_FSMC

/**
 * @brief 1 if the target MCU has a USB peripheral, and 0 otherwise.
 *
 * Note that a variety of USB peripherals are available across the
 * different series, with widely varying feature sets and programming
 * interfaces. This macro will be 1 if any such peripheral is present.
 */
#define STM32_HAVE_USB

#endif  /* __DOXYGEN__ */

/*
 * The following are for backwards compatibility only.
 */

/* PCLK1 and PCLK2 are for backwards compatibility only; don't use in
 * new code. */
#ifndef PCLK1
#define PCLK1 STM32_PCLK1
#endif
#if PCLK1 != STM32_PCLK1
#error "PCLK1 (which is deprecated) differs from STM32_PCLK1."
#endif
#ifndef PCLK2
#define PCLK2 STM32_PCLK2
#endif
#if PCLK2 != STM32_PCLK2
#error "PCLK2 (which is deprecated) differs from STM32_PCLK2."
#endif

/** @brief Deprecated. Use STM32_NR_INTERRUPTS instead. */
#define NR_INTERRUPTS                   STM32_NR_INTERRUPTS
/** @brief Deprecated. Use STM32_NR_GPIO_PORTS instead. */
#define NR_GPIO_PORTS                   STM32_NR_GPIO_PORTS
/** @brief Deprecated. Use STM32_DELAY_US_MULT instead. */
#define DELAY_US_MULT                   STM32_DELAY_US_MULT

#ifdef __cplusplus
}
#endif

#endif
