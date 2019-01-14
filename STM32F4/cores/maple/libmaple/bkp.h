/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs, LLC.
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
 * @file bkp.h
 * @brief Backup register support.
 */

#ifndef _BKP_H_
#define _BKP_H_

#include "libmaple.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32_MEDIUM_DENSITY)
#define BKP_NR_DATA_REGS 10
#elif defined(STM32_HIGH_DENSITY)
#define BKP_NR_DATA_REGS 42
#endif

/** Backup peripheral register map type. */
typedef struct bkp_reg_map {
    const uint32 RESERVED1;     ///< Reserved
    __IO uint32 DR1;            ///< Data register 1
    __IO uint32 DR2;            ///< Data register 2
    __IO uint32 DR3;            ///< Data register 3
    __IO uint32 DR4;            ///< Data register 4
    __IO uint32 DR5;            ///< Data register 5
    __IO uint32 DR6;            ///< Data register 6
    __IO uint32 DR7;            ///< Data register 7
    __IO uint32 DR8;            ///< Data register 8
    __IO uint32 DR9;            ///< Data register 9
    __IO uint32 DR10;           ///< Data register 10
    __IO uint32 RTCCR;          ///< RTC control register
    __IO uint32 CR;             ///< Control register
    __IO uint32 CSR;            ///< Control and status register
#ifdef STM32_HIGH_DENSITY
    const uint32 RESERVED2;     ///< Reserved
    const uint32 RESERVED3;     ///< Reserved
    __IO uint32 DR11;           ///< Data register 11
    __IO uint32 DR12;           ///< Data register 12
    __IO uint32 DR13;           ///< Data register 13
    __IO uint32 DR14;           ///< Data register 14
    __IO uint32 DR15;           ///< Data register 15
    __IO uint32 DR16;           ///< Data register 16
    __IO uint32 DR17;           ///< Data register 17
    __IO uint32 DR18;           ///< Data register 18
    __IO uint32 DR19;           ///< Data register 19
    __IO uint32 DR20;           ///< Data register 20
    __IO uint32 DR21;           ///< Data register 21
    __IO uint32 DR22;           ///< Data register 22
    __IO uint32 DR23;           ///< Data register 23
    __IO uint32 DR24;           ///< Data register 24
    __IO uint32 DR25;           ///< Data register 25
    __IO uint32 DR26;           ///< Data register 26
    __IO uint32 DR27;           ///< Data register 27
    __IO uint32 DR28;           ///< Data register 28
    __IO uint32 DR29;           ///< Data register 29
    __IO uint32 DR30;           ///< Data register 30
    __IO uint32 DR31;           ///< Data register 31
    __IO uint32 DR32;           ///< Data register 32
    __IO uint32 DR33;           ///< Data register 33
    __IO uint32 DR34;           ///< Data register 34
    __IO uint32 DR35;           ///< Data register 35
    __IO uint32 DR36;           ///< Data register 36
    __IO uint32 DR37;           ///< Data register 37
    __IO uint32 DR38;           ///< Data register 38
    __IO uint32 DR39;           ///< Data register 39
    __IO uint32 DR40;           ///< Data register 40
    __IO uint32 DR41;           ///< Data register 41
    __IO uint32 DR42;           ///< Data register 42
#endif
} bkp_reg_map;

/** Backup peripheral register map base pointer. */
#define BKP_BASE                        ((struct bkp_reg_map*)0x40024000)

/** Backup peripheral device type. */
typedef struct bkp_dev {
    bkp_reg_map *regs; /**< Register map */
} bkp_dev;

extern const bkp_dev *BKP;

/*
 * Register bit definitions
 */

/* Data Registers */

#define BKP_DR_D                        0xFFFF

/* RTC Clock Calibration Register */

#define BKP_RTCCR_ASOS_BIT              9
#define BKP_RTCCR_ASOE_BIT              8
#define BKP_RTCCR_CCO_BIT               7

#define BKP_RTCCR_ASOS                  BIT(BKP_RTCCR_ASOS_BIT)
#define BKP_RTCCR_ASOE                  BIT(BKP_RTCCR_ASOE_BIT)
#define BKP_RTCCR_CCO                   BIT(BKP_RTCCR_CCO_BIT)
#define BKP_RTCCR_CAL                   0x7F

/* Backup control register */

#define BKP_CR_TPAL_BIT                 1
#define BKP_CR_TPE_BIT                  0

#define BKP_CR_TPAL                     BIT(BKP_CR_TPAL_BIT)
#define BKP_CR_TPE                      BIT(BKP_CR_TPE_BIT)

/* Backup control/status register */

#define BKP_CSR_TIF_BIT                 9
#define BKP_CSR_TEF_BIT                 8
#define BKP_CSR_TPIE_BIT                2
#define BKP_CSR_CTI_BIT                 1
#define BKP_CSR_CTE_BIT                 0

#define BKP_CSR_TIF                     BIT(BKP_CSR_TIF_BIT)
#define BKP_CSR_TEF                     BIT(BKP_CSR_TEF_BIT)
#define BKP_CSR_TPIE                    BIT(BKP_CSR_TPIE_BIT)
#define BKP_CSR_CTI                     BIT(BKP_CSR_CTI_BIT)
#define BKP_CSR_CTE                     BIT(BKP_CSR_CTE_BIT)

/*
 * Convenience functions
 */

void bkp_init(void);
void bkp_enable_writes(void);
void bkp_disable_writes(void);
uint16 bkp_read(uint8 reg);
void bkp_write(uint8 reg, uint16 val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
