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
 * @file rcc.c
 * @brief Implements pretty much only the basic clock setup on the
 *        stm32, clock enable/disable and peripheral reset commands.
 */

#include "libmaple.h"
#include "flash.h"
#include "gpio.h"
#include "rcc.h"
#include "bitband.h"
#include "pwr.h"

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
    [RCC_GPIOE]   = { .clk_domain = AHB1, .line_num =  4 }, //*
    [RCC_GPIOF]   = { .clk_domain = AHB1, .line_num =  5 }, //*
    [RCC_GPIOG]   = { .clk_domain = AHB1, .line_num =  6 }, //*
    [RCC_GPIOH]   = { .clk_domain = AHB1, .line_num =  7 }, //*
    [RCC_GPIOI]   = { .clk_domain = AHB1, .line_num =  8 }, //*

    [RCC_CRC]     = { .clk_domain = AHB1, .line_num = 12},  //*
//  [RCC_FLITF]   = { .clk_domain = AHB1, .line_num = 15},
//  [RCC_SRAM1]   = { .clk_domain = AHB1, .line_num = 16},
//  [RCC_SRAM2]   = { .clk_domain = AHB1, .line_num = 17},
//  [RCC_BKPSRAM] = { .clk_domain = AHB1, .line_num = 18},  //*
#ifdef __CCMRAM__
    [RCC_CCMRAM]  = { .clk_domain = AHB1, .line_num = 20 }, //?
#endif
    [RCC_DMA1]    = { .clk_domain = AHB1, .line_num = 21 }, //*
    [RCC_DMA2]    = { .clk_domain = AHB1, .line_num = 22 }, //*
    [RCC_ETHMAC]  = { .clk_domain = AHB1, .line_num = 25 },
    [RCC_ETHMACTX]= { .clk_domain = AHB1, .line_num = 26 },
    [RCC_ETHMACRX]= { .clk_domain = AHB1, .line_num = 27 },
    [RCC_ETHMACPTP]={ .clk_domain = AHB1, .line_num = 28 },

    [RCC_DCMI]    = { .clk_domain = AHB2, .line_num =  0 }, //*
    [RCC_USBFS]   = { .clk_domain = AHB2, .line_num =  7 }, //*

    [RCC_FSMC]    = { .clk_domain = AHB3, .line_num =  0 }, //*

    [RCC_TIMER1]  = { .clk_domain = APB2, .line_num =  0 }, //*
    [RCC_TIMER2]  = { .clk_domain = APB1, .line_num =  0 }, //unchanged
    [RCC_TIMER3]  = { .clk_domain = APB1, .line_num =  1 }, //unchanged
    [RCC_TIMER4]  = { .clk_domain = APB1, .line_num =  2 }, //unchanged
    [RCC_TIMER5]  = { .clk_domain = APB1, .line_num =  3 }, //unchanged
    [RCC_TIMER6]  = { .clk_domain = APB1, .line_num =  4 }, //unchanged
    [RCC_TIMER7]  = { .clk_domain = APB1, .line_num =  5 }, //unchanged
    [RCC_TIMER8]  = { .clk_domain = APB2, .line_num =  1 }, //*
    [RCC_TIMER9]  = { .clk_domain = APB2, .line_num = 16 }, //*
    [RCC_TIMER10] = { .clk_domain = APB2, .line_num = 17 }, //*
    [RCC_TIMER11] = { .clk_domain = APB2, .line_num = 18 }, //*
    [RCC_TIMER12] = { .clk_domain = APB1, .line_num =  6 }, //unchanged
    [RCC_TIMER13] = { .clk_domain = APB1, .line_num =  7 }, //unchanged
    [RCC_TIMER14] = { .clk_domain = APB1, .line_num =  8 }, //unchanged
    [RCC_WDG]     = { .clk_domain = APB1, .line_num = 11},  //?
    [RCC_SPI1]    = { .clk_domain = APB2, .line_num = 12 }, //unchanged
    [RCC_SPI2]    = { .clk_domain = APB1, .line_num = 14 }, //unchanged
    [RCC_SPI3]    = { .clk_domain = APB1, .line_num = 15 }, //unchanged
    [RCC_SPI4]    = { .clk_domain = APB2, .line_num = 13 },
    [RCC_SPI5]    = { .clk_domain = APB2, .line_num = 20 },

    [RCC_USART1]  = { .clk_domain = APB2, .line_num =  4 }, //*
    [RCC_USART2]  = { .clk_domain = APB1, .line_num = 17 }, //unchanged
    [RCC_USART3]  = { .clk_domain = APB1, .line_num = 18 }, //unchanged
    [RCC_UART4]   = { .clk_domain = APB1, .line_num = 19 }, //unchanged
    [RCC_UART5]   = { .clk_domain = APB1, .line_num = 20 }, //unchanged
    [RCC_USART6]  = { .clk_domain = APB2, .line_num =  5 }, //*
    [RCC_ADC1]    = { .clk_domain = APB2, .line_num =  8 }, //*
    [RCC_ADC2]    = { .clk_domain = APB2, .line_num =  9 }, //*
    [RCC_ADC3]    = { .clk_domain = APB2, .line_num = 10 }, //*
    [RCC_SDIO]    = { .clk_domain = APB2, .line_num = 11 }, //*
    [RCC_SYSCFG]  = { .clk_domain = APB2, .line_num = 14 }, //*

    [RCC_I2C1]    = { .clk_domain = APB1, .line_num = 21 }, //unchanged
    [RCC_I2C2]    = { .clk_domain = APB1, .line_num = 22 }, //unchanged
    [RCC_I2C3]    = { .clk_domain = APB1, .line_num = 23 }, //?
    [RCC_CAN1]    = { .clk_domain = APB1, .line_num = 25 }, //?
    [RCC_CAN2]    = { .clk_domain = APB1, .line_num = 26 }, //?
    [RCC_PWR]     = { .clk_domain = APB1, .line_num = 28 }, //unchanged
    [RCC_DAC]     = { .clk_domain = APB1, .line_num = 29 }, //unchanged
};

