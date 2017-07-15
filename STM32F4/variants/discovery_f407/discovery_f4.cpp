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

#include <libmaple/gpio.h>
#include <libmaple/rcc.h>

#include "wirish_types.h"

//static void initSRAMChip(void);
void boardInit(void) {
	// remap TIMER8 to PC6-9
	gpio_set_af_mode(PC6, 3);
	gpio_set_af_mode(PC7, 3);
	gpio_set_af_mode(PC8, 3);
	gpio_set_af_mode(PC9, 3);

	// remap TIMER1 to PE9,11,13,14
	gpio_set_af_mode(PE9, 1);
	gpio_set_af_mode(PE11, 1);
	gpio_set_af_mode(PE13, 1);
	gpio_set_af_mode(PE14, 1);

	// remap TIMER3 to PB4,5,0,1
	gpio_set_af_mode(PB4, 2);
	gpio_set_af_mode(PB5, 2);
	gpio_set_af_mode(PB0, 2);
	gpio_set_af_mode(PB1, 2);

	//gpio_set_af_mode(GPIOA, 2, 7);
	//gpio_set_af_mode(GPIOA, 3, 7);

#ifdef ARDUINO_STM32F4_NETDUINO2PLUS
    // PA8 Output the Master Clock MCO1
    gpio_set_af_mode(PA8, 0);
    // PB4 as alternate MISO Input
    gpio_set_af_mode(PB4, 5);
    // PA5 as alternate SCK Output
    gpio_set_af_mode(PA5, 5);
    // PA7 as alternate MOSI Output
    gpio_set_af_mode(PA7, 5);
#endif
	return;
}


extern timer_dev timer1;
extern timer_dev timer2;
extern timer_dev timer3;
extern timer_dev timer4;
extern timer_dev timer5;
extern timer_dev timer6;
extern timer_dev timer7;
extern timer_dev timer8;

