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

#ifdef STM32F4
 
/**
 * @file rcc.c
 * @brief Implements pretty much only the basic clock setup on the
 *        stm32, clock enable/disable and peripheral reset commands.
 */

#include "libmaple.h"
#include "flash.h"
#include "gpio.h"
#include "rcc.h"
#include "bitband.h"

#define APB1                            RCC_APB1
#define APB2                            RCC_APB2
#define AHB1                            RCC_AHB1
#define AHB2                            RCC_AHB2
#define AHB3                            RCC_AHB3

struct rcc_dev_info {
    const rcc_clk_domain clk_domain;
    const uint8 line_num;
};

static uint32 rcc_dev_clk_speed_table[AHB3];

/* Device descriptor table, maps rcc_clk_id onto bus and enable/reset
 * register bit numbers. */
static const struct rcc_dev_info rcc_dev_table[] = {
    [RCC_GPIOA]   = { .clk_domain = AHB1, .line_num =  0 }, //*
    [RCC_GPIOB]   = { .clk_domain = AHB1, .line_num =  1 }, //*
    [RCC_GPIOC]   = { .clk_domain = AHB1, .line_num =  2 }, //*
    [RCC_GPIOD]   = { .clk_domain = AHB1, .line_num =  3 }, //*

//    [RCC_AFIO]    = { .clk_domain = APB2, .line_num =  0 },
    [RCC_ADC1]    = { .clk_domain = APB2, .line_num =  8 }, //*
    [RCC_ADC2]    = { .clk_domain = APB2, .line_num =  9 }, //*
    [RCC_ADC3]    = { .clk_domain = APB2, .line_num = 10 }, //*
    [RCC_USART1]  = { .clk_domain = APB2, .line_num =  4 }, //*
    [RCC_USART2]  = { .clk_domain = APB1, .line_num = 17 }, //unchanged
    [RCC_USART3]  = { .clk_domain = APB1, .line_num = 18 }, //unchanged
    [RCC_TIMER1]  = { .clk_domain = APB2, .line_num =  0 }, //*
    [RCC_TIMER2]  = { .clk_domain = APB1, .line_num =  0 }, //unchanged
    [RCC_TIMER3]  = { .clk_domain = APB1, .line_num =  1 }, //unchanged
    [RCC_TIMER4]  = { .clk_domain = APB1, .line_num =  2 }, //unchanged
    [RCC_SPI1]    = { .clk_domain = APB2, .line_num = 12 }, //unchanged
    [RCC_SPI2]    = { .clk_domain = APB1, .line_num = 14 }, //unchanged
    [RCC_DMA1]    = { .clk_domain = AHB1, .line_num = 21 }, //*
    [RCC_PWR]     = { .clk_domain = APB1, .line_num = 28},  //unchanged
    [RCC_BKP]     = { .clk_domain = AHB1, .line_num = 18},  //*
    [RCC_I2C1]    = { .clk_domain = APB1, .line_num = 21 }, //unchanged
    [RCC_I2C2]    = { .clk_domain = APB1, .line_num = 22 }, //unchanged
    [RCC_CRC]     = { .clk_domain = AHB1, .line_num = 12},  //*
//    [RCC_FLITF]   = { .clk_domain = AHB,  .line_num =  4},
//    [RCC_SRAM]    = { .clk_domain = AHB,  .line_num =  2},

    [RCC_GPIOE]   = { .clk_domain = AHB1, .line_num =  4 }, //*
    [RCC_GPIOF]   = { .clk_domain = AHB1, .line_num =  5 }, //*
    [RCC_GPIOG]   = { .clk_domain = AHB1, .line_num =  6 }, //*
    [RCC_UART4]   = { .clk_domain = APB1, .line_num = 19 }, //unchanged
    [RCC_UART5]   = { .clk_domain = APB1, .line_num = 20 }, //unchanged
    [RCC_TIMER5]  = { .clk_domain = APB1, .line_num =  3 }, //unchanged
    [RCC_TIMER6]  = { .clk_domain = APB1, .line_num =  4 }, //unchanged
    [RCC_TIMER7]  = { .clk_domain = APB1, .line_num =  5 }, //unchanged
    [RCC_TIMER8]  = { .clk_domain = APB2, .line_num =  1 }, //*
    [RCC_FSMC]    = { .clk_domain = AHB3, .line_num =  0 }, //*
    [RCC_DAC]     = { .clk_domain = APB1, .line_num = 29 }, //unchanged
    [RCC_DMA2]    = { .clk_domain = AHB1, .line_num = 22 }, //*
    [RCC_SDIO]    = { .clk_domain = APB2, .line_num = 11 }, //*
    [RCC_SPI3]    = { .clk_domain = APB1, .line_num = 15 }, //unchanged
    [RCC_TIMER9]  = { .clk_domain = APB2, .line_num = 16 }, //*
    [RCC_TIMER10] = { .clk_domain = APB2, .line_num = 17 }, //*
    [RCC_TIMER11] = { .clk_domain = APB2, .line_num = 18 }, //*
    [RCC_TIMER12] = { .clk_domain = APB1, .line_num =  6 }, //unchanged
    [RCC_TIMER13] = { .clk_domain = APB1, .line_num =  7 }, //unchanged
    [RCC_TIMER14] = { .clk_domain = APB1, .line_num =  8 }, //unchanged
    [RCC_USBFS]   = { .clk_domain = AHB2, .line_num =  7 }, //*
    [RCC_SYSCFG]  = { .clk_domain = APB2, .line_num =  14 }, //*
    [RCC_SPI4]    = { .clk_domain = APB1, .line_num = 15 },
};

