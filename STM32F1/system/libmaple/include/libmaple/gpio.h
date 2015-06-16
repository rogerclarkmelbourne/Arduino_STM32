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
 * @file libmaple/include/libmaple/gpio.h
 * @brief General Purpose I/O (GPIO) interace.
 */

#ifndef _LIBMAPLE_GPIO_H_
#define _LIBMAPLE_GPIO_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Note: Series header must define:
 * - enum gpio_pin_mode (TODO think harder about portability here)
 */

// roger clark. replaced with line below #include <series/gpio.h>
#include "stm32f1/include/series/gpio.h"
#include <libmaple/libmaple_types.h>
#include <libmaple/rcc.h>
#include <libmaple/exti.h>

/*
 * Device type
 */

/** GPIO device type */
typedef struct gpio_dev {
    gpio_reg_map *regs;         /**< Register map */
    rcc_clk_id    clk_id;       /**< RCC clock information */
    /**
     * @brief (Deprecated) External interrupt port.
     * Instead of dev->exti_port, use gpio_exti_port(dev).
     */
    exti_cfg      exti_port;
} gpio_dev;

/*
 * Portable routines
 */

void gpio_init(gpio_dev *dev);
void gpio_init_all(void);
/* TODO flags argument version? */
void gpio_set_mode(gpio_dev *dev, uint8 pin, gpio_pin_mode mode);
gpio_pin_mode gpio_get_mode(gpio_dev *dev, uint8 pin);
/**
 * @brief Get a GPIO port's corresponding EXTI port configuration.
 * @param dev GPIO port whose exti_cfg to return.
 */
static inline exti_cfg gpio_exti_port(gpio_dev *dev) {
    return (exti_cfg)(EXTI_PA + (dev->clk_id - RCC_GPIOA));
}

/**
 * Set or reset a GPIO pin.
 *
 * Pin must have previously been configured to output mode.
 *
 * @param dev GPIO device whose pin to set.
 * @param pin Pin on to set or reset
 * @param val If true, set the pin.  If false, reset the pin.
 */
static inline void gpio_write_bit(gpio_dev *dev, uint8 pin, uint8 val) {
    val = !val;          /* "set" bits are lower than "reset" bits  */
    dev->regs->BSRR = (1U << pin) << (16 * val);
}

/**
 * Determine whether or not a GPIO pin is set.
 *
 * Pin must have previously been configured to input mode.
 *
 * @param dev GPIO device whose pin to test.
 * @param pin Pin on dev to test.
 * @return True if the pin is set, false otherwise.
 */
static inline uint32 gpio_read_bit(gpio_dev *dev, uint8 pin) {
    return dev->regs->IDR & (1U << pin);
}

/**
 * Toggle a pin configured as output push-pull.
 * @param dev GPIO device.
 * @param pin Pin on dev to toggle.
 */
static inline void gpio_toggle_bit(gpio_dev *dev, uint8 pin) {
    dev->regs->ODR = dev->regs->ODR ^ (1U << pin);
}

#ifdef __cplusplus
}
#endif

#endif
