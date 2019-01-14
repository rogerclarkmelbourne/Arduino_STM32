/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file usart.h
 * @author Marti Bolivar <mbolivar@leaflabs.com>,
 *         Perry Hung <perry@leaflabs.com>
 * @brief USART definitions and prototypes
 */

#ifndef _USART_H_
#define _USART_H_

#include "libmaple_types.h"
#include "util.h"
#include "rcc.h"
#include "nvic.h"
#include "ring_buffer.h"
#include "bitband.h"


#ifdef __cplusplus
extern "C"{
#endif

/*
 * Register maps and devices
 */

/** USART register map type */
typedef struct usart_reg_map {
    __IO uint32 SR;             /**< Status register */
    __IO uint32 DR;             /**< Data register */
    __IO uint32 BRR;            /**< Baud rate register */
    __IO uint32 CR1;            /**< Control register 1 */
    __IO uint32 CR2;            /**< Control register 2 */
    __IO uint32 CR3;            /**< Control register 3 */
    __IO uint32 GTPR;           /**< Guard time and prescaler register */
} usart_reg_map;

/** USART1 register map base pointer */
#define USART1_BASE                   ((struct usart_reg_map*)0x40011000)
/** USART2 register map base pointer */
#define USART2_BASE                     ((struct usart_reg_map*)0x40004400)
/** USART3 register map base pointer */
#define USART3_BASE                     ((struct usart_reg_map*)0x40004800)
#ifdef STM32_HIGH_DENSITY
/** UART4 register map base pointer */
#define UART4_BASE                      ((struct usart_reg_map*)0x40004C00)
/** UART5 register map base pointer */
#define UART5_BASE                      ((struct usart_reg_map*)0x40005000)
#endif

/*
 * Register bit definitions
 */

/* Status register */

#define USART_SR_CTS_BIT                9
#define USART_SR_LBD_BIT                8
#define USART_SR_TXE_BIT                7
#define USART_SR_TC_BIT                 6
#define USART_SR_RXNE_BIT               5
#define USART_SR_IDLE_BIT               4
#define USART_SR_ORE_BIT                3
#define USART_SR_NE_BIT                 2
#define USART_SR_FE_BIT                 1
#define USART_SR_PE_BIT                 0

#define USART_SR_CTS                    BIT(USART_SR_CTS_BIT)
#define USART_SR_LBD                    BIT(USART_SR_LBD_BIT)
#define USART_SR_TXE                    BIT(USART_SR_TXE_BIT)
#define USART_SR_TC                     BIT(USART_SR_TC_BIT)
#define USART_SR_RXNE                   BIT(USART_SR_RXNE_BIT)
#define USART_SR_IDLE                   BIT(USART_SR_IDLE_BIT)
#define USART_SR_ORE                    BIT(USART_SR_ORE_BIT)
#define USART_SR_NE                     BIT(USART_SR_NE_BIT)
#define USART_SR_FE                     BIT(USART_SR_FE_BIT)
#define USART_SR_PE                     BIT(USART_SR_PE_BIT)

/* Data register */

#define USART_DR_DR                     0xFF

/* Baud rate register */

#define USART_BRR_DIV_MANTISSA          (0xFFF << 4)
#define USART_BRR_DIV_FRACTION          0xF

/* Control register 1 */

#define USART_CR1_UE_BIT                13
#define USART_CR1_M_BIT                 12
#define USART_CR1_WAKE_BIT              11
#define USART_CR1_PCE_BIT               10
#define USART_CR1_PS_BIT                9
#define USART_CR1_PEIE_BIT              8
#define USART_CR1_TXEIE_BIT             7
#define USART_CR1_TCIE_BIT              6
#define USART_CR1_RXNEIE_BIT            5
#define USART_CR1_IDLEIE_BIT            4
#define USART_CR1_TE_BIT                3
#define USART_CR1_RE_BIT                2
#define USART_CR1_RWU_BIT               1
#define USART_CR1_SBK_BIT               0

#define USART_CR1_UE                    BIT(USART_CR1_UE_BIT)
#define USART_CR1_M                     BIT(USART_CR1_M_BIT)
#define USART_CR1_WAKE                  BIT(USART_CR1_WAKE_BIT)
#define USART_CR1_WAKE_IDLE             (0 << USART_CR1_WAKE_BIT)
#define USART_CR1_WAKE_ADDR             (1 << USART_CR1_WAKE_BIT)
#define USART_CR1_PCE                   BIT(USART_CR1_PCE_BIT)
#define USART_CR1_PS                    BIT(USART_CR1_PS_BIT)
#define USART_CR1_PS_EVEN               (0 << USART_CR1_PS_BIT)
#define USART_CR1_PS_ODD                (1 << USART_CR1_PS_BIT)
#define USART_CR1_PEIE                  BIT(USART_CR1_PEIE_BIT)
#define USART_CR1_TXEIE                 BIT(USART_CR1_TXEIE_BIT)
#define USART_CR1_TCIE                  BIT(USART_CR1_TCIE_BIT)
#define USART_CR1_RXNEIE                BIT(USART_CR1_RXNEIE_BIT)
#define USART_CR1_IDLEIE                BIT(USART_CR1_IDLEIE_BIT)
#define USART_CR1_TE                    BIT(USART_CR1_TE_BIT)
#define USART_CR1_RE                    BIT(USART_CR1_RE_BIT)
#define USART_CR1_RWU                   BIT(USART_CR1_RWU_BIT)
#define USART_CR1_RWU_ACTIVE            (0 << USART_CR1_RWU_BIT)
#define USART_CR1_RWU_MUTE              (1 << USART_CR1_RWU_BIT)
#define USART_CR1_SBK                   BIT(USART_CR1_SBK_BIT)

/* Control register 2 */

#define USART_CR2_LINEN_BIT             14
#define USART_CR2_CLKEN_BIT             11
#define USART_CR2_CPOL_BIT              10
#define USART_CR2_CPHA_BIT              9
#define USART_CR2_LBCL_BIT              8
#define USART_CR2_LBDIE_BIT             6
#define USART_CR2_LBDL_BIT              5

#define USART_CR2_LINEN                 BIT(USART_CR2_LINEN_BIT)
#define USART_CR2_STOP                  (0x3 << 12)
#define USART_CR2_STOP_BITS_1           (0x0 << 12)
/* Not on UART4, UART5 */
#define USART_CR2_STOP_BITS_POINT_5     (0x1 << 12)
/* Not on UART4, UART5 */
#define USART_CR2_STOP_BITS_1_POINT_5   (0x3 << 12)
#define USART_CR2_STOP_BITS_2           (0x2 << 12)
#define USART_CR2_CLKEN                 BIT(USART_CR2_CLKEN_BIT)
/* Not on UART4, UART5 */
#define USART_CR2_CPOL                  BIT(USART_CR2_CPOL_BIT)
#define USART_CR2_CPOL_LOW              (0x0 << USART_CR2_CLKEN_BIT)
#define USART_CR2_CPOL_HIGH             (0x1 << USART_CR2_CLKEN_BIT)
/* Not on UART4, UART5 */
#define USART_CR2_CPHA                  BIT(USART_CR2_CPHA_BIT)
#define USART_CR2_CPHA_FIRST            (0x0 << USART_CR2_CPHA_BIT)
#define USART_CR2_CPHA_SECOND           (0x1 << USART_CR2_CPHA_BIT)
/* Not on UART4, UART5 */
#define USART_CR2_LBCL                  BIT(USART_CR2_LBCL_BIT)
#define USART_CR2_LBDIE                 BIT(USART_CR2_LBDIE_BIT)
#define USART_CR2_LBDL                  BIT(USART_CR2_LBDL_BIT)
#define USART_CR2_LBDL_10_BIT           (0 << USART_CR2_LBDL_BIT)
#define USART_CR2_LBDL_11_BIT           (1 << USART_CR2_LBDL_BIT)
#define USART_CR2_ADD                   0xF

/* Control register 3 */

#define USART_CR3_CTSIE_BIT             10
#define USART_CR3_CTSE_BIT              9
#define USART_CR3_RTSE_BIT              8
#define USART_CR3_DMAT_BIT              7
#define USART_CR3_DMAR_BIT              6
#define USART_CR3_SCEN_BIT              5
#define USART_CR3_NACK_BIT              4
#define USART_CR3_HDSEL_BIT             3
#define USART_CR3_IRLP_BIT              2
#define USART_CR3_IREN_BIT              1
#define USART_CR3_EIE_BIT               0

/* Not on UART4, UART5 */
#define USART_CR3_CTSIE                 BIT(USART_CR3_CTSIE_BIT)
/* Not on UART4, UART5 */
#define USART_CR3_CTSE                  BIT(USART_CR3_CTSE_BIT)
/* Not on UART4, UART5 */
#define USART_CR3_RTSE                  BIT(USART_CR3_RTSE_BIT)
/* Not on UART5 */
#define USART_CR3_DMAT                  BIT(USART_CR3_DMAT_BIT)
/* Not on UART5 */
#define USART_CR3_DMAR                  BIT(USART_CR3_DMAR_BIT)
/* Not on UART4, UART5 */
#define USART_CR3_SCEN                  BIT(USART_CR3_SCEN_BIT)
/* Not on UART4, UART5 */
#define USART_CR3_NACK                  BIT(USART_CR3_NACK_BIT)
#define USART_CR3_HDSEL                 BIT(USART_CR3_HDSEL_BIT)
#define USART_CR3_IRLP                  BIT(USART_CR3_IRLP_BIT)
#define USART_CR3_IRLP_NORMAL           (0 << USART_CR3_IRLP_BIT)
#define USART_CR3_IRLP_LOW_POWER        (1 << USART_CR3_IRLP_BIT)
#define USART_CR3_IREN                  BIT(USART_CR3_IREN_BIT)
#define USART_CR3_EIE                   BIT(USART_CR3_EIE_BIT)

/* Guard time and prescaler register */

/* Not on UART4, UART5 */
#define USART_GTPR_GT                   (0xFF << 8)
/* Not on UART4, UART5 */
#define USART_GTPR_PSC                  0xFF

/*
 * Devices
 */

#ifndef USART_RX_BUF_SIZE
#define USART_RX_BUF_SIZE               256
#endif

#ifndef USART_TX_BUF_SIZE
#define USART_TX_BUF_SIZE               256
#endif

/** USART device type */
typedef struct usart_dev {
    usart_reg_map *regs;             /**< Register map */
    ring_buffer rbRX;                 /**< RX ring buffer */
    ring_buffer rbTX;                 /**< RX ring buffer */
    uint32 max_baud;                 /**< Maximum baud */
    uint8 rx_buf[USART_RX_BUF_SIZE]; /**< @brief Deprecated.
                                      * Actual RX buffer used by rb.
                                      * This field will be removed in
                                      * a future release. */
    uint8 tx_buf[USART_TX_BUF_SIZE];
    rcc_clk_id clk_id;               /**< RCC clock information */
    nvic_irq_num irq_num;            /**< USART NVIC interrupt */
} usart_dev;

extern usart_dev *USART1;
extern usart_dev *USART2;
extern usart_dev *USART3;
#ifdef STM32_HIGH_DENSITY
extern usart_dev *UART4;
extern usart_dev *UART5;
#endif

void usart_init(usart_dev *dev);
void usart_set_baud_rate(usart_dev *dev, uint32 baud);
void usart_enable(usart_dev *dev);
void usart_disable(usart_dev *dev);
void usart_foreach(void (*fn)(usart_dev *dev));
uint32 usart_tx(usart_dev *dev, const uint8 *buf, uint32 len);
void usart_putudec(usart_dev *dev, uint32 val);

/**
 * @brief Disable all serial ports.
 */
static inline void usart_disable_all(void) {
    usart_foreach(usart_disable);
}

/**
 * @brief Transmit one character on a serial port.
 *
 * This function blocks until the character has been successfully
 * transmitted.
 *
 * @param dev Serial port to send on.
 * @param byte Byte to transmit.
 */
static inline void usart_putc(usart_dev* dev, uint8 byte) {
    while (!usart_tx(dev, &byte, 1))
        ;
}

/**
 * @brief Transmit a character string on a serial port.
 *
 * This function blocks until str is completely transmitted.
 *
 * @param dev Serial port to send on
 * @param str String to send
 */
static inline void usart_putstr(usart_dev *dev, const char* str) {
    uint32 i = 0;
    while (str[i] != '\0') {
        usart_putc(dev, str[i++]);
    }
}

/**
 * @brief Read one character from a serial port.
 *
 * It's not safe to call this function if the serial port has no data
 * available.
 *
 * @param dev Serial port to read from
 * @return byte read
 * @see usart_data_available()
 */
static inline uint8 usart_getc(usart_dev *dev) {
    return rb_remove(&dev->rbRX);
}

/*
 * Roger Clark. 20141125,
 * added peek function.
 * @param dev Serial port to read from
 * @return byte read
 */
static inline int usart_peek(usart_dev *dev)
{
    return rb_peek(&dev->rbRX);
}


/**
 * @brief Return the amount of data available in a serial port's RX buffer.
 * @param dev Serial port to check
 * @return Number of bytes in dev's RX buffer.
 */
static inline uint32 usart_data_available(usart_dev *dev) {
    return rb_full_count(&dev->rbRX);
}

/**
 * @brief Return the amount of data available in a serial port's TX buffer.
 * @param dev Serial port to check
 * @return Number of bytes in dev's TX buffer.
 */
static inline uint32 usart_data_pending(usart_dev *dev) {
    return rb_full_count(&dev->rbTX);
}

/**
 * @brief Discard the contents of a serial port's RX buffer.
 * @param dev Serial port whose buffer to empty.
 */
static inline void usart_reset_rx(usart_dev *dev) {
    rb_reset(&dev->rbRX);
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _USART_H_