/**
 * @brief Initialize the clock control system. Initializes the system
 *        clock source to use the PLL driven by an external oscillator
 * @param sysclk_src system clock source, must be PLL
 * @param pll_src pll clock source, must be HSE
 * @param pll_mul pll multiplier
 */

#define HSE_STARTUP_TIMEOUT  ((uint16)0x0500)   /*!< Time out for HSE start up */
#define  RCC_CFGR_HPRE_DIV1                  ((uint32)0x00000000)        /*!< SYSCLK not divided */
#define  RCC_CFGR_PPRE1_DIV2                 ((uint32)0x00001000)        /*!< HCLK divided by 2 */
#define  RCC_CFGR_PPRE1_DIV4                 ((uint32)0x00001400)        /*!< HCLK divided by 4 */
#define  RCC_CFGR_PPRE2_DIV1                 ((uint32)0x00000000)        /*!< HCLK not divided */
#define  RCC_CFGR_PPRE2_DIV2                 ((uint32)0x00008000)        /*!< HCLK divided by 2 */

#define  RCC_PLLCFGR_PLLSRC_HSE              ((uint32)0x00400000)

/*******************  Bits definition for FLASH_ACR register  *****************/
//#define FLASH_ACR_LATENCY                    ((uint32_t)0x00000007)
#define FLASH_ACR_LATENCY_0WS                ((uint32)0x00000000)
#define FLASH_ACR_LATENCY_1WS                ((uint32)0x00000001)
#define FLASH_ACR_LATENCY_2WS                ((uint32)0x00000002)
#define FLASH_ACR_LATENCY_3WS                ((uint32)0x00000003)
#define FLASH_ACR_LATENCY_4WS                ((uint32)0x00000004)
#define FLASH_ACR_LATENCY_5WS                ((uint32)0x00000005)
#define FLASH_ACR_LATENCY_6WS                ((uint32)0x00000006)
#define FLASH_ACR_LATENCY_7WS                ((uint32)0x00000007)

#define FLASH_ACR_PRFTEN                     ((uint32)0x00000100)
#define FLASH_ACR_ICEN                       ((uint32)0x00000200)
#define FLASH_ACR_DCEN                       ((uint32)0x00000400)
#define FLASH_ACR_ICRST                      ((uint32)0x00000800)
#define FLASH_ACR_DCRST                      ((uint32)0x00001000)
#define FLASH_ACR_BYTE0_ADDRESS              ((uint32)0x40023C00)
#define FLASH_ACR_BYTE2_ADDRESS              ((uint32)0x40023C03)

typedef struct
{
  __IO uint32 CR;   /*!< PWR power control register,        Address offset: 0x00 */
  __IO uint32 CSR;  /*!< PWR power control/status register, Address offset: 0x04 */
} PWR_TypeDef;

#define PWR_BASE                             (0x40007000)
#define PWR                                  ((PWR_TypeDef *) PWR_BASE)
#define PWR_CR_VOS                           ((uint16)0x4000)     /*!< Regulator voltage scaling output selection */

