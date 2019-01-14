/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file libmaple/stm32f1/include/series/rcc.h
 * @brief STM32F1 reset and clock control (RCC) support.
 */

#ifndef _LIBMAPLE_STM32F1_RCC_H_
#define _LIBMAPLE_STM32F1_RCC_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/bitband.h>

/*
 * Register map
 */

/** STM32F1 RCC register map type */
typedef struct rcc_reg_map {
    __IO uint32 CR;             /**< Clock control register */
    __IO uint32 CFGR;           /**< Clock configuration register */
    __IO uint32 CIR;            /**< Clock interrupt register */
    __IO uint32 APB2RSTR;       /**< APB2 peripheral reset register */
    __IO uint32 APB1RSTR;       /**< APB1 peripheral reset register */
    __IO uint32 AHBENR;         /**< AHB peripheral clock enable register */
    __IO uint32 APB2ENR;        /**< APB2 peripheral clock enable register */
    __IO uint32 APB1ENR;        /**< APB1 peripheral clock enable register */
    __IO uint32 BDCR;           /**< Backup domain control register */
    __IO uint32 CSR;            /**< Control/status register */
} rcc_reg_map;

#define RCC_BASE                        ((struct rcc_reg_map*)0x40021000)

/*
 * Register bit definitions
 */

/* Clock control register */

#define RCC_CR_PLLRDY_BIT               25
#define RCC_CR_PLLON_BIT                24
#define RCC_CR_CSSON_BIT                19
#define RCC_CR_HSEBYP_BIT               18
#define RCC_CR_HSERDY_BIT               17
#define RCC_CR_HSEON_BIT                16
#define RCC_CR_HSIRDY_BIT               1
#define RCC_CR_HSION_BIT                0

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

/* Clock configuration register */

#define RCC_CFGR_USBPRE_BIT             22
#define RCC_CFGR_PLLXTPRE_BIT           17
#define RCC_CFGR_PLLSRC_BIT             16

#define RCC_CFGR_MCO                    (0x3 << 24)
#define RCC_CFGR_USBPRE                 (0x3 << RCC_CFGR_USBPRE_BIT)
#define RCC_CFGR_PLLMUL                 (0xF << 18)
#define RCC_CFGR_PLLXTPRE               (1U << RCC_CFGR_PLLXTPRE_BIT)
#define RCC_CFGR_PLLSRC                 (1U << RCC_CFGR_PLLSRC_BIT)
#define RCC_CFGR_ADCPRE                 (0x3 << 14)
#define RCC_CFGR_PPRE2                  (0x7 << 11)
#define RCC_CFGR_PPRE1                  (0x7 << 8)
#define RCC_CFGR_HPRE                   (0xF << 4)
#define RCC_CFGR_SWS                    (0x3 << 2)
#define RCC_CFGR_SWS_PLL                (0x2 << 2)
#define RCC_CFGR_SWS_HSE                (0x1 << 2)
#define RCC_CFGR_SW                     0x3
#define RCC_CFGR_SW_PLL                 0x2
#define RCC_CFGR_SW_HSE                 0x1

/* Clock interrupt register */

#define RCC_CIR_CSSC_BIT                23
#define RCC_CIR_PLLRDYC_BIT             20
#define RCC_CIR_HSERDYC_BIT             19
#define RCC_CIR_HSIRDYC_BIT             18
#define RCC_CIR_LSERDYC_BIT             17
#define RCC_CIR_LSIRDYC_BIT             16
#define RCC_CIR_PLLRDYIE_BIT            12
#define RCC_CIR_HSERDYIE_BIT            11
#define RCC_CIR_HSIRDYIE_BIT            10
#define RCC_CIR_LSERDYIE_BIT            9
#define RCC_CIR_LSIRDYIE_BIT            8
#define RCC_CIR_CSSF_BIT                7
#define RCC_CIR_PLLRDYF_BIT             4
#define RCC_CIR_HSERDYF_BIT             3
#define RCC_CIR_HSIRDYF_BIT             2
#define RCC_CIR_LSERDYF_BIT             1
#define RCC_CIR_LSIRDYF_BIT             0

