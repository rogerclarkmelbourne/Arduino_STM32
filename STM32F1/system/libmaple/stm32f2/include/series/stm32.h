/******************************************************************************
 * The MIT License
 *
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
 * @file libmaple/stm32f2/include/series/stm32.h
 * @brief STM32F2 chip- and series-specific definitions.
 */

#ifndef _LIBMAPLE_STM32F2_STM32_H_
#define _LIBMAPLE_STM32F2_STM32_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Chip configuration
 */

#ifndef STM32_PCLK1
#define STM32_PCLK1                     30000000U
#endif

#ifndef STM32_PCLK2
#define STM32_PCLK2                     60000000U
#endif

#ifndef STM32_DELAY_US_MULT
#define STM32_DELAY_US_MULT             20 /* FIXME: dummy value. */
#endif

/*
 * Series- and MCU-specific values
 */

#define STM32_MCU_SERIES                STM32_SERIES_F2
#define STM32_NR_INTERRUPTS             81
#define STM32_HAVE_FSMC                 1
#define STM32_HAVE_USB                  1
#define STM32_HAVE_DAC                  1

#if defined(MCU_STM32F207IC) || defined(MCU_STM32F207IG)
#   define STM32_NR_GPIO_PORTS          9
#   define STM32_TIMER_MASK             0x7FFE /* TIMER1-TIMER14. */
#   define STM32_SRAM_END               ((void*)0x20020000)
#else
#warning "Unsupported or unspecified STM32F2 MCU."
#endif

#ifdef __cplusplus
}
#endif

#endif
