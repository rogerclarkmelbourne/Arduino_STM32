/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung (from <libmaple/i2c.h>).
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
 * @file libmaple/stm32f1/include/series/i2c.h
 * @brief STM32F1 I2C
 */

#ifndef _LIBMAPLE_STM32F1_I2C_H_
#define _LIBMAPLE_STM32F1_I2C_H_

#include <libmaple/i2c_common.h>
#include <libmaple/gpio.h>
#include <libmaple/stm32.h>

/*
 * Register maps
 */

struct i2c_reg_map;

/** STM32F1 I2C1 register map base pointer */
#define I2C1_BASE               ((struct i2c_reg_map*)0x40005400)
/** STM32F1 I2C2 register map base pointer */
#define I2C2_BASE               ((struct i2c_reg_map*)0x40005800)

/*
 * Devices
 */

extern i2c_dev* const I2C1;
extern i2c_dev* const I2C2;

/*
 * For internal use
 */

static inline uint32 _i2c_bus_clk(i2c_dev *dev __attribute__((unused))) {
    /* Both I2C peripherals are on APB1 */
    return STM32_PCLK1 / (1000 * 1000);
}

#define _I2C_HAVE_IRQ_FIXUP 1
void _i2c_irq_priority_fixup(i2c_dev *dev);

/*
 * Deprecated functionality
 */

/* Flag to use alternate pin mapping in i2c_master_enable(). */
#define _I2C_HAVE_DEPRECATED_I2C_REMAP 1
#define I2C_REMAP 0x4
static inline void _i2c_handle_remap(i2c_dev *dev, uint32 flags) {
    if ((dev == I2C1) && (flags & I2C_REMAP)) {
        afio_remap(AFIO_REMAP_I2C1);
        I2C1->sda_pin = 9;
        I2C1->scl_pin = 8;
    }
}

#endif  /* _LIBMAPLE_STM32F1_I2C_H_ */