#define RCC_CIR_CSSC                    (1U << RCC_CIR_CSSC_BIT)
#define RCC_CIR_PLLRDYC                 (1U << RCC_CIR_PLLRDYC_BIT)
#define RCC_CIR_HSERDYC                 (1U << RCC_CIR_HSERDYC_BIT)
#define RCC_CIR_HSIRDYC                 (1U << RCC_CIR_HSIRDYC_BIT)
#define RCC_CIR_LSERDYC                 (1U << RCC_CIR_LSERDYC_BIT)
#define RCC_CIR_LSIRDYC                 (1U << RCC_CIR_LSIRDYC_BIT)
#define RCC_CIR_PLLRDYIE                (1U << RCC_CIR_PLLRDYIE_BIT)
#define RCC_CIR_HSERDYIE                (1U << RCC_CIR_HSERDYIE_BIT)
#define RCC_CIR_HSIRDYIE                (1U << RCC_CIR_HSIRDYIE_BIT)
#define RCC_CIR_LSERDYIE                (1U << RCC_CIR_LSERDYIE_BIT)
#define RCC_CIR_LSIRDYIE                (1U << RCC_CIR_LSIRDYIE_BIT)
#define RCC_CIR_CSSF                    (1U << RCC_CIR_CSSF_BIT)
#define RCC_CIR_PLLRDYF                 (1U << RCC_CIR_PLLRDYF_BIT)
#define RCC_CIR_HSERDYF                 (1U << RCC_CIR_HSERDYF_BIT)
#define RCC_CIR_HSIRDYF                 (1U << RCC_CIR_HSIRDYF_BIT)
#define RCC_CIR_LSERDYF                 (1U << RCC_CIR_LSERDYF_BIT)
#define RCC_CIR_LSIRDYF                 (1U << RCC_CIR_LSIRDYF_BIT)

/* APB2 peripheral reset register */

#define RCC_APB2RSTR_TIM11RST_BIT       21
#define RCC_APB2RSTR_TIM10RST_BIT       20
#define RCC_APB2RSTR_TIM9RST_BIT        19
#define RCC_APB2RSTR_ADC3RST_BIT        15
#define RCC_APB2RSTR_USART1RST_BIT      14
#define RCC_APB2RSTR_TIM8RST_BIT        13
#define RCC_APB2RSTR_SPI1RST_BIT        12
#define RCC_APB2RSTR_TIM1RST_BIT        11
#define RCC_APB2RSTR_ADC2RST_BIT        10
#define RCC_APB2RSTR_ADC1RST_BIT        9
#define RCC_APB2RSTR_IOPGRST_BIT        8
#define RCC_APB2RSTR_IOPFRST_BIT        7
#define RCC_APB2RSTR_IOPERST_BIT        6
#define RCC_APB2RSTR_IOPDRST_BIT        5
#define RCC_APB2RSTR_IOPCRST_BIT        4
#define RCC_APB2RSTR_IOPBRST_BIT        3
#define RCC_APB2RSTR_IOPARST_BIT        2
#define RCC_APB2RSTR_AFIORST_BIT        0

