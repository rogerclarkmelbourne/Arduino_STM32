/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/include/libmaple/i2c.h
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief Inter-Integrated Circuit (I2C) peripheral support
 *
 * Currently master-only. Usage notes:
 *
 * - Enable an I2C device with i2c_master_enable().
 * - Initialize an array of struct i2c_msg to suit the bus
 *   transactions (reads/writes) you wish to perform.
 * - Call i2c_master_xfer() to do the work.
 */

#ifndef _LIBMAPLE_I2C_H_
#define _LIBMAPLE_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Series header must provide:
 *
 * - uint32 _i2c_bus_clk(i2c_dev*): Clock frequency of dev's bus, in
 *   MHz. (This is for internal use only).
 *
 * - (optional) _I2C_HAVE_IRQ_FIXUP: Leave undefined, or define to 1.
 *   This is for internal use only. It's a hack to work around a
 *   silicon bug related to I2C IRQ pre-emption on some targets. If 1,
 *   the series header must also declare and implement a routine with
 *   this signature (it may also be provided as a macro):
 *
 *       void _i2c_irq_priority_fixup(i2c_dev*)
 *
 *   This will be called by i2c_enable_irq() before actually enabling
 *   I2C interrupts.
 *
 * - Reg. map base pointers, device pointer declarations.
 */

#include <series/i2c.h>
#include <libmaple/i2c_common.h>

#include <libmaple/libmaple_types.h>
#include <libmaple/rcc.h>
#include <libmaple/nvic.h>
#include <libmaple/gpio.h>

/**
 * @brief I2C message type
 */
typedef struct i2c_msg {
    uint16 addr;                /**< Address */

#define I2C_MSG_READ            0x1
#define I2C_MSG_10BIT_ADDR      0x2
    /**
     * Bitwise OR of:
     * - I2C_MSG_READ (write is default)
     * - I2C_MSG_10BIT_ADDR (7-bit is default) */
    uint16 flags;

    uint16 length;              /**< Message length */
    uint16 xferred;             /**< Messages transferred */
    uint8 *data;                /**< Data */
} i2c_msg;

/* I2C enable options */
#define I2C_FAST_MODE           0x1           // 400 khz
#define I2C_DUTY_16_9           0x2           // 16/9 duty ratio
/* Flag 0x4 is reserved; DO NOT USE. */
#define I2C_BUS_RESET           0x8           // Perform a bus reset

/* I2C error flags */
#define I2C_ERROR_PROTOCOL      (-1)
#define I2C_ERROR_TIMEOUT       (-2)

/* Main I2C API */

/**
 * @brief Disable an I2C device
 *
 * This function disables the corresponding peripheral and marks dev's
 * state as I2C_STATE_DISABLED.
 *
 * @param dev Device to disable.
 */
static inline void i2c_disable(i2c_dev *dev) {
    dev->regs->CR1 &= ~I2C_CR1_PE;
    dev->state = I2C_STATE_DISABLED;
}

/**
 * @brief Wait for an I2C event, or time out in case of error.
 * @param dev I2C device
 * @param state I2C_state state to wait for
 * @param timeout Timeout, in milliseconds
 * @return 0 if target state is reached, a negative value on error.
 */
extern int32 wait_for_state_change(i2c_dev *dev,
																	i2c_state state,
																	uint32 timeout);

/**
 * @brief Reset an I2C bus.
 *
 * Reset is accomplished by clocking out pulses until any hung slaves
 * release SDA and SCL, then generating a START condition, then a STOP
 * condition.
 *
 * @param dev I2C device
 */
extern void i2c_bus_reset(const i2c_dev *dev);

/**
 * @brief Initialize an I2C device as bus master
 * @param dev Device to enable
 * @param flags Bitwise or of the following I2C options:
 *              I2C_FAST_MODE: 400 khz operation,
 *              I2C_DUTY_16_9: 16/9 Tlow/Thigh duty cycle (only applicable for
 *                             fast mode),
 *              I2C_BUS_RESET: Reset the bus and clock out any hung slaves on
 *                             initialization,
 *              I2C_10BIT_ADDRESSING: Enable 10-bit addressing,
 *              I2C_REMAP: (deprecated, STM32F1 only) Remap I2C1 to SCL/PB8
 *                         SDA/PB9.
 */
extern void i2c_master_enable(i2c_dev *dev, uint32 flags);

