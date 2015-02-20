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
 * @file libmaple/stm32f3/include/series/crc.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 Cylic Redundancy Check (CRC) support.
 */

#ifndef _LIBMAPLE_STM32F3_CRC_H_
#define _LIBMAPLE_STM32F3_CRC_H_

#ifdef __cplusplus
extern "C"{
#endif

/*
 * Register maps
 */

/** CRC register map type */
typedef struct crc_reg_map {
    __io uint32 DR;	            /**< Data register */
    __io uint32 IDR;            /**< Independent data register */
    __io uint32 CR;	            /**< Control register */
    uint32 reserved;
    __io uint32 INIT;           /**< Initial data register */
    __io uint32 POL;           	/**< Polynomial register */
} crc_reg_map;

/*
 * Register map base pointers
 */

/** CRC register map base pointer */
#define CRC_BASE                     ((struct crc_reg_map*)0x40013800) //TODO

/*
 * Register bit definitions
 */

/* Data register */
#define CRC_DR_DATA						0xFFFFFFFF

/* Independent data register */
#define CRC_IDR_DATA					0xFF

/* Control register */
#define CRC_CR_REV_OUT_BIT		7
#define CRC_CR_REV_IN_SHIFT		5
#define CRC_CR_POLYSIZE_SHIFT	3
#define CRC_CR_RESET_BIT			0

#define CRC_CR_REV_OUT				(1U << CRC_CR_REV_OUT_BIT)
#define CRC_CR_REV_IN					(0x3 << CRC_CR_REV_IN_SHIFT)
#define CRC_CR_POLYSIZE				(0x3 << CRC_CR_POLYSIZE_SHIFT)
#define CRC_CR_RESET					(1U << CRC_CR_RESET_BIT)

/* Initial data register */
#define CRC_INIT_DATA					0xFFFFFFFF

/* Polynomial register */
#define CRC_POL_DATA					0xFFFFFFFF

/* TODO add some routines */

#ifdef __cplusplus
}
#endif

#endif
