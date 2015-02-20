/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs, LLC.
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
 * @file libmaple/include/libmaple/bkp.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief Backup register support (STM32F1 & STM32F3 only).
 */

#ifndef _LIBMAPLE_BKP_H_
#define _LIBMAPLE_BKP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libmaple/libmaple.h>

#include <series/bkp.h>

/** Backup peripheral device type. */
typedef struct bkp_dev {
    bkp_reg_map *regs; /**< Register map */
} bkp_dev;

extern const struct bkp_dev *BKP;

/*
 * this function needs to be implemented for each series separately
 */
extern inline __io uint32* bkp_data_register(uint8 reg);

/**
 * @brief Initialize backup interface.
 *
 * Enables the power and backup interface clocks, and resets the
 * backup device.
 */
void bkp_init(void);

/**
 * Enable write access to the backup registers.  Backup interface must
 * be initialized for subsequent register writes to work.
 * @see bkp_init()
 */
void bkp_enable_writes(void);

/**
 * Disable write access to the backup registers.
 */
void bkp_disable_writes(void);

/**
 * Read a value from given backup data register.
 * @param reg Data register to read, from 1 to BKP_NR_DATA_REGS (10 on
 *            medium-density devices, 42 on high-density devices).
 */
uint16 bkp_read(uint8 reg);

/**
 * @brief Write a value to given data register.
 *
 * Write access to backup registers must be enabled.
 *
 * @param reg Data register to write, from 1 to BKP_NR_DATA_REGS (10
 *            on medium-density devices, 42 on high-density devices).
 * @param val Value to write into the register.
 * @see bkp_enable_writes()
 */
void bkp_write(uint8 reg, uint16 val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
