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
    __IO uint32 BSRR;     /*!< GPIO port bit set/reset register,      Address offset: 0x18      */
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
    GPIO_OUTPUT_OD_PU   = (GPIO_MODE_OUTPUT | GPIO_OTYPE_OD | GPIO_PUPD_PU | GPIO_OSPEED_50MHZ),
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
    __IO uint32 MEMRMP;    /**< memory remap register  */
    __IO uint32 PMC;      /**< peripheral mode configuration register */
    __IO uint32 EXTICR1;  /**< External interrupt configuration register 1. */
    __IO uint32 EXTICR2;  /**< External interrupt configuration register 2. */
    __IO uint32 EXTICR3;  /**< External interrupt configuration register 3. */
    __IO uint32 EXTICR4;  /**< External interrupt configuration register 4. */
    __IO uint32 CMPCR;    /**< Compensation cell control register */
} syscfg_reg_map;

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
    GPIO_AFMODE_SPI1_4   = 5,
    GPIO_AFMODE_SPI3_5   = 6,
    GPIO_AFMODE_USART1_3 = 7,
    GPIO_AFMODE_USART4_6 = 8,
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

/******************************************************************************/
/*                                                                            */
/*                                 SYSCFG                                     */
/*                                                                            */
/******************************************************************************/

#define SYSCFG_BASE                       (0x40013800)

#define SYSCFG                   ((syscfg_reg_map*)SYSCFG_BASE)

/******************  Bit definition for SYSCFG_MEMRMP register  ***************/
#define SYSCFG_MEMRMP_MEM_MODE_Pos           (0U)                              
#define SYSCFG_MEMRMP_MEM_MODE_Msk           (0x3U << SYSCFG_MEMRMP_MEM_MODE_Pos) /*!< 0x00000003 */
#define SYSCFG_MEMRMP_MEM_MODE               SYSCFG_MEMRMP_MEM_MODE_Msk        /*!< SYSCFG_Memory Remap Config */
#define SYSCFG_MEMRMP_MEM_MODE_0             (0x1U << SYSCFG_MEMRMP_MEM_MODE_Pos) /*!< 0x00000001 */
#define SYSCFG_MEMRMP_MEM_MODE_1             (0x2U << SYSCFG_MEMRMP_MEM_MODE_Pos) /*!< 0x00000002 */
/******************  Bit definition for SYSCFG_PMC register  ******************/
#define SYSCFG_PMC_MII_RMII_SEL_Pos          (23U)                             
#define SYSCFG_PMC_MII_RMII_SEL_Msk          (0x1U << SYSCFG_PMC_MII_RMII_SEL_Pos) /*!< 0x00800000 */
#define SYSCFG_PMC_MII_RMII_SEL              SYSCFG_PMC_MII_RMII_SEL_Msk       /*!<Ethernet PHY interface selection */
/* Old MII_RMII_SEL bit definition, maintained for legacy purpose */
#define SYSCFG_PMC_MII_RMII             SYSCFG_PMC_MII_RMII_SEL    

/*****************  Bit definition for SYSCFG_EXTICR1 register  ***************/
#define SYSCFG_EXTICR1_EXTI0_Pos             (0U)                              
#define SYSCFG_EXTICR1_EXTI0_Msk             (0xFU << SYSCFG_EXTICR1_EXTI0_Pos) /*!< 0x0000000F */
#define SYSCFG_EXTICR1_EXTI0                 SYSCFG_EXTICR1_EXTI0_Msk          /*!<EXTI 0 configuration */
#define SYSCFG_EXTICR1_EXTI1_Pos             (4U)                              
#define SYSCFG_EXTICR1_EXTI1_Msk             (0xFU << SYSCFG_EXTICR1_EXTI1_Pos) /*!< 0x000000F0 */
#define SYSCFG_EXTICR1_EXTI1                 SYSCFG_EXTICR1_EXTI1_Msk          /*!<EXTI 1 configuration */
#define SYSCFG_EXTICR1_EXTI2_Pos             (8U)                              
#define SYSCFG_EXTICR1_EXTI2_Msk             (0xFU << SYSCFG_EXTICR1_EXTI2_Pos) /*!< 0x00000F00 */
#define SYSCFG_EXTICR1_EXTI2                 SYSCFG_EXTICR1_EXTI2_Msk          /*!<EXTI 2 configuration */
#define SYSCFG_EXTICR1_EXTI3_Pos             (12U)                             
#define SYSCFG_EXTICR1_EXTI3_Msk             (0xFU << SYSCFG_EXTICR1_EXTI3_Pos) /*!< 0x0000F000 */
#define SYSCFG_EXTICR1_EXTI3                 SYSCFG_EXTICR1_EXTI3_Msk          /*!<EXTI 3 configuration */
/**
  * @brief   EXTI0 configuration  
  */
