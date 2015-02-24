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
 * @file   wirish/boards/nucleo/board.cpp
 * @author Grégoire Passault <g.passault@gmail.com>
 * @brief  Nucleo board file
 *
 * This mapping was done using the NUCLEO documentation and may be incomplete
 * or contains error
 *
 * If you want to use the PWM outputs, consider understanding all the remapping
 * process that can be involved. You may have to tweak this file regarding your goals.
 */

//#include <board/board.h>         // For this board's header file

//#include <wirish_types.h> // For stm32_pin_info and its contents
                                 // (these go into PIN_MAP).

#include "boards_private.h"      // For PMAP_ROW(), which makes
                                 // PIN_MAP easier to read.
#include <board/board.h>

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

#include <wirish_debug.h>
#include <wirish_types.h>
// boardInit(): NUCLEO rely on some remapping
void boardInit(void) {
    afio_remap(AFIO_REMAP_TIM2_FULL);
    afio_remap(AFIO_REMAP_TIM3_PARTIAL);
}
/*
namespace wirish {
namespace priv {
static stm32f1_rcc_pll_data pll_data = {RCC_PLLMUL_9};
rcc_clk w_board_pll_in_clk = RCC_CLK_HSI;
rcc_pll_cfg w_board_pll_cfg = {RCC_PLLSRC_HSI_DIV_2, &pll_data};
}
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

    /* Arduino-like header, right connectors */

    PMAP_ROW(GPIOA,   3,   NULL,  0, ADC1,    3), /* D0/PA3 */
    PMAP_ROW(GPIOA,   2,   NULL,  0, ADC1,    2), /* D1/PA2 */
    PMAP_ROW(GPIOA,  10,   NULL,  0, NULL, ADCx), /* D2/PA10 */
    PMAP_ROW(GPIOB,   3, TIMER2,  2, NULL, ADCx), /* D3/PB3     REMAPPED */
    PMAP_ROW(GPIOB,   5,   NULL,  0, NULL, ADCx), /* D4/PB5 */
    PMAP_ROW(GPIOB,   4, TIMER3,  1, NULL, ADCx), /* D5/PB4     REMAPPED */
    PMAP_ROW(GPIOB,  10, TIMER2,  3, NULL, ADCx), /* D6/PB10    REMAPPED */
    PMAP_ROW(GPIOA,   8,   NULL,  0, NULL, ADCx), /* D7/PA8 */

    PMAP_ROW(GPIOA,   9,   NULL,  0, NULL, ADCx), /* D8/PA9  */
    PMAP_ROW(GPIOC,   7,   NULL,  0, NULL, ADCx), /* D9/PC7  */
    PMAP_ROW(GPIOB,   6, TIMER4,  1, NULL, ADCx), /* D10/PB6 */
    PMAP_ROW(GPIOA,   7, TIMER3,  2, ADC1,    7), /* D11/PA7 */
    PMAP_ROW(GPIOA,   6,   NULL,  0, ADC1,    6), /* D12/PA6 */
    PMAP_ROW(GPIOA,   5,   NULL,  0, ADC1,    5), /* D13/PA5 LED*/
    PMAP_ROW(GPIOB,   9,   NULL,  0, NULL, ADCx), /* D14/PB9 */
    PMAP_ROW(GPIOB,   8,   NULL,  0, NULL, ADCx), /* D15/PB8 */
    
    /* Arduino-like header, left connectors */

    PMAP_ROW(GPIOA,   0, TIMER2,  1, ADC1,    0), /* D16/A0/PA0 */
    PMAP_ROW(GPIOA,   1, TIMER2,  2, ADC1,    1), /* D17/A1/PA1 */
    PMAP_ROW(GPIOA,   4,   NULL,  0, ADC1,    4), /* D18/A2/PA4 */
    PMAP_ROW(GPIOB,   0, TIMER3,  3, ADC1,    8), /* D19/A3/PB0 */
    PMAP_ROW(GPIOC,   1,   NULL,  0, ADC1,   11), /* D20/A4/PC1 */
    PMAP_ROW(GPIOC,   0,   NULL,  0, ADC1,   10), /* D21/A5/PC0 */

    /* Other pins */

    PMAP_ROW(GPIOB,   7,   NULL,  0, NULL, ADCx), /* D22/PB7 */
    PMAP_ROW(GPIOC,   2,   NULL,  0, ADC1,   12), /* D23/PC2 */
    PMAP_ROW(GPIOC,   3,   NULL,  0, ADC1,   13), /* D24/PC3 */
    PMAP_ROW(GPIOC,   4,   NULL,  0, ADC1,   14), /* D25/PC4 */
    PMAP_ROW(GPIOC,   5,   NULL,  0, ADC1,   15), /* D26/PC5 */

    PMAP_ROW(GPIOC,  13,   NULL,  0, NULL, ADCx), /* D27/PC13 USER BLUE BUTTON */
    PMAP_ROW(GPIOC,  14,   NULL,  0, NULL, ADCx), /* D28/PC14 */
    PMAP_ROW(GPIOC,  15,   NULL,  0, NULL, ADCx), /* D29/PC15 */
    PMAP_ROW(GPIOD,   2,   NULL,  0, NULL, ADCx), /* D30/PD2 */
    PMAP_ROW(GPIOC,  10,   NULL,  0, NULL, ADCx), /* D31/PC10 */
    PMAP_ROW(GPIOB,   1,   NULL,  0, ADC1,    9), /* D32/PB1 */
    PMAP_ROW(GPIOB,  11,   NULL,  0, NULL, ADCx), /* D33/PB11 */
    PMAP_ROW(GPIOB,  12,   NULL,  0, NULL, ADCx), /* D34/PB12 */
    PMAP_ROW(GPIOB,  13,   NULL,  0, NULL, ADCx), /* D35/PB13 */
    PMAP_ROW(GPIOB,  14,   NULL,  0, NULL, ADCx), /* D36/PB14 */
    PMAP_ROW(GPIOB,  15,   NULL,  0, NULL, ADCx), /* D37/PB15 */
    PMAP_ROW(GPIOC,   6,   NULL,  0, NULL, ADCx), /* D38/PC6 */
    PMAP_ROW(GPIOC,   8,   NULL,  0, NULL, ADCx), /* D39/PC8 */
    PMAP_ROW(GPIOC,   9,   NULL,  0, NULL, ADCx), /* D40/PC9 */

    PMAP_ROW(GPIOA,  13,   NULL,  0, NULL, ADCx), /* D41/PA13 */
    PMAP_ROW(GPIOA,  14,   NULL,  0, NULL, ADCx), /* D42/PA14 */
    PMAP_ROW(GPIOA,  15,   NULL,  0, NULL, ADCx), /* D43/PA15 */
};

// Array of pins you can use for pwmWrite(). Keep it in Flash because
// it doesn't change, and so we don't waste RAM.
extern const uint8 boardPWMPins[] __FLASH__ = {
    3, 5, 6, 10, 11, 16, 17, 19
};

// Array of pins you can use for analogRead().
extern const uint8 boardADCPins[] __FLASH__ = {
    16, 17, 18, 19, 20, 21
};

// Array of pins that the board uses for something special. Other than
// the button and the LED, it's usually best to leave these pins alone
// unless you know what you're doing.
extern const uint8 boardUsedPins[] __FLASH__ = {
    BOARD_LED_PIN, BOARD_BUTTON_PIN
};
