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
 *  @brief Arduino-style PWM implementation.
 */

#include <libmaple/libmaple_types.h>
#include <libmaple/timer.h>

#include "boards.h"
#include "pwm.h"

void pwmWrite(uint8 pin, uint16 duty_cycle) {
	if ( pin >= BOARD_NR_GPIO_PINS ) return;
	const timer_info * t_info = &timer_map[pin];
	uint8_t index = t_info->index;
    const timer_dev *dev = timer_devices[index];
	
    if (index==NO_TIM || index>=LAST_TIM || dev == NULL || dev->type == TIMER_BASIC) {
        return;
    }

    timer_set_compare(dev, t_info->channel, duty_cycle);
}