/**
 * @brief Initialize the clock control system. Initializes the system
 *        clock source to use the PLL driven by an external oscillator
 * @param sysclk_src system clock source, must be PLL
 * @param pll_src pll clock source, must be HSE
 * @param pll_mul pll multiplier
 */


//-----------------------------------------------------------------------------
void InitMCO1()
{
    // Turn MCO1 Master Clock Output mode
    RCC->CFGR &= RCC_CFGR_MCO1_RESET_MASK;
    RCC->CFGR |= RCC_CFGR_MCO1Source_HSE | RCC_CFGR_MCO1Div_1;
    // PA8 Output the Master Clock MCO1
    gpio_set_af_mode(PA8, GPIO_AFMODE_SYSTEM);
    gpio_set_mode(PA8, GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_OSPEED_100MHZ);
}

#ifdef USE_CUSTOM_RCC_CLK
typedef struct {
	int pll_n; // Internal system clock: PLL_N = (PLL_P * CLOCK_SPEED_MHZ), must be between 100..432
	int pll_p; // PLL_P = [2, 4, 6]
	int pll_q; // PLL_Q = (PLL_N / 48) - if USB wanted, then this must be an integer between 1..10
	int apb2_div;
	int flash_ws;
} clk_cfg_t;

clk_cfg_t clock_config = { 336, 2, 7, 2, 5}; // set here your custom values

#endif
		
