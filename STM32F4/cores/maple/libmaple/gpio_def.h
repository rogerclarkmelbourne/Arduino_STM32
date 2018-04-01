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
    __IO uint32 MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
    __IO uint32 OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
    __IO uint32 OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
    __IO uint32 PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
    __IO uint32 IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
    __IO uint32 ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
    __IO uint16 BSRRL;    /*!< GPIO port bit set/reset low register,  Address offset: 0x18      */
    __IO uint16 BSRRH;    /*!< GPIO port bit set/reset high register, Address offset: 0x1A      */
    __IO uint32 LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
    __IO uint32 AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x24-0x28 */
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
#ifdef STM32_HIGH_DENSITY
    AFIO_EXTI_PE,               /**< Use port E (PEx) pin. */
 #if 0 // not available on LQFP 100 package
    AFIO_EXTI_PF,               /**< Use port F (PFx) pin. */
    AFIO_EXTI_PG,               /**< Use port G (PGx) pin. */
 #endif // not available on LQFP 100 package
#endif
} afio_exti_port;


/** GPIO device type */
typedef struct gpio_dev {
    gpio_reg_map *regs;       /**< Register map */
    rcc_clk_id clk_id;        /**< RCC clock information */
    afio_exti_port exti_port; /**< AFIO external interrupt port value */
} gpio_dev;

extern const gpio_dev GPIOA;
extern const gpio_dev GPIOB;
extern const gpio_dev GPIOC;
extern const gpio_dev GPIOD;
#ifdef STM32_HIGH_DENSITY
extern const gpio_dev GPIOE;
 #if 0 // not available on LQFP 100 package
extern gpio_dev gpiof;
extern gpio_dev* const GPIOF;
extern gpio_dev gpiog;
extern gpio_dev* const GPIOG;
 #endif // not available on LQFP 100 package
#endif

/** GPIO port register map base pointer */
#define GPIOA_BASE                      ((struct gpio_reg_map*)0x40020000)
#define GPIOB_BASE                      ((struct gpio_reg_map*)0x40020400)
#define GPIOC_BASE                      ((struct gpio_reg_map*)0x40020800)
#define GPIOD_BASE                      ((struct gpio_reg_map*)0x40020C00)
#ifdef STM32_HIGH_DENSITY
#define GPIOE_BASE                      ((struct gpio_reg_map*)0x40021000)
 #if 0 // not available on LQFP 100 package
#define GPIOF_BASE                      ((struct gpio_reg_map*)0x40021400)
#define GPIOG_BASE                      ((struct gpio_reg_map*)0x40021800)
 #endif // not available on LQFP 100 package
#endif

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
    GPIO_OUTPUT_PP = (GPIO_MODE_OUTPUT | GPIO_OTYPE_PP |
                      GPIO_OSPEED_50MHZ), /**< Output push-pull. */
    GPIO_OUTPUT_OD = (GPIO_MODE_OUTPUT | GPIO_OTYPE_OD |
                      GPIO_OSPEED_50MHZ), /**< Output open-drain. */
    GPIO_AF_OUTPUT_PP = (GPIO_MODE_AF | GPIO_OTYPE_PP |
                         GPIO_OSPEED_50MHZ), /**< Alternate function
                                                        output push-pull. */
    GPIO_AF_OUTPUT_PP_PU = (GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_PUPD_PU |
                         GPIO_OSPEED_50MHZ), /**< Alternate function
                                                        output push-pull. */
    GPIO_AF_OUTPUT_OD = (GPIO_MODE_AF | GPIO_OTYPE_OD |
                         GPIO_OSPEED_50MHZ), /**< Alternate function
                                                        output open drain. */
    GPIO_INPUT_ANALOG = (GPIO_MODE_ANALOG), /**< Analog input. */
    GPIO_INPUT_FLOATING = (GPIO_MODE_INPUT |
                           GPIO_PUPD_NONE), /**< Input floating. */
    GPIO_INPUT_PU = (GPIO_MODE_INPUT | GPIO_PUPD_PU), /**< Input pull-up. */
    GPIO_INPUT_PD = (GPIO_MODE_INPUT | GPIO_PUPD_PD), /**< Input pull-down. */
    GPIO_AF_INPUT_PU = (GPIO_MODE_AF | GPIO_PUPD_PU), /**< Alternate input pull-up. */
    GPIO_AF_INPUT_PD = (GPIO_MODE_AF | GPIO_PUPD_PD), /**< Alternate input pull-down. */
	GPIO_BIGNUMBER = 0xfff
} gpio_pin_mode;


