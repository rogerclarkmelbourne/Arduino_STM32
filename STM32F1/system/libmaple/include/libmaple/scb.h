/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2011-2012 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The  above copyright  notice and  this permission  notice  shall be
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
 * @file libmaple/include/libmaple/scb.h
 * @brief System control block header
 */

/*
 * FIXME: STM32F2?
 */

#ifndef _LIBMAPLE_SCB_H_
#define _LIBMAPLE_SCB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libmaple/libmaple_types.h>

/*
 * Register map and base pointer
 */

/** System control block register map type */
typedef struct scb_reg_map {
    __IO uint32 CPUID;   /**< CPU ID Base Register */
    __IO uint32 ICSR;    /**< Interrupt Control State Register */
    __IO uint32 VTOR;    /**< Vector Table Offset Register */
    __IO uint32 AIRCR;   /**< Application Interrupt / Reset Control Register */
    __IO uint32 SCR;     /**< System Control Register */
    __IO uint32 CCR;     /**< Configuration and Control Register */
    __IO uint8  SHP[12]; /**< System Handler Priority Registers
                              (4-7, 8-11, 12-15) */
    __IO uint32 SHCSR;   /**< System Handler Control and State Register */
    __IO uint32 CFSR;    /**< Configurable Fault Status Register */
    __IO uint32 HFSR;    /**< Hard Fault Status Register */
    /* DFSR is not documented by ST in PM0056 (as of Revision 4), but
     * there's a 4 byte hole in the SCB register map docs right where
     * it belongs.  Since it's specified as "always implemented" in
     * the ARM v7-M ARM, I'm assuming its absence is a bug in the ST
     * doc, but I haven't proven it. [mbolivar] */
    __IO uint32 DFSR;    /**< Debug Fault Status Register */
    __IO uint32 MMFAR;   /**< Mem Manage Address Register */
    __IO uint32 BFAR;    /**< Bus Fault Address Register */
#if 0
    /* The following registers are implementation-defined according to
     * ARM v7-M, and I can't find evidence of their existence in ST's
     * docs.  I'm removing them.  Feel free to yell at me if they do
     * exist. [mbolivar]
     */
    __IO uint32 AFSR;    /**< Auxiliary Fault Status Register */
    __IO uint32 PFR[2];  /**< Processor Feature Register */
    __IO uint32 DFR;     /**< Debug Feature Register */
    __IO uint32 AFR;     /**< Auxiliary Feature Register */
    __IO uint32 MMFR[4]; /**< Memory Model Feature Register */
    __IO uint32 ISAR[5]; /**< ISA Feature Register */
#endif
} scb_reg_map;

/** System control block register map base pointer */
#define SCB_BASE                        ((struct scb_reg_map*)0xE000ED00)

/*
 * Register bit definitions
 */

/* No SCB_REG_FIELD_BIT macros as the relevant addresses are not in a
 * bit-band region. */

/* CPUID base register (SCB_CPUID) */

#define SCB_CPUID_IMPLEMENTER           (0xFF << 24)
#define SCB_CPUID_VARIANT               (0xF << 20)
#define SCB_CPUID_CONSTANT              (0xF << 16)
#define SCB_CPUID_PARTNO                (0xFFF << 4)
#define SCB_CPUID_REVISION              0xF

/* Interrupt control state register (SCB_ICSR) */

#define SCB_ICSR_NMIPENDSET             (1U << 31)
#define SCB_ICSR_PENDSVSET              (1U << 28)
#define SCB_ICSR_PENDSVCLR              (1U << 27)
#define SCB_ICSR_PENDSTSET              (1U << 26)
#define SCB_ICSR_PENDSTCLR              (1U << 25)
#define SCB_ICSR_ISRPENDING             (1U << 22)
#define SCB_ICSR_VECTPENDING            (0x3FF << 12)
#define SCB_ICSR_RETOBASE               (1U << 11)
#define SCB_ICSR_VECTACTIVE             0xFF

/* Vector table offset register (SCB_VTOR) */

#define SCB_VTOR_TBLOFF                 (0x1FFFFF << 9)

/* Application interrupt and reset control register (SCB_AIRCR) */

#define SCB_AIRCR_VECTKEYSTAT           (0x5FA << 16)
#define SCB_AIRCR_VECTKEY               (0x5FA << 16)
#define SCB_AIRCR_ENDIANNESS            (1U << 15)
#define SCB_AIRCR_PRIGROUP              (0x7 << 8)
#define SCB_AIRCR_SYSRESETREQ           (1U << 2)
#define SCB_AIRCR_VECTCLRACTIVE         (1U << 1)
#define SCB_AIRCR_VECTRESET             (1U << 0)

