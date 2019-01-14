/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
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
 * @file fsmc.h
 * @brief Flexible static memory controller support.
 */

/*
 * See ../notes/fsmc.txt for more info
 */

/**
 * @file fsmc.h
 */

#ifndef _FSMC_H_
#define _FSMC_H_

#include <libmaple/util.h>
#include "libmaple_types.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifdef STM32_HIGH_DENSITY

/*
 * Register maps and devices
 */

/** FSMC register map type */
typedef struct fsmc_reg_map {
    __IO uint32 BCR1; /**< SRAM/NOR-Flash chip-select control register 1 */
    __IO uint32 BTR1; /**< SRAM/NOR-Flash chip-select timing register 1 */
    __IO uint32 BCR2; /**< SRAM/NOR-Flash chip-select control register 2 */
    __IO uint32 BTR2; /**< SRAM/NOR-Flash chip-select timing register 2 */
    __IO uint32 BCR3; /**< SRAM/NOR-Flash chip-select control register 3 */
    __IO uint32 BTR3; /**< SRAM/NOR-Flash chip-select timing register 3 */
    __IO uint32 BCR4; /**< SRAM/NOR-Flash chip-select control register 4 */
    __IO uint32 BTR4; /**< SRAM/NOR-Flash chip-select timing register 4 */
    const uint32 RESERVED1[16];  /**< Reserved */
    __IO uint32 PCR2;  /**< PC Card/NAND Flash control register 2 */
    __IO uint32 SR2;   /**< FIFO status and interrupt register 2 */
    __IO uint32 PMEM2; /**< Common memory space timing register 2 */
    __IO uint32 PATT2; /**< Attribute memory space timing register 2 */
    const uint32 RESERVED2;   /**< Reserved */
    __IO uint32 ECCR2; /**< ECC result register 2 */
    const uint32 RESERVED3[2];   /**< Reserved */
    __IO uint32 PCR3;  /**< PC Card/NAND Flash control register 3 */
    __IO uint32 SR3;   /**< FIFO status and interrupt register 3 */
    __IO uint32 PMEM3; /**< Common memory space timing register 3 */
    __IO uint32 PATT3; /**< Attribute memory space timing register 3 */
    const uint32 RESERVED4;     /**< Reserved */
    __IO uint32 ECCR3; /**< ECC result register 3 */
    const uint32 RESERVED5[2];   /**< Reserved */
    __IO uint32 PCR4;  /**< PC Card/NAND Flash control register 4 */
    __IO uint32 SR4;   /**< FIFO status and interrupt register 4 */
    __IO uint32 PMEM4; /**< Common memory space timing register 4 */
    __IO uint32 PATT4; /**< Attribute memory space timing register 4 */
    __IO uint32 PIO4;  /**< I/O space timing register 4 */
    const uint32 RESERVED6[20];  /**< Reserved */
    __IO uint32 BWTR1; /**< SRAM/NOR-Flash write timing register 1 */
    const uint32 RESERVED7;     /**< Reserved */
    __IO uint32 BWTR2; /**< SRAM/NOR-Flash write timing register 2 */
    const uint32 RESERVED8;     /**< Reserved */
    __IO uint32 BWTR3; /**< SRAM/NOR-Flash write timing register 3 */
    const uint32 RESERVED9;     /**< Reserved */
    __IO uint32 BWTR4; /**< SRAM/NOR-Flash write timing register 4 */
} __attribute__((packed)) fsmc_reg_map;

#define __FSMCB                         0xA0000000

/** FSMC register map base pointer */
#define FSMC_BASE                       ((struct fsmc_reg_map*)__FSMCB)

/** FSMC NOR/PSRAM register map type */
typedef struct fsmc_nor_psram_reg_map {
    __IO uint32 BCR;            /**< Chip-select control register */
    __IO uint32 BTR;            /**< Chip-select timing register */
    const uint32 RESERVED[63];  /**< Reserved */
    __IO uint32 BWTR;           /**< Write timing register */
} fsmc_nor_psram_reg_map;

/** FSMC NOR/PSRAM base pointer 1 */
#define FSMC_NOR_PSRAM1_BASE ((struct fsmc_nor_psram_reg_map*)__FSMCB)

