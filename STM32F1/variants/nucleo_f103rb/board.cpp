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
 * @file   board.cpp for STM32 Nucleo-F103RB
 * @original author Grégoire Passault <g.passault@gmail.com>
 * @brief  Nucleo board file
 * edited and tested by Matthias Diro, Release Date: 27.01.2015
 * there are some solderings neccessary for complete compatibility
 * consider the Nucleo User manual for:
 * OSC clock: clock must be driven either from "MCO from ST-Link" or Oscillator from external PF0/PD0/PH0. Soldering is neccessary if board number is MB1136 C-01, see -> 5.7 OSC clock
 * USART: If PA2/PA3 needed, solder bridges must be changed. see -> 5.8 USART communication
 */

//#include <board/board.h>         // For this board's header file

//#include <wirish_types.h> // For stm32_pin_info and its contents
                                 // (these go into PIN_MAP).

//#include "boards_private.h"      // For PMAP_ROW(), which makes
                                 // PIN_MAP easier to read.
#include <board/board.h>

#include <libmaple/gpio.h>
#include <libmaple/timer.h>

/* Roger Clark. Added next to includes for changes to Serial */
#include <libmaple/usart.h>
#include <HardwareSerial.h>


#include <wirish_debug.h>
#include <wirish_types.h>
// boardInit(): NUCLEO rely on some remapping
void boardInit(void) {
     afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); // relase PC3 and PC5 on nucleo
     afio_remap(AFIO_REMAP_USART3_PARTIAL); // remap Serial2(USART3)PB10/PB11
    // to PC10/PC11 -> don't forget to insert into gpio.h:
    //     AFIO_REMAP_USART3_PARTIAL = AFIO_MAPR_USART3_REMAP_PARTIAL
   afio_remap(AFIO_REMAP_TIM2_FULL); // better PWM compatibility
   afio_remap(AFIO_REMAP_TIM3_PARTIAL);// better PWM compatibility
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
// - GPIO device for the pin (&gpioa, etc.)
// - GPIO bit for the pin (0 through 15)
// - Timer device, or NULL if none
// - Timer channel (1 to 4, for PWM), or 0 if none
// - ADC device, or NULL if none
// - ADC channel, or ADCx if none
// gpioX, PINbit, TIMER/NULL, timerch/0, &adc1/NULL, adcsub/0
// gpioX, TIMER/NULL, &adc1/NULL, PINbit, timerch/0, adcsub/0
//       0    1    2      3    4     5
//       0    3    1      4    2     5

