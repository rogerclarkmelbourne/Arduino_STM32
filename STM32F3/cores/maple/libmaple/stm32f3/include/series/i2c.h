/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2013 OpenMusicKontrollers.
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
 * @file libmaple/stm32f3/include/series/i2c.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 I2C
 */

#ifndef _LIBMAPLE_STM32F3_I2C_H_
#define _LIBMAPLE_STM32F3_I2C_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/i2c_common.h>
#include <libmaple/gpio.h>
#include <libmaple/stm32.h>

/*
 * Register maps
 */

/** I2C register map type */
typedef struct i2c_reg_map {
    __io uint32 CR1;            /**< Control register 1 */
    __io uint32 CR2;            /**< Control register 2 */
    __io uint32 OAR1;           /**< Own address register 1 */
    __io uint32 OAR2;           /**< Own address register 2 */
    __io uint32 TIMINGR;        /**< Timing register */
    __io uint32 TIMEOUTR;       /**< Timeout register */
    __io uint32 ISR;            /**< Interrupt and status register */
    __io uint32 ICR;            /**< Interrupt clear register */
    __io uint32 PECR;           /**< PEC register */
    __io uint32 RXDR;           /**< Receive data register */
    __io uint32 TXDR;           /**< Transmit data register */
} i2c_reg_map;

extern i2c_dev* const I2C1;
extern i2c_dev* const I2C2;

/*
 * Register map base pointers
 */

/** STM32F3 I2C1 register map base pointer */
#define I2C1_BASE               ((struct i2c_reg_map*)0x40005400)
/** STM32F3 I2C2 register map base pointer */
#define I2C2_BASE               ((struct i2c_reg_map*)0x40005800)

/*
 * Register bit definitions
 */

/* Control register 1 */
#define I2C_CR1_PECEN_BIT							23
#define I2C_CR1_ALERTEN_BIT						22
#define I2C_CR1_SMBDEN_BIT						21
#define I2C_CR1_SMBHEN_BIT						20
#define I2C_CR1_GCEN_BIT							19
#define I2C_CR1_WUPEN_BIT							18
#define I2C_CR1_NOSTRETCH_BIT					17
#define I2C_CR1_SBC_BIT								16
#define I2C_CR1_RXDMAEN_BIT						15
#define I2C_CR1_TXDMAEN_BIT						14
#define I2C_CR1_ANFOFF_BIT						12
#define I2C_CR1_DNF_SHIFT							8
#define I2C_CR1_ERRIE_BIT							7
#define I2C_CR1_TCIE_BIT							6
#define I2C_CR1_STOPIE_BIT						5
#define I2C_CR1_NACKIE_BIT						4
#define I2C_CR1_ADDRIE_BIT						3
#define I2C_CR1_RXIE_BIT							2
#define I2C_CR1_TXIE_BIT							1
#define I2C_CR1_PE_BIT								0

#define I2C_CR1_PECEN									(1U << I2C_CR1_PECEN_BIT)
#define I2C_CR1_ALERTEN								(1U << I2C_CR1_ALERTEN_BIT)
#define I2C_CR1_SMBDEN								(1U << I2C_CR1_SMBDEN_BIT)
#define I2C_CR1_SMBHEN								(1U << I2C_CR1_SMBHEN_BIT)
#define I2C_CR1_GCEN									(1U << I2C_CR1_GCEN_BIT)
#define I2C_CR1_WUPEN									(1U << I2C_CR1_WUPEN_BIT)
#define I2C_CR1_NOSTRETCH							(1U << I2C_CR1_NOSTRETCH_BIT)
#define I2C_CR1_SBC										(1U << I2C_CR1_SBC_BIT)
#define I2C_CR1_RXDMAEN								(1U << I2C_CR1_RXDMAEN_BIT)
#define I2C_CR1_TXDMAEN								(1U << I2C_CR1_TXDMAEN_BIT)
#define I2C_CR1_ANFOFF								(1U << I2C_CR1_ANFOFF_BIT)
#define I2C_CR1_DNF										(0xF << I2C_CR1_DNF_SHIFT)
#define I2C_CR1_ERRIE									(1U << I2C_CR1_ERRIE_BIT)
#define I2C_CR1_TCIE									(1U << I2C_CR1_TCIE_BIT)
#define I2C_CR1_STOPIE								(1U << I2C_CR1_STOPIE_BIT)
#define I2C_CR1_NACKIE								(1U << I2C_CR1_NACKIE_BIT)
#define I2C_CR1_ADDRIE								(1U << I2C_CR1_ADDRIE_BIT)
#define I2C_CR1_RXIE									(1U << I2C_CR1_RXIE_BIT)
#define I2C_CR1_TXIE									(1U << I2C_CR1_TXIE_BIT)
#define I2C_CR1_PE										(1U << I2C_CR1_PE_BIT)

