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
 * @file   disco_f411.h
 * @author stevestrong
 * @brief  Private include file for Maple Native in boards.h
 *
 * See maple.h for more information on these definitions.
 */

#ifndef _BOARD_DISCO_F411_H_
#define _BOARD_DISCO_F411_H_


//#define Port2Pin(port, bit) ((port-'A')*16+bit)

#define CLOCK_SPEED_MHZ   96


#define BOARD_LED_PIN           PD12
#define BOARD_LED_PIN2          PD13
#define BOARD_LED_PIN3          PD14
#define BOARD_LED_PIN4          PD15

#define BOARD_BUTTON_PIN        PA0

#define BOARD_USB_DM_PIN		PA11
#define BOARD_USB_DP_PIN		PA12

#define BOARD_NR_USARTS         3
#define BOARD_USART1_TX_PIN     PA9
#define BOARD_USART1_RX_PIN     PA10
#define BOARD_USART2_TX_PIN     PA2
#define BOARD_USART2_RX_PIN     PA3
#define BOARD_USART6_TX_PIN     PC6
#define BOARD_USART6_RX_PIN     PC7

#define BOARD_NR_I2C            3
#define BOARD_I2C1_SCL_PIN      PB6
#define BOARD_I2C1_SDA_PIN      PB7
#define BOARD_I2C1A_SCL_PIN     PB8
#define BOARD_I2C1A_SDA_PIN     PB9
#define BOARD_I2C2_SCL_PIN      PB10
#define BOARD_I2C2_SDA_PIN      PB11
#ifdef PACKAGE_LQFP144
#define BOARD_I2C2A_SCL_PIN     PF1
#define BOARD_I2C2A_SDA_PIN     PF0
#endif
#define BOARD_I2C3_SCL_PIN      PA8
#define BOARD_I2C3_SDA_PIN      PC9
#ifdef PACKAGE_LQFP144
#define BOARD_I2C3A_SCL_PIN     PH7
#define BOARD_I2C3A_SDA_PIN     PH8
#endif

#define BOARD_NR_SPI            5
#define BOARD_SPI1_NSS_PIN      PA4
#define BOARD_SPI1_SCK_PIN      PA5
#define BOARD_SPI1_MISO_PIN     PA6
#define BOARD_SPI1_MOSI_PIN     PA7
#define BOARD_SPI1A_NSS_PIN      PA15
#define BOARD_SPI1A_SCK_PIN      PB3
#define BOARD_SPI1A_MISO_PIN     PB4
#define BOARD_SPI1A_MOSI_PIN     PB5

#define BOARD_SPI2_NSS_PIN      PB12
#define BOARD_SPI2_SCK_PIN      PB13
#define BOARD_SPI2_MISO_PIN     PB14
#define BOARD_SPI2_MOSI_PIN     PB15
#define BOARD_SPI2A_NSS_PIN      PB9
#define BOARD_SPI2A_SCK_PIN      PB10 // PC7 // PB7
#define BOARD_SPI2A_MISO_PIN     PC2
#define BOARD_SPI2A_MOSI_PIN     PC3

#define BOARD_SPI3_NSS_PIN      PA15
#define BOARD_SPI3_SCK_PIN      PB3
#define BOARD_SPI3_MISO_PIN     PB4
#define BOARD_SPI3_MOSI_PIN     PB5
#define BOARD_SPI3A_NSS_PIN      PA4
#define BOARD_SPI3A_SCK_PIN      PC10
#define BOARD_SPI3A_MISO_PIN     PC11
#define BOARD_SPI3A_MOSI_PIN     PC12

#define BOARD_SPI4_NSS_PIN      PE4
#define BOARD_SPI4_SCK_PIN      PE2
#define BOARD_SPI4_MISO_PIN     PE5
#define BOARD_SPI4_MOSI_PIN     PE6

#define BOARD_SPI5_NSS_PIN      PE11
#define BOARD_SPI5_SCK_PIN      PE12
#define BOARD_SPI5_MISO_PIN     PE13
#define BOARD_SPI5_MOSI_PIN     PE14

#define BOARD_NR_PWM_PINS       22
#define BOARD_NR_ADC_PINS       16
#define BOARD_NR_USED_PINS      43 // ala42 not set yet
#define BOARD_JTMS_SWDIO_PIN    PA13
#define BOARD_JTCK_SWCLK_PIN    PA14
#define BOARD_JTDI_PIN          PA15
#define BOARD_JTDO_PIN          PB3
#define BOARD_NJTRST_PIN        PB4


enum {
PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,
PB0,PB1,PB2,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,
PC0,PC1,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15,
PD0,PD1,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15,
PE0,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15,
BOARD_NR_GPIO_PINS
};

#endif
