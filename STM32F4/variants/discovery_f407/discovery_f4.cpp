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

#ifdef BOARD_discovery_f4

#include "discovery_f4.h"

//#include "fsmc.h"
#include "gpio.h"
#include "rcc.h"
#include "timer.h"

#include "wirish_types.h"

//static void initSRAMChip(void);
void boardInit(void) {
	// remap TIMER8 to PC6-9
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
	return;
}


#if 0
typedef struct stm32_pin_info {
    gpio_dev *gpio_device;      /**< Maple pin's GPIO device */
    timer_dev *timer_device;    /**< Pin's timer device, if any. */
    const adc_dev *adc_device;  /**< ADC device, if any. */
    uint8 gpio_bit;             /**< Pin's GPIO port bit. */
    uint8 timer_channel;        /**< Timer channel, or 0 if none. */
    uint8 adc_channel;          /**< Pin ADC channel, or ADCx if none. */
} stm32_pin_info;

#endif

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {
    {GPIOA, TIMER5, ADC1,  0, 1,    0}, /* D00/PA0  */
    {GPIOA, TIMER5, ADC1,  1, 2,    1}, /* D01/PA1  */
    {GPIOA, TIMER5, ADC1,  2, 3,    2}, /* D02/PA2  */
    {GPIOA, TIMER5, ADC1,  3, 4,    3}, /* D03/PA3  */
    {GPIOA,   NULL, ADC1,  4, 0,    4}, /* D04/PA4  */
    {GPIOA,   NULL, ADC1,  5, 0,    5}, /* D05/PA5  */
    {GPIOA,   NULL, ADC1,  6, 1,    6}, /* D06/PA6  */ // ala check TIMER3
    {GPIOA,   NULL, ADC1,  7, 0,    7}, /* D07/PA7  */
    {GPIOA,   NULL, NULL,  8, 0, ADCx}, /* D08/PA8  */ // remap out
    {GPIOA,   NULL, NULL,  9, 0, ADCx}, /* D09/PA9  */ // remap out
    {GPIOA,   NULL, NULL, 10, 0, ADCx}, /* D10/PA10 */ // remap out
    {GPIOA,   NULL, NULL, 11, 0, ADCx}, /* D11/PA11 */ // remap out
    {GPIOA,   NULL, NULL, 12, 0, ADCx}, /* D12/PA12 */
    {GPIOA,   NULL, NULL, 13, 0, ADCx}, /* D13/PA13 */
    {GPIOA,   NULL, NULL, 14, 0, ADCx}, /* D14/PA14 */
    {GPIOA, TIMER2, NULL, 15, 1, ADCx}, /* D15/PA15 */ // remap in

    {GPIOB, TIMER3, ADC1,  0, 3,    8}, /* D16/PB0  */
    {GPIOB, TIMER3, ADC1,  1, 4,    9}, /* D17/PB1  */
    {GPIOB,   NULL, NULL,  2, 0, ADCx}, /* D18/PB2  */
    {GPIOB, TIMER2, NULL,  3, 2, ADCx}, /* D19/PB3  */ // remap in
    {GPIOB, TIMER3, NULL,  4, 1, ADCx}, /* D20/PB4  */ // remap in
    {GPIOB, TIMER3, NULL,  5, 2, ADCx}, /* D21/PB5  */ // remap in
    {GPIOB,   NULL, NULL,  6, 0, ADCx}, /* D22/PB6  */ // remap out
    {GPIOB,   NULL, NULL,  7, 0, ADCx}, /* D23/PB7  */ // remap out
    {GPIOB,   NULL, NULL,  8, 0, ADCx}, /* D24/PB8  */ // remap out
    {GPIOB,   NULL, NULL,  9, 0, ADCx}, /* D25/PB9  */ // remap out
    {GPIOB,   NULL, NULL, 10, 0, ADCx}, /* D26/PB10 */
    {GPIOB,   NULL, NULL, 11, 0, ADCx}, /* D27/PB11 */
    {GPIOB,   NULL, NULL, 12, 0, ADCx}, /* D28/PB12 */
    {GPIOB,   NULL, NULL, 13, 0, ADCx}, /* D29/PB13 */
    {GPIOB,   NULL, NULL, 14, 0, ADCx}, /* D30/PB14 */
    {GPIOB,   NULL, NULL, 15, 0, ADCx}, /* D31/PB15 */

    {GPIOC,   NULL, ADC1,  0, 0,   10}, /* D32/PC0  */
    {GPIOC,   NULL, ADC1,  1, 0,   11}, /* D33/PC1  */
    {GPIOC,   NULL, ADC1,  2, 0,   12}, /* D34/PC2  */
    {GPIOC,   NULL, ADC1,  3, 0,   13}, /* D35/PC3  */
    {GPIOC,   NULL, ADC1,  4, 0,   14}, /* D36/PC4  */
    {GPIOC,   NULL, ADC1,  5, 0,   15}, /* D37/PC5  */
    {GPIOC, TIMER8, NULL,  6, 1, ADCx}, /* D38/PC6  */
    {GPIOC, TIMER8, NULL,  7, 2, ADCx}, /* D39/PC7  */
    {GPIOC, TIMER8, NULL,  8, 3, ADCx}, /* D40/PC8  */
    {GPIOC, TIMER8, NULL,  9, 4, ADCx}, /* D41/PC9  */
    {GPIOC,   NULL, NULL, 10, 0, ADCx}, /* D42/PC10 */
    {GPIOC,   NULL, NULL, 11, 0, ADCx}, /* D43/PC11 */
    {GPIOC,   NULL, NULL, 12, 0, ADCx}, /* D44/PC12 */
    {GPIOC,   NULL, NULL, 13, 0, ADCx}, /* D45/PC13 */
    {GPIOC,   NULL, NULL, 14, 0, ADCx}, /* D46/PC14 */
    {GPIOC,   NULL, NULL, 15, 0, ADCx}, /* D47/PC15 */

    {GPIOD,   NULL, NULL,  0, 0, ADCx}, /* D48/PD0  */
    {GPIOD,   NULL, NULL,  1, 0, ADCx}, /* D49/PD1  */
    {GPIOD,   NULL, NULL,  2, 0, ADCx}, /* D50/PD2  */
    {GPIOD,   NULL, NULL,  3, 0, ADCx}, /* D51/PD3  */
    {GPIOD,   NULL, NULL,  4, 0, ADCx}, /* D52/PD4  */
    {GPIOD,   NULL, NULL,  5, 0, ADCx}, /* D53/PD5  */
    {GPIOD,   NULL, NULL,  6, 0, ADCx}, /* D54/PD6  */
    {GPIOD,   NULL, NULL,  7, 0, ADCx}, /* D55/PD7  */
    {GPIOD,   NULL, NULL,  8, 0, ADCx}, /* D56/PD8  */
    {GPIOD,   NULL, NULL,  9, 0, ADCx}, /* D57/PD9  */
    {GPIOD,   NULL, NULL, 10, 0, ADCx}, /* D58/PD10 */
    {GPIOD,   NULL, NULL, 11, 0, ADCx}, /* D59/PD11 */
    {GPIOD, TIMER4, NULL, 12, 1, ADCx}, /* D60/PD12 */  // remap in
    {GPIOD, TIMER4, NULL, 13, 2, ADCx}, /* D61/PD13 */  // remap in
    {GPIOD, TIMER4, NULL, 14, 3, ADCx}, /* D62/PD14 */  // remap in
    {GPIOD, TIMER4, NULL, 15, 4, ADCx}, /* D63/PD15 */  // remap in

    {GPIOE,   NULL, NULL,  0, 0, ADCx}, /* D64/PE0  */
    {GPIOE,   NULL, NULL,  1, 0, ADCx}, /* D65/PE1  */
    {GPIOE,   NULL, NULL,  2, 0, ADCx}, /* D66/PE2  */
    {GPIOE,   NULL, NULL,  3, 0, ADCx}, /* D67/PE3  */
    {GPIOE,   NULL, NULL,  4, 0, ADCx}, /* D68/PE4  */
    {GPIOE,   NULL, NULL,  5, 0, ADCx}, /* D69/PE5  */
    {GPIOE,   NULL, NULL,  6, 0, ADCx}, /* D70/PE6  */
    {GPIOE,   NULL, NULL,  7, 0, ADCx}, /* D71/PE7  */
    {GPIOE,   NULL, NULL,  8, 0, ADCx}, /* D72/PE8  */
    {GPIOE, TIMER1, NULL,  9, 1, ADCx}, /* D73/PE9  */  // remap in
    {GPIOE,   NULL, NULL, 10, 0, ADCx}, /* D74/PE10 */
    {GPIOE, TIMER1, NULL, 11, 2, ADCx}, /* D75/PE11 */  // remap in
    {GPIOE,   NULL, NULL, 12, 0, ADCx}, /* D76/PE12 */
    {GPIOE, TIMER1, NULL, 13, 3, ADCx}, /* D77/PE13 */  // remap in
    {GPIOE, TIMER1, NULL, 14, 4, ADCx}, /* D78/PE14 */  // remap in
    {GPIOE,   NULL, NULL, 15, 0, ADCx}  /* D79/PE15 */
};

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
#endif