/* Control register 2 */
#define I2C_CR2_PECBYTE_BIT						26
#define I2C_CR2_AUTOEND_BIT						25
#define I2C_CR2_RELOAD_BIT						24
#define I2C_CR2_NBYTES_SHIFT					16
#define I2C_CR2_NACK_BIT							15
#define I2C_CR2_STOP_BIT							14
#define I2C_CR2_START_BIT							13
#define I2C_CR2_HEAD10R_BIT						12
#define I2C_CR2_ADD10_BIT							11
#define I2C_CR2_RD_WRN_BIT						10
#define I2C_CR2_SADD_7_BIT_SHIFT			1
#define I2C_CR2_SADD_10_BIT_SHIFT			0

#define I2C_CR2_PECBYTE								(1U << I2C_CR2_PECBYTE_BIT)
#define I2C_CR2_AUTOEND								(1U << I2C_CR2_AUTOEND_BIT)
#define I2C_CR2_RELOAD								(1U << I2C_CR2_RELOAD_BIT)
#define I2C_CR2_NBYTES								(0xFF << I2C_CR2_NBYTES_SHIFT)
#define I2C_CR2_NACK									(1U << I2C_CR2_NACK_BIT)
#define I2C_CR2_STOP									(1U << I2C_CR2_STOP_BIT)
#define I2C_CR2_START									(1U << I2C_CR2_START_BIT)
#define I2C_CR2_HEAD10R								(1U << I2C_CR2_HEAD10R_BIT)
#define I2C_CR2_ADD10									(1U << I2C_CR2_ADD10_BIT)
#define I2C_CR2_RD_WRN								(1U << I2C_CR2_RD_WRN_BIT)
#define I2C_CR2_SADD_7_BIT						(0x7F << I2C_CR2_SADD_7_BIT_SHIFT)
#define I2C_CR2_SADD_10_BIT						(0x3FF << I2C_CR2_SADD_10_BIT_SHIFT)

/* Own address register 1 */
#define I2C_OAR1_OA1EN_BIT						15
#define I2C_OAR1_OA1MODE_BIT					10
#define I2C_OAR1_OA1_7_BIT_SHIFT			1
#define I2C_OAR1_OA1_10_BIT_SHIFT			0

#define I2C_OAR1_OA1EN								(1U << I2C_OAR1_OA1EN_BIT)
#define I2C_OAR1_OA1MODE							(1U << I2C_OAR1_OA1MODE_BIT)
#define I2C_OAR1_OA1_7_BIT						(0x7F << I2C_OAR1_OA1_7_BIT_SHIFT)
#define I2C_OAR1_OA1_10_BIT						(0x3FF << I2C_OAR1_OA1_10_BIT_SHIFT)

/* Own address register 2 */
#define I2C_OAR2_OA2EN_BIT						15
#define I2C_OAR2_OA2MSK_SHIFT					8
#define I2C_OAR2_OA2_7_BIT_SHIFT			1

#define I2C_OAR2_OA2EN								(1U << I2C_OAR2_OA2EN_BIT)
#define I2C_OAR2_OA2MSK								(0x7 << I2C_OAR2_OA2MSK_SHIFT)
#define I2C_OAR2_OA2_7_BIT						(0x7F << I2C_OAR2_OA2_7_BIT_SHIFT)

/* Timing register */
#define I2C_TIMINGR_PRESC_SHIFT				28
#define I2C_TIMINGR_SCLDEL_SHIFT			20
#define I2C_TIMINGR_SDADEL_SHIFT			16
#define I2C_TIMINGR_SCLH_SHIFT				8
#define I2C_TIMINGR_SCLL_SHIFT				0

