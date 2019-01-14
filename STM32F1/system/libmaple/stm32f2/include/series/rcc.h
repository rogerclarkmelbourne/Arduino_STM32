/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
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
 * @file libmaple/stm32f2/include/series/rcc.h
 * @brief STM32F2 reset and clock control (RCC) support.
 */

#ifndef _LIBMAPLE_STM32F2_RCC_H_
#define _LIBMAPLE_STM32F2_RCC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/** STM32F2 RCC register map type */
typedef struct rcc_reg_map {
    __IO uint32 CR;             /**< Clock control register */
    __IO uint32 PLLCFGR;        /**< PLL configuration register */
    __IO uint32 CFGR;           /**< Clock configuration register */
    __IO uint32 CIR;            /**< Clock interrupt register */
    __IO uint32 AHB1RSTR;       /**< AHB1 peripheral reset register */
    __IO uint32 AHB2RSTR;       /**< AHB2 peripheral reset register */
    __IO uint32 AHB3RSTR;       /**< AHB3 peripheral reset register */
    const uint32 RESERVED1;     /**< Reserved */
    __IO uint32 APB1RSTR;       /**< APB1 peripheral reset register */
    __IO uint32 APB2RSTR;       /**< APB2 peripheral reset register */
    const uint32 RESERVED2;     /**< Reserved */
    const uint32 RESERVED3;     /**< Reserved */
    __IO uint32 AHB1ENR;        /**< AHB1 peripheral clock enable register */
    __IO uint32 AHB2ENR;        /**< AHB2 peripheral clock enable register */
    __IO uint32 AHB3ENR;        /**< AHB3 peripheral clock enable register */
    const uint32 RESERVED4;     /**< Reserved */
    __IO uint32 APB1ENR;        /**< APB1 peripheral clock enable register */
    __IO uint32 APB2ENR;        /**< APB2 peripheral clock enable register */
    const uint32 RESERVED5;     /**< Reserved */
    const uint32 RESERVED6;     /**< Reserved */
    __IO uint32 AHB1LPENR;      /**< AHB1 peripheral clock enable in
                                   low power mode register */
    __IO uint32 AHB2LPENR;      /**< AHB2 peripheral clock enable in
                                   low power mode register */
    __IO uint32 AHB3LPENR;      /**< AHB3 peripheral clock enable in
                                   low power mode register */
    const uint32 RESERVED7;     /**< Reserved */
    __IO uint32 APB1LPENR;      /**< APB1 peripheral clock enable in
                                   low power mode register */
    __IO uint32 APB2LPENR;      /**< APB2 peripheral clock enable in
                                   low power mode register */
    const uint32 RESERVED8;     /**< Reserved */
    const uint32 RESERVED9;     /**< Reserved */
    __IO uint32 BDCR;           /**< Backup domain control register */
    __IO uint32 CSR;            /**< Clock control and status register */
    const uint32 RESERVED10;     /**< Reserved */
    const uint32 RESERVED11;    /**< Reserved */
    __IO uint32 SSCGR;          /**< Spread spectrum clock generation
                                   register */
    __IO uint32 PLLI2SCFGR;     /**< PLLI2S configuration register */
} rcc_reg_map;

#define RCC_BASE                        ((struct rcc_reg_map*)0x40023800)

/*
 * Register bit definitions
 */

/* Clock control register */

#define RCC_CR_PLLI2SRDY_BIT            27
#define RCC_CR_PLLI2SON_BIT             26
#define RCC_CR_PLLRDY_BIT               25
#define RCC_CR_PLLON_BIT                24
#define RCC_CR_CSSON_BIT                19
#define RCC_CR_HSEBYP_BIT               18
#define RCC_CR_HSERDY_BIT               17
#define RCC_CR_HSEON_BIT                16
#define RCC_CR_HSIRDY_BIT               1
#define RCC_CR_HSION_BIT                0

#define RCC_CR_PLLI2SRDY                (1U << RCC_CR_PLLI2SRDY_BIT)
#define RCC_CR_PLLI2SON                 (1U << RCC_CR_PLLI2SON_BIT)
#define RCC_CR_PLLRDY                   (1U << RCC_CR_PLLRDY_BIT)
#define RCC_CR_PLLON                    (1U << RCC_CR_PLLON_BIT)
#define RCC_CR_CSSON                    (1U << RCC_CR_CSSON_BIT)
#define RCC_CR_HSEBYP                   (1U << RCC_CR_HSEBYP_BIT)
#define RCC_CR_HSERDY                   (1U << RCC_CR_HSERDY_BIT)
#define RCC_CR_HSEON                    (1U << RCC_CR_HSEON_BIT)
#define RCC_CR_HSICAL                   (0xFF << 8)
#define RCC_CR_HSITRIM                  (0x1F << 3)
#define RCC_CR_HSIRDY                   (1U << RCC_CR_HSIRDY_BIT)
#define RCC_CR_HSION                    (1U << RCC_CR_HSION_BIT)

/* PLL configuration register */

#define RCC_PLLCFGR_PLLSRC_BIT          22

#define RCC_PLLCFGR_PLLQ                (0xF << 24)
#define RCC_PLLCFGR_PLLSRC              (1U << RCC_PLLCFGR_PLLSRC_BIT)
#define RCC_PLLCFGR_PLLSRC_HSI          (0x0 << RCC_PLLCFGR_PLLSRC_BIT)
#define RCC_PLLCFGR_PLLSRC_HSE          (0x1 << RCC_PLLCFGR_PLLSRC_BIT)
#define RCC_PLLCFGR_PLLP                (0x3 << 16)
#define RCC_PLLCFGR_PLLN                (0x1FF << 6)
#define RCC_PLLCFGR_PLLM                0x1F

/* Clock configuration register */

#define RCC_CFGR_I2SSRC_BIT             23

#define RCC_CFGR_MCO2                   (0x3 << 30)
#define RCC_CFGR_MCO2_SYSCLK            (0x0 << 30)
#define RCC_CFGR_MCO2_PLLI2S            (0x1 << 30)
#define RCC_CFGR_MCO2_HSE               (0x2 << 30)
#define RCC_CFGR_MCO2_PLL               (0x3 << 30)

#define RCC_CFGR_MCO2PRE                (0x7 << 27)
#define RCC_CFGR_MCO2PRE_DIV_1          (0x0 << 27)
#define RCC_CFGR_MCO2PRE_DIV_2          (0x4 << 27)
#define RCC_CFGR_MCO2PRE_DIV_3          (0x5 << 27)
#define RCC_CFGR_MCO2PRE_DIV_4          (0x6 << 27)
#define RCC_CFGR_MCO2PRE_DIV_5          (0x7 << 27)

#define RCC_CFGR_MCO1PRE                (0x7 << 24)
#define RCC_CFGR_MCO1PRE_DIV_1          (0x0 << 24)
#define RCC_CFGR_MCO1PRE_DIV_2          (0x4 << 24)
#define RCC_CFGR_MCO1PRE_DIV_3          (0x5 << 24)
#define RCC_CFGR_MCO1PRE_DIV_4          (0x6 << 24)
#define RCC_CFGR_MCO1PRE_DIV_5          (0x7 << 24)

#define RCC_CFGR_I2SSRC                 (1U << RCC_CFGR_I2SSRC_BIT)
#define RCC_CFGR_I2SSRC_PLLI2S          (0 << RCC_CFGR_I2SSRC_BIT)
#define RCC_CFGR_I2SSRC_I2S_CKIN        (1 << RCC_CFGR_I2SSRC_BIT)

#define RCC_CFGR_MCO1                   (0x3 << 21)
#define RCC_CFGR_MCO1_HSI               (0x0 << 21)
#define RCC_CFGR_MCO1_LSE               (0x1 << 21)
#define RCC_CFGR_MCO1_HSE               (0x2 << 21)
#define RCC_CFGR_MCO1_PLL               (0x3 << 21)

