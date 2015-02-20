/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010, 2011 LeafLabs, LLC.
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
 * @file libmaple/stm32f1/include/series/stm32.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 chip- and series-specific definitions.
 */

#ifndef _LIBMAPLE_STM32F3_H_
#define _LIBMAPLE_STM32F3_H_

#ifdef __cplusplus
extern "C" {
#endif

#define __CCM__ __attribute__((section(".CCM")))

#define STM32_MCU_SERIES                STM32_SERIES_F3

/* The STM32F3 series is subdivided into "lines". libmaple currently
 * officially supports STM32F303 performance line MCUs (see the
 * MCU-specific value section below).
 *
 * You can use these F3 line defines if porting libmaple to support
 * MCUs on other lines. */
/** STM32F3 302 line (STM32F302 MCUs). */
#define STM32_F3_LINE_302					0
/** STM32F3 303 line (STM32F303 MCUs). */
#define STM32_F3_LINE_303					1

/*
 * MCU-specific values.
 *
 * You can use this section to override any of the below settings on a
 * per-MCU basis. For example, if your MCU has different STM32_PCLK1
 * or STM32_PCLK2 values, you can set them here and the values for
 * STM32F303 microcontrollers set below won't take effect.
 */

#if defined(MCU_STM32F302CB)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          20
#   define STM32_SRAM_END               ((void*)0x20008000)
#   define STM32_MEDIUM_DENSITY // 48pin package

#elif defined(MCU_STM32F302RB)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          27
#   define STM32_SRAM_END               ((void*)0x20008000)
#   define STM32_HIGH_DENSITY // 64pin package

#elif defined(MCU_STM32F302VB)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          45
#   define STM32_SRAM_END               ((void*)0x20008000)
#   define STM32_XL_DENSITY // 100pin package

#elif defined(MCU_STM32F302CC)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          20
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_MEDIUM_DENSITY // 48pin package

#elif defined(MCU_STM32F302RC)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          27
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_HIGH_DENSITY // 64pin package

#elif defined(MCU_STM32F302VC)
#   define STM32_F3_LINE                STM32_F3_LINE_302
#   define STM32_NR_GPIO_PORTS          45
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_XL_DENSITY // 100pin package

#elif defined(MCU_STM32F303CB)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          20
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_MEDIUM_DENSITY // 48pin package

#elif defined(MCU_STM32F303RB)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          27
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_HIGH_DENSITY // 64pin package

#elif defined(MCU_STM32F303VB)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          45
#   define STM32_SRAM_END               ((void*)0x2000A000)
#   define STM32_XL_DENSITY // 100pin package

#elif defined(MCU_STM32F303CC)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          20
#   define STM32_SRAM_END               ((void*)0x2000C000)
#   define STM32_MEDIUM_DENSITY // 48pin package

#elif defined(MCU_STM32F303RC)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          27
#   define STM32_SRAM_END               ((void*)0x2000C000)
#   define STM32_HIGH_DENSITY // 64pin package

#elif defined(MCU_STM32F303VC)
#   define STM32_F3_LINE                STM32_F3_LINE_303
#   define STM32_NR_GPIO_PORTS          45
#   define STM32_SRAM_END               ((void*)0x2000C000)
// #   define STM32_XL_DENSITY // 100pin package

#else
#warning "Unsupported or unspecified STM32F3 MCU."
#endif

/*
 * Derived values.
 */

#if STM32_F3_LINE == STM32_F3_LINE_302
#    define STM32_HAVE_USB              1

#    ifdef STM32_MEDIUM_DENSITY
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000001011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    elif defined(STM32_HIGH_DENSITY)
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000001011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    elif defined(STM32_XL_DENSITY)
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000001011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    endif

#elif STM32_F3_LINE == STM32_F3_LINE_303
#    define STM32_HAVE_USB              1

#    ifdef STM32_MEDIUM_DENSITY
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000111011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    elif defined(STM32_HIGH_DENSITY)
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000111011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    elif defined(STM32_XL_DENSITY)
#       define STM32_NR_INTERRUPTS      82
#       define STM32_TIMER_MASK         0b111000000111011110
#       define STM32_HAVE_FSMC          0
#       define STM32_HAVE_DAC           1
#    endif

#endif

/*
 * Clock configuration.
 *
 * You can patch these for your line, MCU, clock configuration,
 * etc. here or by setting cflags when compiling libmaple.
 */

#ifndef STM32_PCLK1
#define STM32_PCLK1                     36000000U
#endif

#ifndef STM32_PCLK2
#define STM32_PCLK2                     72000000U
#endif

#ifndef STM32_DELAY_US_MULT
#define STM32_DELAY_US_MULT             12 /* FIXME: value is incorrect. */
#endif

/*
 * Sanity checks.
 *
 * Make sure we have the F3-specific defines we need.
 * <libmaple/stm32.h> will check that we've defined everything it needs.
 */

#if !defined(STM32_F3_LINE)
#error "Bad STM32F3 configuration. Check STM32F3 <series/stm32.h> header."
#endif

/*
 * Doxygen
 */

#ifdef __DOXYGEN__

/**
 * @brief STM32 line value for the STM32F3 MCU being targeted.
 *
 * At time of writing, allowed values are: STM32_F3_LINE_303,
 * STM32_F3_LINE_302. This set of values may expand as libmaple adds
 * support for more STM32F3 lines.
 */
#define STM32_F3_LINE

#endif /* __DOXYGEN__ */

#ifdef __cplusplus
}
#endif

#endif
