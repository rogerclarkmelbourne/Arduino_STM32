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
 * @file exti.h
 * @brief External interrupt control prototypes and defines
 */

/* See notes/exti.txt for more info */

#include "libmaple.h"
#include "gpio.h"

#ifndef _EXTI_H_
#define _EXTI_H_

#ifdef __cplusplus
extern "C"{
#endif

/** EXTI register map type */
typedef struct exti_reg_map {
    __IO uint32 IMR;   /**< Interrupt mask register */
    __IO uint32 EMR;   /**< Event mask register */
    __IO uint32 RTSR;  /**< Rising trigger selection register */
    __IO uint32 FTSR;  /**< Falling trigger selection register */
    __IO uint32 SWIER; /**< Software interrupt event register */
    __IO uint32 PR;    /**< Pending register */
} exti_reg_map;

/** EXTI register map base pointer */
#define EXTI_BASE                       ((struct exti_reg_map*)0x40013C00)

/** External interrupt trigger mode */
typedef enum exti_trigger_mode {
    EXTI_RISING,         /**< Trigger on the rising edge */
    EXTI_FALLING,        /**< Trigger on the falling edge */
    EXTI_RISING_FALLING  /**< Trigger on both the rising and falling edges */
} exti_trigger_mode;

void exti_attach_interrupt(afio_exti_num num,
                           afio_exti_port port,
                           voidFuncPtr handler,
                           exti_trigger_mode mode);
void exti_detach_interrupt(afio_exti_num num);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