/*
 * AFIO register map
 */

/** AFIO register map */
typedef struct syscfg_reg_map {
    __IO uint32 MEMRM;    /**< memory remap register  */
    __IO uint32 PMC;      /**< peripheral mode configuration register */
    __IO uint32 EXTICR1;  /**< External interrupt configuration register 1. */
    __IO uint32 EXTICR2;  /**< External interrupt configuration register 2. */
    __IO uint32 EXTICR3;  /**< External interrupt configuration register 3. */
    __IO uint32 EXTICR4;  /**< External interrupt configuration register 4. */
    __IO uint32 CMPCR;    /**< Compensation cell control register */
} syscfg_reg_map;

/** AFIO register map base pointer. */
#define SYSCFG_BASE                       ((struct syscfg_reg_map *)0x40013800)

/*
 * AFIO register bit definitions
 */

/* Event control register */

#define AFIO_EVCR_EVOE                  (0x1 << 7)
#define AFIO_EVCR_PORT_PA               (0x0 << 4)
#define AFIO_EVCR_PORT_PB               (0x1 << 4)
#define AFIO_EVCR_PORT_PC               (0x2 << 4)
#define AFIO_EVCR_PORT_PD               (0x3 << 4)
#define AFIO_EVCR_PORT_PE               (0x4 << 4)
#define AFIO_EVCR_PIN_0                 0x0
#define AFIO_EVCR_PIN_1                 0x1
#define AFIO_EVCR_PIN_2                 0x2
#define AFIO_EVCR_PIN_3                 0x3
#define AFIO_EVCR_PIN_4                 0x4
#define AFIO_EVCR_PIN_5                 0x5
#define AFIO_EVCR_PIN_6                 0x6
#define AFIO_EVCR_PIN_7                 0x7
#define AFIO_EVCR_PIN_8                 0x8
#define AFIO_EVCR_PIN_9                 0x9
#define AFIO_EVCR_PIN_10                0xA
#define AFIO_EVCR_PIN_11                0xB
#define AFIO_EVCR_PIN_12                0xC
#define AFIO_EVCR_PIN_13                0xD
#define AFIO_EVCR_PIN_14                0xE
#define AFIO_EVCR_PIN_15                0xF

/* AF remap and debug I/O configuration register */

#define AFIO_MAPR_SWJ_CFG                      (0x7 << 24)
#define AFIO_MAPR_SWJ_CFG_FULL_SWJ             (0x0 << 24)
#define AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_NJRST    (0x1 << 24)
#define AFIO_MAPR_SWJ_CFG_NO_JTAG_SW           (0x2 << 24)
#define AFIO_MAPR_SWJ_CFG_NO_JTAG_NO_SW        (0x4 << 24)
#define AFIO_MAPR_ADC2_ETRGREG_REMAP           BIT(20)
#define AFIO_MAPR_ADC2_ETRGINJ_REMAP           BIT(19)
#define AFIO_MAPR_ADC1_ETRGREG_REMAP           BIT(18)
#define AFIO_MAPR_ADC1_ETRGINJ_REMAP           BIT(17)
#define AFIO_MAPR_TIM5CH4_IREMAP               BIT(16)
#define AFIO_MAPR_PD01_REMAP                   BIT(15)
#define AFIO_MAPR_CAN_REMAP                    (0x3 << 13)
#define AFIO_MAPR_CAN_REMAP_NONE               (0x0 << 13)
#define AFIO_MAPR_CAN_REMAP_PB8_PB9            (0x2 << 13)
#define AFIO_MAPR_CAN_REMAP_PD0_PD1            (0x3 << 13)
#define AFIO_MAPR_TIM4_REMAP                   BIT(12)
#define AFIO_MAPR_TIM3_REMAP                   (0x3 << 10)
#define AFIO_MAPR_TIM3_REMAP_NONE              (0x0 << 10)
#define AFIO_MAPR_TIM3_REMAP_PARTIAL           (0x2 << 10)
#define AFIO_MAPR_TIM3_REMAP_FULL              (0x3 << 10)
#define AFIO_MAPR_TIM2_REMAP                   (0x3 << 8)
#define AFIO_MAPR_TIM2_REMAP_NONE              (0x0 << 8)
#define AFIO_MAPR_TIM2_REMAP_PA15_PB3_PA2_PA3  (0x1 << 8)
#define AFIO_MAPR_TIM2_REMAP_PA0_PA1_PB10_PB11 (0x2 << 8)
#define AFIO_MAPR_TIM2_REMAP_FULL              (0x3 << 8)
#define AFIO_MAPR_TIM1_REMAP                   (0x3 << 6)
#define AFIO_MAPR_TIM1_REMAP_NONE              (0x0 << 6)
#define AFIO_MAPR_TIM1_REMAP_PARTIAL           (0x1 << 6)
#define AFIO_MAPR_TIM1_REMAP_FULL              (0x3 << 6)
#define AFIO_MAPR_USART3_REMAP                 (0x3 << 4)
#define AFIO_MAPR_USART3_REMAP_NONE            (0x0 << 4)
#define AFIO_MAPR_USART3_REMAP_PARTIAL         (0x1 << 4)
#define AFIO_MAPR_USART3_REMAP_FULL            (0x3 << 4)
#define AFIO_MAPR_USART2_REMAP                 BIT(3)
#define AFIO_MAPR_USART1_REMAP                 BIT(2)
#define AFIO_MAPR_I2C1_REMAP                   BIT(1)
#define AFIO_MAPR_SPI1_REMAP                   BIT(0)