#define SYSCFG_EXTICR1_EXTI0_PA              0x0000U                           /*!<PA[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PB              0x0001U                           /*!<PB[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PC              0x0002U                           /*!<PC[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PD              0x0003U                           /*!<PD[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PE              0x0004U                           /*!<PE[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PF              0x0005U                           /*!<PF[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PG              0x0006U                           /*!<PG[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PH              0x0007U                           /*!<PH[0] pin */
#define SYSCFG_EXTICR1_EXTI0_PI              0x0008U                           /*!<PI[0] pin */

/**
  * @brief   EXTI1 configuration  
  */
#define SYSCFG_EXTICR1_EXTI1_PA              0x0000U                           /*!<PA[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PB              0x0010U                           /*!<PB[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PC              0x0020U                           /*!<PC[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PD              0x0030U                           /*!<PD[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PE              0x0040U                           /*!<PE[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PF              0x0050U                           /*!<PF[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PG              0x0060U                           /*!<PG[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PH              0x0070U                           /*!<PH[1] pin */
#define SYSCFG_EXTICR1_EXTI1_PI              0x0080U                           /*!<PI[1] pin */

/**
  * @brief   EXTI2 configuration  
  */
#define SYSCFG_EXTICR1_EXTI2_PA              0x0000U                           /*!<PA[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PB              0x0100U                           /*!<PB[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PC              0x0200U                           /*!<PC[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PD              0x0300U                           /*!<PD[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PE              0x0400U                           /*!<PE[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PF              0x0500U                           /*!<PF[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PG              0x0600U                           /*!<PG[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PH              0x0700U                           /*!<PH[2] pin */
#define SYSCFG_EXTICR1_EXTI2_PI              0x0800U                           /*!<PI[2] pin */

/**
  * @brief   EXTI3 configuration  
  */
#define SYSCFG_EXTICR1_EXTI3_PA              0x0000U                           /*!<PA[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PB              0x1000U                           /*!<PB[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PC              0x2000U                           /*!<PC[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PD              0x3000U                           /*!<PD[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PE              0x4000U                           /*!<PE[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PF              0x5000U                           /*!<PF[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PG              0x6000U                           /*!<PG[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PH              0x7000U                           /*!<PH[3] pin */
#define SYSCFG_EXTICR1_EXTI3_PI              0x8000U                           /*!<PI[3] pin */

/*****************  Bit definition for SYSCFG_EXTICR2 register  ***************/
#define SYSCFG_EXTICR2_EXTI4_Pos             (0U)                              
#define SYSCFG_EXTICR2_EXTI4_Msk             (0xFU << SYSCFG_EXTICR2_EXTI4_Pos) /*!< 0x0000000F */
#define SYSCFG_EXTICR2_EXTI4                 SYSCFG_EXTICR2_EXTI4_Msk          /*!<EXTI 4 configuration */
#define SYSCFG_EXTICR2_EXTI5_Pos             (4U)                              
#define SYSCFG_EXTICR2_EXTI5_Msk             (0xFU << SYSCFG_EXTICR2_EXTI5_Pos) /*!< 0x000000F0 */
#define SYSCFG_EXTICR2_EXTI5                 SYSCFG_EXTICR2_EXTI5_Msk          /*!<EXTI 5 configuration */
#define SYSCFG_EXTICR2_EXTI6_Pos             (8U)                              
#define SYSCFG_EXTICR2_EXTI6_Msk             (0xFU << SYSCFG_EXTICR2_EXTI6_Pos) /*!< 0x00000F00 */
#define SYSCFG_EXTICR2_EXTI6                 SYSCFG_EXTICR2_EXTI6_Msk          /*!<EXTI 6 configuration */
#define SYSCFG_EXTICR2_EXTI7_Pos             (12U)                             
#define SYSCFG_EXTICR2_EXTI7_Msk             (0xFU << SYSCFG_EXTICR2_EXTI7_Pos) /*!< 0x0000F000 */
#define SYSCFG_EXTICR2_EXTI7                 SYSCFG_EXTICR2_EXTI7_Msk          /*!<EXTI 7 configuration */