#define I2C_TIMINGR_PRESC							(0xF << I2C_TIMINGR_PRESC_SHIFT)
#define I2C_TIMINGR_SCLDEL						(0xF << I2C_TIMINGR_SCLDEL_SHIFT)
#define I2C_TIMINGR_SCADEL						(0xF << I2C_TIMINGR_SCADEL_SHIFT)
#define I2C_TIMINGR_SCLH							(0xFF << I2C_TIMINGR_SCLH_SHIFT)
#define I2C_TIMINGR_SCLL							(0xFF << I2C_TIMINGR_SCLL_SHIFT)

/* Timeout register */
#define I2C_TIMEOUTR_TEXTEN_BIT				31
#define I2C_TIMEOUTR_TIMEOUTB_SHIFT		16
#define I2C_TIMEOUTR_TIMOUTEN_BIT			15
#define I2C_TIMEOUTR_TIDLE_BIT				12
#define I2C_TIMEOUTR_TIMEOUTA_SHIFT		0

#define I2C_TIMEOUTR_TEXTEN						(1U << I2C_TIMEOUTR_TEXTEN_BIT)
#define I2C_TIMEOUTR_TIMEOUTB					(0xFFF << I2C_TIMEOUTR_TIMEOUTB_SHIFT)
#define I2C_TIMEOUTR_TIMOUTEN					(1U << I2C_TIMEOUTR_TIMOUTEN_BIT)
#define I2C_TIMEOUTR_TIDLE						(1U << I2C_TIMEOUTR_TIDLE_BIT)
#define I2C_TIMEOUTR_TIMEOUTA					(0xFFF << I2C_TIMEOUTR_TIMEOUTA_SHIFT)

/* Interrupt and status register */
#define I2C_ISR_ADDCODE_SHIFT					17
#define I2C_ISR_DIR_BIT								16
#define I2C_ISR_BUSY_BIT							15
#define I2C_ISR_ALERT_BIT							13
#define I2C_ISR_TIMEOUT_BIT						12
#define I2C_ISR_PECERR_BIT						11
#define I2C_ISR_OVR_BIT								10
#define I2C_ISR_ARLO_BIT							9
#define I2C_ISR_BERR_BIT							8
#define I2C_ISR_TCR_BIT								7
#define I2C_ISR_TC_BIT								6
#define I2C_ISR_STOPF_BIT							5
#define I2C_ISR_NACKF_BIT							4
#define I2C_ISR_ADDR_BIT							3
#define I2C_ISR_RXNE_BIT							2
#define I2C_ISR_TXIS_BIT							1
#define I2C_ISR_TXE_BIT								0

#define I2C_ISR_ADDCODE								(0x7F << I2C_ISR_ADDCODE_SHIFT)
#define I2C_ISR_ALERTCF								(1U << I2C_ISR_ALERTCF_BIT)
#define I2C_ISR_DIR										(1U << I2C_ISR_DIR_BIT)
#define I2C_ISR_BUSY									(1U << I2C_ISR_BUSY_BIT)
#define I2C_ISR_ALERT									(1U << I2C_ISR_ALERT_BIT)
#define I2C_ISR_TIMEOUT								(1U << I2C_ISR_TIMEOUT_BIT)
#define I2C_ISR_PECERR								(1U << I2C_ISR_PECERR_BIT)
#define I2C_ISR_OVR										(1U << I2C_ISR_OVR_BIT)
#define I2C_ISR_ARLO									(1U << I2C_ISR_ARLO_BIT)
#define I2C_ISR_BERR									(1U << I2C_ISR_BERR_BIT)
#define I2C_ISR_TCR										(1U << I2C_ISR_TCR_BIT)
#define I2C_ISR_TC										(1U << I2C_ISR_TC_BIT)
#define I2C_ISR_STOPF									(1U << I2C_ISR_STOPF_BIT)
#define I2C_ISR_NACKF									(1U << I2C_ISR_NACKF_BIT)
#define I2C_ISR_ADDR									(1U << I2C_ISR_ADDR_BIT)
#define I2C_ISR_RXNE									(1U << I2C_ISR_RXNE_BIT)
#define I2C_ISR_TXIS									(1U << I2C_ISR_TXIS_BIT)
#define I2C_ISR_TXE										(1U << I2C_ISR_TXE_BIT)

