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
 * @file wirish/stm32f3/wirish_debug.cpp
 * @brief High level debug port configuration
 */

#include <wirish/boards.h>
#include <wirish/wirish_debug.h>
#include <libmaple/gpio.h>

void disableDebugPorts(void) {
    // BOARD_JTMS_SWDIO_PIN-BOARD_JTDI_PIN and BOARD_JTDO_PIN-BOARD_NJTRST_PIN are in system alternate function mode on
    // reset, to support JTAG. Just put them in input mode to release
    // them.
    gpio_set_modef(PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_device, PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_bit,
			GPIO_MODE_INPUT, GPIO_MODEF_PUPD_NONE);
    gpio_set_modef(PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_device, PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_bit,
			GPIO_MODE_INPUT, GPIO_MODEF_PUPD_NONE);
    gpio_set_modef(PIN_MAP[BOARD_JTDI_PIN].gpio_device, PIN_MAP[BOARD_JTDI_PIN].gpio_bit,
			GPIO_MODE_INPUT, GPIO_MODEF_PUPD_NONE);
    gpio_set_modef(PIN_MAP[BOARD_JTDO_PIN].gpio_device, PIN_MAP[BOARD_JTDO_PIN].gpio_bit,
			GPIO_MODE_INPUT, GPIO_MODEF_PUPD_NONE);
    gpio_set_modef(PIN_MAP[BOARD_NJTRST_PIN].gpio_device, PIN_MAP[BOARD_NJTRST_PIN].gpio_bit,
			GPIO_MODE_INPUT, GPIO_MODEF_PUPD_NONE);
}

void enableDebugPorts(void) {
    // Put BOARD_JTMS_SWDIO_PIN-BOARD_JTDI_PIN and BOARD_JTDO_PIN-BOARD_NJTRST_PIN back in system AF mode.
    gpio_set_modef(PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_device, PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_bit,
			GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
    gpio_set_modef(PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_device, PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_bit,
			GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
    gpio_set_modef(PIN_MAP[BOARD_JTDI_PIN].gpio_device, PIN_MAP[BOARD_JTDI_PIN].gpio_bit,
			GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
    gpio_set_modef(PIN_MAP[BOARD_JTDO_PIN].gpio_device, PIN_MAP[BOARD_JTDO_PIN].gpio_bit,
			GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);
    gpio_set_modef(PIN_MAP[BOARD_NJTRST_PIN].gpio_device, PIN_MAP[BOARD_NJTRST_PIN].gpio_bit,
			GPIO_MODE_AF, GPIO_MODEF_TYPE_PP);

    gpio_set_af(PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_device, PIN_MAP[BOARD_JTMS_SWDIO_PIN].gpio_bit, GPIO_AF_0);
    gpio_set_af(PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_device, PIN_MAP[BOARD_JTCK_SWCLK_PIN].gpio_bit, GPIO_AF_0);
    gpio_set_af(PIN_MAP[BOARD_JTDI_PIN].gpio_device, PIN_MAP[BOARD_JTDI_PIN].gpio_bit, GPIO_AF_0);
    gpio_set_af(PIN_MAP[BOARD_JTDO_PIN].gpio_device, PIN_MAP[BOARD_JTDO_PIN].gpio_bit, GPIO_AF_0);
    gpio_set_af(PIN_MAP[BOARD_NJTRST_PIN].gpio_device, PIN_MAP[BOARD_NJTRST_PIN].gpio_bit, GPIO_AF_0);
}
