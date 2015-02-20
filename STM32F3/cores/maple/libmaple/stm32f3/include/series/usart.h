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
 * @file libmaple/stm32f3/include/series/usart.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 USART support.
 */

#ifndef _LIBMAPLE_STM32F3_USART_H_
#define _LIBMAPLE_STM32F3_USART_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/gpio.h>      /* for gpio_af */

/*
 * Register maps
 */

/** USART register map type */
typedef struct usart_reg_map {
    __io uint32 CR1;            /**< Control register 1 */
    __io uint32 CR2;            /**< Control register 2 */
    __io uint32 CR3;            /**< Control register 3 */
    __io uint32 BRR;            /**< Baud rate register */
    __io uint32 GTPR;           /**< Guard time and prescaler register */
    __io uint32 RTOR;           /**< Receiver timeout register */
    __io uint32 RQR;            /**< Request register */
    __io uint32 SR;             /**< ISR Interrupt and status register */
    __io uint32 ICR;            /**< Interrupt clear register */
    __io uint16 RDR;            /**< Receive data register */
		uint16 RESERVED1;
    __io uint16 TDR;            /**< Transmit data register */
		uint16 RESERVED2;
} usart_reg_map;

/*
 * Register map base pointers
 */

/** USART1 register map base pointer */
#define USART1_BASE                     ((struct usart_reg_map*)0x40013800)
/** USART2 register map base pointer */
#define USART2_BASE                     ((struct usart_reg_map*)0x40004400)
/** USART3 register map base pointer */
#define USART3_BASE                     ((struct usart_reg_map*)0x40004800)

#if defined(STM32_HIGH_DENSITY) || defined(STM32_XL_DENSITY)
/** UART4 register map base pointer */
#define UART4_BASE                      ((struct usart_reg_map*)0x40004C00)
/** UART5 register map base pointer */
#define UART5_BASE                      ((struct usart_reg_map*)0x40005000)
#endif

/*
 * Devices
 */

struct usart_dev;
extern struct usart_dev *USART1;
extern struct usart_dev *USART2;
extern struct usart_dev *USART3;
#ifdef STM32_HIGH_DENSITY
extern struct usart_dev *UART4;
extern struct usart_dev *UART5;
#endif

/*
 * F3-only register bit definitions.
 */

/* Control register 1 */
#define USART_CR1_EOBIE_BIT					27
#define USART_CR1_RTOIE_BIT					26
#define USART_CR1_DEAT4_BIT					25
#define USART_CR1_DEAT3_BIT					24
#define USART_CR1_DEAT2_BIT					23
#define USART_CR1_DEAT1_BIT					22
#define USART_CR1_DEAT0_BIT					21
#define USART_CR1_DEDT4_BIT					20
#define USART_CR1_DEDT3_BIT					19
#define USART_CR1_DEDT2_BIT					18
#define USART_CR1_DEDT1_BIT					17
#define USART_CR1_DEDT0_BIT					16
#define USART_CR1_OVER8_BIT					15
#define USART_CR1_CMIE_BIT					14
#define USART_CR1_MME_BIT						13
#define USART_CR1_UESM_BIT					1
#define USART_CR1_UE_BIT						0

#define USART_CR1_EOBIE							(1UL << USART_CR1_EOBIE_BIT)
#define USART_CR1_RTOIE             (1UL << USART_CR1_RTOIE_BIT)
#define USART_CR1_DEAT4             (1UL << USART_CR1_DEAT4_BIT)
#define USART_CR1_DEAT3             (1UL << USART_CR1_DEAT3_BIT)
#define USART_CR1_DEAT2             (1UL << USART_CR1_DEAT2_BIT)
#define USART_CR1_DEAT1             (1UL << USART_CR1_DEAT1_BIT)
#define USART_CR1_DEAT0             (1UL << USART_CR1_DEAT0_BIT)
#define USART_CR1_DEDT4             (1UL << USART_CR1_DEDT4_BIT)
#define USART_CR1_DEDT3             (1UL << USART_CR1_DEDT3_BIT)
#define USART_CR1_DEDT2             (1UL << USART_CR1_DEDT2_BIT)
#define USART_CR1_DEDT1             (1UL << USART_CR1_DEDT1_BIT)
#define USART_CR1_DEDT0             (1UL << USART_CR1_DEDT0_BIT)
#define USART_CR1_OVER8             (1UL << USART_CR1_OVER8_BIT)
#define USART_CR1_CMIE              (1UL << USART_CR1_CMIE_BIT)
#define USART_CR1_MME               (1UL << USART_CR1_MME_BIT)
#define USART_CR1_UESM              (1UL << USART_CR1_UESM_BIT)
#define USART_CR1_UE                (1UL << USART_CR1_UE_BIT)