/** FSMC NOR/PSRAM base pointer 2 */
#define FSMC_NOR_PSRAM2_BASE ((struct fsmc_nor_psram_reg_map*)(__FSMCB + 0x8))

/** FSMC NOR/PSRAM base pointer 3 */
#define FSMC_NOR_PSRAM3_BASE ((struct fsmc_nor_psram_reg_map*)(__FSMCB + 0x10))

/** FSMC NOR/PSRAM base pointer 4 */
#define FSMC_NOR_PSRAM4_BASE ((struct fsmc_nor_psram_reg_map*)(__FSMCB + 0x18))

/*
 * Register bit definitions
 */

/* NOR/PSRAM chip-select control registers */

#define FSMC_BCR_CBURSTRW_BIT           19
#define FSMC_BCR_ASYNCWAIT_BIT          15
#define FSMC_BCR_EXTMOD_BIT             14
#define FSMC_BCR_WAITEN_BIT             13
#define FSMC_BCR_WREN_BIT               12
#define FSMC_BCR_WAITCFG_BIT            11
#define FSMC_BCR_WRAPMOD_BIT            10
#define FSMC_BCR_WAITPOL_BIT            9
#define FSMC_BCR_BURSTEN_BIT            8
#define FSMC_BCR_FACCEN_BIT             6
#define FSMC_BCR_MUXEN_BIT              1
#define FSMC_BCR_MBKEN_BIT              0

#define FSMC_BCR_CBURSTRW               BIT(FSMC_BCR_CBURSTRW_BIT)
#define FSMC_BCR_ASYNCWAIT              BIT(FSMC_BCR_ASYNCWAIT_BIT)
#define FSMC_BCR_EXTMOD                 BIT(FSMC_BCR_EXTMOD_BIT)
#define FSMC_BCR_WAITEN                 BIT(FSMC_BCR_WAITEN_BIT)
#define FSMC_BCR_WREN                   BIT(FSMC_BCR_WREN_BIT)
#define FSMC_BCR_WAITCFG                BIT(FSMC_BCR_WAITCFG_BIT)
#define FSMC_BCR_WRAPMOD                BIT(FSMC_BCR_WRAPMOD_BIT)
#define FSMC_BCR_WAITPOL                BIT(FSMC_BCR_WAITPOL_BIT)
#define FSMC_BCR_BURSTEN                BIT(FSMC_BCR_BURSTEN_BIT)
#define FSMC_BCR_FACCEN                 BIT(FSMC_BCR_FACCEN_BIT)
#define FSMC_BCR_MWID                   (0x3 << 4)
#define FSMC_BCR_MWID_8BITS             (0x0 << 4)
#define FSMC_BCR_MWID_16BITS            (0x1 << 4)
#define FSMC_BCR_MTYP                   (0x3 << 2)
#define FSMC_BCR_MTYP_SRAM              (0x0 << 2)
#define FSMC_BCR_MTYP_PSRAM             (0x1 << 2)
#define FSMC_BCR_MTYP_NOR_FLASH         (0x2 << 2)
#define FSMC_BCR_MUXEN                  BIT(FSMC_BCR_MUXEN_BIT)
#define FSMC_BCR_MBKEN                  BIT(FSMC_BCR_MBKEN_BIT)

/* SRAM/NOR-Flash chip-select timing registers */

#define FSMC_BTR_ACCMOD                 (0x3 << 28)
#define FSMC_BTR_ACCMOD_A               (0x0 << 28)
#define FSMC_BTR_ACCMOD_B               (0x1 << 28)
#define FSMC_BTR_ACCMOD_C               (0x2 << 28)
#define FSMC_BTR_ACCMOD_D               (0x3 << 28)
#define FSMC_BTR_DATLAT                 (0xF << 24)
#define FSMC_BTR_CLKDIV                 (0xF << 20)
#define FSMC_BTR_BUSTURN                (0xF << 16)
#define FSMC_BTR_DATAST                 (0xFF << 8)
#define FSMC_BTR_ADDHLD                 (0xF << 4)
#define FSMC_BTR_ADDSET                 0xF