typedef struct
{
  __IO uint32 ACR;      /*!< FLASH access control register, Address offset: 0x00 */
  __IO uint32 KEYR;     /*!< FLASH key register,            Address offset: 0x04 */
  __IO uint32 OPTKEYR;  /*!< FLASH option key register,     Address offset: 0x08 */
  __IO uint32 SR;       /*!< FLASH status register,         Address offset: 0x0C */
  __IO uint32 CR;       /*!< FLASH control register,        Address offset: 0x10 */
  __IO uint32 OPTCR;    /*!< FLASH option control register, Address offset: 0x14 */
} FLASH_TypeDef;

#define FLASH_R_BASE          (0x40023C00)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define RESET 0


void InitMCO1()
{
    rcc_reg_map *RCC = RCC_BASE;
    // Turn MCO1 Master Clock Output mode
    RCC->CFGR &= RCC_CFGR_MCO1_RESET_MASK;
    RCC->CFGR |= RCC_CFGR_MCO1Source_HSE | RCC_CFGR_MCO1Div_1;
    // PA8 Output the Master Clock MCO1
    gpio_set_af_mode(PA8, 0);
    gpio_set_mode(PA8, GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_OSPEED_100MHZ);
}


void SetupClock72MHz()
{
	uint32_t SystemCoreClock = 72000000;

	/******************************************************************************/
	/*            PLL (clocked by HSE) used as System clock source                */
	/******************************************************************************/
	/************************* PLL Parameters *************************************/
	/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
	int PLL_M = 4;
	int PLL_N = 216;

	/* SYSCLK = PLL_VCO / PLL_P */
	int PLL_P = 6;

	/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
	int PLL_Q = 9;


	uint32 StartUpCounter = 0, HSEStatus = 0;
	rcc_reg_map *RCC = RCC_BASE;

	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* Select regulator voltage output Scale 2 mode, System frequency up to 144 MHz */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR &= (uint32_t)~(PWR_CR_VOS);

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 1*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

		/* PCLK1 = HCLK / 2*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

		// save bus clock values
		rcc_dev_clk_speed_table[RCC_AHB1] = (SystemCoreClock/1);
		rcc_dev_clk_speed_table[RCC_APB2] = (SystemCoreClock/1);
		rcc_dev_clk_speed_table[RCC_APB1] = (SystemCoreClock/2);

		/* Configure the main PLL */
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
			(RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_2WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
		{
		}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
	  configuration. User can add here some code to deal with this error */
	}
}


void SetupClock120MHz()
{
	uint32_t SystemCoreClock = 120000000;

	/******************************************************************************/
	/*            PLL (clocked by HSE) used as System clock source                */
	/******************************************************************************/
	/************************* PLL Parameters *************************************/
	/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
	int PLL_M = 8;
	int PLL_N = 240;

	/* SYSCLK = PLL_VCO / PLL_P */
	int PLL_P = 2;

	/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
	int PLL_Q = 5;


	uint32 StartUpCounter = 0, HSEStatus = 0;
	rcc_reg_map *RCC = RCC_BASE;

	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* Select regulator voltage output Scale 2 mode, System frequency up to 144 MHz */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR &= (uint32_t)~(PWR_CR_VOS);

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 2*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

		/* PCLK1 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

		// save bus clock values
		rcc_dev_clk_speed_table[RCC_AHB1] = (SystemCoreClock/1);
		rcc_dev_clk_speed_table[RCC_APB2] = (SystemCoreClock/2);
		rcc_dev_clk_speed_table[RCC_APB1] = (SystemCoreClock/4);

		/* Configure the main PLL */
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
			(RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
		{
		}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
	  configuration. User can add here some code to deal with this error */
	}
}