#define RCC_CFGR_RTCPRE                 (0x1F << 16)

/* Skipped: all the 0b0xx values meaning "not divided" */
#define RCC_CFGR_PPRE2                  (0x7 << 13)
#define RCC_CFGR_PPRE2_AHB_DIV_2        (0x4 << 13)
#define RCC_CFGR_PPRE2_AHB_DIV_4        (0x5 << 13)
#define RCC_CFGR_PPRE2_AHB_DIV_8        (0x6 << 13)
#define RCC_CFGR_PPRE2_AHB_DIV_16       (0x7 << 13)

/* Skipped: all the 0b0xx values meaning "not divided" */
#define RCC_CFGR_PPRE1                  (0x7 << 10)
#define RCC_CFGR_PPRE1_AHB_DIV_2        (0x4 << 10)
#define RCC_CFGR_PPRE1_AHB_DIV_4        (0x5 << 10)
#define RCC_CFGR_PPRE1_AHB_DIV_8        (0x6 << 10)
#define RCC_CFGR_PPRE1_AHB_DIV_16       (0x7 << 10)

/* Skipped: all the 0b0xxx values meaning "not divided" */
#define RCC_CFGR_HPRE                   (0xF << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_2      (0x8 << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_4      (0x9 << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_8      (0xA << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_16     (0xB << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_64     (0xC << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_128    (0xD << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_256    (0xE << 4)
#define RCC_CFGR_HPRE_SYSCLK_DIV_512    (0xF << 4)

#define RCC_CFGR_SWS                    (0x3 << 2)
#define RCC_CFGR_SWS_HSI                (0x0 << 2)
#define RCC_CFGR_SWS_HSE                (0x1 << 2)
#define RCC_CFGR_SWS_PLL                (0x2 << 2)

#define RCC_CFGR_SW                     0x3
#define RCC_CFGR_SW_HSI                 0x0
#define RCC_CFGR_SW_HSE                 0x1
#define RCC_CFGR_SW_PLL                 0x2

/* Clock interrupt register */

#define RCC_CIR_CSSC_BIT                23

#define RCC_CIR_PLLI2SRDYC_BIT          21
#define RCC_CIR_PLLRDYC_BIT             20
#define RCC_CIR_HSERDYC_BIT             19
#define RCC_CIR_HSIRDYC_BIT             18
#define RCC_CIR_LSERDYC_BIT             17
#define RCC_CIR_LSIRDYC_BIT             16

#define RCC_CIR_PLLI2SRDYIE_BIT         13
#define RCC_CIR_PLLRDYIE_BIT            12
#define RCC_CIR_HSERDYIE_BIT            11
#define RCC_CIR_HSIRDYIE_BIT            10
#define RCC_CIR_LSERDYIE_BIT            9
#define RCC_CIR_LSIRDYIE_BIT            8

#define RCC_CIR_CSSF_BIT                7

#define RCC_CIR_PLLI2SRDYF_BIT          5
#define RCC_CIR_PLLRDYF_BIT             4
#define RCC_CIR_HSERDYF_BIT             3
#define RCC_CIR_HSIRDYF_BIT             2
#define RCC_CIR_LSERDYF_BIT             1
#define RCC_CIR_LSIRDYF_BIT             0

#define RCC_CIR_CSSC                    (1U << RCC_CIR_CSSC_BIT)

#define RCC_CIR_PLLI2SRDYC              (1U << RCC_CIR_PLLI2SRDYC_BIT)
#define RCC_CIR_PLLRDYC                 (1U << RCC_CIR_PLLRDYC_BIT)
#define RCC_CIR_HSERDYC                 (1U << RCC_CIR_HSERDYC_BIT)
#define RCC_CIR_HSIRDYC                 (1U << RCC_CIR_HSIRDYC_BIT)
#define RCC_CIR_LSERDYC                 (1U << RCC_CIR_LSERDYC_BIT)
#define RCC_CIR_LSIRDYC                 (1U << RCC_CIR_LSIRDYC_BIT)

#define RCC_CIR_PLLI2SRDYIE             (1U << RCC_CIR_PLLI2SRDYIE_BIT)
#define RCC_CIR_PLLRDYIE                (1U << RCC_CIR_PLLRDYIE_BIT)
#define RCC_CIR_HSERDYIE                (1U << RCC_CIR_HSERDYIE_BIT)
#define RCC_CIR_HSIRDYIE                (1U << RCC_CIR_HSIRDYIE_BIT)
#define RCC_CIR_LSERDYIE                (1U << RCC_CIR_LSERDYIE_BIT)
#define RCC_CIR_LSIRDYIE                (1U << RCC_CIR_LSIRDYIE_BIT)

#define RCC_CIR_CSSF                    (1U << RCC_CIR_CSSF_BIT)

#define RCC_CIR_PLLI2SRDYF              (1U << RCC_CIR_PLLI2SRDYF_BIT)
#define RCC_CIR_PLLRDYF                 (1U << RCC_CIR_PLLRDYF_BIT)
#define RCC_CIR_HSERDYF                 (1U << RCC_CIR_HSERDYF_BIT)
#define RCC_CIR_HSIRDYF                 (1U << RCC_CIR_HSIRDYF_BIT)
#define RCC_CIR_LSERDYF                 (1U << RCC_CIR_LSERDYF_BIT)
#define RCC_CIR_LSIRDYF                 (1U << RCC_CIR_LSIRDYF_BIT)

/* AHB1 peripheral reset register */

#define RCC_AHB1RSTR_OTGHSRST_BIT       29
#define RCC_AHB1RSTR_ETHMACRST_BIT      25
#define RCC_AHB1RSTR_DMA2RST_BIT        22
#define RCC_AHB1RSTR_DMA1RST_BIT        21
#define RCC_AHB1RSTR_CRCRST_BIT         12
#define RCC_AHB1RSTR_GPIOIRST_BIT       8
#define RCC_AHB1RSTR_GPIOHRST_BIT       7
#define RCC_AHB1RSTR_GPIOGRST_BIT       6
#define RCC_AHB1RSTR_GPIOFRST_BIT       5
#define RCC_AHB1RSTR_GPIOERST_BIT       4
#define RCC_AHB1RSTR_GPIODRST_BIT       3
#define RCC_AHB1RSTR_GPIOCRST_BIT       2
#define RCC_AHB1RSTR_GPIOBRST_BIT       1
#define RCC_AHB1RSTR_GPIOARST_BIT       0

#define RCC_AHB1RSTR_OTGHSRST           (1U << RCC_AHB1RSTR_OTGHSRST_BIT)
#define RCC_AHB1RSTR_ETHMACRST          (1U << RCC_AHB1RSTR_ETHMACRST_BIT)
#define RCC_AHB1RSTR_DMA2RST            (1U << RCC_AHB1RSTR_DMA2RST_BIT)
#define RCC_AHB1RSTR_DMA1RST            (1U << RCC_AHB1RSTR_DMA1RST_BIT)
#define RCC_AHB1RSTR_CRCRST             (1U << RCC_AHB1RSTR_CRCRST_BIT)
#define RCC_AHB1RSTR_GPIOIRST           (1U << RCC_AHB1RSTR_GPIOIRST_BIT)
#define RCC_AHB1RSTR_GPIOHRST           (1U << RCC_AHB1RSTR_GPIOHRST_BIT)
#define RCC_AHB1RSTR_GPIOGRST           (1U << RCC_AHB1RSTR_GPIOGRST_BIT)
#define RCC_AHB1RSTR_GPIOFRST           (1U << RCC_AHB1RSTR_GPIOFRST_BIT)
#define RCC_AHB1RSTR_GPIOERST           (1U << RCC_AHB1RSTR_GPIOERST_BIT)
#define RCC_AHB1RSTR_GPIODRST           (1U << RCC_AHB1RSTR_GPIODRST_BIT)
#define RCC_AHB1RSTR_GPIOCRST           (1U << RCC_AHB1RSTR_GPIOCRST_BIT)
#define RCC_AHB1RSTR_GPIOBRST           (1U << RCC_AHB1RSTR_GPIOBRST_BIT)
#define RCC_AHB1RSTR_GPIOARST           (1U << RCC_AHB1RSTR_GPIOARST_BIT)

