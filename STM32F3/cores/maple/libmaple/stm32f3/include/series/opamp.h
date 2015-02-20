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
 * @file libmaple/stm32f3/include/series/ompamp.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 Operational Amplifier support.
 */

#ifndef _LIBMAPLE_STM32F3_OPAMP_H_
#define _LIBMAPLE_STM32F3_OPAMP_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/*
 * OpAmp individual register map type.
 */
typedef struct opamp_reg_map {
	__io uint32 CSR;				/**< */
} opamp_reg_map;

/** OpAmp device type. */
typedef struct opamp_dev {
    opamp_reg_map *regs; /**< Register map */
} opamp_dev;

/*
 * Devices
 */

extern const struct opamp_dev *OPAMP1;	/* OpAmp amplifier 1 */
extern const struct opamp_dev *OPAMP2;	/* OpAmp amplifier 2 */
extern const struct opamp_dev *OPAMP3;	/* OpAmp amplifier 3 */
extern const struct opamp_dev *OPAMP4;	/* OpAmp amplifier 4 */

/*
 * Register map base pointers
 */

#define OPAMP1_BASE                       ((struct opamp_reg_map*)0x40010038)
#define OPAMP2_BASE                       ((struct opamp_reg_map*)0x4001003C)
#define OPAMP3_BASE                       ((struct opamp_reg_map*)0x40010040)
#define OPAMP4_BASE                       ((struct opamp_reg_map*)0x40010044)

/*
 * Register bit definitions
 */

/* Control and status register */
#define OPAMP_CSR_LOCK_BIT						31
#define OPAMP_CSR_OUTCAL_BIT					30
#define OPAMP_CSR_TSTREF_BIT					29
#define OPAMP_CSR_TRIMOFFSETN_SHIFT		24
#define OPAMP_CSR_TRIMOFFSETP_SHIFT		19
#define OPAMP_CSR_USER_TRIM_BIT				18
#define OPAMP_CSR_PGA_GAIN_SHIFT			14
#define OPAMP_CSR_CAL_SEL_SHIFT				12
#define OPAMP_CSR_CAL_ON_BIT					11
#define OPAMP_CSR_VPS_SEL_SHIFT				9
#define OPAMP_CSR_VMS_SEL_BIT					8
#define OPAMP_CSR_TCM_EN_BIT					7
#define OPAMP_CSR_VM_SEL_SHIFT				5
#define OPAMP_CSR_VP_SEL_SHIFT				2
#define OPAMP_CSR_FORCE_VP_BIT				1
#define OPAMP_CSR_EN_BIT							0

#define OPAMP_CSR_LOCK								(1U 	<< OPAMP_CSR_LOCK_BIT)
#define OPAMP_CSR_OUTCAL							(1U 	<< OPAMP_CSR_OUTCAL_BIT)
#define OPAMP_CSR_TSTREF							(1U 	<< OPAMP_CSR_TSTREF_BIT)
#define COMP_CSR_TRIMOFFSETN					(0x1F << COMP_CSR_TRIMOFFSETN_SHIFT)
#define COMP_CSR_TRIMOFFSETP					(0x1F << COMP_CSR_TRIMOFFSETP_SHIFT)
#define OPAMP_CSR_USER_TRIM						(1U 	<< OPAMP_CSR_USER_TRIM_BIT)
#define COMP_CSR_PGA_GAIN							(0xF 	<< COMP_CSR_PGA_GAIN_SHIFT)
#define COMP_CSR_CAL_SEL							(0x3 	<< COMP_CSR_CAL_SEL_SHIFT)
#define OPAMP_CSR_CAL_ON							(1U 	<< OPAMP_CSR_CAL_ON_BIT)
#define COMP_CSR_VPS_SEL							(0x3 	<< COMP_CSR_VPS_SEL_SHIFT)
#define OPAMP_CSR_VMS_SEL							(1U 	<< OPAMP_CSR_VMS_SEL_BIT)
#define OPAMP_CSR_TCM_EN							(1U 	<< OPAMP_CSR_TCM_EN_BIT)
#define COMP_CSR_VM_SEL								(0x3 	<< COMP_CSR_VM_SEL_SHIFT)
#define COMP_CSR_VP_SEL								(0x3 	<< COMP_CSR_VP_SEL_SHIFT)
#define OPAMP_CSR_FORCE_VP						(1U 	<< OPAMP_CSR_FORCE_VP_BIT)
#define OPAMP_CSR_EN									(1U 	<< OPAMP_CSR_EN_BIT)

/* TODO
 * actually implement me ;-)
 */

#ifdef __cplusplus
}
#endif

#endif
