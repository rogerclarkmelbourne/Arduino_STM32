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
 * @file   blackpill_f401.cpp
 * @author stevestrong
 * @brief  blackpill_f401 board file.
 */

#include "blackpill_f401.h"

#include <libmaple/gpio.h>
#include <libmaple/rcc.h>

#include <libmaple/libmaple_types.h>


void boardInit(void)
{
}


//extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
//    0, 1, 2, 3, 15, 16, 17, 19, 20, 21, 38, 39, 49, 41, 60, 61, 62, 63, 73, 75, 77, 78
//};
//
//extern const uint8 boardADCPins[BOARD_NR_ADC_PINS] __FLASH__ = {
//    0, 1, 2, 3, 4, 5, 6, 7, 16, 17, 32, 33, 34, 35, 36, 37
//};
//
//extern const uint8 boardUsedPins[BOARD_NR_USED_PINS] __FLASH__ = {
//    BOARD_LED_PIN, BOARD_BUTTON_PIN, BOARD_JTMS_SWDIO_PIN,
//    BOARD_JTCK_SWCLK_PIN, BOARD_JTDI_PIN, BOARD_JTDO_PIN, BOARD_NJTRST_PIN,
//    56, 58, 59, 61, 62, 64, 65, 67, 68, 70, 71, 73, 74, 76, 77, 78, 79, 81,
//    82, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
//};