/**
  * @brief   EXTI4 configuration  
  */
#define SYSCFG_EXTICR2_EXTI4_PA              0x0000U                           /*!<PA[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PB              0x0001U                           /*!<PB[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PC              0x0002U                           /*!<PC[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PD              0x0003U                           /*!<PD[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PE              0x0004U                           /*!<PE[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PF              0x0005U                           /*!<PF[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PG              0x0006U                           /*!<PG[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PH              0x0007U                           /*!<PH[4] pin */
#define SYSCFG_EXTICR2_EXTI4_PI              0x0008U                           /*!<PI[4] pin */

/**
  * @brief   EXTI5 configuration  
  */
#define SYSCFG_EXTICR2_EXTI5_PA              0x0000U                           /*!<PA[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PB              0x0010U                           /*!<PB[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PC              0x0020U                           /*!<PC[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PD              0x0030U                           /*!<PD[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PE              0x0040U                           /*!<PE[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PF              0x0050U                           /*!<PF[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PG              0x0060U                           /*!<PG[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PH              0x0070U                           /*!<PH[5] pin */
#define SYSCFG_EXTICR2_EXTI5_PI              0x0080U                           /*!<PI[5] pin */

/**
  * @brief   EXTI6 configuration  
  */
#define SYSCFG_EXTICR2_EXTI6_PA              0x0000U                           /*!<PA[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PB              0x0100U                           /*!<PB[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PC              0x0200U                           /*!<PC[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PD              0x0300U                           /*!<PD[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PE              0x0400U                           /*!<PE[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PF              0x0500U                           /*!<PF[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PG              0x0600U                           /*!<PG[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PH              0x0700U                           /*!<PH[6] pin */
#define SYSCFG_EXTICR2_EXTI6_PI              0x0800U                           /*!<PI[6] pin */

/**
  * @brief   EXTI7 configuration  
  */
#define SYSCFG_EXTICR2_EXTI7_PA              0x0000U                           /*!<PA[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PB              0x1000U                           /*!<PB[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PC              0x2000U                           /*!<PC[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PD              0x3000U                           /*!<PD[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PE              0x4000U                           /*!<PE[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PF              0x5000U                           /*!<PF[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PG              0x6000U                           /*!<PG[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PH              0x7000U                           /*!<PH[7] pin */
#define SYSCFG_EXTICR2_EXTI7_PI              0x8000U                           /*!<PI[7] pin */

/*****************  Bit definition for SYSCFG_EXTICR3 register  ***************/
#define SYSCFG_EXTICR3_EXTI8_Pos             (0U)                              
#define SYSCFG_EXTICR3_EXTI8_Msk             (0xFU << SYSCFG_EXTICR3_EXTI8_Pos) /*!< 0x0000000F */
#define SYSCFG_EXTICR3_EXTI8                 SYSCFG_EXTICR3_EXTI8_Msk          /*!<EXTI 8 configuration */
#define SYSCFG_EXTICR3_EXTI9_Pos             (4U)                              
#define SYSCFG_EXTICR3_EXTI9_Msk             (0xFU << SYSCFG_EXTICR3_EXTI9_Pos) /*!< 0x000000F0 */
#define SYSCFG_EXTICR3_EXTI9                 SYSCFG_EXTICR3_EXTI9_Msk          /*!<EXTI 9 configuration */
#define SYSCFG_EXTICR3_EXTI10_Pos            (8U)                              
#define SYSCFG_EXTICR3_EXTI10_Msk            (0xFU << SYSCFG_EXTICR3_EXTI10_Pos) /*!< 0x00000F00 */
#define SYSCFG_EXTICR3_EXTI10                SYSCFG_EXTICR3_EXTI10_Msk         /*!<EXTI 10 configuration */
#define SYSCFG_EXTICR3_EXTI11_Pos            (12U)                             
#define SYSCFG_EXTICR3_EXTI11_Msk            (0xFU << SYSCFG_EXTICR3_EXTI11_Pos) /*!< 0x0000F000 */
#define SYSCFG_EXTICR3_EXTI11                SYSCFG_EXTICR3_EXTI11_Msk         /*!<EXTI 11 configuration */