#if 0
typedef struct stm32_pin_info {
    gpio_dev *gpio_device;      /**< Maple pin's GPIO device */
    timer_dev *timer_device;    /**< Pin's timer device, if any. */
    const adc_dev *adc_device;  /**< ADC device, if any. */
    uint8 timer_channel;        /**< Timer channel, or 0 if none. */
    uint8 adc_channel;          /**< Pin ADC channel, or ADCx if none. */
} stm32_pin_info;
#endif

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {
    {&GPIOA, &timer5, &ADC1, 1,    0}, /* D00/PA0  */
    {&GPIOA, &timer5, &ADC1, 2,    1}, /* D01/PA1  */
    {&GPIOA, &timer5, &ADC1, 3,    2}, /* D02/PA2  */
    {&GPIOA, &timer5, &ADC1, 4,    3}, /* D03/PA3  */
    {&GPIOA,    NULL, &ADC1, 0,    4}, /* D04/PA4  */
    {&GPIOA,    NULL, &ADC1, 0,    5}, /* D05/PA5  */
    {&GPIOA,    NULL, &ADC1, 1,    6}, /* D06/PA6  */ // ala check TIMER3
    {&GPIOA,    NULL, &ADC1, 0,    7}, /* D07/PA7  */
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D08/PA8  */ // remap out
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D09/PA9  */ // remap out
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D10/PA10 */ // remap out
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D11/PA11 */ // remap out
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D12/PA12 */
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D13/PA13 */
    {&GPIOA,    NULL,  NULL, 0, ADCx}, /* D14/PA14 */
    {&GPIOA, &timer2,  NULL, 1, ADCx}, /* D15/PA15 */ // remap in

    {&GPIOB, &timer3, &ADC1, 3,    8}, /* D16/PB0  */
    {&GPIOB, &timer3, &ADC1, 4,    9}, /* D17/PB1  */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D18/PB2  */
    {&GPIOB, &timer2,  NULL, 2, ADCx}, /* D19/PB3  */ // remap in
    {&GPIOB, &timer3,  NULL, 1, ADCx}, /* D20/PB4  */ // remap in
    {&GPIOB, &timer3,  NULL, 2, ADCx}, /* D21/PB5  */ // remap in
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D22/PB6  */ // remap out
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D23/PB7  */ // remap out
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D24/PB8  */ // remap out
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D25/PB9  */ // remap out
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D26/PB10 */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D27/PB11 */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D28/PB12 */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D29/PB13 */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D30/PB14 */
    {&GPIOB,    NULL,  NULL, 0, ADCx}, /* D31/PB15 */

    {&GPIOC,    NULL, &ADC1, 0,   10}, /* D32/PC0  */
    {&GPIOC,    NULL, &ADC1, 0,   11}, /* D33/PC1  */
    {&GPIOC,    NULL, &ADC1, 0,   12}, /* D34/PC2  */
    {&GPIOC,    NULL, &ADC1, 0,   13}, /* D35/PC3  */
    {&GPIOC,    NULL, &ADC1, 0,   14}, /* D36/PC4  */
    {&GPIOC,    NULL, &ADC1, 0,   15}, /* D37/PC5  */
    {&GPIOC, &timer8,  NULL, 1, ADCx}, /* D38/PC6  */
    {&GPIOC, &timer8,  NULL, 2, ADCx}, /* D39/PC7  */
    {&GPIOC, &timer8,  NULL, 3, ADCx}, /* D40/PC8  */
    {&GPIOC, &timer8,  NULL, 4, ADCx}, /* D41/PC9  */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D42/PC10 */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D43/PC11 */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D44/PC12 */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D45/PC13 */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D46/PC14 */
    {&GPIOC,    NULL,  NULL, 0, ADCx}, /* D47/PC15 */

    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D48/PD0  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D49/PD1  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D50/PD2  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D51/PD3  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D52/PD4  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D53/PD5  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D54/PD6  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D55/PD7  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D56/PD8  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D57/PD9  */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D58/PD10 */
    {&GPIOD,    NULL, NULL, 0, ADCx}, /* D59/PD11 */
    {&GPIOD, &timer4, NULL, 1, ADCx}, /* D60/PD12 */  // remap in
    {&GPIOD, &timer4, NULL, 2, ADCx}, /* D61/PD13 */  // remap in
    {&GPIOD, &timer4, NULL, 3, ADCx}, /* D62/PD14 */  // remap in
    {&GPIOD, &timer4, NULL, 4, ADCx}, /* D63/PD15 */  // remap in

    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D64/PE0  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D65/PE1  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D66/PE2  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D67/PE3  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D68/PE4  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D69/PE5  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D70/PE6  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D71/PE7  */
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D72/PE8  */
    {&GPIOE, &timer1, NULL, 1, ADCx}, /* D73/PE9  */  // remap in
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D74/PE10 */
    {&GPIOE, &timer1, NULL, 2, ADCx}, /* D75/PE11 */  // remap in
    {&GPIOE,    NULL, NULL, 0, ADCx}, /* D76/PE12 */
    {&GPIOE, &timer1, NULL, 3, ADCx}, /* D77/PE13 */  // remap in
    {&GPIOE, &timer1, NULL, 4, ADCx}, /* D78/PE14 */  // remap in
    {&GPIOE,    NULL, NULL, 0, ADCx},  /* D79/PE15 */
#if 0 // not available on LQFP100 package
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D80/PF0  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D81/PF1  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D82/PF2  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D83/PF3  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D84/PF4  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D85/PF5  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D86/PF6  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D87/PF7  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D88/PF8  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D89/PF9  */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D90/PF10 */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D91/PF11 */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D92/PF12 */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D93/PF13 */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D94/PF14 */
    {&GPIOF,   NULL, NULL, 0, ADCx}, /* D95/PF15 */

    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D96/PG0  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D97/PG1  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D98/PG2  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D99/PG3  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D100/PG4  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D101/PG5  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D102/PG6  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D103/PG7  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D104/PG8  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D105/PG9  */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D106/PG10 */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D107/PG11 */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D108/PG12 */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D109/PG13 */
    {&GPIOG,   NULL, NULL, 0, ADCx}, /* D110/PG14 */
    {&GPIOG,   NULL, NULL, 0, ADCx}  /* D111/PG15 */
#endif // not available on LQFP100 package
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
