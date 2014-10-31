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
 * @file libmaple/stm32f2/rcc.c
 * @brief STM32F2 RCC.
 */

#include <libmaple/rcc.h>
#include <libmaple/libmaple.h>

#include "rcc_private.h"

#define DEV_ENTRY(domain, dev) \
    {.clk_domain = RCC_##domain, .line_num = RCC_##domain##ENR_##dev##EN_BIT}

const struct rcc_dev_info rcc_dev_table[] = {
    /* AHB1 */
    [RCC_OTGHSULPI] = DEV_ENTRY(AHB1, OTGHSULPI),
    [RCC_OTGHS]     = DEV_ENTRY(AHB1, OTGHS),
    [RCC_ETHMACPTP] = DEV_ENTRY(AHB1, ETHMACPTP),
    [RCC_ETHMACRX]  = DEV_ENTRY(AHB1, ETHMACRX),
    [RCC_ETHMACTX]  = DEV_ENTRY(AHB1, ETHMACTX),
    [RCC_ETHMAC]    = DEV_ENTRY(AHB1, ETHMAC),
    [RCC_DMA2]      = DEV_ENTRY(AHB1, DMA2),
    [RCC_DMA1]      = DEV_ENTRY(AHB1, DMA1),
    [RCC_BKPSRAM]   = DEV_ENTRY(AHB1, BKPSRAM),
    [RCC_CRC]       = DEV_ENTRY(AHB1, CRC),
    [RCC_GPIOI]     = DEV_ENTRY(AHB1, GPIOI),
    [RCC_GPIOH]     = DEV_ENTRY(AHB1, GPIOH),
    [RCC_GPIOG]     = DEV_ENTRY(AHB1, GPIOG),
    [RCC_GPIOF]     = DEV_ENTRY(AHB1, GPIOF),
    [RCC_GPIOE]     = DEV_ENTRY(AHB1, GPIOE),
    [RCC_GPIOD]     = DEV_ENTRY(AHB1, GPIOD),
    [RCC_GPIOC]     = DEV_ENTRY(AHB1, GPIOC),
    [RCC_GPIOB]     = DEV_ENTRY(AHB1, GPIOB),
    [RCC_GPIOA]     = DEV_ENTRY(AHB1, GPIOA),

    /* AHB2 */
    [RCC_OTGFS] = DEV_ENTRY(AHB2, OTGFS),
    [RCC_RNG]   = DEV_ENTRY(AHB2, RNG),
    [RCC_HASH]  = DEV_ENTRY(AHB2, HASH),
    [RCC_CRYP]  = DEV_ENTRY(AHB2, CRYP),
    [RCC_DCMI]  = DEV_ENTRY(AHB2, DCMI),

    /* AHB3 */
    [RCC_FSMC] = DEV_ENTRY(AHB3, FSMC),

    /* APB1 */
    [RCC_DAC]     = DEV_ENTRY(APB1, DAC),
    [RCC_PWR]     = DEV_ENTRY(APB1, PWR),
    [RCC_CAN2]    = DEV_ENTRY(APB1, CAN2),
    [RCC_CAN1]    = DEV_ENTRY(APB1, CAN1),
    [RCC_I2C3]    = DEV_ENTRY(APB1, I2C3),
    [RCC_I2C2]    = DEV_ENTRY(APB1, I2C2),
    [RCC_I2C1]    = DEV_ENTRY(APB1, I2C1),
    [RCC_UART5]   = DEV_ENTRY(APB1, UART5),
    [RCC_UART4]   = DEV_ENTRY(APB1, UART4),
    [RCC_USART3]  = DEV_ENTRY(APB1, USART3),
    [RCC_USART2]  = DEV_ENTRY(APB1, USART2),
    [RCC_SPI3]    = DEV_ENTRY(APB1, SPI3),
    [RCC_SPI2]    = DEV_ENTRY(APB1, SPI2),
    [RCC_WWDG]    = DEV_ENTRY(APB1, WWDG),
    [RCC_TIMER14] = DEV_ENTRY(APB1, TIM14),
    [RCC_TIMER13] = DEV_ENTRY(APB1, TIM13),
    [RCC_TIMER12] = DEV_ENTRY(APB1, TIM12),
    [RCC_TIMER7]  = DEV_ENTRY(APB1, TIM7),
    [RCC_TIMER6]  = DEV_ENTRY(APB1, TIM6),
    [RCC_TIMER5]  = DEV_ENTRY(APB1, TIM5),
    [RCC_TIMER4]  = DEV_ENTRY(APB1, TIM4),
    [RCC_TIMER3]  = DEV_ENTRY(APB1, TIM3),
    [RCC_TIMER2]  = DEV_ENTRY(APB1, TIM2),

    /* APB2 */
    [RCC_TIMER11] = DEV_ENTRY(APB2, TIM11),
    [RCC_TIMER10] = DEV_ENTRY(APB2, TIM10),
    [RCC_TIMER9]  = DEV_ENTRY(APB2, TIM9),
    [RCC_SYSCFG]  = DEV_ENTRY(APB2, SYSCFG),
    [RCC_SPI1]    = DEV_ENTRY(APB2, SPI1),
    [RCC_SDIO]    = DEV_ENTRY(APB2, SDIO),
    [RCC_ADC3]    = DEV_ENTRY(APB2, ADC3),
    [RCC_ADC2]    = DEV_ENTRY(APB2, ADC2),
    [RCC_ADC1]    = DEV_ENTRY(APB2, ADC1),
    [RCC_USART6]  = DEV_ENTRY(APB2, USART6),
    [RCC_USART1]  = DEV_ENTRY(APB2, USART1),
    [RCC_TIMER8]  = DEV_ENTRY(APB2, TIM8),
    [RCC_TIMER1]  = DEV_ENTRY(APB2, TIM1),
};

void rcc_clk_enable(rcc_clk_id id) {
    static __io uint32* enable_regs[] = {
        [RCC_AHB1] = &RCC_BASE->AHB1ENR,
        [RCC_AHB2] = &RCC_BASE->AHB2ENR,
        [RCC_AHB3] = &RCC_BASE->AHB3ENR,
        [RCC_APB1] = &RCC_BASE->APB1ENR,
        [RCC_APB2] = &RCC_BASE->APB2ENR,
    };
    rcc_do_clk_enable(enable_regs, id);
}

void rcc_reset_dev(rcc_clk_id id) {
    static __io uint32* reset_regs[] = {
        [RCC_AHB1] = &RCC_BASE->AHB1RSTR,
        [RCC_AHB2] = &RCC_BASE->AHB2RSTR,
        [RCC_AHB3] = &RCC_BASE->AHB3RSTR,
        [RCC_APB1] = &RCC_BASE->AHB3RSTR,
        [RCC_APB2] = &RCC_BASE->AHB3RSTR,
    };
    rcc_do_reset_dev(reset_regs, id);
}

void rcc_set_prescaler(rcc_prescaler prescaler, uint32 divider) {
    static const uint32 masks[] = {
        [RCC_PRESCALER_MCO2] = RCC_CFGR_MCO2PRE,
        [RCC_PRESCALER_MCO1] = RCC_CFGR_MCO1PRE,
        [RCC_PRESCALER_RTC] = RCC_CFGR_RTCPRE,
        [RCC_PRESCALER_APB2] = RCC_CFGR_PPRE2,
        [RCC_PRESCALER_APB1] = RCC_CFGR_PPRE1,
        [RCC_PRESCALER_AHB] = RCC_CFGR_HPRE,
    };
    rcc_do_set_prescaler(masks, prescaler, divider);
}

/* pll_cfg->data must point to a struct stm32f2_rcc_pll_data. */
void rcc_configure_pll(rcc_pll_cfg *pll_cfg) {
    stm32f2_rcc_pll_data *data = pll_cfg->data;
    uint32 pllcfgr;

    /* Check that the PLL is disabled. */
    ASSERT_FAULT(!rcc_is_clk_on(RCC_CLK_PLL));

    /* Sanity-check all the parameters */
    ASSERT_FAULT((data->pllq >= 4) && (data->pllq <= 15));
    ASSERT_FAULT((data->pllp >= 2) && (data->pllp <= 8));
    ASSERT_FAULT(!(data->pllp & 1));
    ASSERT_FAULT((data->plln >= 192) && (data->plln <= 432));
    ASSERT_FAULT((data->pllm >= 2) && (data->pllm <= 63));

    /* Update RCC_PLLCFGR to reflect new values. */
    pllcfgr = RCC_BASE->PLLCFGR;
    pllcfgr &= ~(RCC_PLLCFGR_PLLQ |
                 RCC_PLLCFGR_PLLP |
                 RCC_PLLCFGR_PLLN |
                 RCC_PLLCFGR_PLLM |
                 RCC_PLLCFGR_PLLSRC);
    pllcfgr |= (pll_cfg->pllsrc |
                (data->pllq << 24) |
                (((data->pllp >> 1) - 1) << 16) |
                (data->plln << 6) |
                data->pllm);
    RCC_BASE->PLLCFGR = pllcfgr;
}
