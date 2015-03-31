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
 * @file   wirish/boards/maple/board.cpp
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Maple board file.
 */

#include <board/board.h>         // For this board's header file

#include <wirish_types.h> // For stm32_pin_info and its contents
                                 // (these go into PIN_MAP).

#include "boards_private.h"      // For PMAP_ROW(), which makes
                                 // PIN_MAP easier to read.

// boardInit(): nothing special to do for Maple.
//
// When defining your own board.cpp, you can put extra code in this
// function if you have anything you want done on reset, before main()
// or setup() are called.
//
// If there's nothing special you need done, feel free to leave this
// function out, as we do here.
/*
void boardInit(void) {
}
*/

// Pin map: this lets the basic I/O functions (digitalWrite(),
// analogRead(), pwmWrite()) translate from pin numbers to STM32
// peripherals.
//
// PMAP_ROW() lets us specify a row (really a struct stm32_pin_info)
// in the pin map. Its arguments are:
//
// - GPIO device for the pin (GPIOA, etc.)
// - GPIO bit for the pin (0 through 15)
// - Timer device, or NULL if none
// - Timer channel (1 to 4, for PWM), or 0 if none
// - ADC device, or NULL if none
// - ADC channel, or ADCx if none

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {
/*
    gpio_dev *gpio_device;      GPIO device 
    timer_dev *timer_device;    Pin's timer device, if any.
    const adc_dev *adc_device;  ADC device, if any. 
    uint8 gpio_bit;             Pin's GPIO port bit. 
    uint8 timer_channel;        Timer channel, or 0 if none. 
    uint8 adc_channel;          Pin ADC channel, or ADCx if none. 
	uint8 pinMode; 				mode specific by pinMode call (Roger Clark added to optimize compatibility with Arduino API
*/

    {GPIOA, TIMER2, ADC1,  0, 1,    0}, /* PA0 */
    {GPIOA, TIMER2, ADC1,  1, 2,    1}, /* PA1 */
	{GPIOA, TIMER2, ADC1,  2, 3,    2}, /* PA2 */
    {GPIOA, TIMER2, ADC1,  3, 4,    3}, /* PA3 */
	{GPIOA,   NULL, ADC1,  4, 0,    4}, /* PA4 */	
    {GPIOA,   NULL, ADC1,  5, 0,    5}, /* PA5 */
    {GPIOA, TIMER3, ADC1,  6, 1,    6}, /* PA6 */
    {GPIOA, TIMER3, ADC1,  7, 2,    7}, /* PA7 */
    {GPIOA, TIMER1, NULL,  8, 1, ADCx}, /* PA8 */
    {GPIOA, TIMER1, NULL,  9, 2, ADCx}, /* PA9 */
    {GPIOA, TIMER1, NULL, 10, 3, ADCx}, /* PA10 */
    {GPIOA,   NULL, NULL, 11, 0, ADCx}, /* PA11 */
    {GPIOA,   NULL, NULL, 12, 0, ADCx}, /* PA12 */	
    {GPIOA,   NULL, NULL, 13, 0, ADCx}, /* PA13 */
    {GPIOA,   NULL, NULL, 14, 0, ADCx}, /* PA14 */
    {GPIOA,   NULL, NULL, 15, 0, ADCx}, /* PA15 */
	
	{GPIOB, TIMER3, ADC1,  0, 3,    8}, /* PB0 */
	{GPIOB, TIMER3, ADC1,  1, 4,    9}, /* PB1 */
	/* NOTE PB2 is not included as its Boot 1 */
	{GPIOB,   NULL, NULL,  3, 0, ADCx}, /* PB3  */
    {GPIOB,   NULL, NULL,  4, 0, ADCx}, /* PB4  */
    {GPIOB,   NULL, NULL,  5, 0, ADCx}, /* PB5 */
    {GPIOB, TIMER4, NULL,  6, 1, ADCx}, /* PB6 */
    {GPIOB, TIMER4, NULL,  7, 2, ADCx}, /* PB7 */
    {GPIOB, TIMER4, NULL,  8, 3, ADCx}, /* PB8 */
	{GPIOB, TIMER4, NULL,  9, 4, ADCx}, /* PB9 */
    {GPIOB,   NULL, NULL, 10, 0, ADCx}, /* PB10 */
    {GPIOB,   NULL, NULL, 11, 0, ADCx}, /* PB11 */
    {GPIOB,   NULL, NULL, 12, 0, ADCx}, /* PB12 */
    {GPIOB,   NULL, NULL, 13, 0, ADCx}, /* PB13 */
    {GPIOB,   NULL, NULL, 14, 0, ADCx}, /* PB14 */
    {GPIOB,   NULL, NULL, 15, 0, ADCx}, /* PB15 */


    {GPIOC,   NULL, ADC1,  0, 0,   10}, /* PC0 */
    {GPIOC,   NULL, ADC1,  1, 0,   11}, /* PC1 */
    {GPIOC,   NULL, ADC1,  2, 0,   12}, /* PC2 */
    {GPIOC,   NULL, ADC1,  3, 0,   13}, /* PC3 */
    {GPIOC,   NULL, ADC1,  4, 0,   14}, /* PC4 */
    {GPIOC,   NULL, ADC1,  5, 0,   15}, /* PC5 */
    {GPIOC, TIMER8, NULL,  6, 1, ADCx}, /* PC6 */	
	{GPIOC, TIMER8, NULL,  7, 2, ADCx}, /* PC7 */
    {GPIOC, TIMER8, NULL,  8, 3, ADCx}, /* PC8 */
    {GPIOC, TIMER8, NULL,  9, 4, ADCx}, /* PC9 */
    {GPIOC,   NULL, NULL, 10, 0, ADCx}, /* PC10 UART4_TX/SDIO_D2 */
    {GPIOC,   NULL, NULL, 11, 0, ADCx}, /* PC11 UART4_RX/SDIO_D3 */
    {GPIOC,   NULL, NULL, 12, 0, ADCx}, /* PC12 UART5_TX/SDIO_CK */	
    {GPIOC,   NULL, NULL, 13, 0, ADCx}, /* PC13 TAMPER-RTC */
    {GPIOC,   NULL, NULL, 14, 0, ADCx}, /* PC14 OSC32_IN */
    {GPIOC,   NULL, NULL, 15, 0, ADCx}, /* PC15 OSC32_OUT */

	{GPIOD,   NULL, NULL,   0, 0, ADCx} , /* PD0 OSC_IN */
	{GPIOD,   NULL, NULL,   1, 0, ADCx} , /* PD1  OSC_OUT */
	{GPIOD,   NULL, NULL,   2, 0, ADCx} , /* PD2  TIM3_ETR/UART5_RX SDIO_CMD */
	
	{GPIOD,   NULL, NULL,   3, 0, ADCx} , /* PD3  FSMC_CLK */
	{GPIOD,   NULL, NULL,   4, 0, ADCx} , /* PD4  FSMC_NOE */
	{GPIOD,   NULL, NULL,   5, 0, ADCx} , /* PD5  FSMC_NWE */
	{GPIOD,   NULL, NULL,   6, 0, ADCx} , /* PD6  FSMC_NWAIT */
	{GPIOD,   NULL, NULL,   7, 0, ADCx} , /* PD7  FSMC_NE1/FSMC_NCE2 */
	{GPIOD,   NULL, NULL,   8, 0, ADCx} , /* PD8  FSMC_D13 */
	{GPIOD,   NULL, NULL,   9, 0, ADCx} , /* PD9  FSMC_D14 */
	{GPIOD,   NULL, NULL,  10, 0, ADCx} , /* PD10  FSMC_D15 */
	{GPIOD,   NULL, NULL,  11, 0, ADCx} , /* PD11  FSMC_A16 */
	{GPIOD,   NULL, NULL,  12, 0, ADCx} , /* PD12  FSMC_A17 */
	{GPIOD,   NULL, NULL,  13, 0, ADCx} , /* PD13  FSMC_A18 */	
	{GPIOD,   NULL, NULL,  14, 0, ADCx} , /* PD14  FSMC_D0 */
	{GPIOD,   NULL, NULL,  15, 0, ADCx} , /* PD15  FSMC_D1 */
	
	{GPIOE,   NULL, NULL,   0, 0, ADCx} , /* PE0   */
	{GPIOE,   NULL, NULL,   1, 0, ADCx} , /* PE1   */
	{GPIOE,   NULL, NULL,   2, 0, ADCx} , /* PE2   */
	{GPIOE,   NULL, NULL,   3, 0, ADCx} , /* PE3   */
	{GPIOE,   NULL, NULL,   4, 0, ADCx} , /* PE4   */
	{GPIOE,   NULL, NULL,   5, 0, ADCx} , /* PE5   */
	{GPIOE,   NULL, NULL,   6, 0, ADCx} , /* PE6   */
	{GPIOE,   NULL, NULL,   7, 0, ADCx} , /* PE7   */
	{GPIOE,   NULL, NULL,   8, 0, ADCx} , /* PE8   */
	{GPIOE,   NULL, NULL,   9, 0, ADCx} , /* PE9   */
	{GPIOE,   NULL, NULL,  10, 0, ADCx} , /* PE10  */
	{GPIOE,   NULL, NULL,  11, 0, ADCx} , /* PE11  */
	{GPIOE,   NULL, NULL,  12, 0, ADCx} , /* PE12  */
	{GPIOE,   NULL, NULL,  13, 0, ADCx} , /* PE13  */	
	{GPIOE,   NULL, NULL,  14, 0, ADCx} , /* PE14  */
	{GPIOE,   NULL, NULL,  15, 0, ADCx}  /* PE15  */	

	{GPIOF,   NULL, NULL,   0, 0, ADCx} , /* PF0   */
	{GPIOF,   NULL, NULL,   1, 0, ADCx} , /* PF1   */
	{GPIOF,   NULL, NULL,   2, 0, ADCx} , /* PF2   */
	{GPIOF,   NULL, NULL,   3, 0, ADCx} , /* PF3   */
	{GPIOF,   NULL, NULL,   4, 0, ADCx} , /* PF4   */
	{GPIOF,   NULL, NULL,   5, 0, ADCx} , /* PF5   */
	{GPIOF,   NULL, NULL,   6, 0, ADCx} , /* PF6   */
	{GPIOF,   NULL, NULL,   7, 0, ADCx} , /* PF7   */
	{GPIOF,   NULL, NULL,   8, 0, ADCx} , /* PF8   */
	{GPIOF,   NULL, NULL,   9, 0, ADCx} , /* PF9   */
	{GPIOF,   NULL, NULL,  10, 0, ADCx} , /* PF10  */
	{GPIOF,   NULL, NULL,  11, 0, ADCx} , /* PF11  */
	{GPIOF,   NULL, NULL,  12, 0, ADCx} , /* PF12  */
	{GPIOF,   NULL, NULL,  13, 0, ADCx} , /* PF13  */	
	{GPIOF,   NULL, NULL,  14, 0, ADCx} , /* PF14  */
	{GPIOF,   NULL, NULL,  15, 0, ADCx}  /* PF15  */		
	
	(GPIOG,   NULL, NULL,   0, 0, ADCx} , /* PG0   */
	{GPIOG,   NULL, NULL,   1, 0, ADCx} , /* PG1   */
	{GPIOG,   NULL, NULL,   2, 0, ADCx} , /* PG2   */
	{GPIOG,   NULL, NULL,   3, 0, ADCx} , /* PG3   */
	{GPIOG,   NULL, NULL,   4, 0, ADCx} , /* PG4   */
	{GPIOG,   NULL, NULL,   5, 0, ADCx} , /* PG5   */
	{GPIOG,   NULL, NULL,   6, 0, ADCx} , /* PG6   */
	{GPIOG,   NULL, NULL,   7, 0, ADCx} , /* PG7   */
	{GPIOG,   NULL, NULL,   8, 0, ADCx} , /* PG8   */
	{GPIOG,   NULL, NULL,   9, 0, ADCx} , /* PG9   */
	{GPIOG,   NULL, NULL,  10, 0, ADCx} , /* PG10  */
	{GPIOG,   NULL, NULL,  11, 0, ADCx} , /* PG11  */
	{GPIOG,   NULL, NULL,  12, 0, ADCx} , /* PG12  */
	{GPIOG,   NULL, NULL,  13, 0, ADCx} , /* PG13  */	
	{GPIOG,   NULL, NULL,  14, 0, ADCx} , /* PG14  */
	{GPIOG,   NULL, NULL,  15, 0, ADCx}  /* PG15  */	
	
};

/*  Basically everything that is defined as having a timer us PWM */
extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    PA0,PA1,PA2,PA3,PA6,PA7,PA8,PA9,PA10,PB0,PB1,PB6,PB7,PB8,PB9,PC6,PC7,PC8,PC9
};

/*  Basically everything that is defined having ADC */
extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
    PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PB0,PB1,PC0,PC1,PC2,PC3,PC4,PC5
};

/* not sure what this us used for */
extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
    BOARD_LED_PIN, BOARD_BUTTON_PIN, BOARD_JTMS_SWDIO_PIN,
    BOARD_JTCK_SWCLK_PIN, BOARD_JTDI_PIN, BOARD_JTDO_PIN, BOARD_NJTRST_PIN
};