#define RCC_APB2RSTR_TIM11RST           (1U << RCC_APB2RSTR_TIM11RST_BIT)
#define RCC_APB2RSTR_TIM10RST           (1U << RCC_APB2RSTR_TIM10RST_BIT)
#define RCC_APB2RSTR_TIM9RST            (1U << RCC_APB2RSTR_TIM9RST_BIT)
#define RCC_APB2RSTR_ADC3RST            (1U << RCC_APB2RSTR_ADC3RST_BIT)
#define RCC_APB2RSTR_USART1RST          (1U << RCC_APB2RSTR_USART1RST_BIT)
#define RCC_APB2RSTR_TIM8RST            (1U << RCC_APB2RSTR_TIM8RST_BIT)
#define RCC_APB2RSTR_SPI1RST            (1U << RCC_APB2RSTR_SPI1RST_BIT)
#define RCC_APB2RSTR_TIM1RST            (1U << RCC_APB2RSTR_TIM1RST_BIT)
#define RCC_APB2RSTR_ADC2RST            (1U << RCC_APB2RSTR_ADC2RST_BIT)
#define RCC_APB2RSTR_ADC1RST            (1U << RCC_APB2RSTR_ADC1RST_BIT)
#define RCC_APB2RSTR_IOPGRST            (1U << RCC_APB2RSTR_IOPGRST_BIT)
#define RCC_APB2RSTR_IOPFRST            (1U << RCC_APB2RSTR_IOPFRST_BIT)
#define RCC_APB2RSTR_IOPERST            (1U << RCC_APB2RSTR_IOPERST_BIT)
#define RCC_APB2RSTR_IOPDRST            (1U << RCC_APB2RSTR_IOPDRST_BIT)
#define RCC_APB2RSTR_IOPCRST            (1U << RCC_APB2RSTR_IOPCRST_BIT)
#define RCC_APB2RSTR_IOPBRST            (1U << RCC_APB2RSTR_IOPBRST_BIT)
#define RCC_APB2RSTR_IOPARST            (1U << RCC_APB2RSTR_IOPARST_BIT)
#define RCC_APB2RSTR_AFIORST            (1U << RCC_APB2RSTR_AFIORST_BIT)

/* APB1 peripheral reset register */

#define RCC_APB1RSTR_DACRST_BIT         29
#define RCC_APB1RSTR_PWRRST_BIT         28
#define RCC_APB1RSTR_BKPRST_BIT         27
#define RCC_APB1RSTR_CANRST_BIT         25
#define RCC_APB1RSTR_USBRST_BIT         23
#define RCC_APB1RSTR_I2C2RST_BIT        22
#define RCC_APB1RSTR_I2C1RST_BIT        21
#define RCC_APB1RSTR_UART5RST_BIT       20
#define RCC_APB1RSTR_UART4RST_BIT       19
#define RCC_APB1RSTR_USART3RST_BIT      18
#define RCC_APB1RSTR_USART2RST_BIT      17
#define RCC_APB1RSTR_SPI3RST_BIT        15
#define RCC_APB1RSTR_SPI2RST_BIT        14
#define RCC_APB1RSTR_WWDRST_BIT         11
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
#define RCC_APB1RSTR_BKPRST             (1U << RCC_APB1RSTR_BKPRST_BIT)
#define RCC_APB1RSTR_CANRST             (1U << RCC_APB1RSTR_CANRST_BIT)
#define RCC_APB1RSTR_USBRST             (1U << RCC_APB1RSTR_USBRST_BIT)
#define RCC_APB1RSTR_I2C2RST            (1U << RCC_APB1RSTR_I2C2RST_BIT)
#define RCC_APB1RSTR_I2C1RST            (1U << RCC_APB1RSTR_I2C1RST_BIT)
#define RCC_APB1RSTR_UART5RST           (1U << RCC_APB1RSTR_UART5RST_BIT)
#define RCC_APB1RSTR_UART4RST           (1U << RCC_APB1RSTR_UART4RST_BIT)
#define RCC_APB1RSTR_USART3RST          (1U << RCC_APB1RSTR_USART3RST_BIT)
#define RCC_APB1RSTR_USART2RST          (1U << RCC_APB1RSTR_USART2RST_BIT)
#define RCC_APB1RSTR_SPI3RST            (1U << RCC_APB1RSTR_SPI3RST_BIT)
#define RCC_APB1RSTR_SPI2RST            (1U << RCC_APB1RSTR_SPI2RST_BIT)
#define RCC_APB1RSTR_WWDRST             (1U << RCC_APB1RSTR_WWDRST_BIT)
#define RCC_APB1RSTR_TIM14RST           (1U << RCC_APB1RSTR_TIM14RST_BIT)
#define RCC_APB1RSTR_TIM13RST           (1U << RCC_APB1RSTR_TIM13RST_BIT)
#define RCC_APB1RSTR_TIM12RST           (1U << RCC_APB1RSTR_TIM12RST_BIT)
#define RCC_APB1RSTR_TIM7RST            (1U << RCC_APB1RSTR_TIM7RST_BIT)
#define RCC_APB1RSTR_TIM6RST            (1U << RCC_APB1RSTR_TIM6RST_BIT)
#define RCC_APB1RSTR_TIM5RST            (1U << RCC_APB1RSTR_TIM5RST_BIT)
#define RCC_APB1RSTR_TIM4RST            (1U << RCC_APB1RSTR_TIM4RST_BIT)
#define RCC_APB1RSTR_TIM3RST            (1U << RCC_APB1RSTR_TIM3RST_BIT)
#define RCC_APB1RSTR_TIM2RST            (1U << RCC_APB1RSTR_TIM2RST_BIT)