#define FSMC_BTR_DATLAT_(x)		((x<<24)&FSMC_BTR_DATLAT)
#define FSMC_BTR_CLKDIV_(x)		((x<<20)&FSMC_BTR_CLKDIV)
#define FSMC_BTR_BUSTURN_(x)	((x<<16)&FSMC_BTR_BUSTURN)
#define FSMC_BTR_DATAST_(x)		((x<<8)&FSMC_BTR_DATAST)
#define FSMC_BTR_ADDHLD_(x)		((x<<4)&FSMC_BTR_ADDHLD)
#define FSMC_BTR_ADDSET_(x)		((x<<0)&FSMC_BTR_ADDSET)

/* SRAM/NOR-Flash write timing registers */

#define FSMC_BWTR_ACCMOD                 (0x3 << 28)
#define FSMC_BWTR_ACCMOD_A               (0x0 << 28)
#define FSMC_BWTR_ACCMOD_B               (0x1 << 28)
#define FSMC_BWTR_ACCMOD_C               (0x2 << 28)
#define FSMC_BWTR_ACCMOD_D               (0x3 << 28)
#define FSMC_BWTR_DATLAT                 (0xF << 24)
#define FSMC_BWTR_CLKDIV                 (0xF << 20)
#define FSMC_BWTR_DATAST                 (0xFF << 8)
#define FSMC_BWTR_ADDHLD                 (0xF << 4)
#define FSMC_BWTR_ADDSET                 0xF

/* NAND Flash/PC Card controller registers */

#define FSMC_PCR_ECCEN_BIT               6
#define FSMC_PCR_PTYP_BIT                3
#define FSMC_PCR_PBKEN_BIT               2
#define FSMC_PCR_PWAITEN_BIT             1

#define FSMC_PCR_ECCPS                   (0x7 << 17)
#define FSMC_PCR_ECCPS_256B              (0x0 << 17)
#define FSMC_PCR_ECCPS_512B              (0x1 << 17)
#define FSMC_PCR_ECCPS_1024B             (0x2 << 17)
#define FSMC_PCR_ECCPS_2048B             (0x3 << 17)
#define FSMC_PCR_ECCPS_4096B             (0x4 << 17)
#define FSMC_PCR_ECCPS_8192B             (0x5 << 17)
#define FSMC_PCR_TAR                     (0xF << 13)
#define FSMC_PCR_TCLR                    (0xF << 9)
#define FSMC_PCR_ECCEN                   BIT(FSMC_PCR_ECCEN_BIT)
#define FSMC_PCR_PWID                    (0x3 << 4)
#define FSMC_PCR_PWID_8BITS              (0x0 << 4)
#define FSMC_PCR_PWID_16BITS             (0x1 << 4)
#define FSMC_PCR_PTYP                    BIT(FSMC_PCR_PTYP_BIT)
#define FSMC_PCR_PTYP_PC_CF_PCMCIA       (0x0 << FSMC_PCR_PTYP_BIT)
#define FSMC_PCR_PTYP_NAND               (0x1 << FSMC_PCR_PTYP_BIT)
#define FSMC_PCR_PBKEN                   BIT(FSMC_PCR_PBKEN_BIT)
#define FSMC_PCR_PWAITEN                 BIT(FSMC_PCR_PWAITEN_BIT)

/* FIFO status and interrupt registers */

#define FSMC_SR_FEMPT_BIT                6
#define FSMC_SR_IFEN_BIT                 5
#define FSMC_SR_ILEN_BIT                 4
#define FSMC_SR_IREN_BIT                 3
#define FSMC_SR_IFS_BIT                  2
#define FSMC_SR_ILS_BIT                  1
#define FSMC_SR_IRS_BIT                  0

#define FSMC_SR_FEMPT                    BIT(FSMC_SR_FEMPT_BIT)
#define FSMC_SR_IFEN                     BIT(FSMC_SR_IFEN_BIT)
#define FSMC_SR_ILEN                     BIT(FSMC_SR_ILEN_BIT)
#define FSMC_SR_IREN                     BIT(FSMC_SR_IREN_BIT)
#define FSMC_SR_IFS                      BIT(FSMC_SR_IFS_BIT)
#define FSMC_SR_ILS                      BIT(FSMC_SR_ILS_BIT)
#define FSMC_SR_IRS                      BIT(FSMC_SR_IRS_BIT)

/* Common memory space timing registers */

