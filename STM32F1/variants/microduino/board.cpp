/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 * Modifications Copyright (c) 2014 Ian Harvey
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
 * @file   variants/microduino/board.cpp
 * @author Ian Harvey
 * @brief  Board pin definitions for Microduino STM32
 */

#include <board/board.h>

/* Roger Clark. Added next to includes for changes to Serial */
#include <libmaple/usart.h>
#include <HardwareSerial.h>

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

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


    {&gpioa, &timer1, NULL, 10, 3, ADCx}, /* D0(RxD0)/PA10 */
    {&gpioa, &timer1, NULL,  9, 2, ADCx}, /* D1(TxD1)/PA9 */
    {&gpiob,   NULL, NULL, 11, 0, ADCx}, /* D2/PB11 */
    {&gpiob,   NULL, NULL, 10, 0, ADCx}, /* D3/PB10 */
    {&gpioa, &timer1, NULL,  8, 1, ADCx}, /* D4/PA8 */
    {&gpioa,   NULL, NULL, 13, 0, ADCx}, /* D5/PA13 */
    {&gpioa,   NULL, NULL, 14, 0, ADCx}, /* D6/PA14 */
    {&gpioa,   NULL, NULL, 15, 0, ADCx}, /* D7/PA15 */
    {&gpiob,   NULL, NULL,  3, 0, ADCx}, /* D8/PB3 */
    {&gpiob,   NULL, NULL,  4, 0, ADCx}, /* D9/PB4 */
    {&gpioa,   NULL, &adc1,  4, 0,    4}, /* D10/PA4 */
    {&gpioa, &timer3, &adc1,  7, 2,    7}, /* D11/PA7 */
    {&gpioa, &timer3, &adc1,  6, 1,    6}, /* D12/PA6 */
    {&gpioa,   NULL, &adc1,  5, 0,    5}, /* D13/PA5 */

    {&gpioa, &timer2, &adc1,  0, 1,    0}, /* D14(A0)/PA0 */
    {&gpioa, &timer2, &adc1,  1, 2,    1}, /* D15(A1)/PA1 */
    {&gpioa, &timer2, &adc1,  2, 3,    2}, /* D16(A2)/PA2 */
    {&gpioa, &timer2, &adc1,  3, 4,    3}, /* D17(A3)/PA3 */
    {&gpiob, &timer4, NULL,  7, 2, ADCx}, /* D18(A4)/PB7 */
    {&gpiob, &timer4, NULL,  6, 1, ADCx}, /* D19(A5)/PB6 */
    {&gpiob, &timer3, &adc1,  0, 3,    8}, /* D20(A6)/PB0 */
    {&gpiob, &timer3, &adc1,  1, 4,    9}, /* D21(A7)/PB1 */

    {&gpioa,   NULL, NULL, 12, 0, ADCx}, /* D22/PA12/USB D+ */
    {&gpioa, &timer1, NULL, 11, 4, ADCx}, /* D23/PA11/USB D- */

    // FIXME: find out which pin is the button, if any
    {&gpiob,   NULL, NULL,  8, 0, ADCx}, /* D24/PB8??/Button */
};

extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    0, 1, 4, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 23
};

extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
    14, 15, 16, 17, 18, 19, 20, 21, 10, 11, 12, 13
};

#define USB_DP 22
#define USB_DM 23

extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
    4, 24, USB_DP, USB_DM
};


/* 
 * Roger Clark
 * 
 * 2015/05/28
 *
 * Moved definitions for Hardware Serial devices from HardwareSerial.cpp so that each board can define which Arduino "Serial" instance
 * Maps to which hardware serial port on the microprocessor
 *
 * Note. I think a microduino is a Maple mini clone in another form factor,  
 */	

DEFINE_HWSERIAL(Serial1, 1);
DEFINE_HWSERIAL(Serial2, 2);
DEFINE_HWSERIAL(Serial3, 3);
