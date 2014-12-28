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

 #ifdef STM32F1
 
/**
 * @file rcc.c
 * @brief Implements pretty much only the basic clock setup on the
 *        stm32, clock enable/disable and peripheral reset commands.
 */

#include "libmaple.h"
#include "flash.h"
#include "rcc.h"
#include "bitband.h"

#define APB1                            RCC_APB1
#define APB2                            RCC_APB2
#define AHB                             RCC_AHB

struct rcc_dev_info {
    const rcc_clk_domain clk_domain;
    const uint8 line_num;
};

/* Device descriptor table, maps rcc_clk_id onto bus and enable/reset
 * register bit numbers. */
static const struct rcc_dev_info rcc_dev_table[] = {
    [RCC_GPIOA]  = { .clk_domain = APB2, .line_num = 2 },
    [RCC_GPIOB]  = { .clk_domain = APB2, .line_num = 3 },
    [RCC_GPIOC]  = { .clk_domain = APB2, .line_num = 4 },
    [RCC_GPIOD]  = { .clk_domain = APB2, .line_num = 5 },
    [RCC_AFIO]   = { .clk_domain = APB2, .line_num = 0 },
    [RCC_ADC1]   = { .clk_domain = APB2, .line_num = 9 },
    [RCC_ADC2]   = { .clk_domain = APB2, .line_num = 10 },
    [RCC_ADC3]   = { .clk_domain = APB2, .line_num = 15 },
    [RCC_USART1] = { .clk_domain = APB2, .line_num = 14 },
    [RCC_USART2] = { .clk_domain = APB1, .line_num = 17 },
    [RCC_USART3] = { .clk_domain = APB1, .line_num = 18 },
    [RCC_TIMER1] = { .clk_domain = APB2, .line_num = 11 },
    [RCC_TIMER2] = { .clk_domain = APB1, .line_num = 0 },
    [RCC_TIMER3] = { .clk_domain = APB1, .line_num = 1 },
    [RCC_TIMER4] = { .clk_domain = APB1, .line_num = 2 },
    [RCC_SPI1]   = { .clk_domain = APB2, .line_num = 12 },
    [RCC_SPI2]   = { .clk_domain = APB1, .line_num = 14 },
    [RCC_DMA1]   = { .clk_domain = AHB,  .line_num = 0 },
    [RCC_PWR]    = { .clk_domain = APB1, .line_num = 28},
    [RCC_BKP]    = { .clk_domain = APB1, .line_num = 27},
    [RCC_I2C1]   = { .clk_domain = APB1, .line_num = 21 },
    [RCC_I2C2]   = { .clk_domain = APB1, .line_num = 22 },
    [RCC_CRC]    = { .clk_domain = AHB,  .line_num = 6},
    [RCC_FLITF]  = { .clk_domain = AHB,  .line_num = 4},
    [RCC_SRAM]   = { .clk_domain = AHB,  .line_num = 2},
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
    [RCC_GPIOE]  = { .clk_domain = APB2, .line_num = 6 },
    [RCC_GPIOF]  = { .clk_domain = APB2, .line_num = 7 },
    [RCC_GPIOG]  = { .clk_domain = APB2, .line_num = 8 },
    [RCC_UART4]  = { .clk_domain = APB1, .line_num = 19 },
    [RCC_UART5]  = { .clk_domain = APB1, .line_num = 20 },
    [RCC_TIMER5] = { .clk_domain = APB1, .line_num = 3 },
    [RCC_TIMER6] = { .clk_domain = APB1, .line_num = 4 },
    [RCC_TIMER7] = { .clk_domain = APB1, .line_num = 5 },
    [RCC_TIMER8] = { .clk_domain = APB2, .line_num = 13 },
    [RCC_FSMC]   = { .clk_domain = AHB,  .line_num = 8 },
    [RCC_DAC]    = { .clk_domain = APB1, .line_num = 29 },
    [RCC_DMA2]   = { .clk_domain = AHB,  .line_num = 1 },
    [RCC_SDIO]   = { .clk_domain = AHB,  .line_num = 10 },
    [RCC_SPI3]   = { .clk_domain = APB1, .line_num = 15 },
#endif
#ifdef STM32_XL_DENSITY
    [RCC_TIMER9]  = { .clk_domain = APB2, .line_num = 19 },
    [RCC_TIMER10] = { .clk_domain = APB2, .line_num = 20 },
    [RCC_TIMER11] = { .clk_domain = APB2, .line_num = 21 },
    [RCC_TIMER12] = { .clk_domain = APB1, .line_num = 6 },
    [RCC_TIMER13] = { .clk_domain = APB1, .line_num = 7 },
    [RCC_TIMER14] = { .clk_domain = APB1, .line_num = 8 },
#endif
};

/**
 * @brief Initialize the clock control system. Initializes the system
 *        clock source to use the PLL driven by an external oscillator
 * @param sysclk_src system clock source, must be PLL
 * @param pll_src pll clock source, must be HSE
 * @param pll_mul pll multiplier
 */
void rcc_clk_init(rcc_sysclk_src sysclk_src,
                  rcc_pllsrc pll_src,
                  rcc_pll_multiplier pll_mul) {
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
}

/**
 * @brief Turn on the clock line on a peripheral
 * @param id Clock ID of the peripheral to turn on.
 */
void rcc_clk_enable(rcc_clk_id id) {
    static const __io uint32* enable_regs[] = {
        [APB1] = &RCC_BASE->APB1ENR,
        [APB2] = &RCC_BASE->APB2ENR,
        [AHB] = &RCC_BASE->AHBENR,
    };

    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __io uint32* enr = (__io uint32*)enable_regs[clk_domain];
    uint8 lnum = rcc_dev_table[id].line_num;

    bb_peri_set_bit(enr, lnum, 1);
}

/**
 * @brief Reset a peripheral.
 * @param id Clock ID of the peripheral to reset.
 */
void rcc_reset_dev(rcc_clk_id id) {
    static const __io uint32* reset_regs[] = {
        [APB1] = &RCC_BASE->APB1RSTR,
        [APB2] = &RCC_BASE->APB2RSTR,
    };

    rcc_clk_domain clk_domain = rcc_dev_clk(id);
    __io void* addr = (__io void*)reset_regs[clk_domain];
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
    static const uint32 rcc_dev_clk_speed_table[] = {
        [RCC_AHB]  = 72000000,
        [RCC_APB1] = 36000000,
        [RCC_APB2] = 72000000
	  };
    return rcc_dev_clk_speed_table[rcc_dev_clk(id)];
}

/**
 * @brief Get a peripheral's timer clock domain speed
 * @param id Clock ID of the peripheral whose clock domain speed to return
 * @return Clock speed for the given clock ID
 */
uint32 rcc_dev_timer_clk_speed(rcc_clk_id id) {
    return rcc_dev_clk_speed(RCC_APB2); // 72 MHz for all counter
}

/**
 * @brief Set the divider on a peripheral prescaler
 * @param prescaler prescaler to set
 * @param divider prescaler divider
 */
void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider) {
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
}


#endif
