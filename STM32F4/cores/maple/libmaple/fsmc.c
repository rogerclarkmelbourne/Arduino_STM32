/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
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
 * @file fsmc.c
 * @brief Flexible static memory controller support.
 */

#include "fsmc.h"
#include "gpio.h"

#ifdef STM32_HIGH_DENSITY

/**
 * Configure FSMC GPIOs for use with SRAM.
 */
void fsmc_sram_init_gpios(void) {
    /* Data lines... */
    gpio_set_mode(PD0, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD1, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD8, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD9, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD10, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD14, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD15, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE7, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE8, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE9, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE10, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE11, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE12, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE13, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE14, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PE15, GPIO_AF_OUTPUT_PP);

    /* Address lines... */
    gpio_set_mode(PD11, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD12, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(PD13, GPIO_AF_OUTPUT_PP);
#if 0 // not available on LQFP package
    gpio_set_mode(GPIOF,  0, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF,  1, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF,  2, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF,  3, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF,  4, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF,  5, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF, 12, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF, 13, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF, 14, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOF, 15, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  0, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  1, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  2, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  3, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  4, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOG,  5, GPIO_AF_OUTPUT_PP);
#endif  // not available on LQFP package
    /* And control lines... */
    gpio_set_mode(PD4, GPIO_AF_OUTPUT_PP);   // NOE
    gpio_set_mode(PD5, GPIO_AF_OUTPUT_PP);   // NWE

    gpio_set_mode(PD7, GPIO_AF_OUTPUT_PP);   // NE1
#if 0 // not available on LQFP package
    gpio_set_mode(GPIOG,  9, GPIO_AF_OUTPUT_PP);   // NE2
    gpio_set_mode(GPIOG, 10, GPIO_AF_OUTPUT_PP);   // NE3
    gpio_set_mode(GPIOG, 12, GPIO_AF_OUTPUT_PP);   // NE4
#endif  // not available on LQFP package

    gpio_set_mode(PE0, GPIO_AF_OUTPUT_PP);   // NBL0
    gpio_set_mode(PE1, GPIO_AF_OUTPUT_PP);   // NBL1
}

#endif  /* STM32_HIGH_DENSITY */
