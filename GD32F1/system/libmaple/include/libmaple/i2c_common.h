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
 * @file libmaple/include/libmaple/i2c_common.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief This file is an implementation detail
 *
 * CONTENTS UNSTABLE. The existence of this file is an implementation
 * detail.  Never include it directly.  If you need something from
 * here, include <libmaple/i2c.h> instead.
 */

#ifndef _LIBMAPLE_I2C_COMMON_H_
#define _LIBMAPLE_I2C_COMMON_H_

#include <libmaple/libmaple_types.h>
#include <libmaple/nvic.h>
#include <libmaple/rcc.h>

struct gpio_dev;
struct i2c_reg_map;
struct i2c_msg;

/** I2C device states */
typedef enum i2c_state {
    I2C_STATE_DISABLED          = 0, /**< Disabled */
    I2C_STATE_IDLE              = 1, /**< Idle */
    I2C_STATE_XFER_DONE         = 2, /**< Done with transfer */
    I2C_STATE_BUSY              = 3, /**< Busy */
    I2C_STATE_ERROR             = -1 /**< Error occurred */
} i2c_state;

/**
 * @brief I2C device type.
 */
typedef struct i2c_dev {
    struct i2c_reg_map *regs;   /**< Register map */
    struct i2c_msg *msg;        /**< Messages */
    uint32 error_flags;         /**< Error flags, set on I2C error condition */
    volatile uint32 timestamp;  /**< For internal use */

    /**
     * @brief Deprecated. Use .scl_port or .sda_port instead.
     * If non-null, this will be used as SDA, SCL pins' GPIO port. If
     * null, then .sda_port will be used for SDA, and .sda_port for
     * SDA. */
    struct gpio_dev *gpio_port;

    /**
     * @brief SDA GPIO device (but see .gpio_port).
     */
    struct gpio_dev *sda_port;

    /**
     * @brief SCL GPIO device (but see .gpio_port).
     */
    struct gpio_dev *scl_port;

    uint16 msgs_left;           /**< Messages left */
    uint8 sda_pin;              /**< SDA bit on gpio_port */
    uint8 scl_pin;              /**< SCL bit on gpio_port */
    rcc_clk_id clk_id;          /**< RCC clock information */
    nvic_irq_num ev_nvic_line;  /**< Event IRQ number */
    nvic_irq_num er_nvic_line;  /**< Error IRQ number */
    volatile i2c_state state;   /**< Device state */
} i2c_dev;

#endif
