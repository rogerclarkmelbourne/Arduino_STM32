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
 * @file libmaple/stm32f2/fsmc.c
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief STM32F2 FSMC support.
 */

#include <libmaple/fsmc.h>
#include <libmaple/gpio.h>

#define CONFIG_GPIO(dev, bit)                                           \
    do {                                                                \
        gpio_set_modef(dev, bit, GPIO_MODE_AF, GPIO_MODEF_SPEED_HIGH);  \
        gpio_set_af(dev, bit, GPIO_AF_FSMC_SDIO_OTG_FS);                \
    } while (0)
void fsmc_sram_init_gpios(void) {
    /* Data lines... */
    CONFIG_GPIO(GPIOD,  0);
    CONFIG_GPIO(GPIOD,  1);
    CONFIG_GPIO(GPIOD,  8);
    CONFIG_GPIO(GPIOD,  9);
    CONFIG_GPIO(GPIOD, 10);
    CONFIG_GPIO(GPIOD, 14);
    CONFIG_GPIO(GPIOD, 15);
    CONFIG_GPIO(GPIOE,  7);
    CONFIG_GPIO(GPIOE,  8);
    CONFIG_GPIO(GPIOE,  9);
    CONFIG_GPIO(GPIOE, 10);
    CONFIG_GPIO(GPIOE, 11);
    CONFIG_GPIO(GPIOE, 12);
    CONFIG_GPIO(GPIOE, 13);
    CONFIG_GPIO(GPIOE, 14);
    CONFIG_GPIO(GPIOE, 15);

    /* Address lines... */
    CONFIG_GPIO(GPIOD, 11);
    CONFIG_GPIO(GPIOD, 12);
    CONFIG_GPIO(GPIOD, 13);
    CONFIG_GPIO(GPIOF,  0);
    CONFIG_GPIO(GPIOF,  1);
    CONFIG_GPIO(GPIOF,  2);
    CONFIG_GPIO(GPIOF,  3);
    CONFIG_GPIO(GPIOF,  4);
    CONFIG_GPIO(GPIOF,  5);
    CONFIG_GPIO(GPIOF, 12);
    CONFIG_GPIO(GPIOF, 13);
    CONFIG_GPIO(GPIOF, 14);
    CONFIG_GPIO(GPIOF, 15);
    CONFIG_GPIO(GPIOG,  0);
    CONFIG_GPIO(GPIOG,  1);
    CONFIG_GPIO(GPIOG,  2);
    CONFIG_GPIO(GPIOG,  3);
    CONFIG_GPIO(GPIOG,  4);
    CONFIG_GPIO(GPIOG,  5);

    /* And control lines... */
    CONFIG_GPIO(GPIOD,  4);
    CONFIG_GPIO(GPIOD,  5);
    CONFIG_GPIO(GPIOD,  7);
    CONFIG_GPIO(GPIOG,  9);
    CONFIG_GPIO(GPIOG, 10);
    CONFIG_GPIO(GPIOG, 12);
    CONFIG_GPIO(GPIOE,  0);
    CONFIG_GPIO(GPIOE,  1);
}