/* AHB peripheral clock enable register */

#define RCC_AHBENR_SDIOEN_BIT           10
#define RCC_AHBENR_FSMCEN_BIT           8
#define RCC_AHBENR_CRCEN_BIT            7
#define RCC_AHBENR_FLITFEN_BIT          4
#define RCC_AHBENR_SRAMEN_BIT           2
#define RCC_AHBENR_DMA2EN_BIT           1
#define RCC_AHBENR_DMA1EN_BIT           0

#define RCC_AHBENR_SDIOEN               (1U << RCC_AHBENR_SDIOEN_BIT)
#define RCC_AHBENR_FSMCEN               (1U << RCC_AHBENR_FSMCEN_BIT)
#define RCC_AHBENR_CRCEN                (1U << RCC_AHBENR_CRCEN_BIT)
#define RCC_AHBENR_FLITFEN              (1U << RCC_AHBENR_FLITFEN_BIT)
#define RCC_AHBENR_SRAMEN               (1U << RCC_AHBENR_SRAMEN_BIT)
#define RCC_AHBENR_DMA2EN               (1U << RCC_AHBENR_DMA2EN_BIT)
#define RCC_AHBENR_DMA1EN               (1U << RCC_AHBENR_DMA1EN_BIT)

/* APB2 peripheral clock enable register */

#define RCC_APB2ENR_TIM11EN_BIT         21
#define RCC_APB2ENR_TIM10EN_BIT         20
#define RCC_APB2ENR_TIM9EN_BIT          19
#define RCC_APB2ENR_ADC3EN_BIT          15
#define RCC_APB2ENR_USART1EN_BIT        14
#define RCC_APB2ENR_TIM8EN_BIT          13
#define RCC_APB2ENR_SPI1EN_BIT          12
#define RCC_APB2ENR_TIM1EN_BIT          11
#define RCC_APB2ENR_ADC2EN_BIT          10
#define RCC_APB2ENR_ADC1EN_BIT          9
#define RCC_APB2ENR_IOPGEN_BIT          8
#define RCC_APB2ENR_IOPFEN_BIT          7
#define RCC_APB2ENR_IOPEEN_BIT          6
#define RCC_APB2ENR_IOPDEN_BIT          5
#define RCC_APB2ENR_IOPCEN_BIT          4
#define RCC_APB2ENR_IOPBEN_BIT          3
#define RCC_APB2ENR_IOPAEN_BIT          2
#define RCC_APB2ENR_AFIOEN_BIT          0

