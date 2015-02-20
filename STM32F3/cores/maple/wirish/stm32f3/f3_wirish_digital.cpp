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

/*
 * STM32F3 implementations for basic GPIO functionality.
 */

#include <wirish/io.h>

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

#include <wirish/boards.h>

void pinMode(uint8 pin, WiringPinMode w_mode) {
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    gpio_pin_mode mode;
    // People always do the silly pin-toggle speed benchmark, so let's
    // accomodate them:
    unsigned flags = GPIO_MODEF_SPEED_HIGH;
    bool pwm = false;
    switch(w_mode) {
    case OUTPUT:
        mode = GPIO_MODE_OUTPUT;
        break;
    case OUTPUT_OPEN_DRAIN:
        mode = GPIO_MODE_OUTPUT;
        flags |= GPIO_MODEF_TYPE_OD;
        break;
    case INPUT:
    case INPUT_FLOATING:
        mode = GPIO_MODE_INPUT;
        break;
    case INPUT_ANALOG:
        mode = GPIO_MODE_ANALOG;
        break;
    case INPUT_PULLUP:
        mode = GPIO_MODE_INPUT;
        flags |= GPIO_MODEF_PUPD_PU;
        break;
    case INPUT_PULLDOWN:
        mode = GPIO_MODE_INPUT;
        flags |= GPIO_MODEF_PUPD_PD;
        break;
    case PWM:
        mode = GPIO_MODE_AF;
        pwm = true;
        break;
    case PWM_OPEN_DRAIN:
        mode = GPIO_MODE_AF;
        flags |= GPIO_MODEF_TYPE_OD;
        pwm = true;
        break;
    default:
        ASSERT(0);              // Can't happen
        return;
    }

    const stm32_pin_info *info = &PIN_MAP[pin];

    if (pwm) {
        /* If enabling PWM, tell the timer to do PWM, and tell the pin
         * to listen to the right timer. */
        ASSERT(info->timer_device != NULL);
        if (info->timer_device == NULL) {
            return;
        }
        gpio_af timer_af = timer_get_af(info->timer_device);
        timer_set_mode(info->timer_device, info->timer_channel, TIMER_PWM);
        gpio_set_af(info->gpio_device, info->gpio_bit, timer_af);
    }
    gpio_set_modef(info->gpio_device, info->gpio_bit, mode, flags);
}
