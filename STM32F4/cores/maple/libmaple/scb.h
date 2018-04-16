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
 * @file scb.h
 * @brief System control block header
 */

#include "libmaple_types.h"

#ifndef _SCB_H_
#define _SCB_H_

/** System control block register map type */
typedef struct scb_reg_map {
    __IO uint32 CPUID;   /**< CPU ID Base Register */
    __IO uint32 ICSR;    /**< Interrupt Control State Register */
    __IO uint32 VTOR;    /**< Vector Table Offset Register */
    __IO uint32 AIRCR;   /**< Application Interrupt / Reset Control Register */
    __IO uint32 SCR;     /**< System Control Register */
    __IO uint32 CCR;     /**< Configuration Control Register */
    __IO uint8  SHP[12]; /**< System Handlers Priority Registers
                              (4-7, 8-11, 12-15) */
    __IO uint32 SHCSR;   /**< System Handler Control and State Register */
    __IO uint32 CFSR;    /**< Configurable Fault Status Register */
    __IO uint32 HFSR;    /**< Hard Fault Status Register */
    __IO uint32 DFSR;    /**< Debug Fault Status Register */
    __IO uint32 MMFAR;   /**< Mem Manage Address Register */
    __IO uint32 BFAR;    /**< Bus Fault Address Register */
    __IO uint32 AFSR;    /**< Auxiliary Fault Status Register */
    __IO uint32 PFR[2];  /**< Processor Feature Register */
    __IO uint32 DFR;     /**< Debug Feature Register */
    __IO uint32 ADR;     /**< Auxiliary Feature Register */
    __IO uint32 MMFR[4]; /**< Memory Model Feature Register */
    __IO uint32 ISAR[5]; /**< ISA Feature Register */
} scb_reg_map;

/** System control block register map base pointer */
#define SCB_BASE                        ((struct scb_reg_map*)0xE000ED00)

#endif

