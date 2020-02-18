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
 * @file   arch_max.h
 * @author stevestrong
 * @brief  Private include file for Maple Native in boards.h
 *
 * See maple.h for more information on these definitions.
 */

#ifndef _BOARD_ARCH_MAX_H_
#define _BOARD_ARCH_MAX_H_


#define CLOCK_SPEED_MHZ  168

//-----------------------------------------------------------------------------
// Board pin definitions
//-----------------------------------------------------------------------------
// Ethernet pins - may not overlap with any other pin definition !!!
//-----------------------------------------------------------------------------
#define ETH_REF_CLK		PA1  // * (reserved)
#define ETH_MDIO		PA2  // *
#define ETH_CRS_DV		PA7  // *
#define ETH_TX_EN		PB11 // *
#define ETH_TX_D0		PB12 // *
#define ETH_TX_D1		PB13 // *
#define ETH_MDC			PC1  // *
#define ETH_RX_D0		PC4  // *
#define ETH_RX_D1		PC5  // *
//-----------------------------------------------------------------------------
// USB pins
#define BOARD_USB_DT_PIN		PA9 // detect
#define BOARD_USB_ID_PIN		PA10
#define BOARD_USB_DM_PIN		PA11
#define BOARD_USB_DP_PIN		PA12

#define BOARD_LED_PIN			PB3 // ! BOARD_SPI1A_SCK_PIN
#define BOARD_LED2_PIN			PC6 // ! BOARD_USART6_TX_PIN, DCMI_D0
#define BOARD_LED3_PIN			PC7 // ! BOARD_USART6_RX_PIN, DCMI_D1

#define BOARD_NR_USARTS         6
#define BOARD_USART1_TX_PIN     PB6
#define BOARD_USART1_RX_PIN     PB7
#define BOARD_USART2_TX_PIN     PD5
#define BOARD_USART2_RX_PIN     PD6
#define BOARD_USART3_TX_PIN     PD8
#define BOARD_USART3_RX_PIN     PD9
#define BOARD_UART4_TX_PIN      PC10
#define BOARD_UART4_RX_PIN      PC11
#define BOARD_UART5_TX_PIN      PC12
#define BOARD_UART5_RX_PIN      PD2
#define BOARD_USART6_TX_PIN     PC6 // ! BOARD_LED2_PIN, DCMI_D0
#define BOARD_USART6_RX_PIN     PC7 // ! BOARD_LED3_PIN, DCMI_D1

// I2C pins
#define BOARD_NR_I2C            1
#define BOARD_I2C1_SCL_PIN     PB8
#define BOARD_I2C1_SDA_PIN     PB9

// SPI pins
#define BOARD_NR_SPI            3
#define BOARD_SPI1_NSS_PIN     PA15
#define BOARD_SPI1_SCK_PIN     PB3 // ! LED
#define BOARD_SPI1_MISO_PIN    PB4
#define BOARD_SPI1_MOSI_PIN    PB5

#define BOARD_SPI2_NSS_PIN     PB9
#define BOARD_SPI2_SCK_PIN     PB10
#define BOARD_SPI2_MISO_PIN    PC2
#define BOARD_SPI2_MOSI_PIN    PC3

#define BOARD_SPI3_NSS_PIN     PA4
#define BOARD_SPI3_SCK_PIN     PC10 // ! BOARD_SDIO_D2, BOARD_UART4_TX_PIN
#define BOARD_SPI3_MISO_PIN    PC11 // ! BOARD_SDIO_D3, BOARD_UART4_RX_PIN
#define BOARD_SPI3_MOSI_PIN    PC12

#define DEFAULT_SPI_PORT		1 // valid selections 1, 2 or 3

// SDIO pins
#define BOARD_SDIO_D0          PC8
#define BOARD_SDIO_D1          PC9
#define BOARD_SDIO_D2          PC10
#define BOARD_SDIO_D3          PC11
#define BOARD_SDIO_CLK         PC12
#define BOARD_SDIO_CMD         PD2

