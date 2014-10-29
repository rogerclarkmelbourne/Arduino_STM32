/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f1/fsmc.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Bryan Newbold <bnewbold@robocracy.org>
 * @brief STM32F1 FSMC support.
 */

#include <libmaple/stm32.h>

#if STM32_HAVE_FSMC /* Don't try building the rest for MCUs without FSMC */

#include <libmaple/fsmc.h>
#include <libmaple/gpio.h>

void fsmc_sram_init_gpios(void) {
    /* Data lines... */
    gpio_set_mode(GPIOD,  0, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD,  1, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD,  8, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD,  9, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD, 10, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD, 14, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD, 15, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE,  7, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE,  8, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE,  9, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 10, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 11, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 12, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 13, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 14, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOE, 15, GPIO_AF_OUTPUT_PP);

    /* Address lines... */
    gpio_set_mode(GPIOD, 11, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD, 12, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOD, 13, GPIO_AF_OUTPUT_PP);
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

    /* And control lines... */
    gpio_set_mode(GPIOD,  4, GPIO_AF_OUTPUT_PP);   // NOE
    gpio_set_mode(GPIOD,  5, GPIO_AF_OUTPUT_PP);   // NWE

    gpio_set_mode(GPIOD,  7, GPIO_AF_OUTPUT_PP);   // NE1
    gpio_set_mode(GPIOG,  9, GPIO_AF_OUTPUT_PP);   // NE2
    gpio_set_mode(GPIOG, 10, GPIO_AF_OUTPUT_PP);   // NE3
    gpio_set_mode(GPIOG, 12, GPIO_AF_OUTPUT_PP);   // NE4

    gpio_set_mode(GPIOE,  0, GPIO_AF_OUTPUT_PP);   // NBL0
    gpio_set_mode(GPIOE,  1, GPIO_AF_OUTPUT_PP);   // NBL1
}

#endif  /* STM32_HAVE_FSMC */