/* External interrupt configuration register 1 */

#define AFIO_EXTICR1_EXTI3              (0xF << 12)
#define AFIO_EXTICR1_EXTI3_PA           (0x0 << 12)
#define AFIO_EXTICR1_EXTI3_PB           (0x1 << 12)
#define AFIO_EXTICR1_EXTI3_PC           (0x2 << 12)
#define AFIO_EXTICR1_EXTI3_PD           (0x3 << 12)
#define AFIO_EXTICR1_EXTI3_PE           (0x4 << 12)
#define AFIO_EXTICR1_EXTI3_PF           (0x5 << 12)
#define AFIO_EXTICR1_EXTI3_PG           (0x6 << 12)
#define AFIO_EXTICR1_EXTI2              (0xF << 8)
#define AFIO_EXTICR1_EXTI2_PA           (0x0 << 8)
#define AFIO_EXTICR1_EXTI2_PB           (0x1 << 8)
#define AFIO_EXTICR1_EXTI2_PC           (0x2 << 8)
#define AFIO_EXTICR1_EXTI2_PD           (0x3 << 8)
#define AFIO_EXTICR1_EXTI2_PE           (0x4 << 8)
#define AFIO_EXTICR1_EXTI2_PF           (0x5 << 8)
#define AFIO_EXTICR1_EXTI2_PG           (0x6 << 8)
#define AFIO_EXTICR1_EXTI1              (0xF << 4)
#define AFIO_EXTICR1_EXTI1_PA           (0x0 << 4)
#define AFIO_EXTICR1_EXTI1_PB           (0x1 << 4)
#define AFIO_EXTICR1_EXTI1_PC           (0x2 << 4)
#define AFIO_EXTICR1_EXTI1_PD           (0x3 << 4)
#define AFIO_EXTICR1_EXTI1_PE           (0x4 << 4)
#define AFIO_EXTICR1_EXTI1_PF           (0x5 << 4)
#define AFIO_EXTICR1_EXTI1_PG           (0x6 << 4)
#define AFIO_EXTICR1_EXTI0              0xF
#define AFIO_EXTICR1_EXTI0_PA           0x0
#define AFIO_EXTICR1_EXTI0_PB           0x1
#define AFIO_EXTICR1_EXTI0_PC           0x2
#define AFIO_EXTICR1_EXTI0_PD           0x3
#define AFIO_EXTICR1_EXTI0_PE           0x4
#define AFIO_EXTICR1_EXTI0_PF           0x5
#define AFIO_EXTICR1_EXTI0_PG           0x6

/* External interrupt configuration register 2 */

