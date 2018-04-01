/******************************************************************************
 * The MIT License
 *
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
 * @file libmaple/stm32f2/include/series/gpio.h
 * @brief STM32F2 GPIO support.
 */

#ifndef _LIBMAPLE_STM32F2_GPIO_H_
#define _LIBMAPLE_STM32F2_GPIO_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * GPIO register maps and devices
 */

/** GPIO register map type */
typedef struct gpio_reg_map {
    __IO uint32 MODER;          /**< Mode register */
    __IO uint32 OTYPER;         /**< Output type register */
    __IO uint32 OSPEEDR;        /**< Output speed register */
    __IO uint32 PUPDR;          /**< Pull-up/pull-down register */
    __IO uint32 IDR;            /**< Input data register */
    __IO uint32 ODR;            /**< Output data register */
    __IO uint32 BSRR;           /**< Bit set/reset register */
    __IO uint32 LCKR;           /**< Configuration lock register */
    __IO uint32 AFRL;           /**< Alternate function low register */
    __IO uint32 AFRH;           /**< Alternate function high register */
} gpio_reg_map;

/** GPIO port A register map base pointer */
#define GPIOA_BASE                      ((struct gpio_reg_map*)0x40020000)
/** GPIO port B register map base pointer */
#define GPIOB_BASE                      ((struct gpio_reg_map*)0x40020400)
/** GPIO port C register map base pointer */
#define GPIOC_BASE                      ((struct gpio_reg_map*)0x40020800)
/** GPIO port D register map base pointer */
#define GPIOD_BASE                      ((struct gpio_reg_map*)0x40020C00)
/** GPIO port E register map base pointer */
#define GPIOE_BASE                      ((struct gpio_reg_map*)0x40021000)
/** GPIO port F register map base pointer */
#define GPIOF_BASE                      ((struct gpio_reg_map*)0x40021400)
/** GPIO port G register map base pointer */
#define GPIOG_BASE                      ((struct gpio_reg_map*)0x40021800)
/** GPIO port H register map base pointer */
#define GPIOH_BASE                      ((struct gpio_reg_map*)0x40021C00)
/** GPIO port I register map base pointer */
#define GPIOI_BASE                      ((struct gpio_reg_map*)0x40022000)

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
extern struct gpio_dev* const GPIOG;
extern struct gpio_dev gpiog;
extern struct gpio_dev* const GPIOH;
extern struct gpio_dev gpioh;
extern struct gpio_dev* const GPIOI;
extern struct gpio_dev gpioi;

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
    GPIO_MODEF_SPEED_HIGH = GPIO_OSPEEDR_HIGH << 1, /**< High speed:
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
    GPIO_AF_SYS                  = 0, /**< System. */
    GPIO_AF_TIM_1_2              = 1, /**< Timers 1 and 2. */
    GPIO_AF_TIM_3_4_5            = 2, /**< Timers 3, 4, and 5. */
    GPIO_AF_TIM_8_9_10_11        = 3, /**< Timers 8 through 11. */
    GPIO_AF_I2C                  = 4, /**< I2C 1, 2, and 3. */
    GPIO_AF_SPI_1_2              = 5, /**< SPI1, SPI2/I2S2. */
    GPIO_AF_SPI3                 = 6, /**< SPI3/I2S3. */
    GPIO_AF_USART_1_2_3          = 7, /**< USART 1, 2, and 3. */
    GPIO_AF_USART_4_5_6          = 8, /**< UART 4 and 5, USART 6. */
    GPIO_AF_CAN_1_2_TIM_12_13_14 = 9, /**<
                                       * CAN 1 and 2, timers 12, 13, and 14. */
    GPIO_AF_USB_OTG_FS_HS        = 10, /**< USB OTG HS and FS. */
    GPIO_AF_ETH                  = 11, /**< Ethernet MII and RMII. */
    GPIO_AF_FSMC_SDIO_OTG_FS     = 12, /**< FSMC, SDIO, and USB OTG FS. */
    GPIO_AF_DCMI                 = 13, /**< DCMI. */
    GPIO_AF_EVENTOUT             = 15, /**< EVENTOUT. */
} gpio_af;

void gpio_set_af(struct gpio_dev *dev, uint8 bit, gpio_af af);

#ifdef __cplusplus
}
#endif

#endif
