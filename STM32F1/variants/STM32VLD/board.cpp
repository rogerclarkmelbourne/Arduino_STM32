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
 * @file   wirish/boards/maple_mini/board.cpp
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Maple Mini board file.
 */

#include <board/board.h>

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

/* Roger Clark. Added next to includes for changes to Serial */
#include <libmaple/usart.h>
#include <HardwareSerial.h>

#include <wirish_debug.h>
#include <wirish_types.h>

/* Since we want the Serial Wire/JTAG pins as GPIOs, disable both SW
 * and JTAG debug support, unless configured otherwise. */
void boardInit(void) {
#ifndef CONFIG_MAPLE_MINI_NO_DISABLE_DEBUG
    disableDebugPorts();
#endif
}

// Note. See the enum of pin names in board.h

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {


    {&gpioa, &timer2, &adc1,  0, 1,    0}, /* PA0 */
    {&gpioa, &timer2, &adc1,  1, 2,    1}, /* PA1 */
    {&gpioa, &timer2, &adc1,  2, 3,    2}, /* PA2 */	
    {&gpioa, &timer2, &adc1,  3, 4,    3}, /* PA3 */
    {&gpioa,   	NULL, &adc1,  4, 0,    4}, /* PA4 */
    {&gpioa,   	NULL, &adc1,  5, 0,    5}, /* PA5 */
    {&gpioa, &timer3, &adc1,  6, 1,    6}, /* PA6 */
    {&gpioa, &timer3, &adc1,  7, 2,    7}, /* PA7 */
    {&gpioa, &timer1, NULL,  8, 1, ADCx}, /* PA8 */	
    {&gpioa, &timer1, NULL,  9, 2, ADCx}, /* PA9 */	
    {&gpioa, &timer1, NULL, 10, 3, ADCx}, /* PA10 */
    {&gpioa,	NULL, NULL, 11, 0, ADCx}, /* PA11 */
    {&gpioa,   	NULL, NULL, 12, 0, ADCx}, /* PA12 */	
    {&gpioa,   	NULL, NULL, 13, 0, ADCx}, /* PA13 */	
    {&gpioa,   	NULL, NULL, 14, 0, ADCx}, /* PA14 */
    {&gpioa,   	NULL, NULL, 15, 0, ADCx}, /* PA15 */
	
    {&gpiob, &timer3, &adc1,  0, 3,    8}, /* PB0 */	
    {&gpiob, &timer3, &adc1,  1, 4,    9}, /* PB1 */
    {&gpiob,   	NULL, NULL,  2, 0, ADCx}, /* PB2 */
    {&gpiob,   	NULL, NULL,  3, 0, ADCx}, /* PB3 */
    {&gpiob,   NULL, NULL,  4, 0, ADCx}, /* PB4 */
    {&gpiob,   NULL, NULL,  5, 0, ADCx}, /* PB5 */
    {&gpiob, &timer4, NULL,  6, 1, ADCx}, /* PB6 */
    {&gpiob, &timer4, NULL,  7, 2, ADCx}, /* PB7 */	
    {&gpiob, &timer4, NULL,  8, 3, ADCx}, /* PB8 */	
    {&gpiob, &timer4, NULL,  9, 4, ADCx}, /* PB9 */	
    {&gpiob,   NULL, NULL, 10, 0, ADCx}, /* PB10 */	
    {&gpiob,   NULL, NULL, 11, 0, ADCx}, /* PB11 */
    {&gpiob,   NULL, NULL, 12, 0, ADCx}, /* PB12 */
    {&gpiob,   NULL, NULL, 13, 0, ADCx}, /* PB13 */
    {&gpiob,   NULL, NULL, 14, 0, ADCx}, /* PB14 */
    {&gpiob,   NULL, NULL, 15, 0, ADCx}, /* PB15 */

/* Andy Hull - the R8 is similar to the C8 but exposes more GPIO as follows */
    {&gpioc,   NULL, &adc1, 0, 0, 10},  /* PC0 */
    {&gpioc,   NULL, &adc1, 1, 0, 11},  /* PC1 */
    {&gpioc,   NULL, &adc1, 2, 0, 12},  /* PC2 */
    {&gpioc,   NULL, &adc1, 3, 0, 13},  /* PC3 */
    {&gpioc,   NULL, &adc1, 4, 0, 14},  /* PC4 */
    {&gpioc,   NULL, &adc1, 5, 0, 15},  /* PC5 */

    {&gpioc,   NULL, NULL, 6, 0, ADCx},  /* PC6 */
    {&gpioc,   NULL, NULL, 7, 0, ADCx},  /* PC7 */
    {&gpioc,   NULL, NULL, 8, 0, ADCx},  /* PC8 */
    {&gpioc,   NULL, NULL, 9, 0, ADCx},  /* PC9 */

    {&gpioc,   NULL, NULL, 10, 0, ADCx}, /* PC10 */
    {&gpioc,   NULL, NULL, 11, 0, ADCx}, /* PC11 */
    {&gpioc,   NULL, NULL, 12, 0, ADCx}, /* PC12 */
    {&gpioc,   NULL, NULL, 13, 0, ADCx}, /* PC13 */	
    {&gpioc,   NULL, NULL, 14, 0, ADCx}, /* PC14 */
    {&gpioc,   NULL, NULL, 15, 0, ADCx}, /* PC15 */

    {&gpiod,   NULL, NULL, 0, 0, ADCx},  /* PD2 */
	{&gpiod,   NULL, NULL, 1, 0, ADCx},  /* PD2 */
	{&gpiod,   NULL, NULL, 2, 0, ADCx},  /* PD2 */
};

extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    PA0, PA1, PA2, PA3, PA6, PA7, PA8, PA9, PA10, PB0, PB1, PB6, PB7, PB8, PB9
};

extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
    PA0, PA1, PA2 , PA3 , PA4 , PA5 , PA6 , PA7 , PB0 , PB1, PC0, PC1, PC2, PC3, PC4, PC5
};

// Note. These defines are not really used by generic boards. They are for  Maple Serial USB
//#define USB_DP PA12
//#define USB_DM PA11

// NOte. These definitions are not really used for generic boards, they only relate to boards modified to behave like Maple boards
extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
     BOARD_LED_PIN, BOARD_BUTTON_PIN, BOARD_JTMS_SWDIO_PIN,
    BOARD_JTCK_SWCLK_PIN, BOARD_JTDI_PIN, BOARD_JTDO_PIN, BOARD_NJTRST_PIN
	 //USB_DP, USB_DM
};


/* 
 * Roger Clark
 * 
 * 2015/05/28
 *
 * Moved definitions for Hardware Serial devices from HardwareSerial.cpp so that each board can define which Arduino "Serial" instance
 * Maps to which hardware serial port on the microprocessor
 */
						
#ifdef SERIAL_USB
	DEFINE_HWSERIAL(Serial1, 1);

	DEFINE_HWSERIAL(Serial2, 2);

	DEFINE_HWSERIAL(Serial3, 3);

#else
	DEFINE_HWSERIAL(Serial, 1);

	DEFINE_HWSERIAL(Serial1, 2);

	DEFINE_HWSERIAL(Serial2, 3);


#endif
