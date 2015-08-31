/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file libmaple/flash.c
 * @brief Flash management functions
 */

#include <libmaple/libmaple_types.h>
#include <libmaple/flash.h>

/**
 * @brief Set flash wait states
 *
 * Note that not all wait states are available on every MCU. See the
 * Flash programming manual for your MCU for restrictions on the
 * allowed value of wait_states for a given system clock (SYSCLK)
 * frequency.
 *
 * @param wait_states number of wait states (one of
 *                    FLASH_WAIT_STATE_0, FLASH_WAIT_STATE_1,
 *                    ..., FLASH_WAIT_STATE_7).
 */
void flash_set_latency(uint32 wait_states) {
    uint32 val = FLASH_BASE->ACR;

    val &= ~FLASH_ACR_LATENCY;
    val |= wait_states;

    FLASH_BASE->ACR = val;
}
