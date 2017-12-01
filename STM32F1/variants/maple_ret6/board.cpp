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

/* Roger Clark. Added next to includes for changes to Serial */
#include <libmaple/usart.h>
#include <HardwareSerial.h>

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
// - GPIO device for the pin (&gpioa, etc.)
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
*/

	/* Top header */

	{ &gpioa, &timer2, &adc1, 3, 4, 3 }, /* D0/PA3 */
	{ &gpioa, &timer2, &adc1, 2, 3, 2 }, /* D1/PA2 */
	{ &gpioa, &timer2, &adc1, 0, 1, 0 }, /* D2/PA0 */
	{ &gpioa, &timer2, &adc1, 1, 2, 1 }, /* D3/PA1 */
	{ &gpiob, NULL, NULL, 5, 0, ADCx }, /* D4/PB5 */
	{ &gpiob, &timer4, NULL, 6, 1, ADCx }, /* D5/PB6 */
	{ &gpioa, &timer1, NULL, 8, 1, ADCx }, /* D6/PA8 */
	{ &gpioa, &timer1, NULL, 9, 2, ADCx }, /* D7/PA9 */
	{ &gpioa, &timer1, NULL, 10, 3, ADCx }, /* D8/PA10 */
	{ &gpiob, &timer4, NULL, 7, 2, ADCx }, /* D9/PB7 */
	{ &gpioa, NULL, &adc1, 4, 0, 4 }, /* D10/PA4 */
	{ &gpioa, &timer3, &adc1, 7, 2, 7 }, /* D11/PA7 */
	{ &gpioa, &timer3, &adc1, 6, 1, 6 }, /* D12/PA6 */
	{ &gpioa, NULL, &adc1, 5, 0, 5 }, /* D13/PA5 (LED) */
	{ &gpiob, &timer4, NULL, 8, 3, ADCx }, /* D14/PB8 */

	/* Little header */

	{ &gpioc, NULL, &adc1, 0, 0, 10 }, /* D15/PC0 */
	{ &gpioc, NULL, &adc1, 1, 0, 11 }, /* D16/PC1 */
	{ &gpioc, NULL, &adc1, 2, 0, 12 }, /* D17/PC2 */
	{ &gpioc, NULL, &adc1, 3, 0, 13 }, /* D18/PC3 */
	{ &gpioc, NULL, &adc1, 4, 0, 14 }, /* D19/PC4 */
	{ &gpioc, NULL, &adc1, 5, 0, 15 }, /* D20/PC5 */

	/* External header */

	{ &gpioc, NULL, NULL, 13, 0, ADCx }, /* D21/PC13 */
	{ &gpioc, NULL, NULL, 14, 0, ADCx }, /* D22/PC14 */
	{ &gpioc, NULL, NULL, 15, 0, ADCx }, /* D23/PC15 */
	{ &gpiob, &timer4, NULL, 9, 4, ADCx }, /* D24/PB9 */
	{ &gpiod, NULL, NULL, 2, 0, ADCx }, /* D25/PD2 */
	{ &gpioc, NULL, NULL, 10, 0, ADCx }, /* D26/PC10 */
	{ &gpiob, &timer3, &adc1, 0, 3, 8 }, /* D27/PB0 */
	{ &gpiob, &timer3, &adc1, 1, 4, 9 }, /* D28/PB1 */
	{ &gpiob, NULL, NULL, 10, 0, ADCx }, /* D29/PB10 */
	{ &gpiob, NULL, NULL, 11, 0, ADCx }, /* D30/PB11 */
	{ &gpiob, NULL, NULL, 12, 0, ADCx }, /* D31/PB12 */
	{ &gpiob, NULL, NULL, 13, 0, ADCx }, /* D32/PB13 */
	{ &gpiob, NULL, NULL, 14, 0, ADCx }, /* D33/PB14 */
	{ &gpiob, NULL, NULL, 15, 0, ADCx }, /* D34/PB15 */
	{ &gpioc, &timer8, NULL, 6, 1, ADCx }, /* D35/PC6 */
	{ &gpioc, &timer8, NULL, 7, 2, ADCx }, /* D36/PC7 */
	{ &gpioc, &timer8, NULL, 8, 3, ADCx }, /* D37/PC8 */
	{ &gpioc, &timer8, NULL, 9, 4, ADCx }, /* D38/PC9 (BUT) */

	/* JTAG header */

	{ &gpioa, NULL, NULL, 13, 0, ADCx }, /* D39/PA13 */
	{ &gpioa, NULL, NULL, 14, 0, ADCx }, /* D40/PA14 */
	{ &gpioa, NULL, NULL, 15, 0, ADCx }, /* D41/PA15 */
	{ &gpiob, NULL, NULL, 3, 0, ADCx }, /* D42/PB3  */
	{ &gpiob, NULL, NULL, 4, 0, ADCx }, /* D43/PB4  */

	{ &gpioc, NULL, NULL, 11, 0, ADCx }, /* D44/PC11 UART4_RX/SDIO_D3 */
	{ &gpioc, NULL, NULL, 12, 0, ADCx }, /* D45/PC12 UART5_TX/SDIO_CK */

	{ &gpiod, NULL, NULL, 0, 0, ADCx }, /* PD0 OSC_IN */
	{ &gpiod, NULL, NULL, 1, 0, ADCx }, /* PD1  OSC_OUT */
	{ &gpiob, NULL, NULL, 2, 0, ADCx }, /* PB2  */
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
    BOARD_JTMS_SWDIO_PIN,
    BOARD_JTCK_SWCLK_PIN, BOARD_JTDI_PIN, BOARD_JTDO_PIN, BOARD_NJTRST_PIN
};

/* 
 * Roger Clark
 * 
 * 2015/05/28
 *
 * Moved definitions for Hardware Serial devices from HardwareSerial.cpp so that each board can define which Arduino "Serial" instance
 * Maps to which hardware serial port on the microprocessor
 *
 * Note. Maple Ret always has SERIAL USB, so there is no need for the #fidef for this
 * As its a High Density device (F103RE), it has 6 hardware serial devices. 
 */		
DEFINE_HWSERIAL(Serial1, 1);
DEFINE_HWSERIAL(Serial2, 2);
DEFINE_HWSERIAL(Serial3, 3);
DEFINE_HWSERIAL_UART(Serial4, 4);
DEFINE_HWSERIAL_UART(Serial5, 5);
