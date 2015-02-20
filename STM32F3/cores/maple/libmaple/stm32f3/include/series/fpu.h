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
 * @file libmaple/stm32f3/include/series/fpu.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 Floating Point Unit support.
 */

#ifndef _LIBMAPLE_STM32F3_FPU_H_
#define _LIBMAPLE_STM32F3_FPU_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>
#include <libmaple/scb.h>

/*
 * FPU register maps and devices
 */

/** FPU register map type */
typedef struct fpu_reg_map {
    __io uint32 CPACR;          /**< coprocessor access control register */
    __io uint32 FPCCR;          /**< floating-point context control register */
    __io uint32 FPCAR;          /**< floating-point context address register */
    __io uint32 FPDSCR;         /**< floating-point default status control register */
} fpu_reg_map;

#define FPU_BASE			((struct fpu_reg_map*)(SCB_BASE + 0x88))

/* TODO
 * give registry bitfields here
 */

#ifdef __cplusplus
}
#endif

#endif
