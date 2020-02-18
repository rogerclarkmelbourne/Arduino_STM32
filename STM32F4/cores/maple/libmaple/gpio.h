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
 *  @file gpio.h
 *
 *  @brief General purpose I/O (GPIO) and Alternate Function I/O
 *         (AFIO) prototypes, defines, and inlined access functions.
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <libmaple/gpio_def.h>
#include "libmaple.h"
#include "boards.h"

#ifdef __cplusplus
extern "C"{
#endif

	
extern const gpio_dev * const gpio_devices[];
//static inline const gpio_dev * digitalPinToPort(uint8_t pin) { return gpio_devices[pin/16]; }
#define digitalPinToPort(P)        ( gpio_devices[P/16] )
#define digitalPinToBit(P)         ( (P&0x0F) )
#define digitalPinToBitMask(P)     ( BIT(P&0x0F) )
#define portOutputRegister(port)   ( &(port->regs->ODR) )
#define portInputRegister(port)    ( &(port->regs->IDR) )

#define portSetRegister(pin)		( &(digitalPinToPort(pin)->regs->BSRR) )
#define portClearRegister(pin)		( &(digitalPinToPort(pin)->regs->BSRR) )

#define portConfigRegister(pin)		( &(digitalPinToPort(pin)->regs->CRL) )


/**
 * @brief Get a GPIO port's corresponding afio_exti_port.
 * @param dev GPIO device whose afio_exti_port to return.
 */
static inline afio_exti_port gpio_exti_port(const gpio_dev *dev) {
    return dev->exti_port;
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
static inline void gpio_write_bit(gpio_dev *dev, uint8 bit, uint8 val) {
    val = !val;          /* "set" bits are lower than "reset" bits  */
    dev->regs->BSRR = (1U << bit) << (16 * val);
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
static inline uint32 gpio_read_bit(gpio_dev *dev, uint8 bit) {
    return dev->regs->IDR & (1U << bit);
}

/**
 * Toggle a pin configured as output push-pull.
 * @param dev GPIO device.
 * @param pin Pin on dev to toggle.
 */
static inline void gpio_toggle_bit(gpio_dev *dev, uint8 bit) {
    dev->regs->ODR = dev->regs->ODR ^ (1U << bit);
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
static inline void gpio_set_pin(uint8_t pin) {
	(digitalPinToPort(pin))->regs->BSRR = (uint32_t)BIT(pin&0x0F);
}

static inline void gpio_clear_pin(uint8_t pin) {
	(digitalPinToPort(pin))->regs->BSRR = (uint32_t)BIT(pin&0x0F)<<16;
}

#define gpio_write_pin(pin,val) { if (val) gpio_set_pin(pin); else gpio_clear_pin(pin); }

/**
 * Determine whether or not a GPIO pin is set.
 *
 * Pin must have previously been configured to input mode.
 *
 * @param dev GPIO device whose pin to test.
 * @param pin Pin on dev to test.
 * @return True if the pin is set, false otherwise.
 */
static inline uint32 gpio_read_pin(uint8_t pin) {
    return (digitalPinToPort(pin))->regs->IDR & BIT(pin&0x0F);
}

/**
 * Toggle a pin configured as output push-pull.
 * @param dev GPIO device.
 * @param pin Pin on dev to toggle.
 */
static inline void gpio_toggle_pin(uint8_t pin) {
    (digitalPinToPort(pin))->regs->ODR = (digitalPinToPort(pin))->regs->ODR ^ BIT(pin&0x0F);
}

/*
 * GPIO Convenience routines
 */

extern void gpio_init(const gpio_dev *dev);
extern void gpio_init_all(void);
extern void gpio_set_mode(uint8_t pin, gpio_pin_mode mode);
extern void gpio_set_af_mode(uint8_t pin, gpio_af_mode mode);
extern void gpio_cfg_debug_ports(gpio_debug_cfg config);

extern void afio_exti_select(afio_exti_num exti, afio_exti_port gpio_port);


#ifdef __cplusplus
}
#endif

#endif

