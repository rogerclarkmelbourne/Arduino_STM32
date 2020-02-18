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
 * @file   discovery_f4.cpp
 * @author ala42
 * @brief  discovery_f4 board file.
 */

#include "discovery_f4.h"

#include <libmaple/gpio.h>
#include <libmaple/rcc.h>

#include <libmaple/libmaple_types.h>


void boardInit(void) {
	// remap TIMER8 to PC6-9
	gpio_set_af_mode(PC6, GPIO_AFMODE_TIM8_11);
	gpio_set_af_mode(PC7, GPIO_AFMODE_TIM8_11);
	gpio_set_af_mode(PC8, GPIO_AFMODE_TIM8_11);
	gpio_set_af_mode(PC9, GPIO_AFMODE_TIM8_11);

	// remap TIMER1 to PE9,11,13,14
	gpio_set_af_mode(PE9,  GPIO_AFMODE_TIM1_2);
	gpio_set_af_mode(PE11, GPIO_AFMODE_TIM1_2);
	gpio_set_af_mode(PE13, GPIO_AFMODE_TIM1_2);
	gpio_set_af_mode(PE14, GPIO_AFMODE_TIM1_2);

	// remap TIMER3 to PB4,5,0,1
	gpio_set_af_mode(PB4, GPIO_AFMODE_TIM3_5);
	gpio_set_af_mode(PB5, GPIO_AFMODE_TIM3_5);
	gpio_set_af_mode(PB0, GPIO_AFMODE_TIM3_5);
	gpio_set_af_mode(PB1, GPIO_AFMODE_TIM3_5);

	//gpio_set_af_mode(GPIOA, 2, GPIO_AFMODE_UASRT1_3);
	//gpio_set_af_mode(GPIOA, 3, GPIO_AFMODE_UASRT1_3);

#ifdef BOARD_STM32F4_NETDUINO2PLUS
    // PA8 Output the Master Clock MCO1
    gpio_set_af_mode(PA8, GPIO_AFMODE_SYSTEM);
    gpio_set_mode(PA8, (gpio_pin_mode)(GPIO_MODE_AF | GPIO_OTYPE_PP | GPIO_OSPEED_100MHZ));
    // PB4 as alternate MISO Input
    gpio_set_af_mode(PB4, GPIO_AFMODE_SPI1_2);
    // PA5 as alternate SCK Output
    gpio_set_af_mode(PA5, GPIO_AFMODE_SPI1_2);
    // PA7 as alternate MOSI Output
    gpio_set_af_mode(PA7, GPIO_AFMODE_SPI1_2);
#endif
	return;
}


extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    0, 1, 2, 3, 15, 16, 17, 19, 20, 21, 38, 39, 49, 41, 60, 61, 62, 63, 73, 75, 77, 78
};

extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
    0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 32, 33, 34, 35, 36, 37
};

extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
    BOARD_LED_PIN, BOARD_BUTTON_PIN, BOARD_JTMS_SWDIO_PIN,
    BOARD_JTCK_SWCLK_PIN, BOARD_JTDI_PIN, BOARD_JTDO_PIN, BOARD_NJTRST_PIN,
    56, 58, 59, 61, 62, 64, 65, 67, 68, 70, 71, 73, 74, 76, 77, 78, 79, 81,
    82, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
};

/*
static void initSRAMChip(void) {
    fsmc_nor_psram_reg_map *regs = FSMC_NOR_PSRAM1_BASE;

    fsmc_sram_init_gpios();
    rcc_clk_enable(RCC_FSMC);

    regs->BCR = (FSMC_BCR_WREN | FSMC_BCR_MWID_16BITS | FSMC_BCR_MTYP_SRAM |
                 FSMC_BCR_MBKEN);
    fsmc_nor_psram_set_addset(regs, 0);
    fsmc_nor_psram_set_datast(regs, 3);
}
*/
