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
 * @file libmaple/stm32f1/gpio.c
 * @brief STM32F1 GPIO support.
 */

#include <libmaple/gpio.h>
#include <libmaple/rcc.h>

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

#if STM32_NR_GPIO_PORTS > 4
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

gpio_dev gpiog = {
    .regs      = GPIOG_BASE,
    .clk_id    = RCC_GPIOG,
    .exti_port = EXTI_PG,
};
/** GPIO port G device. */
gpio_dev* const GPIOG = &gpiog;
#endif

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
#if STM32_NR_GPIO_PORTS > 4
    gpio_init(GPIOE);
    gpio_init(GPIOF);
    gpio_init(GPIOG);
#endif
}

/**
 * Set the mode of a GPIO pin.
 *
 * @param dev GPIO device.
 * @param pin Pin on the device whose mode to set, 0--15.
 * @param mode General purpose or alternate function mode to set the pin to.
 * @see gpio_pin_mode
 */
void gpio_set_mode(gpio_dev *dev, uint8 pin, gpio_pin_mode mode) {
    gpio_reg_map *regs = dev->regs;
    __IO uint32 *cr = &regs->CRL + (pin >> 3);
    uint32 shift = (pin & 0x7) * 4;
    uint32 tmp = *cr;

    tmp &= ~(0xF << shift);
    tmp |= (mode == GPIO_INPUT_PU ? GPIO_INPUT_PD : mode) << shift;
    *cr = tmp;

    if (mode == GPIO_INPUT_PD) {
        regs->ODR &= ~(1U << pin);
    } else if (mode == GPIO_INPUT_PU) {
        regs->ODR |= (1U << pin);
    }
}

gpio_pin_mode gpio_get_mode(gpio_dev *dev, uint8 pin) {
    gpio_reg_map *regs = dev->regs;
    __IO uint32 *cr = &regs->CRL + (pin >> 3);
    uint32 shift = (pin & 0x7) * 4;

	uint32 crMode = (*cr>>shift) & 0x0F;
	
	// could be pull up or pull down. Nee to check the ODR
	if (crMode==GPIO_INPUT_PD && ((regs->ODR >> pin) & 0x01) !=0 )
	{
		crMode = GPIO_INPUT_PU;
	}
	
    return(crMode);
}

/*
 * AFIO
 */

/**
 * @brief Initialize the AFIO clock, and reset the AFIO registers.
 */
void afio_init(void) {
    rcc_clk_enable(RCC_AFIO);
    rcc_reset_dev(RCC_AFIO);
}

#define AFIO_EXTI_SEL_MASK 0xF

/**
 * @brief Perform an alternate function remap.
 * @param remapping Remapping to perform.
 */
void afio_remap(afio_remap_peripheral remapping) {
    if (remapping & AFIO_REMAP_USE_MAPR2) {
        remapping &= ~AFIO_REMAP_USE_MAPR2;
        AFIO_BASE->MAPR2 |= remapping;
    } else {
        AFIO_BASE->MAPR |= remapping;
    }
}
