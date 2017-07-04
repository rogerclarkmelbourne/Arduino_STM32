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
 * @file   generic_f407v.cpp
 * @author ala42
 * @brief  generic_f407v board file.
 */

#ifdef BOARD_generic_f407v

#ifdef __cplusplus
extern "C"{
#endif

//#include "generic_f407v.h"

//#include "fsmc.h"
#include <libmaple/gpio.h>
#include <libmaple/rcc.h>
#include <libmaple/timer.h>

#include <wirish_types.h>

extern timer_dev timer1;
extern timer_dev timer2;
extern timer_dev timer3;
extern timer_dev timer4;
extern timer_dev timer5;
extern timer_dev timer6;
extern timer_dev timer7;
extern timer_dev timer8;

/*
typedef struct stm32_pin_info {
    gpio_dev *gpio_device;      // Maple pin's GPIO device
    timer_dev *timer_device;    // Pin's timer device, if any.
    uint8 timer_channel;        // Timer channel, or 0 if none.
    uint8 adc_channel;          // Pin ADC channel, or ADCx if none.
    const adc_dev *adc_device;  // ADC device, if any.
} stm32_pin_info;
*/
const stm32_pin_info PIN_MAP[] = { // LQFP100 package pin
    {&GPIOA, &timer5, 1,    0, &ADC1}, // D00/PA0  | 23 | USART2_CTS | UART4_TX       | ETH_MII_CRS      | TIM2_CH1_ETR   | TIM5_CH1       | TIM8_ETR                      | ADC123_IN0/WKUP
    {&GPIOA, &timer5, 2,    1, &ADC1}, // D01/PA1  | 24 | USART2_RTS | UART4_RX       | ETH_RMII_REF_CLK | ETH_MII_RX_CLK | TIM5_CH2       | TIM2_CH2                      | ADC123_IN1
    {&GPIOA, &timer5, 3,    2, &ADC1}, // D02/PA2  | 25 | USART2_TX  | TIM5_CH3       | TIM9_CH1         | TIM2_CH3       | ETH_MDIO                                       | ADC123_IN2
    {&GPIOA, &timer5, 4,    3, &ADC1}, // D03/PA3  | 26 | USART2_RX  | TIM5_CH4       | TIM9_CH2         | TIM2_CH4       | OTG_HS_ULPI_D0 | ETH_MII_COL                   | ADC123_IN3
    {&GPIOA,    NULL, 0,    4, &ADC1}, // D04/PA4  | 29 | SPI1_NSS   | SPI3_NSS       | USART2_CK        | DCMI_HSYNC     | OTG_HS_SOF     | I2S3_WS                       | ADC12_IN4 / DAC_OUT1
    {&GPIOA,    NULL, 0,    5, &ADC1}, // D05/PA5  | 30 | SPI1_SCK   | OTG_HS_ULPI_CK | TIM2_CH1_ETR     | TIM8_CH1N                                                       | ADC12_IN5 / DAC_OUT2
    {&GPIOA,    NULL, 1,    6, &ADC1}, // D06/PA6  | 31 | SPI1_MISO  | TIM8_BKIN      | TIM13_CH1        | DCMI_PIXCLK    | TIM3_CH1       | TIM1_BKIN                     | ADC12_IN6
    {&GPIOA,    NULL, 0,    7, &ADC1}, // D07/PA7  | 32 | SPI1_MOSI  | TIM8_CH1N      | TIM14_CH1        | TIM3_CH2       | ETH_MII_RX_DV  | TIM1_CH1N   / ETH_RMII_CRS_DV | ADC12_IN7
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D08/PA8  | 67 | MCO1       | USART1_CK      | TIM1_CH1         | I2C3_SCL       | OTG_FS_SOF
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D09/PA9  | 68 | USART1_TX  | TIM1_CH2       | I2C3_SMBA        | DCMI_D0
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D10/PA10 | 69 | USART1_RX  | TIM1_CH3       | OTG_FS_ID        | DCMI_D1
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D11/PA11 | 70 | USART1_CTS | CAN1_RX        | TIM1_CH4         | OTG_FS_DM
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D12/PA12 | 71 | USART1_RTS | CAN1_TX        | TIM1_ETR         | OTG_FS_DP
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D13/PA13 | 72 | JTMS-SWDIO
    {&GPIOA,    NULL, 0, ADCx,  NULL}, // D14/PA14 | 76 | JTCK-SWCLK
    {&GPIOA, &timer2, 1, ADCx,  NULL}, // D15/PA15 | 77 | JTDI       | SPI3_NSS       | I2S3_WS          | TIM2_CH1_ETR   | SPI1_NSS

    {&GPIOB, &timer3, 3,    8, &ADC1}, // D16/PB0  | 35 | TIM3_CH3   | TIM8_CH2N | OTG_HS_ULPI_D1 | ETH_MII_RXD2   | TIM1_CH2N      | ADC12_IN8
    {&GPIOB, &timer3, 4,    9, &ADC1}, // D17/PB1  | 36 | TIM3_CH4   | TIM8_CH3N | OTG_HS_ULPI_D2 | ETH_MII_RXD3   | TIM1_CH3N      | ADC12_IN9
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D18/PB2  | 37 | BOOT1    
    {&GPIOB, &timer2, 2, ADCx,  NULL}, // D19/PB3  | 89 | JTDO       | TRACESWO  | SPI3_SCK       | I2S3_CK        | TIM2_CH2       | SPI1_SCK
    {&GPIOB, &timer3, 1, ADCx,  NULL}, // D20/PB4  | 90 | NJTRST     | SPI3_MISO | TIM3_CH1       | SPI1_MISO      | I2S3ext_SD
    {&GPIOB, &timer3, 2, ADCx,  NULL}, // D21/PB5  | 91 | I2C1_SMBA  | CAN2_RX   | OTG_HS_ULPI_D7 | ETH_PPS_OUT    | TIM3_CH2       | SPI1_MOSI      | SPI3_MOSI      | DCMI_D10      | I2S3_SD
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D22/PB6  | 92 | I2C1_SCL   | TIM4_CH1  | CAN2_TX        | DCMI_D5        | USART1_TX
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D23/PB7  | 93 | I2C1_SDA   | FSMC_NL   | DCMI_VSYNC     | USART1_RX      | TIM4_CH2
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D24/PB8  | 95 | TIM4_CH3   | SDIO_D4   | TIM10_CH1      | DCMI_D6        | ETH_MII_TXD3   | I2C1_SCL       | CAN1_RX
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D25/PB9  | 96 | SPI2_NSS   | I2S2_WS   | TIM4_CH4       | TIM11_CH1      | SDIO_D5        | DCMI_D7        | I2C1_SDA       | CAN1_TX
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D26/PB10 | 47 | SPI2_SCK   | I2S2_CK   | I2C2_SCL       | USART3_TX      | OTG_HS_ULPI_D3 | ETH_MII_RX_ER  | TIM2_CH3
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D27/PB11 | 48 | I2C2_SDA   | USART3_RX | OTG_HS_ULPI_D4 | ETH_RMII_TX_EN | ETH_MII_TX_EN  | TIM2_CH4
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D28/PB12 | 51 | SPI2_NSS   | I2S2_WS   | I2C2_SMBA      | USART3_CK      | TIM1_BKIN      | CAN2_RX        | OTG_HS_ULPI_D5 | ETH_RMII_TXD0 | ETH_MII_TXD0 | OTG_HS_ID
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D29/PB13 | 52 | SPI2_SCK   | I2S2_CK   | USART3_CTS     | TIM1_CH1N      | CAN2_TX        | OTG_HS_ULPI_D6 | ETH_RMII_TXD1  | ETH_MII_TXD1
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D30/PB14 | 53 | SPI2_MISO  | TIM1_CH2N | TIM12_CH1      | OTG_HS_DM      | USART3_RTS     | TIM8_CH2N      | I2S2ext_SD
    {&GPIOB,    NULL, 0, ADCx,  NULL}, // D31/PB15 | 54 | SPI2_MOSI  | I2S2_SD   | TIM1_CH3N      | TIM8_CH3N      | TIM12_CH2      | OTG_HS_DP
                             
    {&GPIOC,    NULL, 0,   10, &ADC1}, // D32/PC0  | 15 | OTG_HS_ULPI_STP                                                  | ADC123_IN10
    {&GPIOC,    NULL, 0,   11, &ADC1}, // D33/PC1  | 16 | ETH_MDC                                                          | ADC123_IN11
    {&GPIOC,    NULL, 0,   12, &ADC1}, // D34/PC2  | 17 | SPI2_MISO  | OTG_HS_ULPI_DIR  | ETH_MII_TXD2    | I2S2ext_SD     | ADC123_IN12
    {&GPIOC,    NULL, 0,   13, &ADC1}, // D35/PC3  | 18 | SPI2_MOSI  | I2S2_SD          | OTG_HS_ULPI_NXT | ETH_MII_TX_CLK | ADC123_IN13
    {&GPIOC,    NULL, 0,   14, &ADC1}, // D36/PC4  | 33 | ETH_RMII_RX_D0                | ETH_MII_RX_D0                    | ADC12_IN14
    {&GPIOC,    NULL, 0,   15, &ADC1}, // D37/PC5  | 34 | ETH_RMII_RX_D1                | ETH_MII_RX_D1                    | ADC12_IN15
    {&GPIOC, &timer8, 1, ADCx,  NULL}, // D38/PC6  | 63 | I2S2_MCK   | TIM8_CH1/SDIO_D6 | USART6_TX       | DCMI_D0/TIM3_CH1 
    {&GPIOC, &timer8, 2, ADCx,  NULL}, // D39/PC7  | 64 | I2S3_MCK   | TIM8_CH2/SDIO_D7 | USART6_RX       | DCMI_D1/TIM3_CH2
    {&GPIOC, &timer8, 3, ADCx,  NULL}, // D40/PC8  | 65 | TIM8_CH3   | SDIO_D0          | TIM3_CH3        | USART6_CK      | DCMI_D2
    {&GPIOC, &timer8, 4, ADCx,  NULL}, // D41/PC9  | 66 | I2S_CKIN   | MCO2             | TIM8_CH4        | SDIO_D1        | I2C3_SDA  | DCMI_D3    | TIM3_CH4
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D42/PC10 | 78 | SPI3_SCK   | I2S3_CK          | UART4_TX        | SDIO_D2        | DCMI_D8   | USART3_TX
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D43/PC11 | 79 | UART4_RX   | SPI3_MISO        | SDIO_D3         | DCMI_D4        | USART3_RX | I2S3ext_SD
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D44/PC12 | 80 | UART5_TX   | SDIO_CK          | DCMI_D9         | SPI3_MOSI      | I2S3_SD   | USART3_CK
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D45/PC13 |  7 | RTC_OUT, RTC_TAMP1, RTC_TS
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D46/PC14 |  8 | OSC32_IN
    {&GPIOC,    NULL, 0, ADCx,  NULL}, // D47/PC15 |  9 | OSC32_OUT
                             
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D48/PD0  | 81 | FSMC_D2    | CAN1_RX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D49/PD1  | 82 | FSMC_D3    | CAN1_TX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D50/PD2  | 83 | TIM3_ETR   | UART5_RX   | SDIO_CMD | DCMI_D11
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D51/PD3  | 84 | FSMC_CLK   | USART2_CTS
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D52/PD4  | 85 | FSMC_NOE   | USART2_RTS
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D53/PD5  | 86 | FSMC_NWE   | USART2_TX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D54/PD6  | 87 | FSMC_NWAIT | USART2_RX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D55/PD7  | 88 | USART2_CK  | FSMC_NE1   | FSMC_NCE2
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D56/PD8  | 55 | FSMC_D13   | USART3_TX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D57/PD9  | 56 | FSMC_D14   | USART3_RX
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D58/PD10 | 57 | FSMC_D15   | USART3_CK
    {&GPIOD,    NULL, 0, ADCx,  NULL}, // D59/PD11 | 58 | FSMC_CLE   | FSMC_A16   | USART3_CTS
    {&GPIOD, &timer4, 1, ADCx,  NULL}, // D60/PD12 | 59 | FSMC_ALE   | FSMC_A17   | TIM4_CH1   | USART3_RTS // remap in
    {&GPIOD, &timer4, 2, ADCx,  NULL}, // D61/PD13 | 60 | FSMC_A18                | TIM4_CH2                // remap in
    {&GPIOD, &timer4, 3, ADCx,  NULL}, // D62/PD14 | 61 | FSMC_D0                 | TIM4_CH3                // remap in
    {&GPIOD, &timer4, 4, ADCx,  NULL}, // D63/PD15 | 62 | FSMC_D1                 | TIM4_CH4                // remap in

    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D64/PE0  | 97 | TIM4_ETR  | FSMC_NBL0 | DCMI_D2
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D65/PE1  | 98 | FSMC_NBL1 | DCMI_D3
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D66/PE2  |  1 | TRACECLK  | FSMC_A23  | ETH_MII_TXD3
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D67/PE3  |  2 | TRACED0   | FSMC_A19
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D68/PE4  |  3 | TRACED1   | FSMC_A20  | DCMI_D4
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D69/PE5  |  4 | TRACED2   | FSMC_A21  | TIM9_CH1     / DCMI_D6
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D70/PE6  |  5 | TRACED3   | FSMC_A22  | TIM9_CH2     / DCMI_D7
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D71/PE7  | 38 | FSMC_D4   | TIM1_ETR
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D72/PE8  | 39 | FSMC_D5   | TIM1_CH1N
    {&GPIOE, &timer1, 1, ADCx,  NULL}, // D73/PE9  | 40 | FSMC_D6   | TIM1_CH1   // remap in
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D74/PE10 | 41 | FSMC_D7   | TIM1_CH2N
    {&GPIOE, &timer1, 2, ADCx,  NULL}, // D75/PE11 | 42 | FSMC_D8   | TIM1_CH2   // remap in
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D76/PE12 | 43 | FSMC_D9   | TIM1_CH3N
    {&GPIOE, &timer1, 3, ADCx,  NULL}, // D77/PE13 | 44 | FSMC_D10  | TIM1_CH3   // remap in
    {&GPIOE, &timer1, 4, ADCx,  NULL}, // D78/PE14 | 45 | FSMC_D11  | TIM1_CH4   // remap in
    {&GPIOE,    NULL, 0, ADCx,  NULL}, // D79/PE15 | 46 | FSMC_D12  | TIM1_BKIN
#if 0
    {GPIOF,  0,   NULL, 0, NULL, ADCx}, // D80/PF0
    {GPIOF,  1,   NULL, 0, NULL, ADCx}, // D81/PF1
    {GPIOF,  2,   NULL, 0, NULL, ADCx}, // D82/PF2
    {GPIOF,  3,   NULL, 0, NULL, ADCx}, // D83/PF3
    {GPIOF,  4,   NULL, 0, NULL, ADCx}, // D84/PF4
    {GPIOF,  5,   NULL, 0, NULL, ADCx}, // D85/PF5
    {GPIOF,  6,   NULL, 0, NULL, ADCx}, // D86/PF6
    {GPIOF,  7,   NULL, 0, NULL, ADCx}, // D87/PF7
    {GPIOF,  8,   NULL, 0, NULL, ADCx}, // D88/PF8
    {GPIOF,  9,   NULL, 0, NULL, ADCx}, // D89/PF9
    {GPIOF, 10,   NULL, 0, NULL, ADCx}, // D90/PF10
    {GPIOF, 11,   NULL, 0, NULL, ADCx}, // D91/PF11
    {GPIOF, 12,   NULL, 0, NULL, ADCx}, // D92/PF12
    {GPIOF, 13,   NULL, 0, NULL, ADCx}, // D93/PF13
    {GPIOF, 14,   NULL, 0, NULL, ADCx}, // D94/PF14
    {GPIOF, 15,   NULL, 0, NULL, ADCx}, // D95/PF15

    {GPIOG,  0,   NULL, 0, NULL, ADCx}, // D96/PG0
    {GPIOG,  1,   NULL, 0, NULL, ADCx}, // D97/PG1
    {GPIOG,  2,   NULL, 0, NULL, ADCx}, // D98/PG2
    {GPIOG,  3,   NULL, 0, NULL, ADCx}, // D99/PG3
    {GPIOG,  4,   NULL, 0, NULL, ADCx}, // D100/PG4
    {GPIOG,  5,   NULL, 0, NULL, ADCx}, // D101/PG5
    {GPIOG,  6,   NULL, 0, NULL, ADCx}, // D102/PG6
    {GPIOG,  7,   NULL, 0, NULL, ADCx}, // D103/PG7 
    {GPIOG,  8,   NULL, 0, NULL, ADCx}, // D104/PG8
    {GPIOG,  9,   NULL, 0, NULL, ADCx}, // D105/PG9
    {GPIOG, 10,   NULL, 0, NULL, ADCx}, // D106/PG10
    {GPIOG, 11,   NULL, 0, NULL, ADCx}, // D107/PG11
    {GPIOG, 12,   NULL, 0, NULL, ADCx}, // D108/PG12
    {GPIOG, 13,   NULL, 0, NULL, ADCx}, // D109/PG13
    {GPIOG, 14,   NULL, 0, NULL, ADCx}, // D110/PG14
    {GPIOG, 15,   NULL, 0, NULL, ADCx}  // D111/PG15
#endif
};
/* to be defined
extern const uint8 boardPWMPins[BOARD_NR_PWM_PINS] __FLASH__ = {
    0, 1, 2, 3, 15, 16, 17, 19, 20, 21, 38, 39, 49, 41, 60, 61, 62, 63, 73, 75, 77, 78
};
*/
const uint8 boardADCPins[BOARD_NR_ADC_PINS] = {
    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PC0, PC1, PC2, PC3, PC4, PC5
};

const uint8 boardUsedPins[BOARD_NR_USED_PINS] = {
    BOARD_LED_PIN, BOARD_LED2_PIN, BOARD_BUTTON1_PIN, BOARD_BUTTON2_PIN, BOARD_BUTTON2_PIN,
	BOARD_JTMS_SWDIO_PIN, BOARD_JTCK_SWCLK_PIN,
	FLASH_CS_PIN, FLASH_CLK_PIN, FLASH_DO_PIN, FLASH_DI_PIN,
	NRF24_CE_PIN, NRF24_CS_PIN, NRF24_IRQ_PIN,
	BOARD_SDIO_D0, BOARD_SDIO_D1, BOARD_SDIO_D2, BOARD_SDIO_D3, BOARD_SDIO_CLK, BOARD_SDIO_CMD,
	USB_DM_PIN, USB_DP_PIN
};

#ifdef __cplusplus
}
#endif


#endif // BOARD_generic_f407v
