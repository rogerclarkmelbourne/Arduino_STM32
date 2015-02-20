/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file wirish/stm32f3/boards_setup.cpp
 * @author	Marti Bolivar <mbolivar@leaflabs.com>,
 * 					Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 chip setup.
 *
 * This file controls how init() behaves on the STM32F3. Be very
 * careful when changing anything here. Many of these values depend
 * upon each other.
 */

#include "boards_private.h"

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

#include <wirish/boards.h>
#include <wirish/usb_serial.h>
#include <libmaple/syscfg.h>
#include <libmaple/fpu.h>

// Allow boards to provide a PLL multiplier. This is useful for
// e.g. STM32F300 value line MCUs, which use slower multipliers.
// (We're leaving the default to RCC_PLLMUL_9 for now, since that
// works for F303 performance line MCUs, which is all that LeafLabs
// currently officially supports).
#ifndef BOARD_RCC_PLLMUL
#define BOARD_RCC_PLLMUL RCC_PLLMUL_9
#endif

namespace wirish {
    namespace priv {

        static stm32f3_rcc_pll_data pll_data = {.pll_mul=BOARD_RCC_PLLMUL, .pclk_prediv=RCC_PREDIV_PCLK_DIV_1};
        __weak rcc_pll_cfg w_board_pll_cfg = {RCC_PLLSRC_HSE, &pll_data};
        __weak adc_prescaler w_adc_pre = ADC_PRE_PCLK_DIV_1;
        __weak adc_smp_rate w_adc_smp = ADC_SMPR_181_5;

        __weak void board_reset_pll(void) {
            // TODO
        }

        __weak void board_setup_clock_prescalers(void) {
            rcc_set_prescaler(RCC_PRESCALER_AHB, RCC_AHB_SYSCLK_DIV_1);
            rcc_set_prescaler(RCC_PRESCALER_APB1, RCC_APB1_HCLK_DIV_2);
            rcc_set_prescaler(RCC_PRESCALER_APB2, RCC_APB2_HCLK_DIV_1);
        }

        __weak void board_setup_gpio(void) {
            gpio_init_all();
        }

        __weak void board_setup_usb(void) {
            SerialUSB.begin();
        }

        __weak void series_init(void) {
            /* We need SYSCFG for external interrupts */
            syscfg_init();

						/* enable floating point unit */
						fpu_enable();
        }
    }
}
