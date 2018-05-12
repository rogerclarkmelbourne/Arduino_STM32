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
 * @file libmaple/stm32f1/i2c.c
 * @brief STM32F1 I2C support
 */

#include "i2c_private.h"
#include <libmaple/i2c.h>

/*
 * Devices
 */

static i2c_dev i2c1 = I2C_DEV_OLD(1, &gpiob, 7, 6);
static i2c_dev i2c2 = I2C_DEV_OLD(2, &gpiob, 11, 10);

/** STM32F1 I2C device 1 */
i2c_dev* const I2C1 = &i2c1;
/** STM32F1 I2C device 2 */
i2c_dev* const I2C2 = &i2c2;

/*
 * Routines
 */

static int i2c1_wants_remap(const i2c_dev *dev) {
    /* Check if we've got I2C1 configured for SDA/SCL remap on PB9/PB8 */
    return (dev->clk_id == RCC_I2C1) &&
        (scl_port(dev)->clk_id == RCC_GPIOB) &&
        (sda_port(dev)->clk_id == RCC_GPIOB) &&
        (dev->sda_pin == 9) &&
        (dev->scl_pin == 8);
}

void i2c_config_gpios(const i2c_dev *dev) {
    if (i2c1_wants_remap(dev)) {
        afio_remap(AFIO_REMAP_I2C1);
    }
    gpio_set_mode(sda_port(dev), dev->sda_pin, GPIO_AF_OUTPUT_OD);
    gpio_set_mode(scl_port(dev), dev->scl_pin, GPIO_AF_OUTPUT_OD);
}

void i2c_master_release_bus(const i2c_dev *dev) {
    gpio_write_bit(scl_port(dev), dev->scl_pin, 1);
    gpio_write_bit(sda_port(dev), dev->sda_pin, 1);
    gpio_set_mode(scl_port(dev), dev->scl_pin, GPIO_OUTPUT_OD);
    gpio_set_mode(sda_port(dev), dev->sda_pin, GPIO_OUTPUT_OD);
}

/*
 * IRQ handlers
 */

__weak void __irq_i2c1_ev(void) {
   _i2c_irq_handler(I2C1);
}

__weak void __irq_i2c2_ev(void) {
   _i2c_irq_handler(I2C2);
}

__weak void __irq_i2c1_er(void) {
    _i2c_irq_error_handler(I2C1);
}

__weak void __irq_i2c2_er(void) {
    _i2c_irq_error_handler(I2C2);
}

/*
 * Internal APIs
 */

void _i2c_irq_priority_fixup(i2c_dev *dev) {
    /*
     * Important STM32 Errata:
     *
     * See STM32F10xx8 and STM32F10xxB Errata sheet (Doc ID 14574 Rev 8),
     * Section 2.11.1, 2.11.2.
     *
     * 2.11.1:
     * When the EV7, EV7_1, EV6_1, EV6_3, EV2, EV8, and EV3 events are not
     * managed before the current byte is being transferred, problems may be
     * encountered such as receiving an extra byte, reading the same data twice
     * or missing data.
     *
     * 2.11.2:
     * In Master Receiver mode, when closing the communication using
     * method 2, the content of the last read data can be corrupted.
     *
     * If the user software is not able to read the data N-1 before the STOP
     * condition is generated on the bus, the content of the shift register
     * (data N) will be corrupted. (data N is shifted 1-bit to the left).
     *
     * ----------------------------------------------------------------------
     *
     * In order to ensure that events are not missed, the i2c interrupt must
     * not be preempted. We set the i2c interrupt priority to be the highest
     * interrupt in the system (priority level 0). All other interrupts have
     * been initialized to priority level 16. See nvic_init().
     */
    nvic_irq_set_priority(dev->ev_nvic_line, 0);
    nvic_irq_set_priority(dev->er_nvic_line, 0);
}
