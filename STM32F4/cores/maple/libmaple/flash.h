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
 * @file flash.h
 * @brief STM32 Medium and high density Flash register map and setup
 * routines
 */

#include "libmaple_types.h"
#include "util.h"

#ifndef _FLASH_H_
#define _FLASH_H_

#ifdef __cplusplus
extern "C"{
#endif

/** Flash register map type */
typedef struct flash_reg_map {
    __IO uint32 ACR;            /**< Access control register, Address offset: 0x00 */
    __IO uint32 KEYR;           /**< Key register, offset: 0x04 */
    __IO uint32 OPTKEYR;        /**< OPTKEY register, offset: 0x08 */
    __IO uint32 SR;             /**< Status register, offset: 0x0c */
    __IO uint32 CR;             /**< Control register, offset: 0x10 */
    __IO uint32 OPTCR;          /**< Flash option control register, offset: 0x14 */
    __IO uint32 OPTCR1;         /**< Flash option control register 1, F42xxx and F43xxx,, offset: 0x18 */
} flash_reg_map;

/** Flash register map base pointer */
#define FLASH                          ((struct flash_reg_map*)0x40023C00)

// taken from CMSIS
#define UID_BASE         0x1FFF7A10U // Unique device ID register base address
#define FLASHSIZE_BASE   0x1FFF7A22U // FLASH Size register base address
#define PACKAGE_BASE     0x1FFF7BF0U // Package size register base address

/*
 * Register bit definitions
 */

/* Access control register */

#define FLASH_ACR_LATENCY_Pos          (0U)
#define FLASH_ACR_LATENCY_Msk          BIT_MASK_SHIFT(0xFUL, FLASH_ACR_LATENCY_Pos)
//NOTE: F405, F407, F415, F417 has only 3 bits in latency mask !
#define FLASH_ACR_LATENCY_0WS          0x00000000U
#define FLASH_ACR_LATENCY_1WS          0x00000001U
#define FLASH_ACR_LATENCY_2WS          0x00000002U
#define FLASH_ACR_LATENCY_3WS          0x00000003U
#define FLASH_ACR_LATENCY_4WS          0x00000004U
#define FLASH_ACR_LATENCY_5WS          0x00000005U
#define FLASH_ACR_LATENCY_6WS          0x00000006U
#define FLASH_ACR_LATENCY_7WS          0x00000007U
//NOTE: only F42xxx, F43xxx
// F401, F411 has 4 bits there, but it is unusual u'd need so many waits ;)
#define FLASH_ACR_LATENCY_8WS          0x00000008U
#define FLASH_ACR_LATENCY_9WS          0x00000009U
#define FLASH_ACR_LATENCY_10WS         0x0000000AU
#define FLASH_ACR_LATENCY_11WS         0x0000000BU
#define FLASH_ACR_LATENCY_12WS         0x0000000CU
#define FLASH_ACR_LATENCY_13WS         0x0000000DU
#define FLASH_ACR_LATENCY_14WS         0x0000000EU
#define FLASH_ACR_LATENCY_15WS         0x0000000FU


#define FLASH_ACR_PRFTEN_Pos           (8U)
#define FLASH_ACR_PRFTEN               BIT(FLASH_ACR_PRFTEN_Pos)
#define FLASH_ACR_ICEN_Pos             (9U)
#define FLASH_ACR_ICEN                 BIT(FLASH_ACR_ICEN_Pos)
#define FLASH_ACR_DCEN_Pos             (10U)
#define FLASH_ACR_DCEN                 BIT(FLASH_ACR_DCEN_Pos)
#define FLASH_ACR_ICRST_Pos            (11U)
#define FLASH_ACR_ICRST                BIT(FLASH_ACR_ICRST_Pos)
#define FLASH_ACR_DCRST_Pos            (12U)
#define FLASH_ACR_DCRST                bit(FLASH_ACR_DCRST_Pos)

/* Flash KEYR */

#define FLASH_KEYR_KEY1                0x45670123
#define FLASH_KEYR_KEY2                0xCDEF89AB

/* FLASH_OPTKEYR */

#define FLASH_OPTKEYR_KEY1             0x08192A3B
#define FLASH_OPTKEYR_KEY2             0x4C5D6E7F

/* Status register */

#define FLASH_SR_EOP_Pos               (0U)
#define FLASH_SR_EOP                   BIT(FLASH_SR_EOP_Pos)
#define FLASH_SR_SOP_Pos               (1U)
#define FLASH_SR_SOP                   BIT(FLASH_SR_SOP_Pos)
#define FLASH_SR_WRPERR_Pos            (4U)
#define FLASH_SR_WRPERR                BIT(FLASH_SR_WRPERR_Pos)
#define FLASH_SR_PGAERR_Pos            (5U)
#define FLASH_SR_PGAERR                BIT(FLASH_SR_PGAERR_Pos)
#define FLASH_SR_PGPERR_Pos            (6U)
#define FLASH_SR_PGPERR                BIT(FLASH_SR_PGPERR_Pos)
#define FLASH_SR_PGSERR_Pos            (7U)
#define FLASH_SR_PGSERR                BIT(FLASH_SR_PGSERR_Pos)
#define FLASH_SR_RDERR_Pos             (8U)
#define FLASH_SR_RDERR                 BIT(FLASH_SR_RDERR_Pos)
#define FLASH_SR_BSY_Pos               (16U)
#define FLASH_SR_BSY                   BIT(FLASH_SR_BSY_Msk)

/* Control register */

#define FLASH_CR_PG_Pos                (0U)
#define FLASH_CR_PG                    BIT(FLASH_CR_PG_Pos)
#define FLASH_CR_SER_Pos               (1U)
#define FLASH_CR_SER                   BIT(FLASH_CR_SER_Pos)
#define FLASH_CR_MER_Pos               (2U)
#define FLASH_CR_MER                   BIT(FLASH_CR_MER_Pos)
#define FLASH_CR_SNB_Pos               (3U)
//SNB - sector number
//note: for F401, F405, F411, F417 this is 4 bits, F42xxx, F43xxx 5 bits
//for 4 bits one, 5th bit (i.e. absolute bit 7) must be 0
#define FLASH_CR_SNB_Msk               BIT_MASK_SHIFT(0x1FUL, FLASH_CR_SNB_Pos)
#define FLASH_CR_PSIZE_Pos             (8U)
#define FLASH_CR_PSIZE_Msk             BIT_MASK_SHIFT(0x3UL, FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE8X               BIT_MASK_SHIFT(0, FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE16X              BIT_MASK_SHIFT(1U, FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE32X              BIT_MASK_SHIFT(2U, FLASH_CR_PSIZE_Pos)
#define FLASH_CR_PSIZE64X              BIT_MASK_SHIFT(3U, FLASH_CR_PSIZE_Pos)
#define FLASH_CR_STRT_Pos              (16U)
#define FLASH_CR_STRT                  BIT(FLASH_CR_STRT_Pos)
#define FLASH_CR_EOPIE_Pos             (24U)
#define FLASH_CR_EOPIE                 BIT(FLASH_CR_EOPIE_Pos)
#define FLASH_CR_LOCK_Pos              (31U)
#define FLASH_CR_LOCK                  BIT(FLASH_CR_LOCK_Pos)

/* Option byte register */

#define FLASH_OPTCR_OPTLOCK_Pos        (0U)
#define FLASH_OPTCR_OPTLOCK            BIT(FLASH_OPTCR_OPTLOCK_Pos)
#define FLASH_OPTCR_OPTSTRT_Pos        (1U)
#define FLASH_OPTCR_OPTSTRT            BIT(FLASH_OPTCR_OPTSTRT_Pos)
#define FLASH_OPTCR_BOR_LEV_Pos        (2U)
#define FLASH_OPTCR_BOR_LEV_Msk        BIT_MASK_SHIFT(0x3UL, FLASH_OPTCR_BOR_LEV_Pos)
#define FLASH_OPTCR_BOR_LEVOFF         BIT_MASK_SHIFT(0x3UL, FLASH_OPTCR_BOR_LEV_Pos)
#define FLASH_OPTCR_BOR_LEV1           BIT_MASK_SHIFT(2U, FLASH_OPTCR_BOR_LEV_Pos)
#define FLASH_OPTCR_BOR_LEV2           BIT_MASK_SHIFT(1U, FLASH_OPTCR_BOR_LEV_Pos)
#define FLASH_OPTCR_BOR_LEV3           BIT_MASK_SHIFT(0U, FLASH_OPTCR_BOR_LEV_Pos)
//note: only for F42xxx, F43xxx, must be 0 otherwise
#define FLASH_OPTCR_BFB2_Pos           (4U)
#define FLASH_OPTCR_BFB2               BIT(FLASH_OPTCR_BFB2_Pos)

#define FLASH_OBR_USER_Msk             BIT_MASK_SHIFT(0x7UL, 5U)
#define FLASH_OPTCR_WDG_SW_Pos         (5U)
#define FLASH_OPTCR_WDG_SW             BIT(FLASH_OPTCR_WDG_SW_Pos)
#define FLASH_OPTCR_nRST_STOP_Pos      (6U)
#define FLASH_OPTCR_nRST_STOP          BIT(FLASH_OPTCR_nRST_STOP_Pos)
#define FLASH_OPTCR_nRST_STDBY_Pos     (7U)
#define FLASH_OPTCR_nRST_STDBY         BIT(FLASH_OPTCR_nRST_STDBY_Pos)
#define FLASH_OPTCR_RDP_Pos            (8U)
#define FLASH_OPTCR_RDP_Msk            BIT_MASK_SHIFT(0xFFUL, FLASH_OPTCR_RDP_Pos)
#define FLASH_OPTCR_RDP_LVL0           BIT_MASK_SHIFT(0xAAU, FLASH_OPTCR_RDP_Pos)
#define FLASH_OPTCR_RDP_LVL2           BIT_MASK_SHIFT(0xCCU, FLASH_OPTCR_RDP_Pos)
#define FLASH_OPTCR_nWRP_Pos           (16U)
#define FLASH_OPTCR_nWRP_Msk           BIT_MASK_SHIFT(0xFFFUL, FLASH_OPTCR_nWRP_Pos)
#define FLASH_OPTCR_nWRP_0             0x00010000U
#define FLASH_OPTCR_nWRP_1             0x00020000U
#define FLASH_OPTCR_nWRP_2             0x00040000U
#define FLASH_OPTCR_nWRP_3             0x00080000U
#define FLASH_OPTCR_nWRP_4             0x00100000U
#define FLASH_OPTCR_nWRP_5             0x00200000U
#define FLASH_OPTCR_nWRP_6             0x00400000U
#define FLASH_OPTCR_nWRP_7             0x00800000U
#define FLASH_OPTCR_nWRP_8             0x01000000U
#define FLASH_OPTCR_nWRP_9             0x02000000U
#define FLASH_OPTCR_nWRP_10            0x04000000U
#define FLASH_OPTCR_nWRP_11            0x08000000U

// only F42xxx, F43xxx
#define FLASH_OPTCR_DB1M_Pos           (30U)
#define FLASH_OPTCR_DB1M               BIT(FLASH_OPTCR_DB1M_Pos)
#define FLASH_OPTCR_SPRMOD_Pos         (31U)
#define FLASH_OPTCR_SPRMOD             BIT(FLASH_OPTCR_SPRMOD_Pos)

/* FLASH_OPTCR1 register, only F42xxx, F43xxx*/

#define FLASH_OPTCR1_nWRP_Pos          (16U)
#define FLASH_OPTCR1_nWRP_Msk          BIT_MASK_SHIFT(0xFFFUL, FLASH_OPTCR1_nWRP_Pos)
#define FLASH_OPTCR1_nWRP_0            (0x001UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00010000 */
#define FLASH_OPTCR1_nWRP_1            (0x002UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00020000 */
#define FLASH_OPTCR1_nWRP_2            (0x004UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00040000 */
#define FLASH_OPTCR1_nWRP_3            (0x008UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00080000 */
#define FLASH_OPTCR1_nWRP_4            (0x010UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00100000 */
#define FLASH_OPTCR1_nWRP_5            (0x020UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00200000 */
#define FLASH_OPTCR1_nWRP_6            (0x040UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00400000 */
#define FLASH_OPTCR1_nWRP_7            (0x080UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x00800000 */
#define FLASH_OPTCR1_nWRP_8            (0x100UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x01000000 */
#define FLASH_OPTCR1_nWRP_9            (0x200UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x02000000 */
#define FLASH_OPTCR1_nWRP_10           (0x400UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x04000000 */
#define FLASH_OPTCR1_nWRP_11           (0x800UL << FLASH_OPTCR1_nWRP_Pos)       /*!< 0x08000000 */


/*
 * Setup routines
 */

// Turn on the hardware 'ART accelerator' i.e. prefetch + data & instruction cache
static inline void flash_init(uint32 acr_latency) {
	FLASH->ACR = (FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN) | acr_latency;
}

// Enable prefetch buffer, instruction cache and data cache
static inline void flash_enable_ART(void) {
	FLASH->ACR |= FLASH_ACR_PRFTEN;
}

// Disable prefetch buffer, instruction cache and data cache
static inline void flash_disable_ART(void) {
	FLASH->ACR &= ~(FLASH_ACR_PRFTEN);
}

/**
 * @brief Set flash wait states
 *
 * See ST PM0042, section 3.1 for restrictions on the acceptable value
 * of wait_states for a given SYSCLK configuration.
 *
 * @param wait_states number of wait states (one of
 *                    FLASH_ACR_LATENCY_0WS .. FLASH_ACR_LATENCY_7WS
 */
static inline void flash_set_latency(uint32 wait_states) {
    uint32 val = FLASH->ACR & ~(FLASH_ACR_LATENCY_Msk);
    FLASH->ACR = val | wait_states;
}


#ifdef __cplusplus
}
#endif

#endif