#define AFIO_EXTICR2_EXTI7              (0xF << 12)
#define AFIO_EXTICR2_EXTI7_PA           (0x0 << 12)
#define AFIO_EXTICR2_EXTI7_PB           (0x1 << 12)
#define AFIO_EXTICR2_EXTI7_PC           (0x2 << 12)
#define AFIO_EXTICR2_EXTI7_PD           (0x3 << 12)
#define AFIO_EXTICR2_EXTI7_PE           (0x4 << 12)
#define AFIO_EXTICR2_EXTI7_PF           (0x5 << 12)
#define AFIO_EXTICR2_EXTI7_PG           (0x6 << 12)
#define AFIO_EXTICR2_EXTI6              (0xF << 8)
#define AFIO_EXTICR2_EXTI6_PA           (0x0 << 8)
#define AFIO_EXTICR2_EXTI6_PB           (0x1 << 8)
#define AFIO_EXTICR2_EXTI6_PC           (0x2 << 8)
#define AFIO_EXTICR2_EXTI6_PD           (0x3 << 8)
#define AFIO_EXTICR2_EXTI6_PE           (0x4 << 8)
#define AFIO_EXTICR2_EXTI6_PF           (0x5 << 8)
#define AFIO_EXTICR2_EXTI6_PG           (0x6 << 8)
#define AFIO_EXTICR2_EXTI5              (0xF << 4)
#define AFIO_EXTICR2_EXTI5_PA           (0x0 << 4)
#define AFIO_EXTICR2_EXTI5_PB           (0x1 << 4)
#define AFIO_EXTICR2_EXTI5_PC           (0x2 << 4)
#define AFIO_EXTICR2_EXTI5_PD           (0x3 << 4)
#define AFIO_EXTICR2_EXTI5_PE           (0x4 << 4)
#define AFIO_EXTICR2_EXTI5_PF           (0x5 << 4)
#define AFIO_EXTICR2_EXTI5_PG           (0x6 << 4)
#define AFIO_EXTICR2_EXTI4              0xF
#define AFIO_EXTICR2_EXTI4_PA           0x0
#define AFIO_EXTICR2_EXTI4_PB           0x1
#define AFIO_EXTICR2_EXTI4_PC           0x2
#define AFIO_EXTICR2_EXTI4_PD           0x3
#define AFIO_EXTICR2_EXTI4_PE           0x4
#define AFIO_EXTICR2_EXTI4_PF           0x5
#define AFIO_EXTICR2_EXTI4_PG           0x6

/* AF remap and debug I/O configuration register 2 */

#define AFIO_MAPR2_FSMC_NADV            BIT(10)
#define AFIO_MAPR2_TIM14_REMAP          BIT(9)
#define AFIO_MAPR2_TIM13_REMAP          BIT(8)
#define AFIO_MAPR2_TIM11_REMAP          BIT(7)
#define AFIO_MAPR2_TIM10_REMAP          BIT(6)
#define AFIO_MAPR2_TIM9_REMAP           BIT(5)

/*
 * AFIO convenience routines
 */

