/**
 * @file   timer_map.c
 * @author stevestrong
 * @brief  Mapping of the pins to timer device and channel.
 */


#include "timer.h"


const timer_dev * const timer_devices[] = {
NULL, TIMER1, TIMER2, TIMER3, TIMER4, TIMER5, TIMER6, TIMER7, TIMER8, TIMER9, TIMER10, TIMER11, TIMER12, TIMER13, TIMER14
};

/*
typedef struct timer_info {
    uint8_t index;    // Pin's timer device, if any.
    uint8_t channel;  // Timer channel, or 0 if none.
} timer_info;
*/

const timer_info timer_map[] =
{
{TIM5, 1}, // PA0  | 23 | USART2_CTS | UART4_TX       | ETH_MII_CRS      | TIM2_CH1_ETR   | TIM5_CH1       | TIM8_ETR                      | ADC123_IN0/WKUP
{TIM5, 2}, // PA1  | 24 | USART2_RTS | UART4_RX       | ETH_RMII_REF_CLK | ETH_MII_RX_CLK | TIM5_CH2       | TIM2_CH2                      | ADC123_IN1
{TIM5, 3}, // PA2  | 25 | USART2_TX  | TIM5_CH3       | TIM9_CH1         | TIM2_CH3       | ETH_MDIO                                       | ADC123_IN2
{TIM5, 4}, // PA3  | 26 | USART2_RX  | TIM5_CH4       | TIM9_CH2         | TIM2_CH4       | OTG_HS_ULPI_D0 | ETH_MII_COL                   | ADC123_IN3
{NULL, 0}, // PA4  | 29 | SPI1_NSS   | SPI3_NSS       | USART2_CK        | DCMI_HSYNC     | OTG_HS_SOF     | I2S3_WS                       | ADC12_IN4 / DAC_OUT1
{NULL, 0}, // PA5  | 30 | SPI1_SCK   | OTG_HS_ULPI_CK | TIM2_CH1_ETR     | TIM8_CH1N                                                       | ADC12_IN5 / DAC_OUT2
{TIM3, 1}, // PA6  | 31 | SPI1_MISO  | TIM8_BKIN      | TIM13_CH1        | DCMI_PIXCLK    | TIM3_CH1       | TIM1_BKIN                     | ADC12_IN6
{TIM3, 2}, // PA7  | 32 | SPI1_MOSI  | TIM8_CH1N      | TIM14_CH1        | TIM3_CH2       | ETH_MII_RX_DV  | TIM1_CH1N   / ETH_RMII_CRS_DV | ADC12_IN7
{TIM1, 1}, // PA8  | 67 | MCO1       | USART1_CK      | TIM1_CH1         | I2C3_SCL       | OTG_FS_SOF
{TIM1, 2}, // PA9  | 68 | USART1_TX  | TIM1_CH2       | I2C3_SMBA        | DCMI_D0
{TIM1, 3}, // PA10 | 69 | USART1_RX  | TIM1_CH3       | OTG_FS_ID        | DCMI_D1
{TIM1, 4}, // PA11 | 70 | USART1_CTS | CAN1_RX        | TIM1_CH4         | OTG_FS_DM
{NULL, 0}, // PA12 | 71 | USART1_RTS | CAN1_TX        | TIM1_ETR         | OTG_FS_DP
{NULL, 0}, // PA13 | 72 | JTMS-SWDIO
{NULL, 0}, // PA14 | 76 | JTCK-SWCLK
{TIM2, 1}, // PA15 | 77 | JTDI       | SPI3_NSS       | I2S3_WS          | TIM2_CH1_ETR   | SPI1_NSS

{TIM3, 3}, // PB0  | 35 | TIM3_CH3   | TIM8_CH2N | OTG_HS_ULPI_D1 | ETH_MII_RXD2   | TIM1_CH2N      | ADC12_IN8
{TIM3, 4}, // PB1  | 36 | TIM3_CH4   | TIM8_CH3N | OTG_HS_ULPI_D2 | ETH_MII_RXD3   | TIM1_CH3N      | ADC12_IN9
{NULL, 0}, // PB2  | 37 | BOOT1    
{TIM2, 2}, // PB3  | 89 | JTDO       | TRACESWO  | SPI3_SCK       | I2S3_CK        | TIM2_CH2       | SPI1_SCK
{TIM3, 1}, // PB4  | 90 | NJTRST     | SPI3_MISO | TIM3_CH1       | SPI1_MISO      | I2S3ext_SD
{TIM3, 2}, // PB5  | 91 | I2C1_SMBA  | CAN2_RX   | OTG_HS_ULPI_D7 | ETH_PPS_OUT    | TIM3_CH2       | SPI1_MOSI      | SPI3_MOSI      | DCMI_D10      | I2S3_SD
{TIM4, 1}, // PB6  | 92 | I2C1_SCL   | TIM4_CH1  | CAN2_TX        | DCMI_D5        | USART1_TX
{TIM4, 2}, // PB7  | 93 | I2C1_SDA   | FSMC_NL   | DCMI_VSYNC     | USART1_RX      | TIM4_CH2
{TIM4, 3}, // PB8  | 95 | TIM4_CH3   | SDIO_D4   | TIM10_CH1      | DCMI_D6        | ETH_MII_TXD3   | I2C1_SCL       | CAN1_RX
{TIM4, 4}, // PB9  | 96 | SPI2_NSS   | I2S2_WS   | TIM4_CH4       | TIM11_CH1      | SDIO_D5        | DCMI_D7        | I2C1_SDA       | CAN1_TX
{NULL, 0}, // PB10 | 47 | SPI2_SCK   | I2S2_CK   | I2C2_SCL       | USART3_TX      | OTG_HS_ULPI_D3 | ETH_MII_RX_ER  | TIM2_CH3
{NULL, 0}, // PB11 | 48 | I2C2_SDA   | USART3_RX | OTG_HS_ULPI_D4 | ETH_RMII_TX_EN | ETH_MII_TX_EN  | TIM2_CH4
{NULL, 0}, // PB12 | 51 | SPI2_NSS   | I2S2_WS   | I2C2_SMBA      | USART3_CK      | TIM1_BKIN      | CAN2_RX        | OTG_HS_ULPI_D5 | ETH_RMII_TXD0 | ETH_MII_TXD0 | OTG_HS_ID
{NULL, 0}, // PB13 | 52 | SPI2_SCK   | I2S2_CK   | USART3_CTS     | TIM1_CH1N      | CAN2_TX        | OTG_HS_ULPI_D6 | ETH_RMII_TXD1  | ETH_MII_TXD1
{NULL, 0}, // PB14 | 53 | SPI2_MISO  | TIM1_CH2N | TIM12_CH1      | OTG_HS_DM      | USART3_RTS     | TIM8_CH2N      | I2S2ext_SD
{NULL, 0}, // PB15 | 54 | SPI2_MOSI  | I2S2_SD   | TIM1_CH3N      | TIM8_CH3N      | TIM12_CH2      | OTG_HS_DP

{NULL, 0}, // PC0  | 15 | OTG_HS_ULPI_STP                                                  | ADC123_IN10
{NULL, 0}, // PC1  | 16 | ETH_MDC                                                          | ADC123_IN11
{NULL, 0}, // PC2  | 17 | SPI2_MISO  | OTG_HS_ULPI_DIR  | ETH_MII_TXD2    | I2S2ext_SD     | ADC123_IN12
{NULL, 0}, // PC3  | 18 | SPI2_MOSI  | I2S2_SD          | OTG_HS_ULPI_NXT | ETH_MII_TX_CLK | ADC123_IN13
{NULL, 0}, // PC4  | 33 | ETH_RMII_RX_D0                | ETH_MII_RX_D0                    | ADC12_IN14
{NULL, 0}, // PC5  | 34 | ETH_RMII_RX_D1                | ETH_MII_RX_D1                    | ADC12_IN15
{TIM8, 1}, // PC6  | 63 | I2S2_MCK   | TIM8_CH1/SDIO_D6 | USART6_TX       | DCMI_D0        | TIM3_CH1 
{TIM8, 2}, // PC7  | 64 | I2S3_MCK   | TIM8_CH2/SDIO_D7 | USART6_RX       | DCMI_D1        | TIM3_CH2
{TIM8, 3}, // PC8  | 65 | TIM8_CH3   | SDIO_D0          | TIM3_CH3        | USART6_CK      | DCMI_D2
{TIM8, 4}, // PC9  | 66 | I2S_CKIN   | MCO2             | TIM8_CH4        | SDIO_D1        | I2C3_SDA  | DCMI_D3    | TIM3_CH4
{NULL, 0}, // PC10 | 78 | SPI3_SCK   | I2S3_CK          | UART4_TX        | SDIO_D2        | DCMI_D8   | USART3_TX
{NULL, 0}, // PC11 | 79 | UART4_RX   | SPI3_MISO        | SDIO_D3         | DCMI_D4        | USART3_RX | I2S3ext_SD
{NULL, 0}, // PC12 | 80 | UART5_TX   | SDIO_CK          | DCMI_D9         | SPI3_MOSI      | I2S3_SD   | USART3_CK
{NULL, 0}, // PC13 |  7 | RTC_OUT, RTC_TAMP1, RTC_TS
{NULL, 0}, // PC14 |  8 | OSC32_IN
{NULL, 0}, // PC15 |  9 | OSC32_OUT

{NULL, 0}, // PD0  | 81 | FSMC_D2    | CAN1_RX
{NULL, 0}, // PD1  | 82 | FSMC_D3    | CAN1_TX
{NULL, 0}, // PD2  | 83 | TIM3_ETR   | UART5_RX   | SDIO_CMD | DCMI_D11
{NULL, 0}, // PD3  | 84 | FSMC_CLK   | USART2_CTS
{NULL, 0}, // PD4  | 85 | FSMC_NOE   | USART2_RTS
{NULL, 0}, // PD5  | 86 | FSMC_NWE   | USART2_TX
{NULL, 0}, // PD6  | 87 | FSMC_NWAIT | USART2_RX
{NULL, 0}, // PD7  | 88 | USART2_CK  | FSMC_NE1   | FSMC_NCE2
{NULL, 0}, // PD8  | 55 | FSMC_D13   | USART3_TX
{NULL, 0}, // PD9  | 56 | FSMC_D14   | USART3_RX
{NULL, 0}, // PD10 | 57 | FSMC_D15   | USART3_CK
{NULL, 0}, // PD11 | 58 | FSMC_CLE   | FSMC_A16   | USART3_CTS
{TIM4, 1}, // PD12 | 59 | FSMC_ALE   | FSMC_A17   | TIM4_CH1   | USART3_RTS // remap in
{TIM4, 2}, // PD13 | 60 | FSMC_A18                | TIM4_CH2                // remap in
{TIM4, 3}, // PD14 | 61 | FSMC_D0                 | TIM4_CH3                // remap in
{TIM4, 4}, // PD15 | 62 | FSMC_D1                 | TIM4_CH4                // remap in

{NULL, 0}, // PE0  | 97 | TIM4_ETR  | FSMC_NBL0 | DCMI_D2
{NULL, 0}, // PE1  | 98 | FSMC_NBL1 | DCMI_D3
{NULL, 0}, // PE2  |  1 | TRACECLK  | FSMC_A23  | ETH_MII_TXD3
{NULL, 0}, // PE3  |  2 | TRACED0   | FSMC_A19
{NULL, 0}, // PE4  |  3 | TRACED1   | FSMC_A20  | DCMI_D4
{TIM9, 1}, // PE5  |  4 | TRACED2   | FSMC_A21  | TIM9_CH1     / DCMI_D6
{TIM9, 2}, // PE6  |  5 | TRACED3   | FSMC_A22  | TIM9_CH2     / DCMI_D7
{NULL, 0}, // PE7  | 38 | FSMC_D4   | TIM1_ETR
{NULL, 0}, // PE8  | 39 | FSMC_D5   | TIM1_CH1N
{TIM1, 1}, // PE9  | 40 | FSMC_D6   | TIM1_CH1   // remap in
{NULL, 0}, // PE10 | 41 | FSMC_D7   | TIM1_CH2N
{TIM1, 2}, // PE11 | 42 | FSMC_D8   | TIM1_CH2   // remap in
{NULL, 0}, // PE12 | 43 | FSMC_D9   | TIM1_CH3N
{TIM1, 3}, // PE13 | 44 | FSMC_D10  | TIM1_CH3   // remap in
{TIM1, 4}, // PE14 | 45 | FSMC_D11  | TIM1_CH4   // remap in
{NULL, 0}, // PE15 | 46 | FSMC_D12  | TIM1_BKIN
#ifdef PACKAGE_LQFP144 // not available on LQFP100 package
{ NULL, 0}, // PF0
{ NULL, 0}, // PF1
{ NULL, 0}, // PF2
{ NULL, 0}, // PF3
{ NULL, 0}, // PF4
{ NULL, 0}, // PF5
{TIM10, 1}, // PF6
{TIM11, 1}, // PF7
{TIM13, 1}, // PF8
{TIM14, 1}, // PF9
{ NULL, 0}, // PF10
{ NULL, 0}, // PF11
{ NULL, 0}, // PF12
{ NULL, 0}, // PF13
{ NULL, 0}, // PF14
{ NULL, 0}, // PF15

{ NULL, 0}, // PG0
{ NULL, 0}, // PG1
{ NULL, 0}, // PG2
{ NULL, 0}, // PG3
{ NULL, 0}, // PG4
{ NULL, 0}, // PG5
{ NULL, 0}, // PG6
{ NULL, 0}, // PG7 
{ NULL, 0}, // PG8
{ NULL, 0}, // PG9
{ NULL, 0}, // PG10
{ NULL, 0}, // PG11
{ NULL, 0}, // PG12
{ NULL, 0}, // PG13
{ NULL, 0}, // PG14
{ NULL, 0}  // PG15
#endif
};
