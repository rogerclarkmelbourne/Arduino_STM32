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

#ifndef _GPIO_DEF_H_
#define _GPIO_DEF_H_

#include "libmaple.h"
#include "rcc.h"

#ifdef __cplusplus
extern "C"{
#endif

/*
 * GPIO register maps and devices
 */

/** GPIO register map type */
typedef struct gpio_reg_map {
    __io uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    __io uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    __io uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    __io uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    __io uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    __io uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    __io uint32_t BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
    __io uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    __io uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x24-0x28 */
} gpio_reg_map;


/**
 * @brief External interrupt line port selector.
 *
 * Used to determine which GPIO port to map an external interrupt line
 * onto. */
/* (See AFIO sections, below) */
typedef enum afio_exti_port {
    AFIO_EXTI_PA,               /**< Use port A (PAx) pin. */
    AFIO_EXTI_PB,               /**< Use port B (PBx) pin. */
    AFIO_EXTI_PC,               /**< Use port C (PCx) pin. */
    AFIO_EXTI_PD,               /**< Use port D (PDx) pin. */
    AFIO_EXTI_PE,               /**< Use port E (PEx) pin. */
#if defined(PACKAGE_LQFP144)
    AFIO_EXTI_PF,               /**< Use port F (PFx) pin. */
    AFIO_EXTI_PG,               /**< Use port G (PGx) pin. */
#endif // PACKAGE_LQFP144
} afio_exti_port;


/** GPIO device type */
typedef struct gpio_dev {
    gpio_reg_map *regs;       /**< Register map */
    rcc_clk_id clk_id;        /**< RCC clock information */
    afio_exti_port exti_port; /**< AFIO external interrupt port value */
} gpio_dev;

extern const gpio_dev gpioa;
extern const gpio_dev gpiob;
extern const gpio_dev gpioc;
extern const gpio_dev gpiod;
extern const gpio_dev gpioe;
#define GPIOA (&gpioa)
#define GPIOB (&gpiob)
#define GPIOC (&gpioc)
#define GPIOD (&gpiod)
#define GPIOE (&gpioe)
#if defined(PACKAGE_LQFP144)
extern const gpio_dev gpiof;
extern const gpio_dev gpiog;
#define GPIOF (&gpiof)
#define GPIOG (&gpiog)
#endif // PACKAGE_LQFP144

/** GPIO port register map base pointer */
#define GPIOA_BASE                      ((struct gpio_reg_map*)0x40020000)
#define GPIOB_BASE                      ((struct gpio_reg_map*)0x40020400)
#define GPIOC_BASE                      ((struct gpio_reg_map*)0x40020800)
#define GPIOD_BASE                      ((struct gpio_reg_map*)0x40020C00)
#define GPIOE_BASE                      ((struct gpio_reg_map*)0x40021000)
#if defined(PACKAGE_LQFP144)
#define GPIOF_BASE                      ((struct gpio_reg_map*)0x40021400)
#define GPIOG_BASE                      ((struct gpio_reg_map*)0x40021800)
#endif // PACKAGE_LQFP144

/*
 * GPIO register bit definitions
 */

#define GPIO_MODE_INPUT                 0
#define GPIO_MODE_OUTPUT                1
#define GPIO_MODE_AF                    2
#define GPIO_MODE_ANALOG                3

#define GPIO_PUPD_NONE            (0 << 2)
#define GPIO_PUPD_PU              (1 << 2)
#define GPIO_PUPD_PD              (2 << 2)

#define GPIO_OSPEED_2MHZ                (0 << 4)
#define GPIO_OSPEED_25MHZ               (1 << 4)
#define GPIO_OSPEED_50MHZ               (2 << 4)
#define GPIO_OSPEED_100MHZ              (3 << 4)

#define GPIO_OTYPE_PP                   (0 << 6)
#define GPIO_OTYPE_OD                   (1 << 6)

/*
MODER
00: Input (reset state)
01: General purpose output mode
10: Alternate function mode
11: Analog mode

OTYPER
0: Output push-pull (reset state)
1: Output open-drain

OSPEEDR
00: 2 MHz Low speed
01: 25 MHz Medium speed
10: 50 MHz Fast speed
11: 100 MHz High speed on 30 pF (80 MHz Output max speed on 15 pF)

PUPDR
00: No pull-up, pull-down
01: Pull-up
10: Pull-down

AFRL 4 bit AF00-AF15
AFRH 4 bit AF00-AF15
*/

/**
 * @brief GPIO Pin modes.
 *
 * These only allow for 50MHZ max output speeds; if you want slower,
 * use direct register access.
 */
typedef enum gpio_pin_mode {
    GPIO_OUTPUT_PP      = (GPIO_MODE_OUTPUT | GPIO_OTYPE_PP | GPIO_OSPEED_50MHZ),
    GPIO_OUTPUT_OD      = (GPIO_MODE_OUTPUT | GPIO_OTYPE_OD | GPIO_OSPEED_50MHZ),
    GPIO_AF_OUTPUT_PP   = (GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_OSPEED_50MHZ),
    GPIO_AF_OUTPUT_PP_PU= (GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_PUPD_PU | GPIO_OSPEED_50MHZ),
    GPIO_AF_OUTPUT_OD   = (GPIO_MODE_AF | GPIO_OTYPE_OD | GPIO_OSPEED_50MHZ),
    GPIO_INPUT_ANALOG   = (GPIO_MODE_ANALOG),
    GPIO_INPUT_FLOATING = (GPIO_MODE_INPUT | GPIO_PUPD_NONE),
    GPIO_INPUT_PU       = (GPIO_MODE_INPUT | GPIO_PUPD_PU),
    GPIO_INPUT_PD       = (GPIO_MODE_INPUT | GPIO_PUPD_PD),
    GPIO_AF_INPUT       = (GPIO_MODE_AF | GPIO_PUPD_NONE),
    GPIO_AF_INPUT_PU    = (GPIO_MODE_AF | GPIO_PUPD_PU),
    GPIO_AF_INPUT_PD    = (GPIO_MODE_AF | GPIO_PUPD_PD),
    GPIO_BIGNUMBER      = 0xfff
} gpio_pin_mode;


/*
 * AFIO register map
 */

/** AFIO register map */
typedef struct syscfg_reg_map {
    __io uint32 MEMRMP;    /**< memory remap register  */
    __io uint32 PMC;      /**< peripheral mode configuration register */
    __io uint32 EXTICR1;  /**< External interrupt configuration register 1. */
    __io uint32 EXTICR2;  /**< External interrupt configuration register 2. */
    __io uint32 EXTICR3;  /**< External interrupt configuration register 3. */
    __io uint32 EXTICR4;  /**< External interrupt configuration register 4. */
    __io uint32 CMPCR;    /**< Compensation cell control register */
} syscfg_reg_map;

extern syscfg_reg_map * const SYSCFG;

/** AFIO register map base pointer. */
#define SYSCFG_BASE                       ((struct syscfg_reg_map *)0x40013800)


/**
 * External interrupt line numbers.
 */
typedef enum afio_exti_num {
    AFIO_EXTI_0,                /**< External interrupt line 0. */
    AFIO_EXTI_1,                /**< External interrupt line 1. */
    AFIO_EXTI_2,                /**< External interrupt line 2. */
    AFIO_EXTI_3,                /**< External interrupt line 3. */
    AFIO_EXTI_4,                /**< External interrupt line 4. */
    AFIO_EXTI_5,                /**< External interrupt line 5. */
    AFIO_EXTI_6,                /**< External interrupt line 6. */
    AFIO_EXTI_7,                /**< External interrupt line 7. */
    AFIO_EXTI_8,                /**< External interrupt line 8. */
    AFIO_EXTI_9,                /**< External interrupt line 9. */
    AFIO_EXTI_10,               /**< External interrupt line 10. */
    AFIO_EXTI_11,               /**< External interrupt line 11. */
    AFIO_EXTI_12,               /**< External interrupt line 12. */
    AFIO_EXTI_13,               /**< External interrupt line 13. */
    AFIO_EXTI_14,               /**< External interrupt line 14. */
    AFIO_EXTI_15,               /**< External interrupt line 15. */
} afio_exti_num;

void afio_exti_select(afio_exti_num exti, afio_exti_port gpio_port);

/* HACK: Use upper bit to denote MAPR2, Bit 31 is reserved and
 * not used in either MAPR or MAPR2 */
#define AFIO_REMAP_USE_MAPR2            (1 << 31)

/**
 * @brief Available peripheral remaps.
 * @see afio_remap()
 */
typedef enum {
    GPIO_AFMODE_SYSTEM   = 0,
    GPIO_AFMODE_TIM1_2   = 1,
    GPIO_AFMODE_TIM3_5   = 2,
    GPIO_AFMODE_TIM8_11  = 3,
    GPIO_AFMODE_I2C1_3   = 4,
    GPIO_AFMODE_SPI1_2   = 5,
    GPIO_AFMODE_SPI3     = 6,
    GPIO_AFMODE_UASRT1_3 = 7,
    GPIO_AFMODE_UASRT4_6 = 8,
    GPIO_AFMODE_CAN1_2   = 9,
    GPIO_AFMODE_TIM12_14 = 9,
    GPIO_AFMODE_OTG_FS   = 10,
    GPIO_AFMODE_ETH      = 11,
    GPIO_AFMODE_FSMC     = 12,
    GPIO_AFMODE_SDIO     = 12,
    GPIO_AFMODE_OTG_HS   = 12,
    GPIO_AFMODE_DCMI     = 13,
    GPIO_AFMODE_14       = 14,
    GPIO_AFMODE_EVENTOUT = 15,
} gpio_af_mode;

/**
 * @brief Debug port configuration
 *
 * Used to configure the behavior of JTAG and Serial Wire (SW) debug
 * ports and their associated GPIO pins.
 *
 * @see afio_cfg_debug_ports()
 */
typedef enum {
    GPIO_DEBUG_FULL_SWJ          = 0, // Full Serial Wire and JTAG debug
    GPIO_DEBUG_FULL_SWJ_NO_NJRST = 1, // Full Serial Wire and JTAG, but no NJTRST.
    GPIO_DEBUG_SW_ONLY           = 2, // Serial Wire debug only (JTAG-DP disabled, SW-DP enabled)
    GPIO_DEBUG_NONE              = 3, // No debug; all JTAG and SW pins are free for use as GPIOs.
} gpio_debug_cfg;


#ifdef __cplusplus
}
#endif

#endif