#define RCC_APB2ENR_TIM11EN             (1U << RCC_APB2ENR_TIM11EN_BIT)
#define RCC_APB2ENR_TIM10EN             (1U << RCC_APB2ENR_TIM10EN_BIT)
#define RCC_APB2ENR_TIM9EN              (1U << RCC_APB2ENR_TIM9EN_BIT)
#define RCC_APB2ENR_ADC3EN              (1U << RCC_APB2ENR_ADC3EN_BIT)
#define RCC_APB2ENR_USART1EN            (1U << RCC_APB2ENR_USART1EN_BIT)
#define RCC_APB2ENR_TIM8EN              (1U << RCC_APB2ENR_TIM8EN_BIT)
#define RCC_APB2ENR_SPI1EN              (1U << RCC_APB2ENR_SPI1EN_BIT)
#define RCC_APB2ENR_TIM1EN              (1U << RCC_APB2ENR_TIM1EN_BIT)
#define RCC_APB2ENR_ADC2EN              (1U << RCC_APB2ENR_ADC2EN_BIT)
#define RCC_APB2ENR_ADC1EN              (1U << RCC_APB2ENR_ADC1EN_BIT)
#define RCC_APB2ENR_IOPGEN              (1U << RCC_APB2ENR_IOPGEN_BIT)
#define RCC_APB2ENR_IOPFEN              (1U << RCC_APB2ENR_IOPFEN_BIT)
#define RCC_APB2ENR_IOPEEN              (1U << RCC_APB2ENR_IOPEEN_BIT)
#define RCC_APB2ENR_IOPDEN              (1U << RCC_APB2ENR_IOPDEN_BIT)
#define RCC_APB2ENR_IOPCEN              (1U << RCC_APB2ENR_IOPCEN_BIT)
#define RCC_APB2ENR_IOPBEN              (1U << RCC_APB2ENR_IOPBEN_BIT)
#define RCC_APB2ENR_IOPAEN              (1U << RCC_APB2ENR_IOPAEN_BIT)
#define RCC_APB2ENR_AFIOEN              (1U << RCC_APB2ENR_AFIOEN_BIT)

/* APB1 peripheral clock enable register */

#define RCC_APB1ENR_DACEN_BIT           29
#define RCC_APB1ENR_PWREN_BIT           28
#define RCC_APB1ENR_BKPEN_BIT           27
#define RCC_APB1ENR_CANEN_BIT           25
#define RCC_APB1ENR_USBEN_BIT           23
#define RCC_APB1ENR_I2C2EN_BIT          22
#define RCC_APB1ENR_I2C1EN_BIT          21
#define RCC_APB1ENR_UART5EN_BIT         20
#define RCC_APB1ENR_UART4EN_BIT         19
#define RCC_APB1ENR_USART3EN_BIT        18
#define RCC_APB1ENR_USART2EN_BIT        17
#define RCC_APB1ENR_SPI3EN_BIT          15
#define RCC_APB1ENR_SPI2EN_BIT          14
#define RCC_APB1ENR_WWDEN_BIT           11
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
#define RCC_APB1ENR_BKPEN               (1U << RCC_APB1ENR_BKPEN_BIT)
#define RCC_APB1ENR_CANEN               (1U << RCC_APB1ENR_CANEN_BIT)
#define RCC_APB1ENR_USBEN               (1U << RCC_APB1ENR_USBEN_BIT)
#define RCC_APB1ENR_I2C2EN              (1U << RCC_APB1ENR_I2C2EN_BIT)
#define RCC_APB1ENR_I2C1EN              (1U << RCC_APB1ENR_I2C1EN_BIT)
#define RCC_APB1ENR_UART5EN             (1U << RCC_APB1ENR_UART5EN_BIT)
#define RCC_APB1ENR_UART4EN             (1U << RCC_APB1ENR_UART4EN_BIT)
#define RCC_APB1ENR_USART3EN            (1U << RCC_APB1ENR_USART3EN_BIT)
#define RCC_APB1ENR_USART2EN            (1U << RCC_APB1ENR_USART2EN_BIT)
#define RCC_APB1ENR_SPI3EN              (1U << RCC_APB1ENR_SPI3EN_BIT)
#define RCC_APB1ENR_SPI2EN              (1U << RCC_APB1ENR_SPI2EN_BIT)
#define RCC_APB1ENR_WWDEN               (1U << RCC_APB1ENR_WWDEN_BIT)
#define RCC_APB1ENR_TIM14EN             (1U << RCC_APB1ENR_TIM14EN_BIT)
#define RCC_APB1ENR_TIM13EN             (1U << RCC_APB1ENR_TIM13EN_BIT)
#define RCC_APB1ENR_TIM12EN             (1U << RCC_APB1ENR_TIM12EN_BIT)
#define RCC_APB1ENR_TIM7EN              (1U << RCC_APB1ENR_TIM7EN_BIT)
#define RCC_APB1ENR_TIM6EN              (1U << RCC_APB1ENR_TIM6EN_BIT)
#define RCC_APB1ENR_TIM5EN              (1U << RCC_APB1ENR_TIM5EN_BIT)
#define RCC_APB1ENR_TIM4EN              (1U << RCC_APB1ENR_TIM4EN_BIT)
#define RCC_APB1ENR_TIM3EN              (1U << RCC_APB1ENR_TIM3EN_BIT)
#define RCC_APB1ENR_TIM2EN              (1U << RCC_APB1ENR_TIM2EN_BIT)