/* AHB2 peripheral reset register */

#define RCC_AHB2RSTR_OTGFSRST_BIT       7
#define RCC_AHB2RSTR_RNGRST_BIT         6
#define RCC_AHB2RSTR_HASHRST_BIT        5
#define RCC_AHB2RSTR_CRYPRST_BIT        4
#define RCC_AHB2RSTR_DCMIRST_BIT        0

#define RCC_AHB2RSTR_OTGFSRST           (1U << RCC_AHB2RSTR_OTGFSRST_BIT)
#define RCC_AHB2RSTR_RNGRST             (1U << RCC_AHB2RSTR_RNGRST_BIT)
#define RCC_AHB2RSTR_HASHRST            (1U << RCC_AHB2RSTR_HASHRST_BIT)
#define RCC_AHB2RSTR_CRYPRST            (1U << RCC_AHB2RSTR_CRYPRST_BIT)
#define RCC_AHB2RSTR_DCMIRST            (1U << RCC_AHB2RSTR_DCMIRST_BIT)

/* AHB3 peripheral reset register */

#define RCC_AHB3RSTR_FSMCRST_BIT        0

#define RCC_AHB3RSTR_FSMCRST            (1U << RCC_AHB3RSTR_FSMCRST_BIT)

/* APB1 peripheral reset register */

#define RCC_APB1RSTR_DACRST_BIT         29
#define RCC_APB1RSTR_PWRRST_BIT         28
#define RCC_APB1RSTR_CAN2RST_BIT        26
#define RCC_APB1RSTR_CAN1RST_BIT        25
#define RCC_APB1RSTR_I2C3RST_BIT        23
#define RCC_APB1RSTR_I2C2RST_BIT        22
#define RCC_APB1RSTR_I2C1RST_BIT        21
#define RCC_APB1RSTR_UART5RST_BIT       20
#define RCC_APB1RSTR_UART4RST_BIT       19
#define RCC_APB1RSTR_UART3RST_BIT       18
#define RCC_APB1RSTR_UART2RST_BIT       17
#define RCC_APB1RSTR_SPI3RST_BIT        15
#define RCC_APB1RSTR_SPI2RST_BIT        14
#define RCC_APB1RSTR_WWDGRST_BIT        11
#define RCC_APB1RSTR_TIM14RST_BIT       8
#define RCC_APB1RSTR_TIM13RST_BIT       7
#define RCC_APB1RSTR_TIM12RST_BIT       6
#define RCC_APB1RSTR_TIM7RST_BIT        5
#define RCC_APB1RSTR_TIM6RST_BIT        4
#define RCC_APB1RSTR_TIM5RST_BIT        3
#define RCC_APB1RSTR_TIM4RST_BIT        2
#define RCC_APB1RSTR_TIM3RST_BIT        1
#define RCC_APB1RSTR_TIM2RST_BIT        0

#define RCC_APB1RSTR_DACRST             (1U << RCC_APB1RSTR_DACRST_BIT)
#define RCC_APB1RSTR_PWRRST             (1U << RCC_APB1RSTR_PWRRST_BIT)
#define RCC_APB1RSTR_CAN2RST            (1U << RCC_APB1RSTR_CAN2RST_BIT)
#define RCC_APB1RSTR_CAN1RST            (1U << RCC_APB1RSTR_CAN1RST_BIT)
#define RCC_APB1RSTR_I2C3RST            (1U << RCC_APB1RSTR_I2C3RST_BIT)
#define RCC_APB1RSTR_I2C2RST            (1U << RCC_APB1RSTR_I2C2RST_BIT)
#define RCC_APB1RSTR_I2C1RST            (1U << RCC_APB1RSTR_I2C1RST_BIT)
#define RCC_APB1RSTR_UART5RST           (1U << RCC_APB1RSTR_UART5RST_BIT)
#define RCC_APB1RSTR_UART4RST           (1U << RCC_APB1RSTR_UART4RST_BIT)
#define RCC_APB1RSTR_UART3RST           (1U << RCC_APB1RSTR_UART3RST_BIT)
#define RCC_APB1RSTR_UART2RST           (1U << RCC_APB1RSTR_UART2RST_BIT)
#define RCC_APB1RSTR_SPI3RST            (1U << RCC_APB1RSTR_SPI3RST_BIT)
#define RCC_APB1RSTR_SPI2RST            (1U << RCC_APB1RSTR_SPI2RST_BIT)
#define RCC_APB1RSTR_WWDGRST            (1U << RCC_APB1RSTR_WWDGRST_BIT)
#define RCC_APB1RSTR_TIM14RST           (1U << RCC_APB1RSTR_TIM14RST_BIT)
#define RCC_APB1RSTR_TIM13RST           (1U << RCC_APB1RSTR_TIM13RST_BIT)
#define RCC_APB1RSTR_TIM12RST           (1U << RCC_APB1RSTR_TIM12RST_BIT)
#define RCC_APB1RSTR_TIM7RST            (1U << RCC_APB1RSTR_TIM7RST_BIT)
#define RCC_APB1RSTR_TIM6RST            (1U << RCC_APB1RSTR_TIM6RST_BIT)
#define RCC_APB1RSTR_TIM5RST            (1U << RCC_APB1RSTR_TIM5RST_BIT)
#define RCC_APB1RSTR_TIM4RST            (1U << RCC_APB1RSTR_TIM4RST_BIT)
#define RCC_APB1RSTR_TIM3RST            (1U << RCC_APB1RSTR_TIM3RST_BIT)
#define RCC_APB1RSTR_TIM2RST            (1U << RCC_APB1RSTR_TIM2RST_BIT)

/* APB2 peripheral reset register */

#define RCC_APB2RSTR_TIM11RST_BIT       18
#define RCC_APB2RSTR_TIM10RST_BIT       17
#define RCC_APB2RSTR_TIM9RST_BIT        16
#define RCC_APB2RSTR_SYSCFGRST_BIT      14
#define RCC_APB2RSTR_SPI1RST_BIT        12
#define RCC_APB2RSTR_SDIORST_BIT        11
#define RCC_APB2RSTR_ADCRST_BIT         8
#define RCC_APB2RSTR_USART6RST_BIT      5
#define RCC_APB2RSTR_USART1RST_BIT      4
#define RCC_APB2RSTR_TIM8RST_BIT        1
#define RCC_APB2RSTR_TIM1RST_BIT        0

#define RCC_APB2RSTR_TIM11RST           (1U << RCC_APB2RSTR_TIM11RST_BIT)
#define RCC_APB2RSTR_TIM10RST           (1U << RCC_APB2RSTR_TIM10RST_BIT)
#define RCC_APB2RSTR_TIM9RST            (1U << RCC_APB2RSTR_TIM9RST_BIT)
#define RCC_APB2RSTR_SYSCFGRST          (1U << RCC_APB2RSTR_SYSCFGRST_BIT)
#define RCC_APB2RSTR_SPI1RST            (1U << RCC_APB2RSTR_SPI1RST_BIT)
#define RCC_APB2RSTR_SDIORST            (1U << RCC_APB2RSTR_SDIORST_BIT)
#define RCC_APB2RSTR_ADCRST             (1U << RCC_APB2RSTR_ADCRST_BIT)
#define RCC_APB2RSTR_USART6RST          (1U << RCC_APB2RSTR_USART6RST_BIT)
#define RCC_APB2RSTR_USART1RST          (1U << RCC_APB2RSTR_USART1RST_BIT)
#define RCC_APB2RSTR_TIM8RST            (1U << RCC_APB2RSTR_TIM8RST_BIT)
#define RCC_APB2RSTR_TIM1RST            (1U << RCC_APB2RSTR_TIM1RST_BIT)

