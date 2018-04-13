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
 * @file libmaple/stm32f1/include/series/flash.h
 * @brief STM32F1 Flash header.
 *
 * Provides register map, base pointer, and register bit definitions
 * for the Flash controller on the STM32F1 line, along with
 * series-specific configuration values.
 */

#ifndef _LIBMAPLE_STM32F1_FLASH_H_
#define _LIBMAPLE_STM32F1_FLASH_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/** @brief STM32F1 Flash register map type */
typedef struct flash_reg_map {
    __IO uint32 ACR;            /**< Access control register */
    __IO uint32 KEYR;           /**< Key register */
    __IO uint32 OPTKEYR;        /**< OPTKEY register */
    __IO uint32 SR;             /**< Status register */
    __IO uint32 CR;             /**< Control register */
    __IO uint32 AR;             /**< Address register */
    __IO uint32 OBR;            /**< Option byte register */
    __IO uint32 WRPR;           /**< Write protection register */
} flash_reg_map;

#define FLASH_BASE                      ((struct flash_reg_map*)0x40022000)

/*
 * Register bit definitions
 */

/* Access control register */

#define FLASH_ACR_PRFTBS_BIT            5
#define FLASH_ACR_PRFTBE_BIT            4
#define FLASH_ACR_HLFCYA_BIT            3

#define FLASH_ACR_PRFTBS                (1U << FLASH_ACR_PRFTBS_BIT)
#define FLASH_ACR_PRFTBE                (1U << FLASH_ACR_PRFTBE_BIT)
#define FLASH_ACR_HLFCYA                (1U << FLASH_ACR_HLFCYA_BIT)
#define FLASH_ACR_LATENCY               0x7

/* Status register */

#define FLASH_SR_EOP_BIT                5
#define FLASH_SR_WRPRTERR_BIT           4
#define FLASH_SR_PGERR_BIT              2
#define FLASH_SR_BSY_BIT                0

#define FLASH_SR_EOP                    (1U << FLASH_SR_EOP_BIT)
#define FLASH_SR_WRPRTERR               (1U << FLASH_SR_WRPRTERR_BIT)
#define FLASH_SR_PGERR                  (1U << FLASH_SR_PGERR_BIT)
#define FLASH_SR_BSY                    (1U << FLASH_SR_BSY_BIT)

/* Control register */

#define FLASH_CR_EOPIE_BIT              12
#define FLASH_CR_ERRIE_BIT              10
#define FLASH_CR_OPTWRE_BIT             9
#define FLASH_CR_LOCK_BIT               7
#define FLASH_CR_STRT_BIT               6
#define FLASH_CR_OPTER_BIT              5
#define FLASH_CR_OPTPG_BIT              4
#define FLASH_CR_MER_BIT                2
#define FLASH_CR_PER_BIT                1
#define FLASH_CR_PG_BIT                 0

#define FLASH_CR_EOPIE                  (1U << FLASH_CR_EOPIE_BIT)
#define FLASH_CR_ERRIE                  (1U << FLASH_CR_ERRIE_BIT)
#define FLASH_CR_OPTWRE                 (1U << FLASH_CR_OPTWRE_BIT)
#define FLASH_CR_LOCK                   (1U << FLASH_CR_LOCK_BIT)
#define FLASH_CR_STRT                   (1U << FLASH_CR_STRT_BIT)
#define FLASH_CR_OPTER                  (1U << FLASH_CR_OPTER_BIT)
#define FLASH_CR_OPTPG                  (1U << FLASH_CR_OPTPG_BIT)
#define FLASH_CR_MER                    (1U << FLASH_CR_MER_BIT)
#define FLASH_CR_PER                    (1U << FLASH_CR_PER_BIT)
#define FLASH_CR_PG                     (1U << FLASH_CR_PG_BIT)

/* Option byte register */

#define FLASH_OBR_nRST_STDBY_BIT        4
#define FLASH_OBR_nRST_STOP_BIT         3
#define FLASH_OBR_WDG_SW_BIT            2
#define FLASH_OBR_RDPRT_BIT             1
#define FLASH_OBR_OPTERR_BIT            0

#define FLASH_OBR_DATA1                 (0xFF << 18)
#define FLASH_OBR_DATA0                 (0xFF << 10)
#define FLASH_OBR_USER                  0x3FF
#define FLASH_OBR_nRST_STDBY            (1U << FLASH_OBR_nRST_STDBY_BIT)
#define FLASH_OBR_nRST_STOP             (1U << FLASH_OBR_nRST_STOP_BIT)
#define FLASH_OBR_WDG_SW                (1U << FLASH_OBR_WDG_SW_BIT)
#define FLASH_OBR_RDPRT                 (1U << FLASH_OBR_RDPRT_BIT)
#define FLASH_OBR_OPTERR                (1U << FLASH_OBR_OPTERR_BIT)

/*
 * Series-specific configuration values.
 */

#define FLASH_SAFE_WAIT_STATES          (F_CPU > 48000000 ? FLASH_WAIT_STATE_2 : F_CPU > 24000000 ? FLASH_WAIT_STATE_1 : FLASH_WAIT_STATE_0)

/* Flash memory features available via ACR */
enum {
    FLASH_PREFETCH   = 0x10,
    FLASH_HALF_CYCLE = 0x8,
    FLASH_ICACHE     = 0x0,     /* Not available on STM32F1 */
    FLASH_DCACHE     = 0x0,     /* Not available on STM32F1 */
};

#ifdef __cplusplus
}
#endif

#endif
