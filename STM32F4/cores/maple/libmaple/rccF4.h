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
 * @file rcc.h
 * @brief reset and clock control definitions and prototypes
 */

#include "libmaple_types.h"
#include "bitband.h"

#ifndef _RCC_H_
#define _RCC_H_

#ifdef __cplusplus
extern "C"{
#endif

/** RCC register map type */
typedef struct
{
  __IO uint32 CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  __IO uint32 PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  __IO uint32 CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  __IO uint32 CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  __IO uint32 AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  __IO uint32 AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  __IO uint32 AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  uint32      RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  __IO uint32 APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  __IO uint32 APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  uint32      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  __IO uint32 AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  __IO uint32 AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  __IO uint32 AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  uint32      RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  __IO uint32 APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  __IO uint32 APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  uint32      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  __IO uint32 AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  __IO uint32 AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  __IO uint32 AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  uint32      RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  __IO uint32 APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  __IO uint32 APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  uint32      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  __IO uint32 BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  __IO uint32 CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  uint32      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  __IO uint32 SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  __IO uint32 PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
} rcc_reg_map;

/** RCC register map base pointer */
//#define RCC_BASE                        ((struct rcc_reg_map*)0x40021000)
#define RCC_BASE              ((rcc_reg_map*)0x40023800)

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

#define RCC_CR_PLLRDY                   BIT(RCC_CR_PLLRDY_BIT)
#define RCC_CR_PLLON                    BIT(RCC_CR_PLLON_BIT)
#define RCC_CR_CSSON                    BIT(RCC_CR_CSSON_BIT)
#define RCC_CR_HSEBYP                   BIT(RCC_CR_HSEBYP_BIT)
#define RCC_CR_HSERDY                   BIT(RCC_CR_HSERDY_BIT)
#define RCC_CR_HSEON                    BIT(RCC_CR_HSEON_BIT)
#define RCC_CR_HSICAL                   (0xFF << 8)
#define RCC_CR_HSITRIM                  (0x1F << 3)
#define RCC_CR_HSIRDY                   BIT(RCC_CR_HSIRDY_BIT)
#define RCC_CR_HSION                    BIT(RCC_CR_HSION_BIT)

/* Clock configuration register */

#define RCC_CFGR_USBPRE_BIT             22
#define RCC_CFGR_PLLXTPRE_BIT           17
#define RCC_CFGR_PLLSRC_BIT             16

#define RCC_CFGR_MCO                    (0x3 << 24)
#define RCC_CFGR_USBPRE                 BIT(RCC_CFGR_USBPRE_BIT)
#define RCC_CFGR_PLLMUL                 (0xF << 18)
#define RCC_CFGR_PLLXTPRE               BIT(RCC_CFGR_PLLXTPRE_BIT)
#define RCC_CFGR_PLLSRC                 BIT(RCC_CFGR_PLLSRC_BIT)
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

#define  RCC_CFGR_MCO1Source_HSI             ((uint32_t)0x00000000)
#define  RCC_CFGR_MCO1Source_LSE             ((uint32_t)0x00200000)
#define  RCC_CFGR_MCO1Source_HSE             ((uint32_t)0x00400000)
#define  RCC_CFGR_MCO1Source_PLLCLK          ((uint32_t)0x00600000)
#define  RCC_CFGR_MCO1Div_1                  ((uint32_t)0x00000000)
#define  RCC_CFGR_MCO1Div_2                  ((uint32_t)0x04000000)
#define  RCC_CFGR_MCO1Div_3                  ((uint32_t)0x05000000)
#define  RCC_CFGR_MCO1Div_4                  ((uint32_t)0x06000000)
#define  RCC_CFGR_MCO1Div_5                  ((uint32_t)0x07000000)
#define  RCC_CFGR_MCO1_RESET_MASK            ((uint32_t)0xF89FFFFF)

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

#define RCC_CIR_CSSC                    BIT(RCC_CIR_CSSC_BIT)
#define RCC_CIR_PLLRDYC                 BIT(RCC_CIR_PLLRDYC_BIT)
#define RCC_CIR_HSERDYC                 BIT(RCC_CIR_HSERDYC_BIT)
#define RCC_CIR_HSIRDYC                 BIT(RCC_CIR_HSIRDYC_BIT)
#define RCC_CIR_LSERDYC                 BIT(RCC_CIR_LSERDYC_BIT)
#define RCC_CIR_LSIRDYC                 BIT(RCC_CIR_LSIRDYC_BIT)
#define RCC_CIR_PLLRDYIE                BIT(RCC_CIR_PLLRDYIE_BIT)
#define RCC_CIR_HSERDYIE                BIT(RCC_CIR_HSERDYIE_BIT)
#define RCC_CIR_HSIRDYIE                BIT(RCC_CIR_HSIRDYIE_BIT)
#define RCC_CIR_LSERDYIE                BIT(RCC_CIR_LSERDYIE_BIT)
#define RCC_CIR_LSIRDYIE                BIT(RCC_CIR_LSIRDYIE_BIT)
#define RCC_CIR_CSSF                    BIT(RCC_CIR_CSSF_BIT)
#define RCC_CIR_PLLRDYF                 BIT(RCC_CIR_PLLRDYF_BIT)
#define RCC_CIR_HSERDYF                 BIT(RCC_CIR_HSERDYF_BIT)
#define RCC_CIR_HSIRDYF                 BIT(RCC_CIR_HSIRDYF_BIT)
#define RCC_CIR_LSERDYF                 BIT(RCC_CIR_LSERDYF_BIT)
#define RCC_CIR_LSIRDYF                 BIT(RCC_CIR_LSIRDYF_BIT)

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

#define RCC_APB2RSTR_TIM11RST           BIT(RCC_APB2RSTR_TIM11RST_BIT)
#define RCC_APB2RSTR_TIM10RST           BIT(RCC_APB2RSTR_TIM10RST_BIT)
#define RCC_APB2RSTR_TIM9RST            BIT(RCC_APB2RSTR_TIM9RST_BIT)
#define RCC_APB2RSTR_ADC3RST            BIT(RCC_APB2RSTR_ADC3RST_BIT)
#define RCC_APB2RSTR_USART1RST          BIT(RCC_APB2RSTR_USART1RST_BIT)
#define RCC_APB2RSTR_TIM8RST            BIT(RCC_APB2RSTR_TIM8RST_BIT)
#define RCC_APB2RSTR_SPI1RST            BIT(RCC_APB2RSTR_SPI1RST_BIT)
#define RCC_APB2RSTR_TIM1RST            BIT(RCC_APB2RSTR_TIM1RST_BIT)
#define RCC_APB2RSTR_ADC2RST            BIT(RCC_APB2RSTR_ADC2RST_BIT)
#define RCC_APB2RSTR_ADC1RST            BIT(RCC_APB2RSTR_ADC1RST_BIT)
#define RCC_APB2RSTR_IOPGRST            BIT(RCC_APB2RSTR_IOPGRST_BIT)
#define RCC_APB2RSTR_IOPFRST            BIT(RCC_APB2RSTR_IOPFRST_BIT)
#define RCC_APB2RSTR_IOPERST            BIT(RCC_APB2RSTR_IOPERST_BIT)
#define RCC_APB2RSTR_IOPDRST            BIT(RCC_APB2RSTR_IOPDRST_BIT)
#define RCC_APB2RSTR_IOPCRST            BIT(RCC_APB2RSTR_IOPCRST_BIT)
#define RCC_APB2RSTR_IOPBRST            BIT(RCC_APB2RSTR_IOPBRST_BIT)
#define RCC_APB2RSTR_IOPARST            BIT(RCC_APB2RSTR_IOPARST_BIT)
#define RCC_APB2RSTR_AFIORST            BIT(RCC_APB2RSTR_AFIORST_BIT)

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

#define RCC_APB1RSTR_DACRST             BIT(RCC_APB1RSTR_DACRST_BIT)
#define RCC_APB1RSTR_PWRRST             BIT(RCC_APB1RSTR_PWRRST_BIT)
#define RCC_APB1RSTR_BKPRST             BIT(RCC_APB1RSTR_BKPRST_BIT)
#define RCC_APB1RSTR_CANRST             BIT(RCC_APB1RSTR_CANRST_BIT)
#define RCC_APB1RSTR_USBRST             BIT(RCC_APB1RSTR_USBRST_BIT)
#define RCC_APB1RSTR_I2C2RST            BIT(RCC_APB1RSTR_I2C2RST_BIT)
#define RCC_APB1RSTR_I2C1RST            BIT(RCC_APB1RSTR_I2C1RST_BIT)
#define RCC_APB1RSTR_UART5RST           BIT(RCC_APB1RSTR_UART5RST_BIT)
#define RCC_APB1RSTR_UART4RST           BIT(RCC_APB1RSTR_UART4RST_BIT)
#define RCC_APB1RSTR_USART3RST          BIT(RCC_APB1RSTR_USART3RST_BIT)
#define RCC_APB1RSTR_USART2RST          BIT(RCC_APB1RSTR_USART2RST_BIT)
#define RCC_APB1RSTR_SPI3RST            BIT(RCC_APB1RSTR_SPI3RST_BIT)
#define RCC_APB1RSTR_SPI2RST            BIT(RCC_APB1RSTR_SPI2RST_BIT)
#define RCC_APB1RSTR_WWDRST             BIT(RCC_APB1RSTR_WWDRST_BIT)
#define RCC_APB1RSTR_TIM14RST           BIT(RCC_APB1RSTR_TIM14RST_BIT)
#define RCC_APB1RSTR_TIM13RST           BIT(RCC_APB1RSTR_TIM13RST_BIT)
#define RCC_APB1RSTR_TIM12RST           BIT(RCC_APB1RSTR_TIM12RST_BIT)
#define RCC_APB1RSTR_TIM7RST            BIT(RCC_APB1RSTR_TIM7RST_BIT)
#define RCC_APB1RSTR_TIM6RST            BIT(RCC_APB1RSTR_TIM6RST_BIT)
#define RCC_APB1RSTR_TIM5RST            BIT(RCC_APB1RSTR_TIM5RST_BIT)
#define RCC_APB1RSTR_TIM4RST            BIT(RCC_APB1RSTR_TIM4RST_BIT)
#define RCC_APB1RSTR_TIM3RST            BIT(RCC_APB1RSTR_TIM3RST_BIT)
#define RCC_APB1RSTR_TIM2RST            BIT(RCC_APB1RSTR_TIM2RST_BIT)

/* AHB peripheral clock enable register */

#define RCC_AHBENR_SDIOEN_BIT           10
#define RCC_AHBENR_FSMCEN_BIT           8
#define RCC_AHBENR_CRCEN_BIT            7
#define RCC_AHBENR_FLITFEN_BIT          4
#define RCC_AHBENR_SRAMEN_BIT           2
#define RCC_AHBENR_DMA2EN_BIT           1
#define RCC_AHBENR_DMA1EN_BIT           0

#define RCC_AHBENR_SDIOEN               BIT(RCC_AHBENR_SDIOEN_BIT)
#define RCC_AHBENR_FSMCEN               BIT(RCC_AHBENR_FSMCEN_BIT)
#define RCC_AHBENR_CRCEN                BIT(RCC_AHBENR_CRCEN_BIT)
#define RCC_AHBENR_FLITFEN              BIT(RCC_AHBENR_FLITFEN_BIT)
#define RCC_AHBENR_SRAMEN               BIT(RCC_AHBENR_SRAMEN_BIT)
#define RCC_AHBENR_DMA2EN               BIT(RCC_AHBENR_DMA2EN_BIT)
#define RCC_AHBENR_DMA1EN               BIT(RCC_AHBENR_DMA1EN_BIT)

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

#define RCC_APB2ENR_TIM11EN             BIT(RCC_APB2ENR_TIM11EN_BIT)
#define RCC_APB2ENR_TIM10EN             BIT(RCC_APB2ENR_TIM10EN_BIT)
#define RCC_APB2ENR_TIM9EN              BIT(RCC_APB2ENR_TIM9EN_BIT)
#define RCC_APB2ENR_ADC3EN              BIT(RCC_APB2ENR_ADC3EN_BIT)
#define RCC_APB2ENR_USART1EN            BIT(RCC_APB2ENR_USART1EN_BIT)
#define RCC_APB2ENR_TIM8EN              BIT(RCC_APB2ENR_TIM8EN_BIT)
#define RCC_APB2ENR_SPI1EN              BIT(RCC_APB2ENR_SPI1EN_BIT)
#define RCC_APB2ENR_TIM1EN              BIT(RCC_APB2ENR_TIM1EN_BIT)
#define RCC_APB2ENR_ADC2EN              BIT(RCC_APB2ENR_ADC2EN_BIT)
#define RCC_APB2ENR_ADC1EN              BIT(RCC_APB2ENR_ADC1EN_BIT)
#define RCC_APB2ENR_IOPGEN              BIT(RCC_APB2ENR_IOPGEN_BIT)
#define RCC_APB2ENR_IOPFEN              BIT(RCC_APB2ENR_IOPFEN_BIT)
#define RCC_APB2ENR_IOPEEN              BIT(RCC_APB2ENR_IOPEEN_BIT)
#define RCC_APB2ENR_IOPDEN              BIT(RCC_APB2ENR_IOPDEN_BIT)
#define RCC_APB2ENR_IOPCEN              BIT(RCC_APB2ENR_IOPCEN_BIT)
#define RCC_APB2ENR_IOPBEN              BIT(RCC_APB2ENR_IOPBEN_BIT)
#define RCC_APB2ENR_IOPAEN              BIT(RCC_APB2ENR_IOPAEN_BIT)
#define RCC_APB2ENR_AFIOEN              BIT(RCC_APB2ENR_AFIOEN_BIT)

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

#define RCC_APB1ENR_DACEN               BIT(RCC_APB1ENR_DACEN_BIT)
#define RCC_APB1ENR_PWREN               BIT(RCC_APB1ENR_PWREN_BIT)
#define RCC_APB1ENR_BKPEN               BIT(RCC_APB1ENR_BKPEN_BIT)
#define RCC_APB1ENR_CANEN               BIT(RCC_APB1ENR_CANEN_BIT)
#define RCC_APB1ENR_USBEN               BIT(RCC_APB1ENR_USBEN_BIT)
#define RCC_APB1ENR_I2C2EN              BIT(RCC_APB1ENR_I2C2EN_BIT)
#define RCC_APB1ENR_I2C1EN              BIT(RCC_APB1ENR_I2C1EN_BIT)
#define RCC_APB1ENR_UART5EN             BIT(RCC_APB1ENR_UART5EN_BIT)
#define RCC_APB1ENR_UART4EN             BIT(RCC_APB1ENR_UART4EN_BIT)
#define RCC_APB1ENR_USART3EN            BIT(RCC_APB1ENR_USART3EN_BIT)
#define RCC_APB1ENR_USART2EN            BIT(RCC_APB1ENR_USART2EN_BIT)
#define RCC_APB1ENR_SPI3EN              BIT(RCC_APB1ENR_SPI3EN_BIT)
#define RCC_APB1ENR_SPI2EN              BIT(RCC_APB1ENR_SPI2EN_BIT)
#define RCC_APB1ENR_WWDEN               BIT(RCC_APB1ENR_WWDEN_BIT)
#define RCC_APB1ENR_TIM14EN             BIT(RCC_APB1ENR_TIM14EN_BIT)
#define RCC_APB1ENR_TIM13EN             BIT(RCC_APB1ENR_TIM13EN_BIT)
#define RCC_APB1ENR_TIM12EN             BIT(RCC_APB1ENR_TIM12EN_BIT)
#define RCC_APB1ENR_TIM7EN              BIT(RCC_APB1ENR_TIM7EN_BIT)
#define RCC_APB1ENR_TIM6EN              BIT(RCC_APB1ENR_TIM6EN_BIT)
#define RCC_APB1ENR_TIM5EN              BIT(RCC_APB1ENR_TIM5EN_BIT)
#define RCC_APB1ENR_TIM4EN              BIT(RCC_APB1ENR_TIM4EN_BIT)
#define RCC_APB1ENR_TIM3EN              BIT(RCC_APB1ENR_TIM3EN_BIT)
#define RCC_APB1ENR_TIM2EN              BIT(RCC_APB1ENR_TIM2EN_BIT)

/* Backup domain control register */

#define RCC_BDCR_BDRST_BIT              16
#define RCC_BDCR_RTCEN_BIT              15
#define RCC_BDCR_LSEBYP_BIT             2
#define RCC_BDCR_LSERDY_BIT             1
#define RCC_BDCR_LSEON_BIT              0

#define RCC_BDCR_BDRST                  BIT(RCC_BDCR_BDRST_BIT)
#define RCC_BDCR_RTCEN                  BIT(RCC_BDCR_RTC_BIT)
#define RCC_BDCR_RTCSEL                 (0x3 << 8)
#define RCC_BDCR_RTCSEL_NONE            (0x0 << 8)
#define RCC_BDCR_RTCSEL_LSE             (0x1 << 8)
#define RCC_BDCR_RTCSEL_LSI             (0x2 << 8)
#define RCC_BDCR_RTCSEL_HSE             (0x3 << 8)
#define RCC_BDCR_LSEBYP                 BIT(RCC_BDCR_LSEBYP_BIT)
#define RCC_BDCR_LSERDY                 BIT(RCC_BDCR_LSERDY_BIT)
#define RCC_BDCR_LSEON                  BIT(RCC_BDCR_LSEON_BIT)

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

#define RCC_CSR_LPWRRSTF                BIT(RCC_CSR_LPWRRSTF_BIT)
#define RCC_CSR_WWDGRSTF                BIT(RCC_CSR_WWDGRSTF_BIT)
#define RCC_CSR_IWDGRSTF                BIT(RCC_CSR_IWDGRSTF_BIT)
#define RCC_CSR_SFTRSTF                 BIT(RCC_CSR_SFTRSTF_BIT)
#define RCC_CSR_PORRSTF                 BIT(RCC_CSR_PORRSTF_BIT)
#define RCC_CSR_PINRSTF                 BIT(RCC_CSR_PINRSTF_BIT)
#define RCC_CSR_RMVF                    BIT(RCC_CSR_RMVF_BIT)
#define RCC_CSR_LSIRDY                  BIT(RCC_CSR_LSIRDY_BIT)
#define RCC_CSR_LSION                   BIT(RCC_CSR_LSION_BIT)

/*
 * Convenience routines
 */

/**
 * SYSCLK sources
 * @see rcc_clk_init()
 */
typedef enum rcc_sysclk_src {
    RCC_CLKSRC_HSI = 0x0,
    RCC_CLKSRC_HSE = 0x1,
    RCC_CLKSRC_PLL = 0x2,
} rcc_sysclk_src;

/**
 * PLL entry clock source
 * @see rcc_clk_init()
 */
typedef enum rcc_pllsrc {
    RCC_PLLSRC_HSE = (0x1 << 16),
    RCC_PLLSRC_HSI_DIV_2 = (0x0 << 16)
} rcc_pllsrc;

/**
 * PLL multipliers
 * @see rcc_clk_init()
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

/**
 * @brief Identifies bus and clock line for a peripheral.
 *
 * Also generally useful as a unique identifier for that peripheral
 * (or its corresponding device struct).
 */
typedef enum rcc_clk_id {
    RCC_GPIOA,
    RCC_GPIOB,
    RCC_GPIOC,
    RCC_GPIOD,
//    RCC_AFIO,
    RCC_ADC1,
    RCC_ADC2,
    RCC_ADC3,
    RCC_USART1,
    RCC_USART2,
    RCC_USART3,
    RCC_TIMER1,
    RCC_TIMER2,
    RCC_TIMER3,
    RCC_TIMER4,
    RCC_SPI1,
    RCC_SPI2,
    RCC_DMA1,
    RCC_PWR,
    RCC_BKP,
    RCC_I2C1,
    RCC_I2C2,
    RCC_CRC,
//    RCC_FLITF,
//    RCC_SRAM,
    RCC_GPIOE,
    RCC_GPIOF,
    RCC_GPIOG,
    RCC_UART4,
    RCC_UART5,
    RCC_TIMER5,
    RCC_TIMER6,
    RCC_TIMER7,
    RCC_TIMER8,
    RCC_FSMC,
    RCC_DAC,
    RCC_DMA2,
    RCC_SDIO,
    RCC_SPI3,
    RCC_TIMER9,
    RCC_TIMER10,
    RCC_TIMER11,
    RCC_TIMER12,
    RCC_TIMER13,
    RCC_TIMER14,
    RCC_USBFS,
    RCC_SYSCFG,
	RCC_SPI4
} rcc_clk_id;

void rcc_clk_init(rcc_sysclk_src sysclk_src,
                  rcc_pllsrc pll_src,
                  rcc_pll_multiplier pll_mul);
void rcc_clk_disable(rcc_clk_id device);
void rcc_clk_enable(rcc_clk_id device);
void rcc_reset_dev(rcc_clk_id device);

void SetupClock72MHz();
void SetupClock120MHz();
void SetupClock168MHz();

typedef enum rcc_clk_domain {
    RCC_APB1,
    RCC_APB2,
    RCC_AHB1,
    RCC_AHB2,
    RCC_AHB3
} rcc_clk_domain;

rcc_clk_domain rcc_dev_clk(rcc_clk_id device);

uint32 rcc_dev_clk_speed(rcc_clk_id id);
uint32 rcc_dev_timer_clk_speed(rcc_clk_id id);

/**
 * Prescaler identifiers
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
 * ADC prescaler dividers
 * @see rcc_set_prescaler()
 */
typedef enum rcc_adc_divider {
    RCC_ADCPRE_PCLK_DIV_2 = 0x0 << 14,
    RCC_ADCPRE_PCLK_DIV_4 = 0x1 << 14,
    RCC_ADCPRE_PCLK_DIV_6 = 0x2 << 14,
    RCC_ADCPRE_PCLK_DIV_8 = 0x3 << 14,
} rcc_adc_divider;

/**
 * APB1 prescaler dividers
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
 * APB2 prescaler dividers
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
 * AHB prescaler dividers
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

void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider);


/**
 * @brief Start the low speed internal oscillatior
 */
static inline void rcc_start_lsi(void) {
	*bb_perip(&RCC_BASE->CSR, RCC_CSR_LSION_BIT) = 1;
	while (*bb_perip(&RCC_BASE->CSR, RCC_CSR_LSIRDY_BIT) == 0);
}

/* FIXME [0.0.13] Just have data point to an rcc_pll_multiplier! */
/**
 * @brief Start the low speed external oscillatior
 */
static inline void rcc_start_lse(void) {
	bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_LSEBYP_BIT, 0);
	bb_peri_set_bit(&RCC_BASE->BDCR, RCC_BDCR_LSEON_BIT, 1);
	while (bb_peri_get_bit(&RCC_BASE->BDCR, RCC_BDCR_LSERDY_BIT ) == 0);
}

/*
 * Deprecated bits.
 */
static inline void rcc_start_hse(void) {				// Added to support RTClock
//	*bb_perip(&RCC_BASE->CR, RCC_CR_HSEON_BIT) = 1;
	while (bb_peri_get_bit(&RCC_BASE->CR, RCC_CR_HSERDY_BIT) == 0);
}



#ifdef __cplusplus
} // extern "C"
#endif

#endif