// FSMC pins
#define FSMC_NOE			PD4
#define FSMC_NWE			PD5
#define FSMC_NE1			PD7
#define FSMC_A16			PD11
#define FSMC_A17			PD12
#define FSMC_A18			PD13
#define FSMC_D0				PD14
#define FSMC_D1				PD15
#define FSMC_D2				PD0
#define FSMC_D3				PD1
#define FSMC_D4				PE7
#define FSMC_D5				PE8
#define FSMC_D6				PE9
#define FSMC_D7				PE10
#define FSMC_D8				PE11
#define FSMC_D9				PE12
#define FSMC_D10			PE13
#define FSMC_D11			PE14
#define FSMC_D12			PE15
#define FSMC_D13			PD8
#define FSMC_D14			PD9
#define FSMC_D15			PD10

// DCMI pins
#define DCMI_PCLK			PA6
#define DCMI_HSYNC			PA4  // PA4/PH8
#define DCMI_VSYNC			PB7  // PB7/PI5
#define DCMI_D0				PC6  // PA9/PC6/PH9
#define DCMI_D1				PC7  // PA10/PC7/PH10
#define DCMI_D2				PC8  // PC8/PE0/PH11
#define DCMI_D3				PC9  // PC9/PE1/PH12
#define DCMI_D4				PE4  // PC11/PE4/PH14
#define DCMI_D5				PB6  // PB6/PI4
#define DCMI_D6				PE5  // PB8/PE5/PI6
#define DCMI_D7				PE6  // PB9/PE6/PI7
#define DCMI_D8				PC10 // PC10/PI1
#define DCMI_D9				PC12 // PC12/PI2
#define DCMI_D10			PB5  // PB5/PI3
#define DCMI_D11			PD2  // PD2/PH15

// JTAG/SWD pins
#define BOARD_JTMS_SWDIO_PIN    PA13
#define BOARD_JTCK_SWCLK_PIN    PA14
//#define BOARD_JTDI_PIN          PA15
//#define BOARD_JTDO_PIN          PB3
//#define BOARD_NJTRST_PIN        PB4

#define BOARD_NR_PWM_PINS       24
#define BOARD_NR_ADC_PINS       8
#define BOARD_NR_USED_PINS      40
/*****************************************************************************/
// Pins reserved for the on-board hardware
#define USB_DM_PIN		BOARD_USB_DM_PIN // PA11
#define USB_DP_PIN		BOARD_USB_DP_PIN // PA12

// SD card, SDIO mode
//#define SD_DAT0         BOARD_SDIO_D0	// PC8
//#define SD_DAT1         BOARD_SDIO_D1	// PC9
//#define SD_DAT2         BOARD_SDIO_D2	// PC10
//#define SD_DAT3         BOARD_SDIO_D3	// PC11
//#define SD_CLK          BOARD_SDIO_CK	// PC12
//#define SD_CMD          BOARD_SDIO_CMD	// PD2
// SD card, only usable in SPI mode
#define SD_CS           PE2
#define SD_SCLK         BOARD_SPI2_SCK_PIN  // PC8
#define SD_DO           BOARD_SPI2_MISO_PIN // PC11
#define SD_DI           BOARD_SPI2_MOSI_PIN // PC12

/*****************************************************************************/

enum {
PA0,PA1,PA2,PA3,PA4,PA5,PA6,PA7,PA8,PA9,PA10,PA11,PA12,PA13,PA14,PA15,
PB0,PB1,PB2,PB3,PB4,PB5,PB6,PB7,PB8,PB9,PB10,PB11,PB12,PB13,PB14,PB15,
PC0,PC1,PC2,PC3,PC4,PC5,PC6,PC7,PC8,PC9,PC10,PC11,PC12,PC13,PC14,PC15,
PD0,PD1,PD2,PD3,PD4,PD5,PD6,PD7,PD8,PD9,PD10,PD11,PD12,PD13,PD14,PD15,
PE0,PE1,PE2,PE3,PE4,PE5,PE6,PE7,PE8,PE9,PE10,PE11,PE12,PE13,PE14,PE15,
#ifdef PACKAGE_LQFP144
PF0,PF1,PF2,PF3,PF4,PF5,PF6,PF7,PF8,PF9,PF10,PF11,PF12,PF13,PF14,PF15,
PG0,PG1,PG2,PG3,PG4,PG5,PG6,PG7,PG8,PG9,PG10,PG11,PG12,PG13,PG14,PG15,
#endif
BOARD_NR_GPIO_PINS
};

#endif // _BOARD_ARCH_MAX_H_
