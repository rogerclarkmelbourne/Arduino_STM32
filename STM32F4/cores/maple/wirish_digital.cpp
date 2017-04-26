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

/*
 * Arduino-compatible digital I/O implementation.
 */

#include "wirish.h"


void pinMode(uint8 pin, WiringPinMode mode) {
    gpio_pin_mode outputMode;
    boolean pwm = false;

    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    switch(mode) {
    case OUTPUT:
        outputMode = GPIO_OUTPUT_PP;
        break;
    case OUTPUT_OPEN_DRAIN:
        outputMode = GPIO_OUTPUT_OD;
        break;
    case INPUT:
    case INPUT_FLOATING:
        outputMode = GPIO_INPUT_FLOATING;
        break;
    case INPUT_ANALOG:
        outputMode = GPIO_INPUT_ANALOG;
        break;
    case INPUT_PULLUP:
        outputMode = GPIO_INPUT_PU;
        break;
    case INPUT_PULLDOWN:
        outputMode = GPIO_INPUT_PD;
        break;
    case PWM:
        outputMode = GPIO_AF_OUTPUT_PP;
        pwm = true;
        break;
    case PWM_OPEN_DRAIN:
        outputMode = GPIO_AF_OUTPUT_OD;
        pwm = true;
        break;
    default:
        ASSERT(0);
        return;
    }

    gpio_set_mode(pin, outputMode);

    if (PIN_MAP[pin].timer_device != NULL) {
        /* Enable/disable timer channels if we're switching into or
         * out of PWM. */
        timer_set_mode(PIN_MAP[pin].timer_device,
                       PIN_MAP[pin].timer_channel,
                       pwm ? TIMER_PWM : TIMER_DISABLED);
    }
}


uint32 digitalRead(uint8 pin)
{
    if (pin >= BOARD_NR_GPIO_PINS) {
        return 0;
    }

    return gpio_read_pin(pin) ?
        HIGH : LOW;
}

void digitalWrite(uint8 pin, uint8 val)
{
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    gpio_write_pin(pin, val);
}

void togglePin(uint8 pin)
{
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    gpio_toggle_pin(pin);
}

#define BUTTON_DEBOUNCE_DELAY 1

uint8 isButtonPressed(uint8_t button) {
    if (digitalRead(button)) {
        delay(BUTTON_DEBOUNCE_DELAY);
        while (digitalRead(button))
            ;
        return true;
    }
    return false;
}

uint8 waitForButtonPress(uint8_t button, uint32 timeout) {
    uint32 start = millis();
    uint32 time;
    if (timeout == 0) {
        while (!isButtonPressed(button))
            ;
        return true;
    }
    do {
        time = millis();
        /* properly handle wrap-around */
        if ((start > time && time + (0xffffffffU - start) > timeout) ||
            time - start > timeout) {
            return false;
        }
    } while (!isButtonPressed(button));
    return true;
}
