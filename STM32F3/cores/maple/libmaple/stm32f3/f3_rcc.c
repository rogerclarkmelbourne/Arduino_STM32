/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2011 LeafLabs, LLC.
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file libmaple/stm32f3/rcc.c
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 RCC.
 */

#include <libmaple/rcc.h>
#include <libmaple/libmaple.h>
#include <libmaple/bitband.h>

#include "rcc_private.h"

#define APB1                            RCC_APB1
#define APB2                            RCC_APB2
#define AHB                             RCC_AHB

/* Device descriptor table, maps rcc_clk_id onto bus and enable/reset
 * register bit numbers. */
const struct rcc_dev_info rcc_dev_table[] = {
    [RCC_GPIOA]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPAEN_BIT },
    [RCC_GPIOB]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPBEN_BIT },
    [RCC_GPIOC]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPCEN_BIT },
    [RCC_GPIOD]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPDEN_BIT },
    [RCC_GPIOE]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPEEN_BIT },
    [RCC_GPIOF]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_IOPFEN_BIT },

    [RCC_ADC12]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_ADC12EN_BIT},
#if STM32_F3_LINE == STM32_F3_LINE_303
    [RCC_ADC34]  = { .clk_domain = AHB, .line_num = RCC_AHBENR_ADC34EN_BIT },
#endif

    [RCC_DAC]    = { .clk_domain = APB1, .line_num = RCC_APB1ENR_DACEN_BIT },

    [RCC_DMA1]   = { .clk_domain = AHB,  .line_num = RCC_AHBENR_DMA1EN_BIT },
    [RCC_DMA2]   = { .clk_domain = AHB,  .line_num = RCC_AHBENR_DMA2EN_BIT },

    [RCC_I2C1]   = { .clk_domain = APB1, .line_num = RCC_APB1ENR_I2C1EN_BIT },
    [RCC_I2C2]   = { .clk_domain = APB1, .line_num = RCC_APB1ENR_I2C2EN_BIT },

    [RCC_SPI1]   = { .clk_domain = APB2, .line_num = RCC_APB2ENR_SPI1EN_BIT },
    [RCC_SPI2]   = { .clk_domain = APB1, .line_num = RCC_APB1ENR_SPI2EN_BIT },
    [RCC_SPI3]   = { .clk_domain = APB1, .line_num = RCC_APB1ENR_SPI3EN_BIT },

    [RCC_USART1] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_USART1EN_BIT },
    [RCC_USART2] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_USART2EN_BIT },
    [RCC_USART3] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_USART3EN_BIT },
#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
    [RCC_UART4]  = { .clk_domain = APB1, .line_num = RCC_APB1ENR_UART4EN_BIT },
    [RCC_UART5]  = { .clk_domain = APB1, .line_num = RCC_APB1ENR_UART5EN_BIT },
#endif

    [RCC_TIMER1] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_TIM1EN_BIT },
    [RCC_TIMER2] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_TIM2EN_BIT },
    [RCC_TIMER3] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_TIM3EN_BIT },
    [RCC_TIMER4] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_TIM4EN_BIT },
    [RCC_TIMER6] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_TIM6EN_BIT },
#if STM32_F3_LINE == STM32_F3_LINE_303
    [RCC_TIMER7] = { .clk_domain = APB1, .line_num = RCC_APB1ENR_TIM7EN_BIT },
    [RCC_TIMER8] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_TIM8EN_BIT },
#endif
    [RCC_TIMER15] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_TIM15EN_BIT },
    [RCC_TIMER16] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_TIM16EN_BIT },
    [RCC_TIMER17] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_TIM17EN_BIT },

    [RCC_SYSCFG] = { .clk_domain = APB2, .line_num = RCC_APB2ENR_SYSCFGEN_BIT },
    [RCC_PWR]    = { .clk_domain = APB1, .line_num = RCC_APB1ENR_PWREN_BIT },
    [RCC_CRC]    = { .clk_domain = AHB,  .line_num = RCC_AHBENR_CRCEN_BIT },
    [RCC_FLITF]  = { .clk_domain = AHB,  .line_num = RCC_AHBENR_FLITFEN_BIT },
    [RCC_SRAM]   = { .clk_domain = AHB,  .line_num = RCC_AHBENR_SRAMEN_BIT },
    [RCC_USB]    = { .clk_domain = APB1, .line_num = RCC_APB1ENR_USBEN_BIT },
};

/* pll_cfg->data must point to a valid struct stm32f3_rcc_pll_data. */
void rcc_configure_pll(rcc_pll_cfg *pll_cfg) {
    stm32f3_rcc_pll_data *data = pll_cfg->data;
    rcc_pll_multiplier pll_mul = data->pll_mul;
		rcc_prediv_divider pclk_prediv = data->pclk_prediv; //TODO use this!!!
    uint32 cfgr;
		uint32 cfgr2;

    /* Check that the PLL is disabled. */
    ASSERT_FAULT(!rcc_is_clk_on(RCC_CLK_PLL));

		cfgr2 = RCC_BASE->CFGR2;
		cfgr2 &= ~(RCC_CFGR2_PREDIV);
		cfgr2 |= pclk_prediv;
		RCC_BASE->CFGR2 = cfgr2;

    cfgr = RCC_BASE->CFGR;
    cfgr &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL);
    cfgr |= pll_cfg->pllsrc | pll_mul;
    RCC_BASE->CFGR = cfgr;
}

void rcc_clk_enable(rcc_clk_id id) {
    static __io uint32* enable_regs[] = {
        [APB1] = &RCC_BASE->APB1ENR,
        [APB2] = &RCC_BASE->APB2ENR,
        [AHB] = &RCC_BASE->AHBENR,
    };
    rcc_do_clk_enable(enable_regs, id);
}

void rcc_reset_dev(rcc_clk_id id) {
    static __io uint32* reset_regs[] = {
        [APB1] = &RCC_BASE->APB1RSTR,
        [APB2] = &RCC_BASE->APB2RSTR,
        [AHB] = &RCC_BASE->AHBRSTR,
    };
    rcc_do_reset_dev(reset_regs, id);
}

void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider) {
    static const uint32 masks[] = {
        [RCC_PRESCALER_AHB] = RCC_CFGR_HPRE,
        [RCC_PRESCALER_APB1] = RCC_CFGR_PPRE1,
        [RCC_PRESCALER_APB2] = RCC_CFGR_PPRE2,
        [RCC_PRESCALER_USB] = RCC_CFGR_USBPRE,
    };
    rcc_do_set_prescaler(masks, prescaler, divider);
}