/* AHB1 peripheral clock enable register */

#define RCC_AHB1ENR_OTGHSULPIEN_BIT     30
#define RCC_AHB1ENR_OTGHSEN_BIT         29
#define RCC_AHB1ENR_ETHMACPTPEN_BIT     28
#define RCC_AHB1ENR_ETHMACRXEN_BIT      27
#define RCC_AHB1ENR_ETHMACTXEN_BIT      26
#define RCC_AHB1ENR_ETHMACEN_BIT        25
#define RCC_AHB1ENR_DMA2EN_BIT          22
#define RCC_AHB1ENR_DMA1EN_BIT          21
#define RCC_AHB1ENR_BKPSRAMEN_BIT       18
#define RCC_AHB1ENR_CRCEN_BIT           12
#define RCC_AHB1ENR_GPIOIEN_BIT         8
#define RCC_AHB1ENR_GPIOHEN_BIT         7
#define RCC_AHB1ENR_GPIOGEN_BIT         6
#define RCC_AHB1ENR_GPIOFEN_BIT         5
#define RCC_AHB1ENR_GPIOEEN_BIT         4
#define RCC_AHB1ENR_GPIODEN_BIT         3
#define RCC_AHB1ENR_GPIOCEN_BIT         2
#define RCC_AHB1ENR_GPIOBEN_BIT         1
#define RCC_AHB1ENR_GPIOAEN_BIT         0

#define RCC_AHB1ENR_OTGHSULPIEN         (1U << RCC_AHB1ENR_OTGHSULPIEN_BIT)
#define RCC_AHB1ENR_OTGHSEN             (1U << RCC_AHB1ENR_OTGHSEN_BIT)
#define RCC_AHB1ENR_ETHMACPTPEN         (1U << RCC_AHB1ENR_ETHMACPTPEN_BIT)
#define RCC_AHB1ENR_ETHMACRXEN          (1U << RCC_AHB1ENR_ETHMACRXEN_BIT)
#define RCC_AHB1ENR_ETHMACTXEN          (1U << RCC_AHB1ENR_ETHMACTXEN_BIT)
#define RCC_AHB1ENR_ETHMACEN            (1U << RCC_AHB1ENR_ETHMACEN_BIT)
#define RCC_AHB1ENR_DMA2EN              (1U << RCC_AHB1ENR_DMA2EN_BIT)
#define RCC_AHB1ENR_DMA1EN              (1U << RCC_AHB1ENR_DMA1EN_BIT)
#define RCC_AHB1ENR_BKPSRAMEN           (1U << RCC_AHB1ENR_BKPSRAMEN_BIT)
#define RCC_AHB1ENR_CRCEN               (1U << RCC_AHB1ENR_CRCEN_BIT)
#define RCC_AHB1ENR_GPIOIEN             (1U << RCC_AHB1ENR_GPIOIEN_BIT)
#define RCC_AHB1ENR_GPIOHEN             (1U << RCC_AHB1ENR_GPIOHEN_BIT)
#define RCC_AHB1ENR_GPIOGEN             (1U << RCC_AHB1ENR_GPIOGEN_BIT)
#define RCC_AHB1ENR_GPIOFEN             (1U << RCC_AHB1ENR_GPIOFEN_BIT)
#define RCC_AHB1ENR_GPIOEEN             (1U << RCC_AHB1ENR_GPIOEEN_BIT)
#define RCC_AHB1ENR_GPIODEN             (1U << RCC_AHB1ENR_GPIODEN_BIT)
#define RCC_AHB1ENR_GPIOCEN             (1U << RCC_AHB1ENR_GPIOCEN_BIT)
#define RCC_AHB1ENR_GPIOBEN             (1U << RCC_AHB1ENR_GPIOBEN_BIT)
#define RCC_AHB1ENR_GPIOAEN             (1U << RCC_AHB1ENR_GPIOAEN_BIT)

/* AHB2 peripheral clock enable register */

#define RCC_AHB2ENR_OTGFSEN_BIT         7
#define RCC_AHB2ENR_RNGEN_BIT           6
#define RCC_AHB2ENR_HASHEN_BIT          5
#define RCC_AHB2ENR_CRYPEN_BIT          4
#define RCC_AHB2ENR_DCMIEN_BIT          0

#define RCC_AHB2ENR_OTGFSEN             (1U << RCC_AHB2ENR_OTGFSEN_BIT)
#define RCC_AHB2ENR_RNGEN               (1U << RCC_AHB2ENR_RNGEN_BIT)
#define RCC_AHB2ENR_HASHEN              (1U << RCC_AHB2ENR_HASHEN_BIT)
#define RCC_AHB2ENR_CRYPEN              (1U << RCC_AHB2ENR_CRYPEN_BIT)
#define RCC_AHB2ENR_DCMIEN              (1U << RCC_AHB2ENR_DCMIEN_BIT)

/* AHB3 peripheral clock enable register */

#define RCC_AHB3ENR_FSMCEN_BIT          0

#define RCC_AHB3ENR_FSMCEN              (1U << RCC_AHB3ENR_FSMCEN_BIT)

/* APB1 peripheral clock enable register */

#define RCC_APB1ENR_DACEN_BIT           29
#define RCC_APB1ENR_PWREN_BIT           28
#define RCC_APB1ENR_CAN2EN_BIT          26
#define RCC_APB1ENR_CAN1EN_BIT          25
#define RCC_APB1ENR_I2C3EN_BIT          23
#define RCC_APB1ENR_I2C2EN_BIT          22
#define RCC_APB1ENR_I2C1EN_BIT          21
#define RCC_APB1ENR_UART5EN_BIT         20
#define RCC_APB1ENR_UART4EN_BIT         19
#define RCC_APB1ENR_USART3EN_BIT        18
#define RCC_APB1ENR_USART2EN_BIT        17
#define RCC_APB1ENR_SPI3EN_BIT          15
#define RCC_APB1ENR_SPI2EN_BIT          14
#define RCC_APB1ENR_WWDGEN_BIT          11
#define RCC_APB1ENR_TIM14EN_BIT         8
#define RCC_APB1ENR_TIM13EN_BIT         7
#define RCC_APB1ENR_TIM12EN_BIT         6
#define RCC_APB1ENR_TIM7EN_BIT          5
#define RCC_APB1ENR_TIM6EN_BIT          4
#define RCC_APB1ENR_TIM5EN_BIT          3
#define RCC_APB1ENR_TIM4EN_BIT          2
#define RCC_APB1ENR_TIM3EN_BIT          1
#define RCC_APB1ENR_TIM2EN_BIT          0

