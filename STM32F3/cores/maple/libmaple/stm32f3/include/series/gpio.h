/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011, 2012 LeafLabs, LLC.
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
 * @file libmaple/stm32f3/include/series/gpio.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 GPIO support.
 */

#ifndef _LIBMAPLE_STM32F3_GPIO_H_
#define _LIBMAPLE_STM32F3_GPIO_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * GPIO register maps and devices
 */

/** GPIO register map type */
typedef struct gpio_reg_map {
    __io uint32 MODER;          /**< Mode register */
    __io uint32 OTYPER;         /**< Output type register */
    __io uint32 OSPEEDR;        /**< Output speed register */
    __io uint32 PUPDR;          /**< Pull-up/pull-down register */
    __io uint32 IDR;            /**< Input data register */
    __io uint32 ODR;            /**< Output data register */
    __io uint32 BSRR;           /**< Bit set/reset register */
    __io uint32 LCKR;           /**< Configuration lock register */
    __io uint32 AFRL;           /**< Alternate function low register */
    __io uint32 AFRH;           /**< Alternate function high register */
    __io uint32 BRR;      			/**< Port bit reset register */
} gpio_reg_map;

/** GPIO port A register map base pointer */
#define GPIOA_BASE                      ((struct gpio_reg_map*)0x48000000)
/** GPIO port B register map base pointer */
#define GPIOB_BASE                      ((struct gpio_reg_map*)0x48000400)
/** GPIO port C register map base pointer */
#define GPIOC_BASE                      ((struct gpio_reg_map*)0x48000800)
/** GPIO port D register map base pointer */
#define GPIOD_BASE                      ((struct gpio_reg_map*)0x48000C00)
/** GPIO port E register map base pointer */
#define GPIOE_BASE                      ((struct gpio_reg_map*)0x48001000)
/** GPIO port F register map base pointer */
#define GPIOF_BASE                      ((struct gpio_reg_map*)0x48001400)

struct gpio_dev;
extern struct gpio_dev* const GPIOA;
extern struct gpio_dev gpioa;
extern struct gpio_dev* const GPIOB;
extern struct gpio_dev gpiob;
extern struct gpio_dev* const GPIOC;
extern struct gpio_dev gpioc;
extern struct gpio_dev* const GPIOD;
extern struct gpio_dev gpiod;
extern struct gpio_dev* const GPIOE;
extern struct gpio_dev gpioe;
extern struct gpio_dev* const GPIOF;
extern struct gpio_dev gpiof;

/*
 * Register bit definitions
 *
 * Currently, we only provide masks to be used for shifting for some
 * registers, rather than repeating the same values 16 times.
 */

/* Mode register */

#define GPIO_MODER_INPUT                0x0
#define GPIO_MODER_OUTPUT               0x1
#define GPIO_MODER_AF                   0x2
#define GPIO_MODER_ANALOG               0x3

/* Output type register */

#define GPIO_OTYPER_PP                  0x0
#define GPIO_OTYPER_OD                  0x1

/* Output speed register */

#define GPIO_OSPEEDR_LOW                0x0
#define GPIO_OSPEEDR_MED                0x1
#define GPIO_OSPEEDR_FAST               0x2
#define GPIO_OSPEEDR_HIGH               0x3

/* Pull-up/pull-down register */

#define GPIO_PUPDR_NOPUPD               0x0
#define GPIO_PUPDR_PU                   0x1
#define GPIO_PUPDR_PD                   0x2

/* Alternate function register low */

#define GPIO_AFRL_AF0                   (0xFU << 0)
#define GPIO_AFRL_AF1                   (0xFU << 4)
#define GPIO_AFRL_AF2                   (0xFU << 8)
#define GPIO_AFRL_AF3                   (0xFU << 12)
#define GPIO_AFRL_AF4                   (0xFU << 16)
#define GPIO_AFRL_AF5                   (0xFU << 20)
#define GPIO_AFRL_AF6                   (0xFU << 24)
#define GPIO_AFRL_AF7                   (0xFU << 28)

/* Alternate function register high */

#define GPIO_AFRH_AF8                   (0xFU << 0)
#define GPIO_AFRH_AF9                   (0xFU << 4)
#define GPIO_AFRH_AF10                  (0xFU << 8)
#define GPIO_AFRH_AF11                  (0xFU << 12)
#define GPIO_AFRH_AF12                  (0xFU << 16)
#define GPIO_AFRH_AF13                  (0xFU << 20)
#define GPIO_AFRH_AF14                  (0xFU << 24)
#define GPIO_AFRH_AF15                  (0xFU << 28)

/*
 * GPIO routines
 */

/**
 * @brief GPIO pin modes
 */
typedef enum gpio_pin_mode {
    GPIO_MODE_INPUT  = GPIO_MODER_INPUT,  /**< Input mode */
    GPIO_MODE_OUTPUT = GPIO_MODER_OUTPUT, /**< Output mode */
    GPIO_MODE_AF     = GPIO_MODER_AF,     /**< Alternate function mode */
    GPIO_MODE_ANALOG = GPIO_MODER_ANALOG, /**< Analog mode */
} gpio_pin_mode;

