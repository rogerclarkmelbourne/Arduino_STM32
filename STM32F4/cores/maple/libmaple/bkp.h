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


#ifdef __cplusplus
extern "C" {
#endif

#include "libmaple.h"
#include "pwr.h"
#include "rcc.h"


#define BKP_NR_DATA_REGS 20

/** Backup peripheral register map type. */
/*
* there are 20 backup registers on stm32 f401, f411, f405, f407, f427, f429 devices.
* in addition for the larger devices e.g. f405, f407, f427, f429 there is an additional
* 4KB backup sram. bkp_reg_map mainly maps the 20 backup registers.
*/

typedef struct bkp_reg_map {
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
} bkp_reg_map;

/** Backup peripheral register map base pointer. */
#define BKP                        ((struct bkp_reg_map*)(0x40002800 + 0x50))

// Backup SRAM(4 KB) base address
#define BKPSRAM_BASE 0x40024000UL
//4KB
#define BKPSIZE 4096


/*
 * Register bit definitions
 */

/* Data Registers */

#define BKP_DR_D                        0xFFFFFFFF

/*
 * Convenience functions
 */
/**
 * Enable write access to the backup registers.  Backup interface must
 * be initialized for subsequent register writes to work.
 * @see bkp_init()
 */
__always_inline void bkp_enable_writes(void) {
    *bb_perip(&PWR->CR, PWR_CR_DBP_BIT) = 1;
}

/**
 * Disable write access to the backup registers.
 */
__always_inline void bkp_disable_writes(void) {
    *bb_perip(&PWR->CR, PWR_CR_DBP_BIT) = 0;
}


/**
 * @brief Initialize backup interface.
 *
 * note that bkp_init() merely enables the backup domain clocks.
 * This is not adequate to use backup domain registers and sram.
 *
 * to access backup registers it is necessary to initiate RTC clock
 * e.g.
 * RTClock rt;
 * void setup() {
 * 	   bkp_init();
 *     rt.begin();
 *
 *     // enable writes before writing to bkp registers
 *     // or it will hardfault, freeze
 *     bkp_enable_writes();
 *     bkp_write(1, 100); //write 100 in bkp register 1
 *     bkp_disable_writes();
 *
 *     int32_t regval = bkp_read(1); // read register 1
 * }
 *
 * if you want to access backup SRAM in addition to bkp_init()
 * it is necessary to call bkp_initsram(), e.g:
 *
 * void setup() {
 *     bkp_init();
 *     bkp_initsram(true);
 *     rt.begin();
 *
 *     ...
 * }
 *
 */
__always_inline void bkp_init(void) {
	rcc_clk_enable(RCC_PWR);
    /* Don't call pwr_init(), or you'll reset the device.
	 * We just need the clock.
    pwr_init();
    */
}

/*
 * enable BKPSRAM
 * requires bkp_init() to be called prior
 *
 * @param bkreg
 *   true enable the backup power regulator, runs on VBAT e.g. coin cell
 *   false BKPSRAM is lost if VDD is lost, but preserves across a reset
 */
__always_inline void bkp_initsram(bool bkreg) {

	bkp_enable_writes();

    //enable backup sram
    RCC->AHB1ENR |= RCC_AHB1ENR_BKPSRAMEN;

    if(bkreg)
    	PWR->CSR |= PWR_CSR_BRE;
    else
    	PWR->CSR &= ~PWR_CSR_BRE;

}

/* functions to read/write bkp registers
 * note that prior to bkp_write() it is necessary to call
 * bkp_enable_writes()
 */

uint32 bkp_read(uint8 reg);

void bkp_write(uint8 reg, uint32 val);


/* functions to read write bkp sram
 *
 * note that the offset is indexed by word sized entries
 * zero is the first offset
 * e.g. bkp_sramwrite32(9, data) writes to the 10th 32 bit uint32 field
 *
 * prior to writing bkp_sram it is necessary to call
 * bkp_enable_writes()
 *
 */
uint8_t bkp_sramread8(uint16_t offset);

void bkp_sramwrite8(uint16_t offset, uint8_t data);

uint16_t bkp_sramread16(uint16_t offset);

void bkp_sramwrite16(uint16_t offset, uint16_t data);

uint32_t bkp_sramread32(uint16_t offset);

void bkp_sramwrite32(uint16_t offset, uint32_t data);

/* note this simply returns a pointer to the BKPSRAM + offset
 * BKPSRAM is normally 4KB, this does not check if you were to read beyond that 4KB
 */
inline uint8_t* bkp_sramread(uint16_t offset) {
		return (uint8_t *)(BKPSRAM_BASE + offset);
}

/* copies  data to bkpsram
 * note this truncate data that doesn't fit in BKPSIZE - offset
 * */
void bkp_sramwrite(uint16_t offset, uint8_t *data, uint16_t length);

/*
 * clear bkpsram entirely
 */
void bkpsram_clear();

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