/* Backup domain control register */

#define RCC_BDCR_BDRST_BIT              16
#define RCC_BDCR_RTCEN_BIT              15
#define RCC_BDCR_LSEBYP_BIT             2
#define RCC_BDCR_LSERDY_BIT             1
#define RCC_BDCR_LSEON_BIT              0

#define RCC_BDCR_BDRST                  (1U << RCC_BDCR_BDRST_BIT)
#define RCC_BDCR_RTCEN                  (1U << RCC_BDCR_RTC_BIT)
#define RCC_BDCR_RTCSEL                 (0x3 << 8)
#define RCC_BDCR_RTCSEL_NONE            (0x0 << 8)
#define RCC_BDCR_RTCSEL_LSE             (0x1 << 8)
#define RCC_BDCR_RTCSEL_LSI             (0x2 << 8) // added to support RTClock
#define RCC_BDCR_RTCSEL_HSE             (0x3 << 8)
#define RCC_BDCR_LSEBYP                 (1U << RCC_BDCR_LSEBYP_BIT)
#define RCC_BDCR_LSERDY                 (1U << RCC_BDCR_LSERDY_BIT)
#define RCC_BDCR_LSEON                  (1U << RCC_BDCR_LSEON_BIT)

/* Control/status register */

#define RCC_CSR_LPWRRSTF_BIT            31
#define RCC_CSR_WWDGRSTF_BIT            30
#define RCC_CSR_IWDGRSTF_BIT            29
#define RCC_CSR_SFTRSTF_BIT             28
#define RCC_CSR_PORRSTF_BIT             27
#define RCC_CSR_PINRSTF_BIT             26
#define RCC_CSR_RMVF_BIT                24
#define RCC_CSR_LSIRDY_BIT              1
#define RCC_CSR_LSION_BIT               0

#define RCC_CSR_LPWRRSTF                (1U << RCC_CSR_LPWRRSTF_BIT)
#define RCC_CSR_WWDGRSTF                (1U << RCC_CSR_WWDGRSTF_BIT)
#define RCC_CSR_IWDGRSTF                (1U << RCC_CSR_IWDGRSTF_BIT)
#define RCC_CSR_SFTRSTF                 (1U << RCC_CSR_SFTRSTF_BIT)
#define RCC_CSR_PORRSTF                 (1U << RCC_CSR_PORRSTF_BIT)
#define RCC_CSR_PINRSTF                 (1U << RCC_CSR_PINRSTF_BIT)
#define RCC_CSR_RMVF                    (1U << RCC_CSR_RMVF_BIT)
#define RCC_CSR_LSIRDY                  (1U << RCC_CSR_LSIRDY_BIT)
#define RCC_CSR_LSION                   (1U << RCC_CSR_LSION_BIT)

/*
 * libmaple-mandated enumeration types.
 */

/**
 * @brief STM32F1 rcc_clk_id.
 */