//       0    1    3      4    2     5
//       0    1    2      4    2     5
extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {

    /* Arduino-like header, right connectors */
    {&gpioa, NULL,   &adc1,   3,  0,    3}, /*  D0/PA3 */
    {&gpioa, NULL,   &adc1,   2,  0,    2}, /*  D1/PA2 */
    {&gpioa, &timer1, NULL,  10,  3, ADCx}, /*  D2/PA10 */
    {&gpiob, &timer2, NULL,   3,  2, ADCx}, /*  D3/PB3  */
    {&gpiob, &timer3, NULL,   5,  2, ADCx}, /*  D4/PB5 */
    {&gpiob, &timer3, NULL,   4,  1, ADCx}, /*  D5/PB4  */
    {&gpiob, &timer2, NULL,  10,  3, ADCx}, /*  D6/PB10 */
    {&gpioa, &timer1, NULL,   8,  1, ADCx}, /*  D7/PA8 */
                   
    {&gpioa, &timer1, NULL,   9,  2, ADCx}, /*  D8/PA9  */
    {&gpioc, NULL,   NULL,   7,  0, ADCx}, /*  D9/PC7  */
    {&gpiob, &timer4, NULL,   6,  1, ADCx}, /*  D10/PB6 */
    {&gpioa, NULL,   &adc1,   7,  0,    7}, /*  D11/PA7 */
    {&gpioa, NULL,   &adc1,   6,  0, 	  6}, /*  D12/PA6 */
    {&gpioa, NULL,   NULL,   5,  0, ADCx}, /*  D13/PA5 LED - no &adc12_IN5 !*/
    {&gpiob, &timer4, NULL,   9,  4, ADCx}, /*  D14/PB9 */
    {&gpiob, &timer4, NULL,   8,  3, ADCx}, /*  D15/PB8 */
                   
    {&gpioa, NULL,   &adc1,   0,  0,    0}, /*  D16/A0/PA0 */
    {&gpioa, NULL,   &adc1,   1,  0,    1}, /*  D17/A1/PA1 */
    {&gpioa, NULL,   &adc1,   4,  0,    4}, /*  D18/A2/PA4 */
    {&gpiob, &timer3, &adc1,   0,  3,    8}, /*  D19/A3/PB0 */
    {&gpioc, NULL,   &adc1,   1,  0,   11}, /*  D20/A4/PC1 */
    {&gpioc, NULL,   &adc1,   0,  0,   10}, /*  D21/A5/PC0 */
                   
    {&gpioc, NULL,   NULL,  10,  0, ADCx}, /*  D22/PC10 */
	{&gpioc, NULL,   NULL,  12,  0, ADCx}, /*  D23/PC12 */
    {&gpiob, &timer4, NULL,   7,  2, ADCx}, /*  D24/PB7 */
    {&gpioc, NULL,   NULL,  13,  0, ADCx}, /*  D25/PC13 USER BLUE BUTTON */
    {&gpioc, NULL,   NULL,  14,  0, ADCx}, /*  D26/PC14 */
    {&gpioc, NULL,   NULL,  15,  0, ADCx}, /*  D27/PC15 */
    {&gpioc, NULL,   &adc1,   2,  0,   12}, /*  D28/PC2 */
    {&gpioc, NULL,   &adc1,   3,  0,   13}, /*  D29/PC3 */
    {&gpioc, NULL,   NULL,  11,  0, ADCx}, /*  D30/PC11 */
    {&gpiod, NULL,   NULL,   2,  0, ADCx}, /*  D31/PD2 */
    {&gpioc, NULL,   NULL,   9,  0, ADCx}, /*  D32/PC9 */
    {&gpioc, NULL,   NULL,   8,  0, ADCx}, /*  D33/PC8 */
    {&gpioc, NULL,   NULL,   6,  0, ADCx}, /*  D34/PC6 */
    {&gpioc, NULL,   &adc1,   5,  0,   15}, /*  D35/PC5 */
    {&gpioa, NULL,   NULL,  12,  0, ADCx}, /*  D36/PA12 */
    {&gpioa, &timer1, NULL,  11,  4, ADCx}, /*  D37/PA11 */
    {&gpiob, NULL,   NULL,  12,  0, ADCx}, /*  D38/PB12 */
    {&gpiob, &timer2, NULL,  11,  4, ADCx}, /*  D39/PB11 PWM-not working?*/
    {&gpiob, NULL,   NULL,  2,   0, ADCx}, /*  D40/PB2 BOOT1 !!*/
    {&gpiob, &timer3, &adc1,   1,  4,    9}, /*  D41/PB1 */
    {&gpiob, NULL,   NULL,  15,  0, ADCx}, /*  D42/PB15 */
    {&gpiob, NULL,   NULL,  14,  0, ADCx}, /*  D43/PB14 */
    {&gpiob, NULL,   NULL,  13,  0, ADCx}, /*  D44/PB13 */
    {&gpioc, NULL,   &adc1,   4,  0,   14}, /*  D45/PC4 */
  //  PMAP_ROW(&gpioa,  13,   NULL,  0, NULL, ADCx), /* D41/PA13 do not use*/
   // PMAP_ROW(&gpioa,  14,   NULL,  0, NULL, ADCx), /* D42/PA14  do not use*/
   // PMAP_ROW(&gpioa,  15,   &timer2,  1, NULL, ADCx), /* D43/PA15 do not use*/
};

// Array of pins you can use for pwmWrite(). Keep it in Flash because
// it doesn't change, and so we don't waste RAM.
extern const uint8 boardPWMPins[] __FLASH__ = {
    2,3,5,6,7,8,10,14,15,19,24,37,39,41
};