/**
 * @brief Additional flags to be used when setting a pin's mode.
 *
 * Beyond the basic modes (input, general purpose output, alternate
 * function, and analog), there are three parameters that can affect a
 * pin's mode:
 *
 * 1. Output type: push/pull or open-drain. This only has an effect
 *    for output modes. Choices are: GPIO_MODEF_TYPE_PP (the default)
 *    and GPIO_MODEF_TYPE_OD.
 *
 * 2. Output speed: specifies the frequency at which a pin changes
 *    state. This only has an effect for output modes. Choices are:
 *    GPIO_MODEF_SPEED_LOW (default), GPIO_MODEF_SPEED_MED,
 *    GPIO_MODEF_SPEED_FAST, and GPIO_MODEF_SPEED_HIGH.
 *
 * 3. Push/pull setting: All GPIO pins have weak pull-up and pull-down
 *    resistors that can be enabled when the pin's mode is
 *    set. Choices are: GPIO_MODEF_PUPD_NONE (default),
 *    GPIO_MODEF_PUPD_PU, and GPIO_MODEF_PUPD_PD.
 */
typedef enum gpio_mode_flags {
    /* Output type in bit 0 */
    GPIO_MODEF_TYPE_PP = GPIO_OTYPER_PP, /**< Output push/pull (default).
                                            Applies only when the mode
                                            specifies output. */
    GPIO_MODEF_TYPE_OD = GPIO_OTYPER_OD, /**< Output open drain.
                                            Applies only when the mode
                                            specifies output. */

    /* Speed in bits 2:1 */
    GPIO_MODEF_SPEED_LOW = GPIO_OSPEEDR_LOW << 1, /**< Low speed (default):
                                                     2 MHz. */
    GPIO_MODEF_SPEED_MED = GPIO_OSPEEDR_MED << 1, /**< Medium speed: 25 MHz. */
    GPIO_MODEF_SPEED_FAST = GPIO_OSPEEDR_FAST << 1, /**< Fast speed: 50 MHz. */
    GPIO_MODEF_SPEED_HIGH = GPIO_OSPEEDR_HIGH << 1, /**< High speed: FIXME one of those does not exist on the F3
                                                       100 MHz on 30 pF,
                                                       80 MHz on 15 pF. */

    /* Pull-up/pull-down in bits 4:3 */
    GPIO_MODEF_PUPD_NONE = GPIO_PUPDR_NOPUPD << 3, /**< No pull-up/pull-down
                                                      (default). */
    GPIO_MODEF_PUPD_PU = GPIO_PUPDR_PU << 3, /**< Pull-up */
    GPIO_MODEF_PUPD_PD = GPIO_PUPDR_PD << 3, /**< Pull-down */
} gpio_mode_flags;

void gpio_set_modef(struct gpio_dev *dev,
                    uint8 bit,
                    gpio_pin_mode mode,
                    unsigned flags);

/**
 * @brief Set the mode of a GPIO pin.
 *
 * Calling this function is equivalent to calling gpio_set_modef(dev,
 * pin, mode, GPIO_MODE_SPEED_HIGH). Note that this overrides the
 * default speed.
 *
 * @param dev GPIO device.
 * @param bit Bit on the device whose mode to set, 0--15.
 * @param mode Mode to set the pin to.
 */
static inline void gpio_set_mode(struct gpio_dev *dev,
                                 uint8 bit,
                                 gpio_pin_mode mode) {
    gpio_set_modef(dev, bit, mode, GPIO_MODEF_SPEED_HIGH);
}

/**
 * @brief GPIO alternate functions.
 * Use these to select an alternate function for a pin.
 * @see gpio_set_af()
 */
typedef enum gpio_af {
		GPIO_AF_0			= 0x0, /**< alternate function 0. */
		GPIO_AF_1			= 0x1, /**< alternate function 1. */
		GPIO_AF_2			= 0x2, /**< alternate function 2. */
		GPIO_AF_3			= 0x3, /**< alternate function 3. */
		GPIO_AF_4			= 0x4, /**< alternate function 4. */
		GPIO_AF_5			= 0x5, /**< alternate function 5. */
		GPIO_AF_6			= 0x6, /**< alternate function 6. */
		GPIO_AF_7			= 0x7, /**< alternate function 7. */
		GPIO_AF_8			= 0x8, /**< alternate function 8. */
		GPIO_AF_9			= 0x9, /**< alternate function 9. */
		GPIO_AF_10		= 0xA,	/**< alternate function 10. */
		GPIO_AF_11		= 0xB,	/**< alternate function 11. */
		GPIO_AF_12		= 0xC,	/**< alternate function 12. */
		GPIO_AF_13		= 0xD,	/**< alternate function 13. */
		GPIO_AF_14		= 0xE,	/**< alternate function 14. */
		GPIO_AF_15		= 0xF,	/**< alternate function 14. */
} gpio_af;

void gpio_set_af(struct gpio_dev *dev, uint8 bit, gpio_af af);

#ifdef __cplusplus
}
#endif

#endif
