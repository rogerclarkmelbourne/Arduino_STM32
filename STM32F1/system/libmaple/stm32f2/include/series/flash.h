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
 * @file libmaple/stm32f2/include/series/flash.h
 * @brief STM32F2 Flash header.
 *
 * Provides register map, base pointer, and register bit definitions
 * for the Flash controller on the STM32F2 series, along with
 * series-specific configuration values.
 */

#ifndef _LIBMAPLE_STM32F2_FLASH_H_
#define _LIBMAPLE_STM32F2_FLASH_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map
 */

/** @brief STM32F2 Flash register map type */
typedef struct flash_reg_map {
    __IO uint32 ACR;            /**< Access control register */
    __IO uint32 KEYR;           /**< Key register */
    __IO uint32 OPTKEYR;        /**< Option key register */
    __IO uint32 SR;             /**< Status register */
    __IO uint32 CR;             /**< Control register */
    __IO uint32 OPTCR;          /**< Option control register */
} flash_reg_map;

#define FLASH_BASE                      ((struct flash_reg_map*)0x40023C00)

/*
 * Register bit definitions
 */

/* Access control register */

#define FLASH_ACR_DCRST_BIT             12
#define FLASH_ACR_ICRST_BIT             11
#define FLASH_ACR_DCEN_BIT              10
#define FLASH_ACR_ICEN_BIT              9
#define FLASH_ACR_PRFTEN_BIT            8

#define FLASH_ACR_DCRST                 (1U << FLASH_ACR_DCRST_BIT)
#define FLASH_ACR_ICRST                 (1U << FLASH_ACR_ICRST_BIT)
#define FLASH_ACR_DCEN                  (1U << FLASH_ACR_DCEN_BIT)
#define FLASH_ACR_ICEN                  (1U << FLASH_ACR_ICEN_BIT)
#define FLASH_ACR_PRFTEN                (1U << FLASH_ACR_PRFTEN_BIT)
#define FLASH_ACR_LATENCY               0x7
#define FLASH_ACR_LATENCY_0WS           0x0
#define FLASH_ACR_LATENCY_1WS           0x1
#define FLASH_ACR_LATENCY_2WS           0x2
#define FLASH_ACR_LATENCY_3WS           0x3
#define FLASH_ACR_LATENCY_4WS           0x4
#define FLASH_ACR_LATENCY_5WS           0x5
#define FLASH_ACR_LATENCY_6WS           0x6
#define FLASH_ACR_LATENCY_7WS           0x7

/* Key register */

#define FLASH_KEYR_KEY1                 0x45670123
#define FLASH_KEYR_KEY2                 0xCDEF89AB

/* Option key register */

#define FLASH_OPTKEYR_OPTKEY1           0x08192A3B
#define FLASH_OPTKEYR_OPTKEY2           0x4C5D6E7F

/* Status register */

#define FLASH_SR_BSY_BIT                16
#define FLASH_SR_PGSERR_BIT             7
#define FLASH_SR_PGPERR_BIT             6
#define FLASH_SR_PGAERR_BIT             5
#define FLASH_SR_WRPERR_BIT             4
#define FLASH_SR_OPERR_BIT              1
#define FLASH_SR_EOP_BIT                0

#define FLASH_SR_BSY                    (1U << FLASH_SR_BSY_BIT)
#define FLASH_SR_PGSERR                 (1U << FLASH_SR_PGSERR_BIT)
#define FLASH_SR_PGPERR                 (1U << FLASH_SR_PGPERR_BIT)
#define FLASH_SR_PGAERR                 (1U << FLASH_SR_PGAERR_BIT)
#define FLASH_SR_WRPERR                 (1U << FLASH_SR_WRPERR_BIT)
#define FLASH_SR_OPERR                  (1U << FLASH_SR_OPERR_BIT)
#define FLASH_SR_EOP                    (1U << FLASH_SR_EOP_BIT)

/* Control register */

#define FLASH_CR_LOCK_BIT               31
#define FLASH_CR_ERRIE_BIT              25
#define FLASH_CR_EOPIE_BIT              24
#define FLASH_CR_STRT_BIT               16
#define FLASH_CR_MER_BIT                2
#define FLASH_CR_SER_BIT                1
#define FLASH_CR_PG_BIT                 0

