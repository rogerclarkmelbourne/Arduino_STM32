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

#ifdef BOARD_generic_f407v

#include "generic_f407v.h"

#include "wirish_types.h"

//static void initSRAMChip(void);
/*****************************************************************************/
//  Alternate functions, see DocID022152 Rev 8, Table 9. 
/*****************************************************************************/
void boardInit(void) {
/*	// remap TIMER8 to PC6-9
	gpio_set_af_mode(GPIOC, 6, 3);
	gpio_set_af_mode(GPIOC, 7, 3);
	gpio_set_af_mode(GPIOC, 8, 3);
	gpio_set_af_mode(GPIOC, 9, 3);

	// remap TIMER1 to PE9,11,13,14
	gpio_set_af_mode(GPIOE,  9, 1);
	gpio_set_af_mode(GPIOE, 11, 1);
	gpio_set_af_mode(GPIOE, 13, 1);
	gpio_set_af_mode(GPIOE, 14, 1);

	// remap TIMER3 to PB4,5,0,1
	gpio_set_af_mode(GPIOB,  4, 2);
	gpio_set_af_mode(GPIOB,  5, 2);
	gpio_set_af_mode(GPIOB,  0, 2);
	gpio_set_af_mode(GPIOB,  1, 2);

	//gpio_set_af_mode(GPIOA, 2, 7);
	//gpio_set_af_mode(GPIOA, 3, 7);
*/
	return;
}

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
#endif