#define RCC_APB1ENR_DACEN               (1U << RCC_APB1ENR_DACEN_BIT)
#define RCC_APB1ENR_PWREN               (1U << RCC_APB1ENR_PWREN_BIT)
#define RCC_APB1ENR_CAN2EN              (1U << RCC_APB1ENR_CAN2EN_BIT)
#define RCC_APB1ENR_CAN1EN              (1U << RCC_APB1ENR_CAN1EN_BIT)
#define RCC_APB1ENR_I2C3EN              (1U << RCC_APB1ENR_I2C3EN_BIT)
#define RCC_APB1ENR_I2C2EN              (1U << RCC_APB1ENR_I2C2EN_BIT)
#define RCC_APB1ENR_I2C1EN              (1U << RCC_APB1ENR_I2C1EN_BIT)
#define RCC_APB1ENR_UART5EN             (1U << RCC_APB1ENR_UART5EN_BIT)
#define RCC_APB1ENR_UART4EN             (1U << RCC_APB1ENR_UART4EN_BIT)
#define RCC_APB1ENR_USART3EN            (1U << RCC_APB1ENR_USART3EN_BIT)
#define RCC_APB1ENR_USART2EN            (1U << RCC_APB1ENR_USART2EN_BIT)
#define RCC_APB1ENR_SPI3EN              (1U << RCC_APB1ENR_SPI3EN_BIT)
#define RCC_APB1ENR_SPI2EN              (1U << RCC_APB1ENR_SPI2EN_BIT)
#define RCC_APB1ENR_WWDGEN              (1U << RCC_APB1ENR_WWDGEN_BIT)
#define RCC_APB1ENR_TIM14EN             (1U << RCC_APB1ENR_TIM14EN_BIT)
#define RCC_APB1ENR_TIM13EN             (1U << RCC_APB1ENR_TIM13EN_BIT)
#define RCC_APB1ENR_TIM12EN             (1U << RCC_APB1ENR_TIM12EN_BIT)
#define RCC_APB1ENR_TIM7EN              (1U << RCC_APB1ENR_TIM7EN_BIT)
#define RCC_APB1ENR_TIM6EN              (1U << RCC_APB1ENR_TIM6EN_BIT)
#define RCC_APB1ENR_TIM5EN              (1U << RCC_APB1ENR_TIM5EN_BIT)
#define RCC_APB1ENR_TIM4EN              (1U << RCC_APB1ENR_TIM4EN_BIT)
#define RCC_APB1ENR_TIM3EN              (1U << RCC_APB1ENR_TIM3EN_BIT)
#define RCC_APB1ENR_TIM2EN              (1U << RCC_APB1ENR_TIM2EN_BIT)

/* APB2 peripheral clock enable register */

#define RCC_APB2ENR_TIM11EN_BIT         18
#define RCC_APB2ENR_TIM10EN_BIT         17
#define RCC_APB2ENR_TIM9EN_BIT          16
#define RCC_APB2ENR_SYSCFGEN_BIT        14
#define RCC_APB2ENR_SPI1EN_BIT          12
#define RCC_APB2ENR_SDIOEN_BIT          11
#define RCC_APB2ENR_ADC3EN_BIT          10
#define RCC_APB2ENR_ADC2EN_BIT          9
#define RCC_APB2ENR_ADC1EN_BIT          8
#define RCC_APB2ENR_USART6EN_BIT        5
#define RCC_APB2ENR_USART1EN_BIT        4
#define RCC_APB2ENR_TIM8EN_BIT          1
#define RCC_APB2ENR_TIM1EN_BIT          0

#define RCC_APB2ENR_TIM11EN             (1U << RCC_APB2ENR_TIM11EN_BIT)
#define RCC_APB2ENR_TIM10EN             (1U << RCC_APB2ENR_TIM10EN_BIT)
#define RCC_APB2ENR_TIM9EN              (1U << RCC_APB2ENR_TIM9EN_BIT)
#define RCC_APB2ENR_SYSCFGEN            (1U << RCC_APB2ENR_SYSCFGEN_BIT)
#define RCC_APB2ENR_SPI1EN              (1U << RCC_APB2ENR_SPI1EN_BIT)
#define RCC_APB2ENR_SDIOEN              (1U << RCC_APB2ENR_SDIOEN_BIT)
#define RCC_APB2ENR_ADC3EN              (1U << RCC_APB2ENR_ADC3EN_BIT)
#define RCC_APB2ENR_ADC2EN              (1U << RCC_APB2ENR_ADC2EN_BIT)
#define RCC_APB2ENR_ADC1EN              (1U << RCC_APB2ENR_ADC1EN_BIT)
#define RCC_APB2ENR_USART6EN            (1U << RCC_APB2ENR_USART6EN_BIT)
#define RCC_APB2ENR_USART1EN            (1U << RCC_APB2ENR_USART1EN_BIT)
#define RCC_APB2ENR_TIM8EN              (1U << RCC_APB2ENR_TIM8EN_BIT)
#define RCC_APB2ENR_TIM1EN              (1U << RCC_APB2ENR_TIM1EN_BIT)

/* AHB1 peripheral clock enable in low power mode register */

#define RCC_AHB1LPENR_OTGHSULPILPEN_BIT 30
#define RCC_AHB1LPENR_OTGHSLPEN_BIT     29
#define RCC_AHB1LPENR_ETHMACPTPLPEN_BIT 28
#define RCC_AHB1LPENR_ETHMACRXLPEN_BIT  27
#define RCC_AHB1LPENR_ETHMACTXLPEN_BIT  26
#define RCC_AHB1LPENR_ETHMACLPEN_BIT    25
#define RCC_AHB1LPENR_DMA2LPEN_BIT      22
#define RCC_AHB1LPENR_DMA1LPEN_BIT      21
#define RCC_AHB1LPENR_BKPSRAMLPEN_BIT   18
#define RCC_AHB1LPENR_SRAM2LPEN_BIT     17
#define RCC_AHB1LPENR_SRAM1LPEN_BIT     16
#define RCC_AHB1LPENR_FLITFLPEN_BIT     15
#define RCC_AHB1LPENR_CRCLPEN_BIT       12
#define RCC_AHB1LPENR_GPIOILPEN_BIT     8
#define RCC_AHB1LPENR_GPIOGLPEN_BIT     6
#define RCC_AHB1LPENR_GPIOFLPEN_BIT     5
#define RCC_AHB1LPENR_GPIOELPEN_BIT     4
#define RCC_AHB1LPENR_GPIODLPEN_BIT     3
#define RCC_AHB1LPENR_GPIOCLPEN_BIT     2
#define RCC_AHB1LPENR_GPIOBLPEN_BIT     1
#define RCC_AHB1LPENR_GPIOALPEN_BIT     0

#define RCC_AHB1LPENR_OTGHSULPILPEN     (1U << RCC_AHB1LPENR_OTGHSULPILPEN_BIT)
#define RCC_AHB1LPENR_OTGHSLPEN         (1U << RCC_AHB1LPENR_OTGHSLPEN_BIT)
#define RCC_AHB1LPENR_ETHMACPTPLPEN     (1U << RCC_AHB1LPENR_ETHMACPTPLPEN_BIT)
#define RCC_AHB1LPENR_ETHMACRXLPEN      (1U << RCC_AHB1LPENR_ETHMACRXLPEN_BIT)
#define RCC_AHB1LPENR_ETHMACTXLPEN      (1U << RCC_AHB1LPENR_ETHMACTXLPEN_BIT)
#define RCC_AHB1LPENR_ETHMACLPEN        (1U << RCC_AHB1LPENR_ETHMACLPEN_BIT)
#define RCC_AHB1LPENR_DMA2LPEN          (1U << RCC_AHB1LPENR_DMA2LPEN_BIT)
#define RCC_AHB1LPENR_DMA1LPEN          (1U << RCC_AHB1LPENR_DMA1LPEN_BIT)
#define RCC_AHB1LPENR_BKPSRAMLPEN       (1U << RCC_AHB1LPENR_BKPSRAMLPEN_BIT)
#define RCC_AHB1LPENR_SRAM2LPEN         (1U << RCC_AHB1LPENR_SRAM2LPEN_BIT)
#define RCC_AHB1LPENR_SRAM1LPEN         (1U << RCC_AHB1LPENR_SRAM1LPEN_BIT)
#define RCC_AHB1LPENR_FLITFLPEN         (1U << RCC_AHB1LPENR_FLITFLPEN_BIT)
#define RCC_AHB1LPENR_CRCLPEN           (1U << RCC_AHB1LPENR_CRCLPEN_BIT)
#define RCC_AHB1LPENR_GPIOILPEN         (1U << RCC_AHB1LPENR_GPIOILPEN_BIT)
#define RCC_AHB1LPENR_GPIOGLPEN         (1U << RCC_AHB1LPENR_GPIOGLPEN_BIT)
#define RCC_AHB1LPENR_GPIOFLPEN         (1U << RCC_AHB1LPENR_GPIOFLPEN_BIT)
#define RCC_AHB1LPENR_GPIOELPEN         (1U << RCC_AHB1LPENR_GPIOELPEN_BIT)
#define RCC_AHB1LPENR_GPIODLPEN         (1U << RCC_AHB1LPENR_GPIODLPEN_BIT)
#define RCC_AHB1LPENR_GPIOCLPEN         (1U << RCC_AHB1LPENR_GPIOCLPEN_BIT)
#define RCC_AHB1LPENR_GPIOBLPEN         (1U << RCC_AHB1LPENR_GPIOBLPEN_BIT)
#define RCC_AHB1LPENR_GPIOALPEN         (1U << RCC_AHB1LPENR_GPIOALPEN_BIT)

