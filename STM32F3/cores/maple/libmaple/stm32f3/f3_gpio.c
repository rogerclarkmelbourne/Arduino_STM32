/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
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
 * @file libmaple/stm32f3/gpio.c
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 GPIO.
 */

#include <libmaple/gpio.h>
#include <libmaple/rcc.h>
#include <libmaple/bitband.h>

/*
 * GPIO devices
 */

gpio_dev gpioa = {
    .regs      = GPIOA_BASE,
    .clk_id    = RCC_GPIOA,
    .exti_port = EXTI_PA,
};
/** GPIO port A device. */
gpio_dev* const GPIOA = &gpioa;

gpio_dev gpiob = {
    .regs      = GPIOB_BASE,
    .clk_id    = RCC_GPIOB,
    .exti_port = EXTI_PB,
};
/** GPIO port B device. */
gpio_dev* const GPIOB = &gpiob;

gpio_dev gpioc = {
    .regs      = GPIOC_BASE,
    .clk_id    = RCC_GPIOC,
    .exti_port = EXTI_PC,
};
/** GPIO port C device. */
gpio_dev* const GPIOC = &gpioc;

gpio_dev gpiod = {
    .regs      = GPIOD_BASE,
    .clk_id    = RCC_GPIOD,
    .exti_port = EXTI_PD,
};
/** GPIO port D device. */
gpio_dev* const GPIOD = &gpiod;

gpio_dev gpioe = {
    .regs      = GPIOE_BASE,
    .clk_id    = RCC_GPIOE,
    .exti_port = EXTI_PE,
};
/** GPIO port E device. */
gpio_dev* const GPIOE = &gpioe;

gpio_dev gpiof = {
    .regs      = GPIOF_BASE,
    .clk_id    = RCC_GPIOF,
    .exti_port = EXTI_PF,
};
/** GPIO port F device. */
gpio_dev* const GPIOF = &gpiof;

/*
 * GPIO routines
 */

/**
 * Initialize and reset all available GPIO devices.
 */
void gpio_init_all(void) {
    gpio_init(GPIOA);
    gpio_init(GPIOB);
    gpio_init(GPIOC);
    gpio_init(GPIOD);
    gpio_init(GPIOE);
    gpio_init(GPIOF);
}

/**
 * @brief Set the mode of a GPIO pin.
 * @param dev GPIO device.
 * @param bit Bit on dev whose mode to set, 0--15.
 * @param mode Mode to set the pin to.
 * @param flags Flags to modify basic mode configuration
 */
void gpio_set_modef(gpio_dev *dev,
                    uint8 bit,
                    gpio_pin_mode mode,
                    unsigned flags) {
    gpio_reg_map *regs = dev->regs;
    unsigned shift = bit * 2;
    uint32 tmp;

    /* Mode */
    tmp = regs->MODER;
    tmp &= ~(0x3 << shift);
    tmp |= mode << shift;
    regs->MODER = tmp;

    /* Output type */
    bb_peri_set_bit(&regs->OTYPER, bit, flags & 0x1);

    /* Speed */
    tmp = regs->OSPEEDR;
    tmp &= ~(0x3 << shift);
    tmp |= ((flags >> 1) & 0x3) << shift;
    regs->OSPEEDR = tmp;

    /* Pull-up/pull-down */
    tmp = regs->PUPDR;
    tmp &= ~(0x3 << shift);
    tmp |= ((flags >> 3) & 0x3) << shift;
    regs->PUPDR = tmp;
}

/**
 * @brief Set a pin's alternate function.
 *
 * The pin must have its mode set to GPIO_MODE_AF for this to take
 * effect.
 *
 * @param dev Device whose pin to configure.
 * @param bit Pin whose alternate function to set.
 * @param af  Alternate function to use for pin.
 * @see gpio_set_modef()
 */
void gpio_set_af(gpio_dev *dev, uint8 bit, gpio_af af) {
    __io uint32 *afr;
    unsigned shift;
    uint32 tmp;
    if (bit >= 8) {
        afr = &dev->regs->AFRH;
        shift = 4 * (bit - 8);
    } else{
        afr = &dev->regs->AFRL;
        shift = 4 * bit;
    }
    tmp = *afr;
    tmp &= ~(0xF << shift);
    tmp |= (af << shift);
    *afr = tmp;
}