const uint32_t SystemCoreClock = CLOCK_SPEED_HZ;
uint32 PLL_M, PLL_N, PLL_P, PLL_Q;
const uint32 multiples_of_48[] = {0,48,96,144,192,240,288,336,384,432};
const uint32 flash_ws[] = {2,3,5};
//-----------------------------------------------------------------------------
void rcc_clk_init(void)
{
#ifdef CRYSTAL_FREQ
	PLL_M = CRYSTAL_FREQ;
#else
	#error Please define CRYSTAL_FREQ!
#endif

#ifdef USE_CUSTOM_RCC_CLK
	PLL_N = clock_config.pll_n;
	PLL_P = clock_config.pll_p;
	PLL_Q = clock_config.pll_q;
#else
	uint32 found = 0;
	for (PLL_Q=3; PLL_Q<9; PLL_Q++)
	{
		PLL_N = multiples_of_48[PLL_Q];
		for (PLL_P = 2; PLL_P<7; PLL_P+=2)
		{
			if ( (CLOCK_SPEED_MHZ*PLL_P)==PLL_N ) {
				found = 1;
				break;
			}
		}
		if ( found ) break;
	}
#endif

	/* Enable HSE */
	RCC->CR |= (uint32_t)(RCC_CR_HSEON);

	/* Wait till HSE is ready and if Time out is reached exit */
	while (!(RCC->CR & RCC_CR_HSERDY));

	/* Select regulator voltage output Scale 2 mode, System frequency up to 144 MHz */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//*bb_perip(&PWR->CR, PWR_CR_VOS_BIT) = 0;

	/* HCLK = SYSCLK / 1*/
	rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_1);

	/* PCLK2 = HCLK / 1*/
	rcc_set_prescaler(RCC_PRESCALER_APB2, (CLOCK_SPEED_MHZ>100) ? RCC_APB2_HCLK_DIV_2 : RCC_APB2_HCLK_DIV_1);

	/* PCLK1 = HCLK / 2*/
	rcc_set_prescaler(RCC_PRESCALER_APB1, (CLOCK_SPEED_MHZ>100) ? RCC_APB1_HCLK_DIV_4 : RCC_APB1_HCLK_DIV_2);

	uint32 apb2_clk = CLOCK_SPEED_HZ / (1+(CLOCK_SPEED_MHZ/100));
	// save bus clock values
	rcc_dev_clk_speed_table[RCC_AHB1] = CLOCK_SPEED_HZ;
	rcc_dev_clk_speed_table[RCC_APB2] = apb2_clk;
	rcc_dev_clk_speed_table[RCC_APB1] = apb2_clk/2;

	/* Configure the main PLL */
	RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
		(RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

	/* Enable the main PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till the main PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	/* Configure Flash prefetch, Instruction cache, Data cache and wait state */
	flash_init(flash_ws[(CLOCK_SPEED_MHZ/80)]);

	/* Select the main PLL as system clock source */
	RCC->CFGR &= ~(RCC_CFGR_SW_MASK);
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & RCC_CFGR_SWS_MASK ) != RCC_CFGR_SWS_PLL);
}
	

//-----------------------------------------------------------------------------
static const __IO uint32* enable_regs[] = {
	[APB1] = &RCC->APB1ENR,
	[APB2] = &RCC->APB2ENR,
	[AHB1] = &RCC->AHB1ENR,
	[AHB2] = &RCC->AHB2ENR,
	[AHB3] = &RCC->AHB3ENR,
};
/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
void rcc_clk_enable(rcc_clk_id id)
{
    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __IO uint32* enr = (__IO uint32*)enable_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(enr, lnum, 1);
}

/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
void rcc_clk_disable(rcc_clk_id id)
{
    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __IO uint32* enr = (__IO uint32*)enable_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(enr, lnum, 0);
}

static const __IO uint32* reset_regs[] = {
	[APB1] = &RCC->APB1RSTR,
	[APB2] = &RCC->APB2RSTR,
	[AHB1] = &RCC->AHB1RSTR,
	[AHB2] = &RCC->AHB2RSTR,
	[AHB3] = &RCC->AHB3RSTR,
};
/**
 * @brief Reset a peripheral.
 * @param id Clock ID of the peripheral to reset.
 */
void rcc_reset_dev(rcc_clk_id id)
{
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
void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider)
{
    uint32 cfgr = RCC->CFGR & ~(prescaler);
    RCC->CFGR = cfgr | divider;
}