/* AHB2 peripheral clock enable in low power mode register */

#define RCC_AHB2LPENR_OTGFSLPEN_BIT     7
#define RCC_AHB2LPENR_RNGLPEN_BIT       6
#define RCC_AHB2LPENR_HASHLPEN_BIT      5
#define RCC_AHB2LPENR_CRYPLPEN_BIT      4
#define RCC_AHB2LPENR_DCMILPEN_BIT      0

#define RCC_AHB2LPENR_OTGFSLPEN         (1U << RCC_AHB2LPENR_OTGFSLPEN_BIT)
#define RCC_AHB2LPENR_RNGLPEN           (1U << RCC_AHB2LPENR_RNGLPEN_BIT)
#define RCC_AHB2LPENR_HASHLPEN          (1U << RCC_AHB2LPENR_HASHLPEN_BIT)
#define RCC_AHB2LPENR_CRYPLPEN          (1U << RCC_AHB2LPENR_CRYPLPEN_BIT)
#define RCC_AHB2LPENR_DCMILPEN          (1U << RCC_AHB2LPENR_DCMILPEN_BIT)

/* AHB3 peripheral clock enable in low power mode register */

#define RCC_AHB3LPENR_FSMCLPEN_BIT      0

#define RCC_AHB3LPENR_FSMCLPEN          (1U << RCC_AHB3LPENR_FSMCLPEN_BIT)

/* APB1 peripheral clock enable in low power mode register */

#define RCC_APB1LPENR_DACLPEN_BIT       29
#define RCC_APB1LPENR_PWRLPEN_BIT       28
#define RCC_APB1LPENR_CAN2LPEN_BIT      26
#define RCC_APB1LPENR_CAN1LPEN_BIT      25
#define RCC_APB1LPENR_I2C3LPEN_BIT      23
#define RCC_APB1LPENR_I2C2LPEN_BIT      22
#define RCC_APB1LPENR_I2C1LPEN_BIT      21
#define RCC_APB1LPENR_UART5LPEN_BIT     20
#define RCC_APB1LPENR_UART4LPEN_BIT     19
#define RCC_APB1LPENR_USART3LPEN_BIT    18
#define RCC_APB1LPENR_USART2LPEN_BIT    17
#define RCC_APB1LPENR_SPI3LPEN_BIT      15
#define RCC_APB1LPENR_SPI2LPEN_BIT      14
#define RCC_APB1LPENR_WWDGLPEN_BIT      11
#define RCC_APB1LPENR_TIM14LPEN_BIT     8
#define RCC_APB1LPENR_TIM13LPEN_BIT     7
#define RCC_APB1LPENR_TIM12LPEN_BIT     6
#define RCC_APB1LPENR_TIM7LPEN_BIT      5
#define RCC_APB1LPENR_TIM6LPEN_BIT      4
#define RCC_APB1LPENR_TIM5LPEN_BIT      3
#define RCC_APB1LPENR_TIM4LPEN_BIT      2
#define RCC_APB1LPENR_TIM3LPEN_BIT      1
#define RCC_APB1LPENR_TIM2LPEN_BIT      0

#define RCC_APB1LPENR_DACLPEN           (1U << RCC_APB1LPENR_DACLPEN_BIT)
#define RCC_APB1LPENR_PWRLPEN           (1U << RCC_APB1LPENR_PWRLPEN_BIT)
#define RCC_APB1LPENR_CAN2LPEN          (1U << RCC_APB1LPENR_CAN2LPEN_BIT)
#define RCC_APB1LPENR_CAN1LPEN          (1U << RCC_APB1LPENR_CAN1LPEN_BIT)
#define RCC_APB1LPENR_I2C3LPEN          (1U << RCC_APB1LPENR_I2C3LPEN_BIT)
#define RCC_APB1LPENR_I2C2LPEN          (1U << RCC_APB1LPENR_I2C2LPEN_BIT)
#define RCC_APB1LPENR_I2C1LPEN          (1U << RCC_APB1LPENR_I2C1LPEN_BIT)
#define RCC_APB1LPENR_UART5LPEN         (1U << RCC_APB1LPENR_UART5LPEN_BIT)
#define RCC_APB1LPENR_UART4LPEN         (1U << RCC_APB1LPENR_UART4LPEN_BIT)
#define RCC_APB1LPENR_USART3LPEN        (1U << RCC_APB1LPENR_USART3LPEN_BIT)
#define RCC_APB1LPENR_USART2LPEN        (1U << RCC_APB1LPENR_USART2LPEN_BIT)
#define RCC_APB1LPENR_SPI3LPEN          (1U << RCC_APB1LPENR_SPI3LPEN_BIT)
#define RCC_APB1LPENR_SPI2LPEN          (1U << RCC_APB1LPENR_SPI2LPEN_BIT)
#define RCC_APB1LPENR_WWDGLPEN          (1U << RCC_APB1LPENR_WWDGLPEN_BIT)
#define RCC_APB1LPENR_TIM14LPEN         (1U << RCC_APB1LPENR_TIM14LPEN_BIT)
#define RCC_APB1LPENR_TIM13LPEN         (1U << RCC_APB1LPENR_TIM13LPEN_BIT)
#define RCC_APB1LPENR_TIM12LPEN         (1U << RCC_APB1LPENR_TIM12LPEN_BIT)
#define RCC_APB1LPENR_TIM7LPEN          (1U << RCC_APB1LPENR_TIM7LPEN_BIT)
#define RCC_APB1LPENR_TIM6LPEN          (1U << RCC_APB1LPENR_TIM6LPEN_BIT)
#define RCC_APB1LPENR_TIM5LPEN          (1U << RCC_APB1LPENR_TIM5LPEN_BIT)
#define RCC_APB1LPENR_TIM4LPEN          (1U << RCC_APB1LPENR_TIM4LPEN_BIT)
#define RCC_APB1LPENR_TIM3LPEN          (1U << RCC_APB1LPENR_TIM3LPEN_BIT)
#define RCC_APB1LPENR_TIM2LPEN          (1U << RCC_APB1LPENR_TIM2LPEN_BIT)

/* APB2 peripheral clock enable in low power mode register */