/**
  * @brief   EXTI8 configuration  
  */
#define SYSCFG_EXTICR3_EXTI8_PA              0x0000U                           /*!<PA[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PB              0x0001U                           /*!<PB[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PC              0x0002U                           /*!<PC[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PD              0x0003U                           /*!<PD[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PE              0x0004U                           /*!<PE[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PF              0x0005U                           /*!<PF[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PG              0x0006U                           /*!<PG[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PH              0x0007U                           /*!<PH[8] pin */
#define SYSCFG_EXTICR3_EXTI8_PI              0x0008U                           /*!<PI[8] pin */

/**
  * @brief   EXTI9 configuration  
  */
#define SYSCFG_EXTICR3_EXTI9_PA              0x0000U                           /*!<PA[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PB              0x0010U                           /*!<PB[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PC              0x0020U                           /*!<PC[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PD              0x0030U                           /*!<PD[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PE              0x0040U                           /*!<PE[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PF              0x0050U                           /*!<PF[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PG              0x0060U                           /*!<PG[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PH              0x0070U                           /*!<PH[9] pin */
#define SYSCFG_EXTICR3_EXTI9_PI              0x0080U                           /*!<PI[9] pin */

/**
  * @brief   EXTI10 configuration  
  */
#define SYSCFG_EXTICR3_EXTI10_PA             0x0000U                           /*!<PA[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PB             0x0100U                           /*!<PB[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PC             0x0200U                           /*!<PC[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PD             0x0300U                           /*!<PD[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PE             0x0400U                           /*!<PE[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PF             0x0500U                           /*!<PF[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PG             0x0600U                           /*!<PG[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PH             0x0700U                           /*!<PH[10] pin */
#define SYSCFG_EXTICR3_EXTI10_PI             0x0800U                           /*!<PI[10] pin */

/**
  * @brief   EXTI11 configuration  
  */
#define SYSCFG_EXTICR3_EXTI11_PA             0x0000U                           /*!<PA[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PB             0x1000U                           /*!<PB[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PC             0x2000U                           /*!<PC[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PD             0x3000U                           /*!<PD[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PE             0x4000U                           /*!<PE[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PF             0x5000U                           /*!<PF[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PG             0x6000U                           /*!<PG[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PH             0x7000U                           /*!<PH[11] pin */
#define SYSCFG_EXTICR3_EXTI11_PI             0x8000U                           /*!<PI[11] pin */

