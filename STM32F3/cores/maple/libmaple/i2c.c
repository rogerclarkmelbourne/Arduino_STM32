/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/i2c.c
 * @author	Perry Hung <perry@leaflabs.com>,
 *        	F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief Inter-Integrated Circuit (I2C) support.
 *
 * Currently, only master mode is supported.
 */

#include "i2c_private.h"

#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>
#include <libmaple/gpio.h>
#include <libmaple/nvic.h>
#include <libmaple/i2c.h>
#include <libmaple/systick.h>

#include <string.h>

void i2c_init(i2c_dev *dev) {
    rcc_reset_dev(dev->clk_id);
    rcc_clk_enable(dev->clk_id);
}