/* System control register (SCB_SCR) */

#define SCB_SCR_SEVONPEND               (1U << 4)
#define SCB_SCR_SLEEPDEEP               (1U << 2)
#define SCB_SCR_SLEEPONEXIT             (1U << 1)

/* Configuration and Control Register (SCB_CCR) */

#define SCB_CCR_STKALIGN                (1U << 9)
#define SCB_CCR_BFHFNMIGN               (1U << 8)
#define SCB_CCR_DIV_0_TRP               (1U << 4)
#define SCB_CCR_UNALIGN_TRP             (1U << 3)
#define SCB_CCR_USERSETMPEND            (1U << 1)
#define SCB_CCR_NONBASETHRDENA          (1U << 0)

/* System handler priority registers (SCB_SHPRx) */

#define SCB_SHPR1_PRI6                  (0xFF << 16)
#define SCB_SHPR1_PRI5                  (0xFF << 8)
#define SCB_SHPR1_PRI4                  0xFF

#define SCB_SHPR2_PRI11                 (0xFF << 24)

#define SCB_SHPR3_PRI15                 (0xFF << 24)
#define SCB_SHPR3_PRI14                 (0xFF << 16)

/* System Handler Control and state register (SCB_SHCSR) */

#define SCB_SHCSR_USGFAULTENA           (1U << 18)
#define SCB_SHCSR_BUSFAULTENA           (1U << 17)
#define SCB_SHCSR_MEMFAULTENA           (1U << 16)
#define SCB_SHCSR_SVCALLPENDED          (1U << 15)
#define SCB_SHCSR_BUSFAULTPENDED        (1U << 14)
#define SCB_SHCSR_MEMFAULTPENDED        (1U << 13)
#define SCB_SHCSR_USGFAULTPENDED        (1U << 12)
#define SCB_SHCSR_SYSTICKACT            (1U << 11)
#define SCB_SHCSR_PENDSVACT             (1U << 10)
#define SCB_SHCSR_MONITORACT            (1U << 8)
#define SCB_SHCSR_SVCALLACT             (1U << 7)
#define SCB_SHCSR_USGFAULTACT           (1U << 3)
#define SCB_SHCSR_BUSFAULTACT           (1U << 1)
#define SCB_SHCSR_MEMFAULTACT           (1U << 0)

/* Configurable fault status register (SCB_CFSR) */

#define SCB_CFSR_DIVBYZERO              (1U << 25)
#define SCB_CFSR_UNALIGNED              (1U << 24)
#define SCB_CFSR_NOCP                   (1U << 19)
#define SCB_CFSR_INVPC                  (1U << 18)
#define SCB_CFSR_INVSTATE               (1U << 17)
#define SCB_CFSR_UNDEFINSTR             (1U << 16)
#define SCB_CFSR_BFARVALID              (1U << 15)
#define SCB_CFSR_STKERR                 (1U << 12)
#define SCB_CFSR_UNSTKERR               (1U << 11)
#define SCB_CFSR_IMPRECISERR            (1U << 10)
#define SCB_CFSR_PRECISERR              (1U << 9)
#define SCB_CFSR_IBUSERR                (1U << 8)
#define SCB_CFSR_MMARVALID              (1U << 7)
#define SCB_CFSR_MSTKERR                (1U << 4)
#define SCB_CFSR_MUNSTKERR              (1U << 3)
#define SCB_CFSR_DACCVIOL               (1U << 1)
#define SCB_CFSR_IACCVIOL               (1U << 0)

/* Hard Fault Status Register (SCB_HFSR) */

#define SCB_HFSR_DEBUG_VT               (1U << 31)
#define SCB_CFSR_FORCED                 (1U << 30)
#define SCB_CFSR_VECTTBL                (1U << 1)

/* Debug Fault Status Register */

/* Not specified by PM0056, but required by ARM.  The bit definitions
 * here are based on the names given in the ARM v7-M ARM. */

#define SCB_DFSR_EXTERNAL               (1U << 4)
#define SCB_DFSR_VCATCH                 (1U << 3)
#define SCB_DFSR_DWTTRAP                (1U << 2)
#define SCB_DFSR_BKPT                   (1U << 1)
#define SCB_DFSR_HALTED                 (1U << 0)

#ifdef __cplusplus
}
#endif

#endif