#define FLASH_CR_LOCK                   (1U << FLASH_CR_LOCK_BIT)
#define FLASH_CR_ERRIE                  (1U << FLASH_CR_ERRIE_BIT)
#define FLASH_CR_EOPIE                  (1U << FLASH_CR_EOPIE_BIT)
#define FLASH_CR_STRT                   (1U << FLASH_CR_STRT_BIT)

#define FLASH_CR_PSIZE                  (0x3 << 8)
#define FLASH_CR_PSIZE_MUL8             (0x0 << 8)
#define FLASH_CR_PSIZE_MUL16            (0x1 << 8)
#define FLASH_CR_PSIZE_MUL32            (0x2 << 8)
#define FLASH_CR_PSIZE_MUL64            (0x3 << 8)

#define FLASH_CR_SNB                    (0xF << 3)
#define FLASH_CR_SNB_0                  (0x0 << 3)
#define FLASH_CR_SNB_1                  (0x1 << 3)
#define FLASH_CR_SNB_2                  (0x2 << 3)
#define FLASH_CR_SNB_3                  (0x3 << 3)
#define FLASH_CR_SNB_4                  (0x4 << 3)
#define FLASH_CR_SNB_5                  (0x5 << 3)
#define FLASH_CR_SNB_6                  (0x6 << 3)
#define FLASH_CR_SNB_7                  (0x7 << 3)
#define FLASH_CR_SNB_8                  (0x8 << 3)
#define FLASH_CR_SNB_9                  (0x9 << 3)
#define FLASH_CR_SNB_10                 (0xA << 3)
#define FLASH_CR_SNB_11                 (0xB << 3)

#define FLASH_CR_MER                    (1U << FLASH_CR_MER_BIT)
#define FLASH_CR_SER                    (1U << FLASH_CR_SER_BIT)
#define FLASH_CR_PG                     (1U << FLASH_CR_PG_BIT)

/* Option control register */

#define FLASH_OPTCR_NRST_STDBY_BIT      7
#define FLASH_OPTCR_NRST_STOP_BIT       6
#define FLASH_OPTCR_WDG_SW_BIT          5
#define FLASH_OPTCR_OPTSTRT_BIT         1
#define FLASH_OPTCR_OPTLOCK_BIT         0

#define FLASH_OPTCR_NWRP                (0x3FF << 16)

/* Excluded: The many level 1 values */
#define FLASH_OPTCR_RDP                 (0xFF << 8)
#define FLASH_OPTCR_RDP_LEVEL0          (0xAA << 8)
#define FLASH_OPTCR_RDP_LEVEL2          (0xCC << 8)

#define FLASH_OPTCR_USER                (0x7 << 5)
#define FLASH_OPTCR_nRST_STDBY          (1U << FLASH_OPTCR_nRST_STDBY_BIT)
#define FLASH_OPTCR_nRST_STOP           (1U << FLASH_OPTCR_nRST_STOP_BIT)
#define FLASH_OPTCR_WDG_SW              (1U << FLASH_OPTCR_WDG_SW_BIT)

#define FLASH_OPTCR_BOR_LEV             (0x3 << 2)
#define FLASH_OPTCR_BOR_LEVEL3          (0x0 << 2)
#define FLASH_OPTCR_BOR_LEVEL2          (0x1 << 2)
#define FLASH_OPTCR_BOR_LEVEL1          (0x2 << 2)
#define FLASH_OPTCR_BOR_OFF             (0x3 << 2)

#define FLASH_OPTCR_OPTSTRT             (1U << FLASH_OPTCR_OPTSTRT_BIT)
#define FLASH_OPTCR_OPTLOCK             (1U << FLASH_OPTCR_OPTLOCK_BIT)

/*
 * Series-specific configuration values
 */

/* Note that this value depends on a 2.7V--3.6V supply voltage */
#define FLASH_SAFE_WAIT_STATES          FLASH_WAIT_STATE_3

/* Flash memory features available via ACR. */
enum {
    FLASH_PREFETCH = 0x100,
    FLASH_ICACHE   = 0x200,
    FLASH_DCACHE   = 0x400,
};

#ifdef __cplusplus
}
#endif

#endif
