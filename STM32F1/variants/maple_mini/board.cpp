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

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {

    /* Top header */

    {&gpiob,   NULL, NULL, 11, 0, ADCx}, /* D0/PB11 */
    {&gpiob,   NULL, NULL, 10, 0, ADCx}, /* D1/PB10 */
    {&gpiob,   NULL, NULL,  2, 0, ADCx}, /* D2/PB2 */
    {&gpiob, &timer3, &adc1,  0, 3,    8}, /* D3/PB0 */
    {&gpioa, &timer3, &adc1,  7, 2,    7}, /* D4/PA7 */
    {&gpioa, &timer3, &adc1,  6, 1,    6}, /* D5/PA6 */
    {&gpioa,   NULL, &adc1,  5, 0,    5}, /* D6/PA5 */
    {&gpioa,   NULL, &adc1,  4, 0,    4}, /* D7/PA4 */
    {&gpioa, &timer2, &adc1,  3, 4,    3}, /* D8/PA3 */
    {&gpioa, &timer2, &adc1,  2, 3,    2}, /* D9/PA2 */
    {&gpioa, &timer2, &adc1,  1, 2,    1}, /* D10/PA1 */
    {&gpioa, &timer2, &adc1,  0, 1,    0}, /* D11/PA0 */
    {&gpioc,   NULL, NULL, 15, 0, ADCx}, /* D12/PC15 */
    {&gpioc,   NULL, NULL, 14, 0, ADCx}, /* D13/PC14 */
    {&gpioc,   NULL, NULL, 13, 0, ADCx}, /* D14/PC13 */

    /* Bottom header */

    {&gpiob, &timer4, NULL,  7, 2, ADCx}, /* D15/PB7 */
    {&gpiob, &timer4, NULL,  6, 1, ADCx}, /* D16/PB6 */
    {&gpiob,   NULL, NULL,  5, 0, ADCx}, /* D17/PB5 */
    {&gpiob,   NULL, NULL,  4, 0, ADCx}, /* D18/PB4 */
    {&gpiob,   NULL, NULL,  3, 0, ADCx}, /* D19/PB3 */
    {&gpioa,   NULL, NULL, 15, 0, ADCx}, /* D20/PA15 */
    {&gpioa,   NULL, NULL, 14, 0, ADCx}, /* D21/PA14 */
    {&gpioa,   NULL, NULL, 13, 0, ADCx}, /* D22/PA13 */
    {&gpioa,   NULL, NULL, 12, 0, ADCx}, /* D23/PA12 */
    {&gpioa, &timer1, NULL, 11, 4, ADCx}, /* D24/PA11 */
    {&gpioa, &timer1, NULL, 10, 3, ADCx}, /* D25/PA10 */
    {&gpioa, &timer1, NULL,  9, 2, ADCx}, /* D26/PA9 */
    {&gpioa, &timer1, NULL,  8, 1, ADCx}, /* D27/PA8 */
    {&gpiob,   NULL, NULL, 15, 0, ADCx}, /* D28/PB15 */
    {&gpiob,   NULL, NULL, 14, 0, ADCx}, /* D29/PB14 */
    {&gpiob,   NULL, NULL, 13, 0, ADCx}, /* D30/PB13 */
    {&gpiob,   NULL, NULL, 12, 0, ADCx}, /* D31/PB12 */
    {&gpiob, &timer4, NULL,  8, 3, ADCx}, /* D32/PB8 */
    {&gpiob, &timer3, &adc1,  1, 4,    9}, /* D33/PB1 */
};

extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    3, 4, 5, 8, 9, 10, 11, 15, 16, 25, 26, 27
};

extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
    3, 4, 5, 6, 7, 8, 9, 10, 11
};

#define USB_DP 23
#define USB_DM 24

extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
    32, 32, USB_DP, USB_DM
};


/* 
 * Roger Clark
 * 
 * 2015/05/28
 *
 * Moved definitions for Hardware Serial devices from HardwareSerial.cpp so that each board can define which Arduino "Serial" instance
 * Maps to which hardware serial port on the microprocessor
 *
 * Note. Maple mini always has SERIAL USB, so there is no need for the #fidef for this
 * As its a Medium Density device, it only has 3 hardware serial devices. 
 */							
 
DEFINE_HWSERIAL(Serial1, 1);
DEFINE_HWSERIAL(Serial2, 2);
DEFINE_HWSERIAL(Serial3, 3);
		
	