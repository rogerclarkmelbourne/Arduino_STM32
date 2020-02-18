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
 * @file gpio.c
 * @brief GPIO initialization routine
 */

#include "gpio.h"
#include "rcc.h"

/*
 * GPIO devices
 */

/** GPIO port A device. */
const gpio_dev gpioa = {
    .regs      = GPIOA_BASE,
    .clk_id    = RCC_GPIOA,
    .exti_port = AFIO_EXTI_PA,
};

/** GPIO port B device. */
const gpio_dev gpiob = {
    .regs      = GPIOB_BASE,
    .clk_id    = RCC_GPIOB,
    .exti_port = AFIO_EXTI_PB,
};

/** GPIO port C device. */
const gpio_dev gpioc = {
    .regs      = GPIOC_BASE,
    .clk_id    = RCC_GPIOC,
    .exti_port = AFIO_EXTI_PC,
};

/** GPIO port D device. */
const gpio_dev gpiod = {
    .regs      = GPIOD_BASE,
    .clk_id    = RCC_GPIOD,
    .exti_port = AFIO_EXTI_PD,
};

/** GPIO port E device. */
const gpio_dev gpioe = {
    .regs      = GPIOE_BASE,
    .clk_id    = RCC_GPIOE,
    .exti_port = AFIO_EXTI_PE,
};

#if defined(PACKAGE_LQFP144) // not available on LQFP100 package
/** GPIO port F device. */
const gpio_dev gpiof = {
    .regs      = GPIOF_BASE,
    .clk_id    = RCC_GPIOF,
    .exti_port = AFIO_EXTI_PF,
};

/** GPIO port G device. */
const gpio_dev gpiog = {
    .regs      = GPIOG_BASE,
    .clk_id    = RCC_GPIOG,
    .exti_port = AFIO_EXTI_PG,
};
#endif


const gpio_dev * const gpio_devices[] = {
	GPIOA, GPIOB, GPIOC, GPIOD, GPIOE,
#ifdef PACKAGE_LQFP144 // not available on LQFP100 package
	GPIOF, GPIOG
#endif
};

/*
 * GPIO convenience routines
 */

/**
 * Initialize a GPIO device.
 *
 * Enables the clock for and resets the given device.
 *
 * @param dev GPIO device to initialize.
 */
void gpio_init(const gpio_dev *dev) {
    rcc_clk_enable(dev->clk_id);
    rcc_reset_dev(dev->clk_id);
}

/**
 * Initialize and reset all available GPIO devices.
 */
void gpio_init_all(void) {
    gpio_init(GPIOA);
    gpio_init(GPIOB);
    gpio_init(GPIOC);
    gpio_init(GPIOD);
    gpio_init(GPIOE);
#if defined(PACKAGE_LQFP144) // not available on LQFP 100 package
    gpio_init(GPIOF);
    gpio_init(GPIOG);
#endif // not available on LQFP 100 package
}

/**
 * Set the mode of a GPIO pin.
 *
 * @param dev GPIO device.
 * @param pin Pin on the device whose mode to set, 0--15.
 * @param mode General purpose or alternate function mode to set the pin to.
 * @see gpio_pin_mode
 */
void gpio_set_mode(uint8_t io_pin, gpio_pin_mode mode) {
    gpio_reg_map *regs = digitalPinToPort(io_pin)->regs;
    uint8_t pin = io_pin&0x0f;

    regs->MODER      = (regs->MODER   & ~( 3 << (pin<<1)))  |  (((mode >> 0) & 3)  << (pin<<1));
    regs->PUPDR      = (regs->PUPDR   & ~( 3 << (pin<<1)))  |  (((mode >> 2) & 3)  << (pin<<1));
    regs->OSPEEDR    = (regs->OSPEEDR & ~( 3 << (pin<<1)))  |  (((mode >> 4) & 3)  << (pin<<1));
    regs->OTYPER     = (regs->OTYPER  & ~( 1 << (pin<<0)))  |  (((mode >> 6) & 1)  << (pin<<0));
}

/**
 * Set the alternate function mode of a GPIO pin.
 *
 * @param dev GPIO device.
 * @param pin Pin on the device whose mode to set, 0--15.
 * @param mode alternate function mode to set the pin to.
 * @see gpio_pin_mode
 */
void gpio_set_af_mode(uint8_t io_pin, gpio_af_mode mode) {
    gpio_reg_map *regs = digitalPinToPort(io_pin)->regs;
    uint8_t pin = io_pin&0x0F;

    regs->AFR[pin>>3] = (regs->AFR[pin>>3] & ~(15 << ((pin&7)<<2))) | (((mode >> 0) & 15) << ((pin&7)<<2));
}

/**
 * @brief Enable or disable the JTAG and SW debug ports.
 * @param config Desired debug port configuration
 * @see gpio_debug_cfg
 */
void gpio_cfg_debug_ports(gpio_debug_cfg config) {
    switch ( config ) {
        case GPIO_DEBUG_FULL_SWJ:
            gpio_set_af_mode(PA13, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PA14, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PA15, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PB3,  GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PB4,  GPIO_AFMODE_SYSTEM);
            break;
        case GPIO_DEBUG_FULL_SWJ_NO_NJRST:
            gpio_set_af_mode(PA13, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PA14, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PA15, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PB3,  GPIO_AFMODE_SYSTEM);
            gpio_set_mode(PB4, GPIO_INPUT_FLOATING);
            break;
        case GPIO_DEBUG_SW_ONLY:
            gpio_set_af_mode(PA13, GPIO_AFMODE_SYSTEM);
            gpio_set_af_mode(PA14, GPIO_AFMODE_SYSTEM);
            gpio_set_mode(PA15, GPIO_INPUT_FLOATING);
            gpio_set_mode(PB3, GPIO_INPUT_FLOATING);
            gpio_set_mode(PB4, GPIO_INPUT_FLOATING);
            break;
        case GPIO_DEBUG_NONE:
            gpio_set_mode(PA13, GPIO_INPUT_FLOATING);
            gpio_set_mode(PA14, GPIO_INPUT_FLOATING);
            gpio_set_mode(PA15, GPIO_INPUT_FLOATING);
            gpio_set_mode(PB3, GPIO_INPUT_FLOATING);
            gpio_set_mode(PB4, GPIO_INPUT_FLOATING);
            break;
        default:
            break;
    }
}

#define AFIO_EXTI_SEL_MASK 0xF

/**
 * @brief Select a source input for an external interrupt.
 *
 * @param exti      External interrupt.
 * @param gpio_port Port which contains pin to use as source input.
 * @see afio_exti_num
 * @see afio_exti_port
 */
void afio_exti_select(afio_exti_num exti, afio_exti_port gpio_port) {
    __IO uint32 *exti_cr = &SYSCFG->EXTICR1 + exti / 4;
    uint32 shift = 4 * (exti % 4);
    uint32 cr = *exti_cr;

    cr &= ~(AFIO_EXTI_SEL_MASK << shift);
    cr |= gpio_port << shift;
    *exti_cr = cr;
}