#define RCC_APB2LPENR_TIM11LPEN_BIT     18
#define RCC_APB2LPENR_TIM10LPEN_BIT     17
#define RCC_APB2LPENR_TIM9LPEN_BIT      16
#define RCC_APB2LPENR_SYSCFGLPEN_BIT    14
#define RCC_APB2LPENR_SPI1LPEN_BIT      12
#define RCC_APB2LPENR_SDIOLPEN_BIT      11
#define RCC_APB2LPENR_ADC3LPEN_BIT      10
#define RCC_APB2LPENR_ADC2LPEN_BIT      9
#define RCC_APB2LPENR_ADC1LPEN_BIT      8
#define RCC_APB2LPENR_USART6LPEN_BIT    5
#define RCC_APB2LPENR_USART1LPEN_BIT    4
#define RCC_APB2LPENR_TIM8LPEN_BIT      1
#define RCC_APB2LPENR_TIM1LPEN_BIT      0

#define RCC_APB2LPENR_TIM11LPEN         (1U << RCC_APB2LPENR_TIM11LPEN_BIT)
#define RCC_APB2LPENR_TIM10LPEN         (1U << RCC_APB2LPENR_TIM10LPEN_BIT)
#define RCC_APB2LPENR_TIM9LPEN          (1U << RCC_APB2LPENR_TIM9LPEN_BIT)
#define RCC_APB2LPENR_SYSCFGLPEN        (1U << RCC_APB2LPENR_SYSCFGLPEN_BIT)
#define RCC_APB2LPENR_SPI1LPEN          (1U << RCC_APB2LPENR_SPI1LPEN_BIT)
#define RCC_APB2LPENR_SDIOLPEN          (1U << RCC_APB2LPENR_SDIOLPEN_BIT)
#define RCC_APB2LPENR_ADC3LPEN          (1U << RCC_APB2LPENR_ADC3LPEN_BIT)
#define RCC_APB2LPENR_ADC2LPEN          (1U << RCC_APB2LPENR_ADC2LPEN_BIT)
#define RCC_APB2LPENR_ADC1LPEN          (1U << RCC_APB2LPENR_ADC1LPEN_BIT)
#define RCC_APB2LPENR_USART6LPEN        (1U << RCC_APB2LPENR_USART6LPEN_BIT)
#define RCC_APB2LPENR_USART1LPEN        (1U << RCC_APB2LPENR_USART1LPEN_BIT)
#define RCC_APB2LPENR_TIM8LPEN          (1U << RCC_APB2LPENR_TIM8LPEN_BIT)
#define RCC_APB2LPENR_TIM1LPEN          (1U << RCC_APB2LPENR_TIM1LPEN_BIT)

/* Backup domain control register */

#define RCC_BDCR_BDRST_BIT              16
#define RCC_BDCR_RTCEN_BIT              15
#define RCC_BDCR_LSEBYP_BIT             2
#define RCC_BDCR_LSERDY_BIT             1
#define RCC_BDCR_LSEON_BIT              0

#define RCC_BDCR_BDRST                  (1U << RCC_BDCR_BDRST_BIT)
#define RCC_BDCR_RTCEN                  (1U << RCC_BDCR_RTCEN_BIT)
#define RCC_BDCR_RTCSEL                 (0x3 << 8)
#define RCC_BDCR_RTCSEL_NOCLOCK         (0x0 << 8)
#define RCC_BDCR_RTCSEL_LSE             (0x1 << 8)
#define RCC_BDCR_RTCSEL_LSI             (0x2 << 8)
#define RCC_BDCR_RTCSEL_HSE_DIV         (0x3 << 8)
#define RCC_BDCR_LSEBYP                 (1U << RCC_BDCR_LSEBYP_BIT)
#define RCC_BDCR_LSERDY                 (1U << RCC_BDCR_LSERDY_BIT)
#define RCC_BDCR_LSEON                  (1U << RCC_BDCR_LSEON_BIT)

/* Clock control and status register */

#define RCC_CSR_LPWRRSTF_BIT            31
#define RCC_CSR_WWDGRSTF_BIT            30
#define RCC_CSR_IWDGRSTF_BIT            29
#define RCC_CSR_SFTRSTF_BIT             28
#define RCC_CSR_PORRSTF_BIT             27
#define RCC_CSR_PINRSTF_BIT             26
#define RCC_CSR_BORRSTF_BIT             25
#define RCC_CSR_RMVF_BIT                24
#define RCC_CSR_LSIRDY_BIT              1
#define RCC_CSR_LSION_BIT               0

#define RCC_CSR_LPWRRSTF                (1U << RCC_CSR_LPWRRSTF_BIT)
#define RCC_CSR_WWDGRSTF                (1U << RCC_CSR_WWDGRSTF_BIT)
#define RCC_CSR_IWDGRSTF                (1U << RCC_CSR_IWDGRSTF_BIT)
#define RCC_CSR_SFTRSTF                 (1U << RCC_CSR_SFTRSTF_BIT)
#define RCC_CSR_PORRSTF                 (1U << RCC_CSR_PORRSTF_BIT)
#define RCC_CSR_PINRSTF                 (1U << RCC_CSR_PINRSTF_BIT)
#define RCC_CSR_BORRSTF                 (1U << RCC_CSR_BORRSTF_BIT)
#define RCC_CSR_RMVF                    (1U << RCC_CSR_RMVF_BIT)
#define RCC_CSR_LSIRDY                  (1U << RCC_CSR_LSIRDY_BIT)
#define RCC_CSR_LSION                   (1U << RCC_CSR_LSION_BIT)

/* Spread spectrum clock generation register */

#define RCC_SSCGR_SSCGEN_BIT            31
#define RCC_SSCGR_SPREADSEL_BIT         30

#define RCC_SSCGR_SSCGEN                (1U << RCC_SSCGR_SSCGEN_BIT)
#define RCC_SSCGR_SPREADSEL             (1U << RCC_SSCGR_SPREADSEL_BIT)
#define RCC_SSCGR_SPREADSEL_CENTER      (0x0 << RCC_SSCGR_SPREADSEL_BIT)
#define RCC_SSCGR_SPREADSEL_DOWN        (0x1 << RCC_SSCGR_SPREADSEL_BIT)
#define RCC_SSCGR_INCSTEP               (0xFFF << 16)
#define RCC_SSCGR_MODPER                0xFFFF

/* PLLI2S configuration register */

#define RCC_PLLI2SCFGR_PLLI2SR          (0x7 << 28)
#define RCC_PLLI2SCFGR_PLLI2SN          (0x1FF << 6)

/*
 * Clock sources, domains, and peripheral clock IDs.
 */

/**
 * @brief STM32F2 clock sources.
 */
typedef enum rcc_clk {
    RCC_CLK_PLLI2S = (uint16)((offsetof(struct rcc_reg_map, CR) << 8) |
                              RCC_CR_PLLI2SON_BIT), /**< Dedicated PLL
                                                       for I2S. */
    RCC_CLK_PLL    = (uint16)((offsetof(struct rcc_reg_map, CR) << 8) |
                              RCC_CR_PLLON_BIT), /**< Main PLL, clocked by
                                                    HSI or HSE. */
    RCC_CLK_HSE    = (uint16)((offsetof(struct rcc_reg_map, CR) << 8) |
                              RCC_CR_HSEON_BIT), /**< High speed external. */
    RCC_CLK_HSI    = (uint16)((offsetof(struct rcc_reg_map, CR) << 8) |
                              RCC_CR_HSION_BIT), /**< High speed internal. */
    RCC_CLK_LSE    = (uint16)((offsetof(struct rcc_reg_map, BDCR) << 8) |
                              RCC_BDCR_LSEON_BIT), /**< Low-speed external
                                                    * (32.768 KHz). */
    RCC_CLK_LSI    = (uint16)((offsetof(struct rcc_reg_map, CSR) << 8) |
                              RCC_CSR_LSION_BIT), /**< Low-speed internal
                                                   * (approximately 32 KHz). */
} rcc_clk;

/**
 * @brief STM32F2 rcc_clk_id.
 */