// Array of pins you can use for analogRead().
extern const uint8 boardADCPins[] __FLASH__ = {
    0,1,11,12,16,17,18,19,20,21,28,29,35,41,45
};

// Array of pins that the board uses for something special. Other than
// the button and the LED, it's usually best to leave these pins alone
// unless you know what you're doing.


/* remappings Infos
*************************************
Bit 12 TIM4_REMAP: TIM4 remapping
This bit is set and cleared by software. It controls the mapping of TIM4 channels 1 to 4 onto
the GPIO ports.
0: No remap (TIM4_CH1/PB6, TIM4_CH2/PB7, TIM4_CH3/PB8, TIM4_CH4/PB9)
1: Full remap (TIM4_CH1/PD12, TIM4_CH2/
*************************************
Bits 11:10 TIM3_REMAP[1:0]: TIM3 remapping
These bits are set and cleared by software. They control the mapping of TIM3 channels 1 to
4 on the GPIO ports.
00: No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1)
01: Not used
10: Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1)
11: Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9)
Note: TIM3_ETR on PE0 is not re-mapped.
*************************************
Bits 9:8 TIM2_REMAP[1:0]: TIM2 remapping
These bits are set and cleared by software. They control the mapping of TIM2 channels 1 to
4 and external trigger (ETR) on the GPIO ports.
00: No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3)
01: Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3)
10: Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11)
11: Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11)
*************************************
Bits 7:6 TIM1_REMAP[1:0]: TIM1 remapping
These bits are set and cleared by software. They control the mapping of TIM1 channels 1 to
4, 1N to 3N, external trigger (ETR) and Break input (BKIN) on the GPIO ports.
00: No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12,
CH1N/PB13, CH2N/PB14, CH3N/PB15)
01: Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6,
CH1N/PA7, CH2N/PB0, CH3N/PB1)
10: not used
11: Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15,
CH1N/PE8, CH2N/PE10, CH3N/PE12)
*************************************
Bits 5:4 USART3_REMAP[1:0]: USART3 remapping
These bits are set and cleared by software. They control the mapping of USART3 CTS,
RTS,CK,TX and RX alternate functions on the GPIO ports.
00: No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14)
01: Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14)
10: not used
11: Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12)
*************************************
Bit 3 USART2_REMAP: USART2 remapping
This bit is set and cleared by software. It controls the mapping of USART2 CTS, RTS,CK,TX
and RX alternate functions on the GPIO ports.
0: No remap (CTS/PA0, RTS/PA1, TX/PA2, RX/PA3, CK/PA4)
1: Remap (CTS/PD3, RTS/PD4, TX/PD5, RX/PD6, CK/PD7)
*************************************
Bit 2 USART1_REMAP: USART1 remapping
This bit is set and cleared by software. It controls the mapping of USART1 TX and RX
alternate functions on the GPIO ports.
0: No remap (TX/PA9, RX/PA10)
1: Remap (TX/PB6, RX/PB7)
*************************************
Bit 1 I2C1_REMAP: I2C1 remapping
This bit is set and cleared by software. It controls the mapping of I2C1 SCL and SDA
alternate functions on the GPIO ports.
0: No remap (SCL/PB6, SDA/PB7)
1: Remap (SCL/PB8, SDA/PB9)
*************************************
Bit 0 SPI1_REMAP: SPI1 remapping
This bit is set and cleared by software. It controls the mapping of SPI1 NSS, SCK, MISO,
MOSI alternate functions on the GPIO ports.
0: No remap (NSS/PA4, SCK/PA5, MISO/PA6, MOSI/PA7)
1: Remap (NSS/PA15, SCK/PB3, MISO/PB4, MOSI/PB5)


*/



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
   DEFINE_HWSERIAL(Serial, 3);// Use HW Serial 2 as "Serial"
   DEFINE_HWSERIAL(Serial1, 2);
   DEFINE_HWSERIAL(Serial2, 1);
#endif