/* Control register 2 */
#define USART_CR2_ADD_SHIFT					24
#define USART_CR2_RTOEN_BIT					23
#define USART_CR2_ABRMOD1_BIT				22
#define USART_CR2_ABRMOD0_BIT				21
#define USART_CR2_ABREN_BIT					20
#define USART_CR2_MSBFIRST_BIT			19
#define USART_CR2_DATAINV_BIT				18
#define USART_CR2_TXINV_BIT					17
#define USART_CR2_RXINV_BIT					16
#define USART_CR2_SWAP_BIT					15
#define USART_CR2_ADDM7_BIT					4

#define USART_CR2_ADD								(0xFF << USART_CR2_ADD_SHIFT)
#define USART_CR2_RTOEN							(1UL << USART_CR2_RTOEN_BIT)
#define USART_CR2_ABRMOD1           (1UL << USART_CR2_ABRMOD1_BIT)
#define USART_CR2_ABRMOD0           (1UL << USART_CR2_ABRMOD0_BIT)
#define USART_CR2_ABREN             (1UL << USART_CR2_ABREN_BIT)
#define USART_CR2_MSBFIRST          (1UL << USART_CR2_MSBFIRST_BIT)
#define USART_CR2_DATAINV           (1UL << USART_CR2_DATAINV_BIT)
#define USART_CR2_TXINV             (1UL << USART_CR2_TXINV_BIT)
#define USART_CR2_RXINV             (1UL << USART_CR2_RXINV_BIT)
#define USART_CR2_SWAP              (1UL << USART_CR2_SWAP_BIT)
#define USART_CR2_ADDM7             (1UL << USART_CR2_ADDM7_BIT)

/* Control register 3 */
#define USART_CR3_WUFIE_BIT					22
#define USART_CR3_WUS_SHIFT					20
#define USART_CR3_SCAR_SHIFT				17
#define USART_CR3_DEP_BIT						15
#define USART_CR3_DEM_BIT						14
#define USART_CR3_DDRE_BIT					13
#define USART_CR3_OVRDIS_BIT				12
#define USART_CR3_ONEBIT_BIT				11

#define USART_CR3_WUFIE							(1UL << USART_CR3_WUFIE_BIT)
#define USART_CR3_WUS								(0x3 << USART_CR3_WUS_SHIFT)
#define USART_CR3_SCAR							(0x7 << USART_CR3_SCAR_SHIFT)
#define USART_CR3_DEP								(1UL << USART_CR3_DEP_BIT)
#define USART_CR3_DEM               (1UL << USART_CR3_DEM_BIT)
#define USART_CR3_DDRE              (1UL << USART_CR3_DDRE_BIT)
#define USART_CR3_OVRDIS            (1UL << USART_CR3_OVRDIS_BIT)
#define USART_CR3_ONEBIT            (1UL << USART_CR3_ONEBIT_BIT)

/* Receive timeout register */
#define USART_RTOR_BLEN_SHIFT				24
#define USART_RTOR_RTO_SHIFT				0

#define USART_RTOR_BLEN							(0xF << USART_RTOR_BLEN_SHIFT)
#define USART_RTOR_RTO							(0xFFF << USART_RTOR_RTO_SHIFT)

/* Request register */
#define USART_RQR_TXFRQ_BIT					4
#define USART_RQR_RXFRQ_BIT					3
#define USART_RQR_MMRQ_BIT					2
#define USART_RQR_SBKRQ_BIT					1
#define USART_RQR_ABRRQ_BIT					0

#define USART_RQR_TXFRQ							(1UL << USART_RQR_TXFRQ_BIT)
#define USART_RQR_RXFRQ             (1UL << USART_RQR_RXFRQ_BIT)
#define USART_RQR_MMRQ              (1UL << USART_RQR_MMRQ_BIT)
#define USART_RQR_SBKRQ             (1UL << USART_RQR_SBKRQ_BIT)
#define USART_RQR_ABRRQ             (1UL << USART_RQR_ABRRQ_BIT)