typedef enum rcc_clk_id {
    RCC_ADC1,
    RCC_ADC2,
    RCC_ADC3,
    RCC_BKPSRAM,
    RCC_CAN1,
    RCC_CAN2,
    RCC_CRC,
    RCC_CRYP,
    RCC_DAC,
    RCC_DCMI,
    RCC_DMA1,
    RCC_DMA2,
    RCC_ETHMAC,
    RCC_ETHMACPTP,
    RCC_ETHMACRX,
    RCC_ETHMACTX,
    RCC_FSMC,
    RCC_GPIOA,
    RCC_GPIOB,
    RCC_GPIOC,
    RCC_GPIOD,
    RCC_GPIOE,
    RCC_GPIOF,
    RCC_GPIOG,
    RCC_GPIOH,
    RCC_GPIOI,
    RCC_HASH,
    RCC_I2C1,
    RCC_I2C2,
    RCC_I2C3,
    RCC_OTGFS,
    RCC_OTGHS,
    RCC_OTGHSULPI,
    RCC_PWR,
    RCC_RNG,
    RCC_SDIO,
    RCC_SPI1,
    RCC_SPI2,
    RCC_SPI3,
    RCC_SYSCFG,
    RCC_TIMER1,
    RCC_TIMER10,
    RCC_TIMER11,
    RCC_TIMER12,
    RCC_TIMER13,
    RCC_TIMER14,
    RCC_TIMER2,
    RCC_TIMER3,
    RCC_TIMER4,
    RCC_TIMER5,
    RCC_TIMER6,
    RCC_TIMER7,
    RCC_TIMER8,
    RCC_TIMER9,
    RCC_USART1,
    RCC_USART2,
    RCC_USART3,
    RCC_UART4,
    RCC_UART5,
    RCC_USART6,
    RCC_WWDG,
} rcc_clk_id;

/**
 * @brief STM32F2 PLL entry clock source
 * @see rcc_configure_pll()
 */
typedef enum rcc_pllsrc {
    RCC_PLLSRC_HSI = 0,
    RCC_PLLSRC_HSE = RCC_PLLCFGR_PLLSRC,
} rcc_pllsrc;

/**
 * @brief STM32F2 Peripheral clock domains.
 */
typedef enum rcc_clk_domain {
    RCC_APB1,
    RCC_APB2,
    RCC_AHB1,
    RCC_AHB2,
    RCC_AHB3,
} rcc_clk_domain;

/*
 * Prescalers and dividers.
 */

/**
 * @brief STM32F2 Prescaler identifiers.
 */
typedef enum rcc_prescaler {
    RCC_PRESCALER_MCO2,
    RCC_PRESCALER_MCO1,
    RCC_PRESCALER_RTC,
    RCC_PRESCALER_APB2,
    RCC_PRESCALER_APB1,
    RCC_PRESCALER_AHB
} rcc_prescaler;

/**
 * @brief STM32F2 MCO2 prescaler dividers.
 */
typedef enum rcc_mco2_divider {
    RCC_MCO2_DIV_1 = RCC_CFGR_MCO2PRE_DIV_1,
    RCC_MCO2_DIV_2 = RCC_CFGR_MCO2PRE_DIV_2,
    RCC_MCO2_DIV_3 = RCC_CFGR_MCO2PRE_DIV_3,
    RCC_MCO2_DIV_4 = RCC_CFGR_MCO2PRE_DIV_4,
    RCC_MCO2_DIV_5 = RCC_CFGR_MCO2PRE_DIV_5,
} rcc_mco2_divider;

/**
 * @brief STM32F2 MCO1 prescaler dividers.
 */
typedef enum rcc_mco1_divider {
    RCC_MCO1_DIV_1 = RCC_CFGR_MCO1PRE_DIV_1,
    RCC_MCO1_DIV_2 = RCC_CFGR_MCO1PRE_DIV_2,
    RCC_MCO1_DIV_3 = RCC_CFGR_MCO1PRE_DIV_3,
    RCC_MCO1_DIV_4 = RCC_CFGR_MCO1PRE_DIV_4,
    RCC_MCO1_DIV_5 = RCC_CFGR_MCO1PRE_DIV_5,
} rcc_mco1_divider;

/**
 * @brief STM32F2 RTC prescaler dividers.
 */
typedef enum rcc_rtc_divider {  /* FIXME [0.0.13] TODO */
    RCC_RTC_DIV_TODO = 0xFFFFFFFF,
} rcc_rtc_divider;

/**
 * @brief STM32F2 AP2 prescaler dividers.
 */
typedef enum rcc_apb2_divider {
    RCC_APB2_HCLK_DIV_1  = 0,
    RCC_APB2_HCLK_DIV_2  = RCC_CFGR_PPRE2_AHB_DIV_2,
    RCC_APB2_HCLK_DIV_4  = RCC_CFGR_PPRE2_AHB_DIV_4,
    RCC_APB2_HCLK_DIV_8  = RCC_CFGR_PPRE2_AHB_DIV_8,
    RCC_APB2_HCLK_DIV_16 = RCC_CFGR_PPRE2_AHB_DIV_16,
} rcc_apb2_divider;

/**
 * @brief STM32F2 APB1 prescaler dividers.
 */
typedef enum rcc_apb1_divider {
    RCC_APB1_HCLK_DIV_1  = 0,
    RCC_APB1_HCLK_DIV_2  = RCC_CFGR_PPRE1_AHB_DIV_2,
    RCC_APB1_HCLK_DIV_4  = RCC_CFGR_PPRE1_AHB_DIV_4,
    RCC_APB1_HCLK_DIV_8  = RCC_CFGR_PPRE1_AHB_DIV_8,
    RCC_APB1_HCLK_DIV_16 = RCC_CFGR_PPRE1_AHB_DIV_16,
} rcc_apb1_divider;

/**
 * @brief STM32F2 AHB prescaler dividers.
 */
typedef enum rcc_ahb_divider {
    RCC_AHB_SYSCLK_DIV_1   = 0,
    RCC_AHB_SYSCLK_DIV_2   = RCC_CFGR_HPRE_SYSCLK_DIV_2,
    RCC_AHB_SYSCLK_DIV_4   = RCC_CFGR_HPRE_SYSCLK_DIV_4,
    RCC_AHB_SYSCLK_DIV_8   = RCC_CFGR_HPRE_SYSCLK_DIV_8,
    RCC_AHB_SYSCLK_DIV_16  = RCC_CFGR_HPRE_SYSCLK_DIV_16,
    RCC_AHB_SYSCLK_DIV_64  = RCC_CFGR_HPRE_SYSCLK_DIV_64,
    RCC_AHB_SYSCLK_DIV_128 = RCC_CFGR_HPRE_SYSCLK_DIV_128,
    RCC_AHB_SYSCLK_DIV_256 = RCC_CFGR_HPRE_SYSCLK_DIV_256,
    RCC_AHB_SYSCLK_DIV_512 = RCC_CFGR_HPRE_SYSCLK_DIV_512,
} rcc_ahb_divider;

/**
 * @brief STM32F2 PLL configuration values.
 * Point to one of these with the "data" field in a struct rcc_pll_cfg.
 * @see struct rcc_pll_cfg.
 */
typedef struct stm32f2_rcc_pll_data {
    uint8 pllq;      /**<
                      * @brief PLLQ value.
                      * Allowed values: 4, 5, ..., 15. */
    uint8 pllp;      /**<
                      * @brief PLLP value.
                      * Allowed values: 2, 4, 6, 8. */
    uint16 plln;     /**<
                      * @brief PLLN value.
                      * Allowed values: 192, 193, ..., 432. */
    uint8 pllm;      /**<
                      * @brief PLLM value.
                      * Allowed values: 2, 3, ..., 63. */
} stm32f2_rcc_pll_data;

#ifdef __cplusplus
}
#endif

#endif
