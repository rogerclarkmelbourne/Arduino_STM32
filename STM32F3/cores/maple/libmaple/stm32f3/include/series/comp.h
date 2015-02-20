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
 * @file libmaple/stm32f3/include/series/comp.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 Comparator support.
 */

#ifndef _LIBMAPLE_STM32F3_COMP_H_
#define _LIBMAPLE_STM32F3_COMP_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/*
 * COMP individual register map type.
 */
typedef struct comp_reg_map {
	__io uint32 CSR;				/**< */
} comp_reg_map;

/** COMP device type. */
typedef struct comp_dev {
    comp_reg_map *regs; /**< Register map */
} comp_dev;

/*
 * Devices
 */

extern const struct comp_dev *COMP1;	/* comparator 1 */
extern const struct comp_dev *COMP2;	/* comparator 2 */
extern const struct comp_dev *COMP3;	/* comparator 3 */
extern const struct comp_dev *COMP4;	/* comparator 4 */
extern const struct comp_dev *COMP5;	/* comparator 5 */
extern const struct comp_dev *COMP6;	/* comparator 6 */
extern const struct comp_dev *COMP7;	/* comparator 7 */

/*
 * Register map base pointers
 */

#define COMP1_BASE                       ((struct comp_reg_map*)0x4001001C)
#define COMP2_BASE                       ((struct comp_reg_map*)0x40010020)
#define COMP3_BASE                       ((struct comp_reg_map*)0x40010024)
#define COMP4_BASE                       ((struct comp_reg_map*)0x40010028)
#define COMP5_BASE                       ((struct comp_reg_map*)0x4001002C)
#define COMP6_BASE                       ((struct comp_reg_map*)0x40010030)
#define COMP7_BASE                       ((struct comp_reg_map*)0x40010034)

/*
 * Register bit definitions
 */

/* Control and status register */
#define COMP_CSR_LOCK_BIT					31
#define COMP_CSR_OUT_BIT					30
#define COMP_CSR_BLANKING_SHIFT		18
#define COMP_CSR_HIST_SHIFT				16
#define COMP_CSR_POL_BIT					15
#define COMP_CSR_OUTSEL_SHIFT			10
#define COMP_CSR_WINMODE_BIT			9
#define COMP_CSR_INPSEL_BIT				7
#define COMP_CSR_INMSEL_SHIFT			4
#define COMP_CSR_MODE_SHIFT				2
#define COMP_CSR_EN_BIT						0

#define COMP_CSR_LOCK							(1U << COMP_CSR_LOCK_BIT)
#define COMP_CSR_OUT							(1U << COMP_CSR_OUT_BIT)
#define COMP_CSR_BLANKING					(0x7 << COMP_CSR_BLANKING_SHIFT)
#define COMP_CSR_HIST							(0x3 << COMP_CSR_HIST_SHIFT)
#define COMP_CSR_POL							(1U << COMP_CSR_POL_BIT)
#define COMP_CSR_OUTSEL						(0xF << COMP_CSR_OUTSEL_SHIFT)
#define COMP_CSR_WINMODE					(0xF << COMP_CSR_WINMODE_SHIFT)
#define COMP_CSR_INPSEL						(1U << COMP_CSR_INPSEL_BIT)
#define COMP_CSR_INMSEL						(0x7 << COMP_CSR_INMSEL_SHIFT)
#define COMP_CSR_MODE							(0x3 << COMP_CSR_MODE_SHIFT)
#define COMP_CSR_EN								(1U << COMP_CSR_EN_BIT)

/* TODO
 * actually implement me ;-)
 */

#ifdef __cplusplus
}
#endif

#endif