/*****************  Bit definition for SYSCFG_EXTICR4 register  ***************/
#define SYSCFG_EXTICR4_EXTI12_Pos            (0U)                              
#define SYSCFG_EXTICR4_EXTI12_Msk            (0xFU << SYSCFG_EXTICR4_EXTI12_Pos) /*!< 0x0000000F */
#define SYSCFG_EXTICR4_EXTI12                SYSCFG_EXTICR4_EXTI12_Msk         /*!<EXTI 12 configuration */
#define SYSCFG_EXTICR4_EXTI13_Pos            (4U)                              
#define SYSCFG_EXTICR4_EXTI13_Msk            (0xFU << SYSCFG_EXTICR4_EXTI13_Pos) /*!< 0x000000F0 */
#define SYSCFG_EXTICR4_EXTI13                SYSCFG_EXTICR4_EXTI13_Msk         /*!<EXTI 13 configuration */
#define SYSCFG_EXTICR4_EXTI14_Pos            (8U)                              
#define SYSCFG_EXTICR4_EXTI14_Msk            (0xFU << SYSCFG_EXTICR4_EXTI14_Pos) /*!< 0x00000F00 */
#define SYSCFG_EXTICR4_EXTI14                SYSCFG_EXTICR4_EXTI14_Msk         /*!<EXTI 14 configuration */
#define SYSCFG_EXTICR4_EXTI15_Pos            (12U)                             
#define SYSCFG_EXTICR4_EXTI15_Msk            (0xFU << SYSCFG_EXTICR4_EXTI15_Pos) /*!< 0x0000F000 */
#define SYSCFG_EXTICR4_EXTI15                SYSCFG_EXTICR4_EXTI15_Msk         /*!<EXTI 15 configuration */

/**
  * @brief   EXTI12 configuration  
  */
#define SYSCFG_EXTICR4_EXTI12_PA             0x0000U                           /*!<PA[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PB             0x0001U                           /*!<PB[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PC             0x0002U                           /*!<PC[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PD             0x0003U                           /*!<PD[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PE             0x0004U                           /*!<PE[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PF             0x0005U                           /*!<PF[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PG             0x0006U                           /*!<PG[12] pin */
#define SYSCFG_EXTICR4_EXTI12_PH             0x0007U                           /*!<PH[12] pin */

/**
  * @brief   EXTI13 configuration  
  */
#define SYSCFG_EXTICR4_EXTI13_PA             0x0000U                           /*!<PA[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PB             0x0010U                           /*!<PB[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PC             0x0020U                           /*!<PC[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PD             0x0030U                           /*!<PD[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PE             0x0040U                           /*!<PE[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PF             0x0050U                           /*!<PF[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PG             0x0060U                           /*!<PG[13] pin */
#define SYSCFG_EXTICR4_EXTI13_PH             0x0070U                           /*!<PH[13] pin */

/**
  * @brief   EXTI14 configuration  
  */
#define SYSCFG_EXTICR4_EXTI14_PA             0x0000U                           /*!<PA[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PB             0x0100U                           /*!<PB[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PC             0x0200U                           /*!<PC[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PD             0x0300U                           /*!<PD[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PE             0x0400U                           /*!<PE[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PF             0x0500U                           /*!<PF[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PG             0x0600U                           /*!<PG[14] pin */
#define SYSCFG_EXTICR4_EXTI14_PH             0x0700U                           /*!<PH[14] pin */

/**
  * @brief   EXTI15 configuration  
  */
#define SYSCFG_EXTICR4_EXTI15_PA             0x0000U                           /*!<PA[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PB             0x1000U                           /*!<PB[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PC             0x2000U                           /*!<PC[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PD             0x3000U                           /*!<PD[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PE             0x4000U                           /*!<PE[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PF             0x5000U                           /*!<PF[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PG             0x6000U                           /*!<PG[15] pin */
#define SYSCFG_EXTICR4_EXTI15_PH             0x7000U                           /*!<PH[15] pin */

/******************  Bit definition for SYSCFG_CMPCR register  ****************/
#define SYSCFG_CMPCR_CMP_PD_Pos              (0U)                              
#define SYSCFG_CMPCR_CMP_PD_Msk              (0x1U << SYSCFG_CMPCR_CMP_PD_Pos) /*!< 0x00000001 */
#define SYSCFG_CMPCR_CMP_PD                  SYSCFG_CMPCR_CMP_PD_Msk           /*!<Compensation cell ready flag */
#define SYSCFG_CMPCR_READY_Pos               (8U)                              
#define SYSCFG_CMPCR_READY_Msk               (0x1U << SYSCFG_CMPCR_READY_Pos)  /*!< 0x00000100 */
#define SYSCFG_CMPCR_READY                   SYSCFG_CMPCR_READY_Msk            /*!<Compensation cell power-down */


#ifdef __cplusplus
}
#endif

#endif