/**
 * @brief Process an i2c transaction.
 *
 * Transactions are composed of one or more i2c_msg's, and may be read
 * or write tranfers.  Multiple i2c_msg's will generate a repeated
 * start in between messages.
 *
 * @param dev I2C device
 * @param msgs Messages to send/receive
 * @param num Number of messages to send/receive
 * @param timeout Bus idle timeout in milliseconds before aborting the
 *                transfer.  0 denotes no timeout.
 * @return 0 on success,
 *         I2C_ERROR_PROTOCOL if there was a protocol error,
 *         I2C_ERROR_TIMEOUT if the transfer timed out.
 */
extern int32 i2c_master_xfer(i2c_dev *dev,
                      i2c_msg *msgs,
                      uint16 num,
                      uint32 timeout);

/**
 * @brief Fill data register with slave address
 * @param dev I2C device
 * @param addr Slave address
 * @param rw Read/write bit
 */
extern void i2c_send_slave_addr(i2c_dev *dev, uint32 addr, uint32 rw);

/* Start/stop conditions */

/**
 * @brief Generate a start condition on the bus.
 * @param dev I2C device
 */
extern void i2c_start_condition(i2c_dev *dev);

/**
 * @brief Generate a stop condition on the bus
 * @param dev I2C device
 */
extern void i2c_stop_condition(i2c_dev *dev);

/* IRQ enable/disable */

#ifndef _I2C_HAVE_IRQ_FIXUP
/* The series header provides this if _I2C_HAVE_IRQ_FIXUP is defined,
 * but we need it either way. */
#define _i2c_irq_priority_fixup(dev) ((void)0)
#endif

/**
 * @brief Enable one or more I2C interrupts
 * @param dev I2C device
 * @param irqs Bitwise or of:
 *             I2C_IRQ_ERROR (error interrupt),
 *             I2C_IRQ_EVENT (event interrupt), and
 *             I2C_IRQ_BUFFER (buffer interrupt).
 */
extern void i2c_enable_irq(i2c_dev *dev, uint32 irqs);

/**
 * @brief Disable one or more I2C interrupts
 * @param dev I2C device
 * @param irqs Bitwise or of:
 *             I2C_IRQ_ERROR (error interrupt),
 *             I2C_IRQ_EVENT (event interrupt), and
 *             I2C_IRQ_BUFFER (buffer interrupt).
 */
extern void i2c_disable_irq(i2c_dev *dev, uint32 irqs);

/* ACK/NACK */

/**
 * @brief Enable I2C acknowledgment
 * @param dev I2C device
 */
extern void i2c_enable_ack(i2c_dev *dev);

/**
 * @brief Disable I2C acknowledgment
 * @param dev I2C device
 */
extern void i2c_disable_ack(i2c_dev *dev);

/* GPIO control */

/**
 * @brief Configure device GPIOs.
 *
 * Configure GPIO bits dev->sda_pin and dev->scl_pin on GPIO device
 * dev->gpio_port for use with I2C device dev.
 *
 * @param dev I2C Device
 * @see i2c_release_gpios()
 */
extern void i2c_config_gpios(const i2c_dev *dev);

/**
 * @brief Release GPIOs controlling an I2C bus
 *
 * Releases the I2C bus controlled by dev as master, and disconnects
 * GPIO bits dev->sda_pin and dev->scl_pin on GPIO device
 * dev->gpio_port from I2C device dev.
 *
 * @param dev I2C device
 * @see i2c_config_gpios()
 */
extern void i2c_master_release_bus(const i2c_dev *dev);

/* Miscellaneous low-level routines */

/**
 * @brief Initialize an I2C device and reset its registers to their
 *        default values.
 * @param dev Device to initialize.
 */
void i2c_init(i2c_dev *dev);

/**
 * @brief Turn on an I2C peripheral
 * @param dev Device to enable
 */
static inline void i2c_peripheral_enable(i2c_dev *dev) {
    dev->regs->CR1 |= I2C_CR1_PE;
}

/**
 * @brief Turn off an I2C peripheral
 * @param dev Device to turn off
 */
static inline void i2c_peripheral_disable(i2c_dev *dev) {
    dev->regs->CR1 &= ~I2C_CR1_PE;
}

/**
 * @brief Fill transmit register
 * @param dev I2C device
 * @param byte Byte to write
 */
extern void i2c_write(i2c_dev *dev, uint8 byte);

#ifdef __cplusplus
}
#endif

#endif
