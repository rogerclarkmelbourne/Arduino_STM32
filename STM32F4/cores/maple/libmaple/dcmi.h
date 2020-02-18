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
 * @file dcmi.h
 * @brief Direct camera memory interface support.
 */

#ifndef _DCMI_H_
#define _DCMI_H_


#include "util.h"
#include "libmaple_types.h"
#include "dmaF4.h"


#ifdef __cplusplus
extern "C"{
#endif


/*
 * Register maps and devices
 */

/** FSMC register map type */
typedef struct dcmi_reg_map {
    __IO uint32 CR;     // Control Register
    __IO uint32 SR;     // Status Register
    __IO uint32 RIS;    // Raw Interrupt Status register
    __IO uint32 IER;    // Interrupt Enable Register
    __IO uint32 MIS;    // Masked Interrupt Status Register
    __IO uint32 ICR;    // Interrupt Clear Register
    __IO uint32 ESCR;   // Embedded Synchronization Code Register
    __IO uint32 ESUR;   // Embedded Synchronization Unmask Register
    __IO uint32 CWSTRT; // Crop Window Start
    __IO uint32 CWSIZE; // Crop Window Size
    __IO uint32 DR;     // Data Register
} dcmi_reg_map;

// DCMI register map base pointer
#define DCMI_BASE                (struct dcmi_reg_map *)0x50050000

typedef struct dcmi_dev {
    dcmi_reg_map * regs;     // Register map
    rcc_clk_id clk_id;       // Clock ID
    voidFuncPtr * handler_p; // IRQ handler pointer
} dcmi_dev;

// DCMI device
extern const dcmi_dev dcmi;
#define DCMI (&dcmi)


//
// Register bit definitions
//

// Control Register
#define DCMI_CR_ENABLE           BIT(14)

#define DCMI_CR_EDM             (0x3 << 10)
#define DCMI_DATA_WIDTH_8BIT    (0x0 << 10)
#define DCMI_DATA_WIDTH_10BIT   (0x1 << 10)
#define DCMI_DATA_WIDTH_12BIT   (0x2 << 10)
#define DCMI_DATA_WIDTH_14BIT   (0x3 << 10)

#define DCMI_CR_FCRC            (0x3 << 8)
#define DCMI_FRAMES_EACH        (0x0 << 8)
#define DCMI_FRAMES_ONE_OF_2    (0x1 << 8)
#define DCMI_FRAMES_ONE_OF_4    (0x2 << 8)

#define DCMI_CR_VSPOL            BIT(7)
#define DCMI_VSPOL_LOW          (0 << 7)
#define DCMI_VSPOL_HIGH         (1 << 7)

#define DCMI_CR_HSPOL            BIT(6)
#define DCMI_HSPOL_LOW          (0 << 6)
#define DCMI_HSPOL_HIGH         (1 << 6)

#define DCMI_CR_PCKPOL           BIT(5)
#define DCMI_PCKPOL_FALLING     (0 << 5)
#define DCMI_PCKPOL_RISING      (1 << 5)

#define DCMI_CR_ESS              BIT(4)
#define DCMI_SYNC_HW            (0 << 4)
#define DCMI_SYNC_EMBEDDED      (1 << 4)

#define DCMI_CR_JPEG             BIT(3)
#define DCMI_JPEG_DISABLE       (0 << 3)
#define DCMI_JPEG_ENABLE        (1 << 3)

#define DCMI_CR_CROP             BIT(2)
#define DCMI_CROP_DISABLE       (0 << 2)
#define DCMI_CROP_ENABLE        (1 << 2)

#define DCMI_CR_CM               BIT(1)
#define DCMI_MODE_CONTINUOUS    (0 << 1)
#define DCMI_MODE_SINGLE        (1 << 1)

#define DCMI_CR_CAPTURE          BIT(0)
#define DCMI_CAPTURE_DISABLE    (0 << 0)
#define DCMI_CAPTURE_ENABLE     (1 << 0)

// Status Register             
#define DCMI_SR_FNE              BIT(2)
#define DCMI_SR_VSYNC            BIT(1)
#define DCMI_SR_HSYNC            BIT(0)

// Raw Interrupt Status Register
// Interrupt Enable Register
// Masked Interrupt Status Register
// Interrupt Clear Register
#define DCMI_FLAG_LINE           BIT(4)
#define DCMI_FLAG_VSYNC          BIT(3)
#define DCMI_FLAG_ERR            BIT(2)
#define DCMI_FLAG_OVR            BIT(1)
#define DCMI_FLAG_FRAME          BIT(0)

// Embedded Synchronization Code Register
#define DCMI_ESCR_FEC           (0xFF << 24)
#define DCMI_ESCR_LEC           (0xFF << 16)
#define DCMI_ESCR_LSC           (0xFF << 8)
#define DCMI_ESCR_FSC           (0xFF << 0)

// Embedded Synchronization Unmask Register
#define DCMI_ESUR_FEU           (0xFF << 24)
#define DCMI_ESUR_LEU           (0xFF << 16)
#define DCMI_ESUR_LSU           (0xFF << 8)
#define DCMI_ESUR_FSU           (0xFF << 0)

// Crop Window Start
#define DCMI_CWSTRT_VST         (0x1FFF << 16)
#define DCMI_CWSTRT_HOFFCNT     (0x3FFF << 0)

// Crop Window Size
#define DCMI_CWSIZE_VLINE       (0x3FFF << 16)
#define DCMI_CWSIZE_CAPCNT      (0x3FFF << 0)


//
// DCMI routines
//

void dcmi_init(void);
void dcmi_init_gpios(void);
void dcmi_begin(); // collection of previous two functions
void dcmi_set_data_width(uint16 data_width);
void dcmi_set_vspol(uint8 vspol);
void dcmi_set_hspol(uint8 hspol);
void dcmi_set_pckpol(uint8 pckpol);
void dcmi_set_mode(uint8 capture_mode);
void dcmi_set_sync_mode(uint8 ess);
void dcmi_attach_interrupt(voidFuncPtr handler, uint8_t irq_mask);
void dcmi_detach_interrupt(uint8_t irq_mask);
void dcmi_config(uint32_t flags);
void dcmi_dma_config(uint32_t * buf, uint16 bufLen, voidFuncPtr func);
void dcmi_dma_reset(uint16 bufLen);

static inline void dcmi_clear_irq_flags(void) { DCMI->regs->ICR = 0x1F; }

static inline void dcmi_enable(void) { DCMI->regs->CR |= DCMI_CR_ENABLE; }

static inline void dcmi_disable(void) { DCMI->regs->CR &= (~DCMI_CR_ENABLE); }

static inline void dcmi_enable_capture(void) 
{
	dcmi_clear_irq_flags();
    DCMI->regs->CR |= DCMI_CAPTURE_ENABLE;
}

static inline void dcmi_disable_capture(void)
{
    DCMI->regs->CR &= (~DCMI_CAPTURE_ENABLE);
}

static inline uint8 dcmi_dma_get_isr_bits(void)
{
    return dma_get_isr_bits(DMA2, DMA_STREAM1);
}


#ifdef __cplusplus
} /* extern "C" */
#endif


#endif // _DCMI_H_