void SetupClock168MHz()
{
	uint32_t SystemCoreClock = 168000000;

	/******************************************************************************/
	/*            PLL (clocked by HSE) used as System clock source                */
	/******************************************************************************/
	/************************* PLL Parameters *************************************/
	/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N */
#ifdef ARDUINO_STM32F4_NETDUINO2PLUS
	int PLL_M = 25; // The NETDUINO has a 25MHz external oscillator
#else
	int PLL_M = 8;
#endif
	int PLL_N = 336;

	/* SYSCLK = PLL_VCO / PLL_P */
	int PLL_P = 2;

	/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
	int PLL_Q = 7;


	uint32 StartUpCounter = 0, HSEStatus = 0;
	rcc_reg_map *RCC = RCC_BASE;

#ifdef ARDUINO_STM32F4_NETDUINO2PLUS
        InitMCO1();
#endif
        
	/* Enable HSE */
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	do
	{
		HSEStatus = RCC->CR & RCC_CR_HSERDY;
		StartUpCounter++;
	} while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

	if ((RCC->CR & RCC_CR_HSERDY) != RESET)
	{
		HSEStatus = (uint32_t)0x01;
	}
	else
	{
		HSEStatus = (uint32_t)0x00;
	}

	if (HSEStatus == (uint32_t)0x01)
	{
		/* Select regulator voltage output Scale 1 mode, System frequency up to 168 MHz */
		RCC->APB1ENR |= RCC_APB1ENR_PWREN;
		PWR->CR |= PWR_CR_VOS;

		/* HCLK = SYSCLK / 1*/
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

		/* PCLK2 = HCLK / 2*/
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

		/* PCLK1 = HCLK / 4*/
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

		// save bus clock values
		rcc_dev_clk_speed_table[RCC_AHB1] = (SystemCoreClock/1);
		rcc_dev_clk_speed_table[RCC_APB2] = (SystemCoreClock/2);
		rcc_dev_clk_speed_table[RCC_APB1] = (SystemCoreClock/4);

		/* Configure the main PLL */
		RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
			(RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

		/* Enable the main PLL */
		RCC->CR |= RCC_CR_PLLON;

		/* Wait till the main PLL is ready */
		while((RCC->CR & RCC_CR_PLLRDY) == 0)
		{
		}

		/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
		FLASH->ACR = FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_5WS;

		/* Select the main PLL as system clock source */
		RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		/* Wait till the main PLL is used as system clock source */
		while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
		{
		}
	}
	else
	{ /* If HSE fails to start-up, the application will have wrong clock
	  configuration. User can add here some code to deal with this error */
	}
}


void rcc_clk_init(rcc_sysclk_src sysclk_src,
                  rcc_pllsrc pll_src,
                  rcc_pll_multiplier pll_mul) {

  //SetupClock72MHz();
#if STM32_TICKS_PER_US == 168
	  SetupClock168MHz();
#endif
#if STM32_TICKS_PER_US == 120
	  SetupClock120MHz();
#endif
#if STM32_TICKS_PER_US == 72
	  SetupClock72MHz();
#endif
}




#define PLL_M      8
#define PLL_N      240
/* SYSCLK = PLL_VCO / PLL_P */
#define PLL_P      2

/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ */
#define PLL_Q      5


void rcc_clk_init2(rcc_sysclk_src sysclk_src,
                  rcc_pllsrc pll_src,
                  rcc_pll_multiplier pll_mul) {

/******************************************************************************/
/*            PLL (clocked by HSE) used as System clock source                */
/******************************************************************************/
  uint32 StartUpCounter = 0, HSEStatus = 0;
  rcc_reg_map *pRCC = RCC_BASE;

  /* Enable HSE */
  pRCC->CR |= RCC_CR_HSEON;

  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = pRCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((pRCC->CR & RCC_CR_HSERDY) != 0)
  {
    HSEStatus = 0x01;
  }
  else
  {
    HSEStatus = 0x00;
  }

  if (HSEStatus == 0x01)
  {
    /* HCLK = SYSCLK / 1*/
    pRCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK / 2*/
    pRCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

    /* PCLK1 = HCLK / 4*/
    pRCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

    /* Configure the main PLL */
    pRCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    /* Enable the main PLL */
    pRCC->CR |= RCC_CR_PLLON;

    /* Wait till the main PLL is ready */
    while((pRCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    ((FLASH_TypeDef*)FLASH)->ACR = FLASH_ACR_PRFTEN |FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_3WS;

    /* Select the main PLL as system clock source */
    pRCC->CFGR &= ~RCC_CFGR_SW;
    pRCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Wait till the main PLL is used as system clock source */
    while ((pRCC->CFGR & RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock
         configuration. User can add here some code to deal with this error */
  }

#if 0
    uint32 cfgr = 0;
    uint32 cr;

    /* Assume that we're going to clock the chip off the PLL, fed by
     * the HSE */
    ASSERT(sysclk_src == RCC_CLKSRC_PLL &&
           pll_src    == RCC_PLLSRC_HSE);

    RCC_BASE->CFGR = pll_src | pll_mul;

    /* Turn on the HSE */
    cr = RCC_BASE->CR;
    cr |= RCC_CR_HSEON;
    RCC_BASE->CR = cr;
    while (!(RCC_BASE->CR & RCC_CR_HSERDY))
        ;

    /* Now the PLL */
    cr |= RCC_CR_PLLON;
    RCC_BASE->CR = cr;
    while (!(RCC_BASE->CR & RCC_CR_PLLRDY))
        ;

    /* Finally, let's switch over to the PLL */
    cfgr &= ~RCC_CFGR_SW;
    cfgr |= RCC_CFGR_SW_PLL;
    RCC_BASE->CFGR = cfgr;
    while ((RCC_BASE->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        ;
#endif
}

/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
void rcc_clk_enable(rcc_clk_id id) {
    static const __IO uint32* enable_regs[] = {
        [APB1] = &RCC_BASE->APB1ENR,
        [APB2] = &RCC_BASE->APB2ENR,
        [AHB1] = &RCC_BASE->AHB1ENR,
        [AHB2] = &RCC_BASE->AHB2ENR,
        [AHB3] = &RCC_BASE->AHB3ENR,
    };

    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __IO uint32* enr = (__IO uint32*)enable_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(enr, lnum, 1);
}

/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
void rcc_clk_disable(rcc_clk_id id) {
    static const __IO uint32* enable_regs[] = {
        [APB1] = &RCC_BASE->APB1ENR,
        [APB2] = &RCC_BASE->APB2ENR,
        [AHB1] = &RCC_BASE->AHB1ENR,
        [AHB2] = &RCC_BASE->AHB2ENR,
        [AHB3] = &RCC_BASE->AHB3ENR,
    };

    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __IO uint32* enr = (__IO uint32*)enable_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(enr, lnum, 0);
}

/**
 * @brief Reset a peripheral.
 * @param id Clock ID of the peripheral to reset.
 */
void rcc_reset_dev(rcc_clk_id id) {
    static const __IO uint32* reset_regs[] = {
        [APB1] = &RCC_BASE->APB1RSTR,
        [APB2] = &RCC_BASE->APB2RSTR,
        [AHB1] = &RCC_BASE->AHB1RSTR,
        [AHB2] = &RCC_BASE->AHB2RSTR,
        [AHB3] = &RCC_BASE->AHB3RSTR,
    };

    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __IO void* addr = (__IO void*)reset_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(addr, lnum, 1);
    bb_peri_set_bit(addr, lnum, 0);
}

/**
 * @brief Get a peripheral's clock domain
 * @param id Clock ID of the peripheral whose clock domain to return
 * @return Clock source for the given clock ID
 */
rcc_clk_domain rcc_dev_clk(rcc_clk_id id) {
    return rcc_dev_table[id].clk_domain;
}

/**
 * @brief Get a peripheral's clock domain speed
 * @param id Clock ID of the peripheral whose clock domain speed to return
 * @return Clock speed for the given clock ID
 */
uint32 rcc_dev_clk_speed(rcc_clk_id id) {
    return rcc_dev_clk_speed_table[rcc_dev_clk(id)];
}

/**
 * @brief Get a peripheral's timer clock domain speed
 * @param id Clock ID of the peripheral whose clock domain speed to return
 * @return Clock speed for the given clock ID
 */
uint32 rcc_dev_timer_clk_speed(rcc_clk_id id) {
    return 2*rcc_dev_clk_speed(id);
}

/**
 * @brief Set the divider on a peripheral prescaler
 * @param prescaler prescaler to set
 * @param divider prescaler divider
 */
void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider) {
#if 0
    static const uint32 masks[] = {
        [RCC_PRESCALER_AHB] = RCC_CFGR_HPRE,
        [RCC_PRESCALER_APB1] = RCC_CFGR_PPRE1,
        [RCC_PRESCALER_APB2] = RCC_CFGR_PPRE2,
        [RCC_PRESCALER_USB] = RCC_CFGR_USBPRE,
        [RCC_PRESCALER_ADC] = RCC_CFGR_ADCPRE,
    };

    uint32 cfgr = RCC_BASE->CFGR;
    cfgr &= ~masks[prescaler];
    cfgr |= divider;
    RCC_BASE->CFGR = cfgr;
#endif
}

#endif
