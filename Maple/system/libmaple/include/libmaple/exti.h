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
 * @file libmaple/include/libmaple/exti.h
 * @brief External interrupt control
 */

/* See notes/exti.txt for more info */

#ifndef _LIBMAPLE_EXTI_H_
#define _LIBMAPLE_EXTI_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <series/exti.h>        /* provides EXTI_BASE */
#include <libmaple/libmaple_types.h>

/*
 * Register map and base pointer.
 */

/** EXTI register map type */
typedef struct exti_reg_map {
    __io uint32 IMR;   /**< Interrupt mask register */
    __io uint32 EMR;   /**< Event mask register */
    __io uint32 RTSR;  /**< Rising trigger selection register */
    __io uint32 FTSR;  /**< Falling trigger selection register */
    __io uint32 SWIER; /**< Software interrupt event register */
    __io uint32 PR;    /**< Pending register */
} exti_reg_map;

/*
 * Types: exti_num, exti_cfg, exti_trigger_mode.
 *
 * A combination of these three specifies an external interrupt
 * configuration (see exti_attach_interrupt()).
 */

/** EXTI line. */
typedef enum exti_num {
    EXTI0,    /**< EXTI line 0 */
    EXTI1,    /**< EXTI line 1 */
    EXTI2,    /**< EXTI line 2 */
    EXTI3,    /**< EXTI line 3 */
    EXTI4,    /**< EXTI line 4 */
    EXTI5,    /**< EXTI line 5 */
    EXTI6,    /**< EXTI line 6 */
    EXTI7,    /**< EXTI line 7 */
    EXTI8,    /**< EXTI line 8 */
    EXTI9,    /**< EXTI line 9 */
    EXTI10,   /**< EXTI line 10 */
    EXTI11,   /**< EXTI line 11 */
    EXTI12,   /**< EXTI line 12 */
    EXTI13,   /**< EXTI line 13 */
    EXTI14,   /**< EXTI line 14 */
    EXTI15,   /**< EXTI line 15 */
} exti_num;

/**
 * @brief EXTI port configuration
 *
 * These specify which GPIO port an external interrupt line should be
 * connected to.
 */
typedef enum exti_cfg {
    EXTI_PA,                    /**< Use PAx pin */
    EXTI_PB,                    /**< Use PBx pin */
    EXTI_PC,                    /**< Use PCx pin */
    EXTI_PD,                    /**< Use PDx pin */
    EXTI_PE,                    /**< Use PEx pin */
    EXTI_PF,                    /**< Use PFx pin */
    EXTI_PG,                    /**< Use PGx pin */
    EXTI_PH,                    /**< Use PHx pin */
    EXTI_PI,                    /**< Use PIx pin */
} exti_cfg;

/** External interrupt trigger mode */
typedef enum exti_trigger_mode {
    EXTI_RISING,         /**< Trigger on the rising edge */
    EXTI_FALLING,        /**< Trigger on the falling edge */
    EXTI_RISING_FALLING  /**< Trigger on both the rising and falling edges */
} exti_trigger_mode;

/*
 * Routines
 */

void exti_attach_interrupt(exti_num num,
                           exti_cfg port,
                           voidFuncPtr handler,
                           exti_trigger_mode mode);
void exti_attach_callback(exti_num num,
                          exti_cfg port,
                          voidArgumentFuncPtr handler,
                          void *arg,
                          exti_trigger_mode mode);
void exti_detach_interrupt(exti_num num);

/**
 * @brief Set the GPIO port for an EXTI line.
 *
 * This is a low-level routine that most users will not
 * need. exti_attach_interrupt() handles calling this function
 * appropriately.
 *
 * @param num EXTI line
 * @param port EXTI configuration for GPIO port to connect to num.
 * @see exti_num
 * @see exti_cfg
 */
extern void exti_select(exti_num num, exti_cfg port);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