typedef enum rcc_clk_id {
    RCC_ADC1,
    RCC_ADC2,
    RCC_ADC3,
    RCC_AFIO,
    RCC_BKP,
    RCC_CRC,
    RCC_DAC,
    RCC_DMA1,
    RCC_DMA2,
    RCC_FLITF,
    RCC_FSMC,
    RCC_GPIOA,
    RCC_GPIOB,
    RCC_GPIOC,
    RCC_GPIOD,
    RCC_GPIOE,
    RCC_GPIOF,
    RCC_GPIOG,
    RCC_I2C1,
    RCC_I2C2,
    RCC_PWR,
    RCC_SDIO,
    RCC_SPI1,
    RCC_SPI2,
    RCC_SPI3,
    RCC_SRAM,
    RCC_TIMER1,
    RCC_TIMER2,
    RCC_TIMER3,
    RCC_TIMER4,
    RCC_TIMER5,
    RCC_TIMER6,
    RCC_TIMER7,
    RCC_TIMER8,
    RCC_TIMER9,
    RCC_TIMER10,
    RCC_TIMER11,
    RCC_TIMER12,
    RCC_TIMER13,
    RCC_TIMER14,
    RCC_USART1,
    RCC_USART2,
    RCC_USART3,
    RCC_UART4,
    RCC_UART5,
    RCC_USB,
} rcc_clk_id;

/**
 * @brief STM32F1 PLL clock sources.
 * @see rcc_configure_pll()
 */
typedef enum rcc_pllsrc {
    RCC_PLLSRC_HSE = (0x1 << 16),
    RCC_PLLSRC_HSI_DIV_2 = (0x0 << 16)
} rcc_pllsrc;

/**
 * @brief STM32F1 clock domains.
 * @see rcc_dev_clk()
 */
typedef enum rcc_clk_domain {
    RCC_APB1,
    RCC_APB2,
    RCC_AHB
} rcc_clk_domain;

/**
 * @brief STM32F1 Prescaler identifiers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_prescaler {
    RCC_PRESCALER_AHB,
    RCC_PRESCALER_APB1,
    RCC_PRESCALER_APB2,
    RCC_PRESCALER_USB,
    RCC_PRESCALER_ADC
} rcc_prescaler;

/**
 * @brief STM32F1 ADC prescaler dividers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_adc_divider {
    RCC_ADCPRE_PCLK_DIV_2 = 0x0 << 14,
    RCC_ADCPRE_PCLK_DIV_4 = 0x1 << 14,
    RCC_ADCPRE_PCLK_DIV_6 = 0x2 << 14,
    RCC_ADCPRE_PCLK_DIV_8 = 0x3 << 14,
} rcc_adc_divider;

/**
 * @brief STM32F1 APB1 prescaler dividers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_apb1_divider {
    RCC_APB1_HCLK_DIV_1 = 0x0 << 8,
    RCC_APB1_HCLK_DIV_2 = 0x4 << 8,
    RCC_APB1_HCLK_DIV_4 = 0x5 << 8,
    RCC_APB1_HCLK_DIV_8 = 0x6 << 8,
    RCC_APB1_HCLK_DIV_16 = 0x7 << 8,
} rcc_apb1_divider;

/**
 * @brief STM32F1 APB2 prescaler dividers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_apb2_divider {
    RCC_APB2_HCLK_DIV_1 = 0x0 << 11,
    RCC_APB2_HCLK_DIV_2 = 0x4 << 11,
    RCC_APB2_HCLK_DIV_4 = 0x5 << 11,
    RCC_APB2_HCLK_DIV_8 = 0x6 << 11,
    RCC_APB2_HCLK_DIV_16 = 0x7 << 11,
} rcc_apb2_divider;

/**
 * @brief STM32F1 AHB prescaler dividers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_ahb_divider {
    RCC_AHB_SYSCLK_DIV_1 = 0x0 << 4,
    RCC_AHB_SYSCLK_DIV_2 = 0x8 << 4,
    RCC_AHB_SYSCLK_DIV_4 = 0x9 << 4,
    RCC_AHB_SYSCLK_DIV_8 = 0xA << 4,
    RCC_AHB_SYSCLK_DIV_16 = 0xB << 4,
    RCC_AHB_SYSCLK_DIV_32 = 0xC << 4,
    RCC_AHB_SYSCLK_DIV_64 = 0xD << 4,
    RCC_AHB_SYSCLK_DIV_128 = 0xD << 4,
    RCC_AHB_SYSCLK_DIV_256 = 0xE << 4,
    RCC_AHB_SYSCLK_DIV_512 = 0xF << 4,
} rcc_ahb_divider;

/**
 * @brief STM32F1 USB prescaler dividers
 * @see rcc_set_prescaler()
 */
 /*
 Set and reset by software to control the USB clock prescaler value. The USB clock
must be 48MHz. These bits can’t be reset if the USB clock is enabled.
00: (CK_PLL / 1.5) selected
01: CK_PLL selected
 */
 
