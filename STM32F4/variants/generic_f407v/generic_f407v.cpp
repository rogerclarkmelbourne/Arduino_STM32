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
 * @file   generic_f407v.cpp
 * @author ala42
 * @brief  generic_f407v board file.
 */

#include "generic_f407v.h"

#include <libmaple/libmaple_types.h>

//static void initSRAMChip(void);
/*****************************************************************************/
//  Alternate functions, see DocID022152 Rev 8, Table 9. 
/*****************************************************************************/
void boardInit(void) {

	return;
}

/* to be defined
const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    0, 1, 2, 3, 15, 16, 17, 19, 20, 21, 38, 39, 49, 41, 60, 61, 62, 63, 73, 75, 77, 78
};
*/
extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] = {
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PC0, PC1, PC2, PC3, PC4, PC5
};

const uint8 boardUsedPins[BOARD_NR_USED_PINS] = {
    BOARD_LED_PIN, BOARD_LED2_PIN, BOARD_BUTTON1_PIN, BOARD_BUTTON2_PIN, BOARD_BUTTON2_PIN,
	BOARD_JTMS_SWDIO_PIN, BOARD_JTCK_SWCLK_PIN,
	FLASH_CS_PIN, FLASH_CLK_PIN, FLASH_DO_PIN, FLASH_DI_PIN,
	NRF24_CE_PIN, NRF24_CS_PIN, NRF24_IRQ_PIN,
	BOARD_SDIO_D0, BOARD_SDIO_D1, BOARD_SDIO_D2, BOARD_SDIO_D3, BOARD_SDIO_CLK, BOARD_SDIO_CMD,
	USB_DM_PIN, USB_DP_PIN
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