/* Interrupt and status register */
// common register bits with other STM32 series are defined as USART_SR_* for compatibility 
#define USART_SR_REACK_BIT					22
#define USART_SR_TEACK_BIT					21
#define USART_SR_WUF_BIT						20
#define USART_SR_RWU_BIT						19
#define USART_SR_SBKF_BIT						18
#define USART_SR_CMF_BIT						17
#define USART_SR_BUSY_BIT						16
#define USART_SR_ABRF_BIT						15
#define USART_SR_ABRE_BIT						14
#define USART_SR_EOBF_BIT						12
#define USART_SR_RTOF_BIT						11
#define USART_SR_CTS_BIT						10
#define USART_SR_CTSIF_BIT					9

#define USART_SR_REACK							(1UL << USART_ISR_REACK_BIT)
#define USART_SR_TEACK        	    (1UL << USART_ISR_TEACK_BIT)
#define USART_SR_WUF          	    (1UL << USART_ISR_WUF_BIT)
#define USART_SR_RWU          	    (1UL << USART_ISR_RWU_BIT)
#define USART_SR_SBKF         	    (1UL << USART_ISR_SBKF_BIT)
#define USART_SR_CMF          	    (1UL << USART_ISR_CMF_BIT)
#define USART_SR_BUSY         	    (1UL << USART_ISR_BUSY_BIT)
#define USART_SR_ABRF         	    (1UL << USART_ISR_ABRF_BIT)
#define USART_SR_ABRE         	    (1UL << USART_ISR_ABRE_BIT)
#define USART_SR_EOBF         	    (1UL << USART_ISR_EOBF_BIT)
#define USART_SR_RTOF         	    (1UL << USART_ISR_RTOF_BIT)
#define USART_SR_CTS	        	    (1UL << USART_ISR_CTS_BIT)
#define USART_SR_CTSIF        	    (1UL << USART_ISR_CTSIF_BIT)

/* Interrupt clear register */
#define USART_ICR_WUFCF_BIT					20
#define USART_ICR_CMCF_BIT					17
#define USART_ICR_EOBCF_BIT					12
#define USART_ICR_RTOCF_BIT					11
#define USART_ICR_CTSCF_BIT					9
#define USART_ICR_LBDCF_BIT					8
#define USART_ICR_TCCF_BIT					6
#define USART_ICR_IDLECF_BIT				4
#define USART_ICR_ORECF_BIT					3
#define USART_ICR_NCF_BIT						2
#define USART_ICR_FECF_BIT					1
#define USART_ICR_PECF_BIT					0

#define USART_ICR_WUFCF        			(1UL << USART_ICR_WUFCF_BIT)
#define USART_ICR_CMCF         			(1UL << USART_ICR_CMCF_BIT)
#define USART_ICR_EOBCF        			(1UL << USART_ICR_EOBCF_BIT)
#define USART_ICR_RTOCF        			(1UL << USART_ICR_RTOCF_BIT)
#define USART_ICR_CTSCF        			(1UL << USART_ICR_CTSCF_BIT)
#define USART_ICR_LBDCF        			(1UL << USART_ICR_LBDCF_BIT)
#define USART_ICR_TCCF         			(1UL << USART_ICR_TCCF_BIT)
#define USART_ICR_IDLECF       			(1UL << USART_ICR_IDLECF_BIT)
#define USART_ICR_ORECF        			(1UL << USART_ICR_ORECF_BIT)
#define USART_ICR_NCF          			(1UL << USART_ICR_NCF_BIT)
#define USART_ICR_FECF         			(1UL << USART_ICR_FECF_BIT)
#define USART_ICR_PECF         			(1UL << USART_ICR_PECF_BIT)

/* Receive data register */
#define USART_RDR_RDR_SHIFT					0

#define USART_RDR_RDR								(0x1FF << USART_RDR_RDR_SHIFT)


/* Transmit data register */
#define USART_TDR_TDR_SHIFT					0

#define USART_TDR_TDR								(0x1FF << USART_TDR_TDR_SHIFT)

/*
 * Routines
 */

gpio_af usart_get_af(struct usart_dev *dev);

#ifdef __cplusplus
}
#endif

#endif