#define FSMC_PMEM_MEMHIZ                 (0xFF << 24)
#define FSMC_PMEM_MEMHOLD                (0xFF << 16)
#define FSMC_PMEM_MEMWAIT                (0xFF << 8)
#define FSMC_PMEM_MEMSET                 0xFF

/* Attribute memory space timing registers */

#define FSMC_PATT_ATTHIZ                 (0xFF << 24)
#define FSMC_PATT_ATTHOLD                (0xFF << 16)
#define FSMC_PATT_ATTWAIT                (0xFF << 8)
#define FSMC_PATT_ATTSET                 0xFF

/* I/O space timing register 4 */

#define FSMC_PIO_IOHIZ                  (0xFF << 24)
#define FSMC_PIO_IOHOLD                 (0xFF << 16)
#define FSMC_PIO_IOWAIT                 (0xFF << 8)
#define FSMC_PIO_IOSET                  0xFF

/*
 * Memory bank boundary addresses
 */

/** Pointer to base address of FSMC memory bank 1 (split into 4
 * regions, each supporting 1 NOR Flash, SRAM, or PSRAM chip)  */
#define FSMC_BANK1                      ((void*)0x60000000)

/** Pointer to base address of FSMC memory bank 1, region 1 (for NOR/PSRAM) */
#define FSMC_NOR_PSRAM_REGION1          FSMC_BANK1

/** Pointer to base address of FSMC memory bank 1, region 2 (for NOR/PSRAM) */
#define FSMC_NOR_PSRAM_REGION2          ((void*)0x64000000)

/** Pointer to base address of FSMC memory bank 1, region 3 (for NOR/PSRAM) */
#define FSMC_NOR_PSRAM_REGION3          ((void*)0x68000000)

/** Pointer to base address of FSMC memory bank 1, region 4 (for NOR/PSRAM) */
#define FSMC_NOR_PSRAM_REGION4          ((void*)0x6C000000)

/** Pointer to base address of FSMC memory bank 2 (for NAND Flash) */
#define FSMC_BANK2                      ((void*)0x70000000)

/** Pointer to base address of FSMC memory bank 3 (for NAND Flash) */
#define FSMC_BANK3                      ((void*)0x80000000)

/** Pointer to base address of FSMC memory bank 4 (for PC card devices */
#define FSMC_BANK4                      ((void*)0x90000000)

/*
 * SRAM/NOR Flash routines
 */
extern volatile uint16_t * fsmcData;
extern volatile uint16_t * fsmcCommand;

void fsmc_lcd_init(void);

static inline void fsmc_nor_psram_set_BCR(fsmc_nor_psram_reg_map *regs, uint32_t bcr) {
    regs->BCR = bcr;
}

static inline void fsmc_nor_psram_set_BTR(fsmc_nor_psram_reg_map *regs, uint32_t btr) {
    regs->BTR = btr;
}

static inline void fsmc_nor_psram_bank_enable(fsmc_nor_psram_reg_map *regs) {
    regs->BCR |= FSMC_BCR_MBKEN;
}

static inline void fsmc_nor_psram_bank_disable(fsmc_nor_psram_reg_map *regs) {
    regs->BCR ^= FSMC_BCR_MBKEN;
}

/**
 * Set the DATAST bits in the given NOR/PSRAM register map's
 * chip-select timing register (FSMC_BTR).
 *
 * @param regs   NOR Flash/PSRAM register map whose chip-select timing
 *               register to set.
 * @param datast Value to use for DATAST bits.
 */
static inline void fsmc_nor_psram_set_datast(fsmc_nor_psram_reg_map *regs,
                                             uint8 datast) {
    regs->BTR &= ~FSMC_BTR_DATAST;
    regs->BTR |= datast << 8;
}

/**
 * Set the ADDHLD bits in the given NOR/PSRAM register map's chip
 * select timing register (FSMC_BTRx).
 *
 * @param regs   NOR Flash/PSRAM register map whose chip-select timing
 *               register to set.
 * @param addset Value to use for ADDSET bits.
 */
static inline void fsmc_nor_psram_set_addset(fsmc_nor_psram_reg_map *regs,
                                             uint8 addset) {
    regs->BTR &= ~FSMC_BTR_ADDSET;
    regs->BTR |= addset & 0xF;
}

#endif /* STM32_HIGH_DENSITY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