void afio_init(void);

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
typedef enum afio_remap_peripheral {
    AFIO_REMAP_ADC2_ETRGREG  = AFIO_MAPR_ADC2_ETRGREG_REMAP, /**<
        ADC 2 external trigger regular conversion remapping */
    AFIO_REMAP_ADC2_ETRGINJ  = AFIO_MAPR_ADC2_ETRGINJ_REMAP, /**<
        ADC 2 external trigger injected conversion remapping */
    AFIO_REMAP_ADC1_ETRGREG  = AFIO_MAPR_ADC1_ETRGREG_REMAP, /**<
        ADC 1 external trigger regular conversion remapping */
    AFIO_REMAP_ADC1_ETRGINJ  = AFIO_MAPR_ADC1_ETRGINJ_REMAP, /**<
        ADC 1 external trigger injected conversion remapping */
    AFIO_REMAP_TIM5CH4_I     = AFIO_MAPR_TIM5CH4_IREMAP, /**<
        Timer 5 channel 4 internal remapping */
    AFIO_REMAP_PD01          = AFIO_MAPR_PD01_REMAP, /**<
        Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
    AFIO_REMAP_CAN_1         = AFIO_MAPR_CAN_REMAP_PB8_PB9, /**<
        CAN alternate function remapping 1 (RX on PB8, TX on PB9) */
    AFIO_REMAP_CAN_2         = AFIO_MAPR_CAN_REMAP_PD0_PD1, /**<
        CAN alternate function remapping 2 (RX on PD0, TX on PD1) */
    AFIO_REMAP_TIM4          = AFIO_MAPR_TIM4_REMAP, /**<
        Timer 4 remapping */
    AFIO_REMAP_TIM3_PARTIAL  = AFIO_MAPR_TIM3_REMAP_PARTIAL, /**<
        Timer 3 partial remapping */
    AFIO_REMAP_TIM3_FULL      = AFIO_MAPR_TIM3_REMAP_FULL, /**<
        Timer 3 full remapping */
    AFIO_REMAP_TIM2_PARTIAL_1 = AFIO_MAPR_TIM2_REMAP_PA15_PB3_PA2_PA3, /**<
        Timer 2 partial remapping 1 (CH1 and ETR on PA15, CH2 on PB3, CH3
        on PA2, CH4 on PA3) */
    AFIO_REMAP_TIM2_PARTIAL_2 = AFIO_MAPR_TIM2_REMAP_PA0_PA1_PB10_PB11, /**<
        Timer 2 partial remapping 2 (CH1 and ETR on PA0, CH2 on PA1, CH3
        on PB10, CH4 on PB11) */
    AFIO_REMAP_TIM2_FULL      = AFIO_MAPR_TIM2_REMAP_FULL, /**<
        Timer 2 full remapping */
    AFIO_REMAP_USART2        = AFIO_MAPR_USART2_REMAP, /**<
        USART 2 remapping */
    AFIO_REMAP_USART1        = AFIO_MAPR_USART1_REMAP, /**<
        USART 1 remapping */
    AFIO_REMAP_I2C1          = AFIO_MAPR_I2C1_REMAP, /**<
        I2C 1 remapping */
    AFIO_REMAP_SPI1          = AFIO_MAPR_SPI1_REMAP, /**<
        SPI 1 remapping */
    AFIO_REMAP_FSMC_NADV     = (AFIO_MAPR2_FSMC_NADV |
                                AFIO_REMAP_USE_MAPR2), /**<
        NADV signal not connected */
    AFIO_REMAP_TIM14         = (AFIO_MAPR2_TIM14_REMAP |
                                AFIO_REMAP_USE_MAPR2), /**<
        Timer 14 remapping */
    AFIO_REMAP_TIM13         = (AFIO_MAPR2_TIM13_REMAP |
                                AFIO_REMAP_USE_MAPR2), /**<
        Timer 13 remapping */
    AFIO_REMAP_TIM11         = (AFIO_MAPR2_TIM11_REMAP |
                                AFIO_REMAP_USE_MAPR2), /**<
        Timer 11 remapping */
    AFIO_REMAP_TIM10         = (AFIO_MAPR2_TIM10_REMAP |
                                AFIO_REMAP_USE_MAPR2), /**<
        Timer 10 remapping */
    AFIO_REMAP_TIM9          = (AFIO_MAPR2_TIM9_REMAP |
                                AFIO_REMAP_USE_MAPR2) /**<
        Timer 9 */
} afio_remap_peripheral;

void afio_remap(afio_remap_peripheral p);

/**
 * @brief Debug port configuration
 *
 * Used to configure the behavior of JTAG and Serial Wire (SW) debug
 * ports and their associated GPIO pins.
 *
 * @see afio_cfg_debug_ports()
 */
typedef enum afio_debug_cfg {
    AFIO_DEBUG_FULL_SWJ = AFIO_MAPR_SWJ_CFG_FULL_SWJ, /**<
                                   Full Serial Wire and JTAG debug */
    AFIO_DEBUG_FULL_SWJ_NO_NJRST = AFIO_MAPR_SWJ_CFG_FULL_SWJ_NO_NJRST, /**<
                                   Full Serial Wire and JTAG, but no NJTRST. */
    AFIO_DEBUG_SW_ONLY = AFIO_MAPR_SWJ_CFG_NO_JTAG_SW, /**<
                                   Serial Wire debug only (JTAG-DP disabled,
                                   SW-DP enabled) */
    AFIO_DEBUG_NONE = AFIO_MAPR_SWJ_CFG_NO_JTAG_NO_SW /**<
                                   No debug; all JTAG and SW pins are free
                                   for use as GPIOs. */
} afio_debug_cfg;


#ifdef __cplusplus
}
#endif

#endif