typedef enum rcc_usb_divider {
    RCC_USB_SYSCLK_DIV_1 = 0x1 << 22,
    RCC_USB_SYSCLK_DIV_1_5 = 0x0 << 22,
    RCC_USB_SYSCLK_DIV_2 = 0x3 << 22,
    RCC_USB_SYSCLK_DIV_2_5 = 0x2 << 22,
} rcc_usb_divider;


/**
 * @brief Start the low speed internal oscillator
 */
static inline void rcc_start_lsi(void) {
	*bb_perip(&RCC_BASE->CSR, RCC_CSR_LSION_BIT) = 1;
	while (*bb_perip(&RCC_BASE->CSR, RCC_CSR_LSIRDY_BIT) == 0);
}

/**
 * @brief STM32F1 clock sources.
 */
typedef enum rcc_clk {
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
 * @brief STM32F1 PLL multipliers.
 */
typedef enum rcc_pll_multiplier {
    RCC_PLLMUL_2 = (0x0 << 18),
    RCC_PLLMUL_3 = (0x1 << 18),
    RCC_PLLMUL_4 = (0x2 << 18),
    RCC_PLLMUL_5 = (0x3 << 18),
    RCC_PLLMUL_6 = (0x4 << 18),
    RCC_PLLMUL_7 = (0x5 << 18),
    RCC_PLLMUL_8 = (0x6 << 18),
    RCC_PLLMUL_9 = (0x7 << 18),
    RCC_PLLMUL_10 = (0x8 << 18),
    RCC_PLLMUL_11 = (0x9 << 18),
    RCC_PLLMUL_12 = (0xA << 18),
    RCC_PLLMUL_13 = (0xB << 18),
    RCC_PLLMUL_14 = (0xC << 18),
    RCC_PLLMUL_15 = (0xD << 18),
    RCC_PLLMUL_16 = (0xE << 18),
} rcc_pll_multiplier;

/* FIXME [0.0.13] Just have data point to an rcc_pll_multiplier! */
/**
 * @brief Start the low speed external oscillatior
 */
static inline void rcc_start_lse(void) {
	bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_LSEBYP_BIT, 0);
	bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_LSEON_BIT, 1);
	while (bb_peri_get_bit(&RCC_BASE->BDCR, RCC_BDCR_LSERDY_BIT ) == 0);
}

/**
 * @brief STM32F1 PLL configuration values.
 * Point to one of these with the "data" field in a struct rcc_pll_cfg.
 * @see struct rcc_pll_cfg.
 */
typedef struct stm32f1_rcc_pll_data {
     rcc_pll_multiplier pll_mul; /**< PLL multiplication factor. */
} stm32f1_rcc_pll_data;

/*
 * Deprecated bits.
 */
static inline void rcc_start_hse(void) {				// Added to support RTClock
//	*bb_perip(&RCC_BASE->CR, RCC_CR_HSEON_BIT) = 1;
	while (bb_peri_get_bit(&RCC_BASE->CR, RCC_CR_HSERDY_BIT) == 0);
}

/**
 * @brief Deprecated; STM32F1 only.
 *
 * Initialize the clock control system. Initializes the system
 * clock source to use the PLL driven by an external oscillator.
 *
 * @param sysclk_src system clock source, must be PLL
 * @param pll_src pll clock source, must be HSE
 * @param pll_mul pll multiplier
 */
__deprecated
void rcc_clk_init(rcc_sysclk_src sysclk_src,
                  rcc_pllsrc pll_src,
                  rcc_pll_multiplier pll_mul);

#ifdef __cplusplus
}
#endif

#endif