/* Interrupt clear register */
#define I2C_ICR_ALERTCF_BIT						13
#define I2C_ICR_TIMOUTCF_BIT					12
#define I2C_ICR_PECCF_BIT							11
#define I2C_ICR_OVRCF_BIT							10
#define I2C_ICR_ARLOCF_BIT						9
#define I2C_ICR_BERRCF_BIT						8
#define I2C_ICR_STOPCF_BIT						5
#define I2C_ICR_NACKCF_BIT						4
#define I2C_ICR_ADDRCF_BIT						3

#define I2C_ICR_ALERTCF								(1U << I2C_ICR_ALERTCF_BIT)
#define I2C_ICR_TIMOUTCF							(1U << I2C_ICR_TIMOUTCF_BIT)
#define I2C_ICR_PECCF									(1U << I2C_ICR_PECERRCF_BIT)
#define I2C_ICR_OVRCF									(1U << I2C_ICR_OVRCF_BIT)
#define I2C_ICR_ARLOCF								(1U << I2C_ICR_ARLOCF_BIT)
#define I2C_ICR_BERRCF								(1U << I2C_ICR_BERRCF_BIT)
#define I2C_ICR_STOPCF								(1U << I2C_ICR_STOPCF_BIT)
#define I2C_ICR_NACKCF								(1U << I2C_ICR_NACKCF_BIT)
#define I2C_ICR_ADDRCF								(1U << I2C_ICR_ADDRCF_BIT)

/* PEC register */
#define I2C_PEC_PEC										0xFF

/* Receive data register */
#define I2C_RXDR_RXDATA								0xFF

/* Transmit data register */
#define I2C_TXDR_TXDATA								0xFF

/*
 * Timing configurations
 */

/* Timing configuration for I2C clock running at 8MHz */

/* NOTE:
 * I2C is clocked by HSI (8MHz) by default
 * if clock source is changed with RCC_CFGR_I2C1SW/I2C2SW,
 * these values have to be adapted
 */
typedef enum i2c_timing {
	I2C_TIMING_10_kHz			= (0x01 << I2C_TIMINGR_PRESC_SHIFT)
												+ (0x04 << I2C_TIMINGR_SCLDEL_SHIFT)
												+ (0x02 << I2C_TIMINGR_SDADEL_SHIFT)
												+ (0xC3 << I2C_TIMINGR_SCLH_SHIFT)
												+ (0xC7 << I2C_TIMINGR_SCLL_SHIFT),

	I2C_TIMING_100_kHz		= (0x01 << I2C_TIMINGR_PRESC_SHIFT)
												+ (0x04 << I2C_TIMINGR_SCLDEL_SHIFT)
												+ (0x02 << I2C_TIMINGR_SDADEL_SHIFT)
												+ (0x0F << I2C_TIMINGR_SCLH_SHIFT)
												+ (0x13 << I2C_TIMINGR_SCLL_SHIFT),

	I2C_TIMING_400_kHz		= (0x00 << I2C_TIMINGR_PRESC_SHIFT)
												+ (0x03 << I2C_TIMINGR_SCLDEL_SHIFT)
												+ (0x01 << I2C_TIMINGR_SDADEL_SHIFT)
												+ (0x03 << I2C_TIMINGR_SCLH_SHIFT)
												+ (0x09 << I2C_TIMINGR_SCLL_SHIFT),

	I2C_TIMING_500_kHz		= (0x00 << I2C_TIMINGR_PRESC_SHIFT)
												+ (0x01 << I2C_TIMINGR_SCLDEL_SHIFT)
												+ (0x01 << I2C_TIMINGR_SDADEL_SHIFT)
												+ (0x03 << I2C_TIMINGR_SCLH_SHIFT)
												+ (0x06 << I2C_TIMINGR_SCLL_SHIFT),
} i2c_timing;

/*
 * For internal use
 */

static inline uint32 _i2c_bus_clk(i2c_dev *dev) { /* FIXME remove, is a remainder of F1 code */
    /* Both I2C peripherals are on APB1 */
    return STM32_PCLK1 / (1000 * 1000);
}

extern uint8 i2c_read(i2c_dev *dev);

#ifdef __cplusplus
}
#endif

#endif  /* _LIBMAPLE_STM32F3_I2C_H_ */
