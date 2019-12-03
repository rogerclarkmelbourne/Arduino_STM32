/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2012 LeafLabs, LLC.
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
 * @file libmaple/i2c.c
 * @author Perry Hung <perry@leaflabs.com>
 * @author Barry Carter <barry.carter@gmail.com>
 * @brief Inter-Integrated Circuit (I2C) support.
 *
 * Master and Slave supported
 * I2C slave support added 2012 by Barry Carter. barry.carter@gmail.com, headfuzz.co.uk
 * 
 * Modified 2019 by Donna Whisnant to merge WireSlave changes with the core to
 * make slave mode work and without having conflicting data type defintions,
 * and rewrote logic to function better.
 * 
 */

#include <libmaple/i2c_common.h>
#include "i2c_private.h"

#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>
#include <libmaple/gpio.h>
#include <libmaple/nvic.h>
#include <libmaple/i2c.h>
#include <libmaple/systick.h>

#include <string.h>

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#ifndef I2C_TIMEOUT_BUSY_FLAG
#define I2C_TIMEOUT_BUSY_FLAG 25U
#endif

/**
 * @brief Fill data register with slave address
 * @param dev I2C device
 * @param addr Slave address
 * @param rw Read/write bit
 */
static inline void i2c_send_slave_addr(i2c_dev *dev, uint32 addr, uint32 rw) {
    dev->regs->DR = (addr << 1) | rw;
}

/*
 * Simple debugging trail. Define I2C_DEBUG to turn on.
 */
#ifdef I2C_DEBUG

// TODO : Add back DEBUG support to the IRQ functions to set these crumbs

#define NR_CRUMBS       128
struct crumb {
    uint32 event;
    uint32 arg0;
    uint32 arg1;
    uint32 arg2;	// filler to make the data fit GDB memory dump screen
};
static struct crumb crumbs[NR_CRUMBS];
static uint32 cur_crumb = 0;

static inline void i2c_drop_crumb(uint32 event, uint32 arg0, uint32 arg1) {
    if (cur_crumb < NR_CRUMBS) {
        struct crumb *crumb = &crumbs[cur_crumb++];
        crumb->event = event;
        crumb->arg0 = arg0;
        crumb->arg1 = arg1;
        crumb->arg2 = cur_crumb-1;
    }
}
#define I2C_CRUMB(event, arg0, arg1) i2c_drop_crumb(event, arg0, arg1)

#else
#define I2C_CRUMB(event, arg0, arg1)
#endif


enum {
    IRQ_ENTRY           = 1,
    TXE_ONLY            = 2,
    TXE_BTF             = 3,
    STOP_SENT           = 4,
    TEST                = 5,
    RX_ADDR_START       = 6,
    RX_ADDR_STOP        = 7,
    RXNE_ONLY           = 8,
    RXNE_SENDING        = 9,
    RXNE_START_SENT     = 10,
    RXNE_STOP_SENT      = 11,
    RXNE_DONE           = 12,
    ERROR_ENTRY         = 13,
};

/**
 * @brief Reset an I2C bus.
 *
 * Reset is accomplished by clocking out pulses until any hung slaves
 * release SDA and SCL, then generating a START condition, then a STOP
 * condition.
 *
 * @param dev I2C device
 */
void i2c_bus_reset(const i2c_dev *dev) {
    /* Release both lines */
    i2c_master_release_bus(dev);        // Note: This configures the pins as GPIO instead of AF

    /* Software Reset the I2C device: */
    dev->regs->CR1 |= I2C_CR1_SWRST;

    /*
     * Make sure the bus is free by clocking it until any slaves release the
     * bus.
     */
    while (!gpio_read_bit(sda_port(dev), dev->sda_pin)) {
        /* Wait for any clock stretching to finish */
        while (!gpio_read_bit(scl_port(dev), dev->scl_pin))
            ;
        delay_us(10);

        /* Pull low */
        gpio_write_bit(scl_port(dev), dev->scl_pin, 0);
        delay_us(10);

        /* Release high again */
        gpio_write_bit(scl_port(dev), dev->scl_pin, 1);
        delay_us(10);
    }

    /* Generate start then stop condition */
    gpio_write_bit(sda_port(dev), dev->sda_pin, 0);
    delay_us(10);
    gpio_write_bit(scl_port(dev), dev->scl_pin, 0);
    delay_us(10);
    gpio_write_bit(scl_port(dev), dev->scl_pin, 1);
    delay_us(10);
    gpio_write_bit(sda_port(dev), dev->sda_pin, 1);

    /* Release Software Reset: */
    dev->regs->CR1 = 0;
}

/**
 * 15-step process to clear stuck busy as outlined in the
 * errata sheet.  Even though the errata sheet describes
 * it for not being able to enter master mode.  It can
 * also happen when entering slave mode.  A normal reset
 * will clear the stuck busy, but leave you with a stuck
 * start mode, which prevents a master from starting its
 * sequence and prevents a slave from acknowledging its
 * addresses when presented on the bus.
 * 
 * This stuck state can also be reset by external controllers.
 * So if you are interfacing two of these micros together,
 * this reset logic will also clear other STM32 controllers
 * on the bus that are also in this stuck-state.
 */
static void i2c_clear_busy_flag_erratum(const i2c_dev *dev) {
    // 1. Clear PE bit.
    dev->regs->CR1 &= ~I2C_CR1_PE;

    // 2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    i2c_master_release_bus(dev);

    // 3. Check SCL and SDA High level in GPIOx_IDR.
    while (gpio_read_bit(scl_port(dev), dev->scl_pin) == 0) { }
    while (gpio_read_bit(sda_port(dev), dev->sda_pin) == 0) { }

    // 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    gpio_write_bit(sda_port(dev), dev->sda_pin, 0);

    // 5. Check SDA Low level in GPIOx_IDR.
    while (gpio_read_bit(sda_port(dev), dev->sda_pin)) { }

    // 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
    gpio_write_bit(scl_port(dev), dev->scl_pin, 0);

    // 7. Check SCL Low level in GPIOx_IDR.
    while (gpio_read_bit(scl_port(dev), dev->scl_pin)) { }

    // 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
    gpio_write_bit(scl_port(dev), dev->scl_pin, 1);

    // 9. Check SCL High level in GPIOx_IDR.
    while (gpio_read_bit(scl_port(dev), dev->scl_pin) == 0) { }

    // 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
    gpio_write_bit(sda_port(dev), dev->sda_pin, 1);

    // 11. Check SDA High level in GPIOx_IDR.
    while (gpio_read_bit(sda_port(dev), dev->sda_pin) == 0) { }

    // 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain.
    i2c_config_gpios(dev);

    // 13. Set SWRST bit in I2Cx_CR1 register.
    dev->regs->CR1 |= I2C_CR1_SWRST;
    delay_us(10);

    // 14. Clear SWRST bit in I2Cx_CR1 register.
    dev->regs->CR1 &= ~I2C_CR1_SWRST;
    delay_us(10);

    // 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register
    // This step handled in i2c_master_enable
}

/**
 * @brief Initialize an I2C device and reset its registers to their
 *        default values.
 * @param dev Device to initialize.
 */
void i2c_init(i2c_dev *dev) {
    rcc_clk_enable(dev->clk_id);    // The device's clock should enabled before we reset it
    rcc_reset_dev(dev->clk_id);
    _i2c_irq_priority_fixup(dev);
}

/* Hack for deprecated bit of STM32F1 functionality */
#ifndef _I2C_HAVE_DEPRECATED_I2C_REMAP
#define _i2c_handle_remap(dev, flags) ((void)0)
#endif

/**
 * @brief Initialize an I2C device as bus master or slave
 * @param dev Device to enable
 * @param flags Bitwise or of the following I2C options:
 *              I2C_DUTY_16_9: 16/9 Tlow/Thigh duty cycle (only applicable for
 *                             fast mode),
 *              I2C_BUS_RESET: Reset the bus and clock out any hung slaves on
 *                             initialization,
 *              I2C_10BIT_ADDRESSING: Enable 10-bit addressing,
 *              I2C_REMAP: (deprecated, STM32F1 only) Remap I2C1 to SCL/PB8
 *                         SDA/PB9.
 *              I2C_SLAVE_MODE: Configure as Slave
 *              I2C_SLAVE_DUAL_ADDRESS: Slave can respond on 2 i2C addresses
 *              I2C_SLAVE_GENERAL_CALL: SLA+W broadcast to all general call
 *                                      listeners on bus. Addr 0x00
 */
void i2c_master_enable(i2c_dev *dev, uint32 flags, uint32 freq) {
    /* Remap I2C if needed */
    _i2c_handle_remap(dev, flags);

    /* Turn on clock */
    i2c_init(dev);              // If clocks aren't running here, the reset and clear logic below doesn't work

    /* If the device is already enabled, disable so we can reconfigure it */
    i2c_disable(dev);

    /* Reset the bus. Clock out any hung slaves. */
    /* Note that this call reconfigs the port pins as GPIO instead of AF */
    if (flags & I2C_BUS_RESET) {
        i2c_bus_reset(dev);
        flags &= ~I2C_BUS_RESET;
    }

    if (flags & I2C_PUP_RESET) {
        i2c_clear_busy_flag_erratum(dev);
        flags &= ~I2C_PUP_RESET;
    }

    /* Set GPIO modes to AF */
    i2c_config_gpios(dev);

    /* Configure clock and rise time, for both master and slave devices.
     * As per ST specs, CR2 must be set for both.  CCR and TRISE is only
     * needed on master devices, but doesn't hurt to be set for slaves
     * too.
     */
    i2c_set_ccr_trise(dev, flags, freq);

    /* Enable event and buffer interrupts */
    nvic_irq_enable(dev->ev_nvic_line);
    nvic_irq_enable(dev->er_nvic_line);

    /* store all of the flags */
    dev->config_flags = flags;

    /* Make it go! */
    dev->regs->CR1 |= I2C_CR1_PE;       // This has to be done before setting the flags below for slave

    /* Configure the slave unit */
    if (flags & I2C_SLAVE_MODE) {
        if (flags & I2C_SLAVE_GENERAL_CALL) {
            dev->regs->CR1 |= I2C_CR1_ENGC;
        }
        dev->regs->CR1 &= ~I2C_CR1_POS;     // Disable POS
        dev->regs->CR1 |= I2C_CR1_ACK;      // Slave mode needs to acknowledge address matches

        /* Enable interrupts immediately for slave input,
         * for master, we'll wait until transmitting.
         */
        i2c_enable_irq(dev, I2C_IRQ_EVENT | I2C_IRQ_ERROR);
    }

    dev->state = I2C_STATE_IDLE;
}


/**
 * @brief Initialize an I2C device as slave
 * @param dev Device to enable
 * @param flags Bitwise or of the following I2C options:
 *              I2C_DUTY_16_9: 16/9 Tlow/Thigh duty cycle (only applicable for
 *                             fast mode),
 *              I2C_BUS_RESET: Reset the bus and clock out any hung slaves on
 *                             initialization,
 *              I2C_10BIT_ADDRESSING: Enable 10-bit addressing,
 *              I2C_REMAP: (deprecated, STM32F1 only) Remap I2C1 to SCL/PB8
 *                         SDA/PB9.
 *              I2C_SLAVE_MODE: Configure as Slave
 *              I2C_SLAVE_DUAL_ADDRESS: Slave can respond on 2 i2C addresses
 *              I2C_SLAVE_GENERAL_CALL: SLA+W broadcast to all general call
 *                                      listeners on bus. Addr 0x00
 */
void i2c_slave_enable(i2c_dev *dev, uint32 flags, uint32 freq)
{
    i2c_master_enable(dev, flags | I2C_SLAVE_MODE, freq);
}


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
int32 i2c_master_xfer(i2c_dev *dev,
                      i2c_msg *msgs,
                      uint16 num,
                      uint32 timeout) {
    int32 rc;

    ASSERT(dev->state == I2C_STATE_IDLE);

    if (num == 0) return 0;

    // Wait for I2C to not be busy:
    uint32_t count = I2C_TIMEOUT_BUSY_FLAG * (F_CPU / 25U /1000U);
    do {
        if (count-- == 0U) {
            return I2C_ERROR_TIMEOUT;
        }
    } while (dev->regs->SR2 & I2C_SR2_BUSY);

    dev->error_flags = 0;
    dev->msg = msgs;
    dev->msgs_left = num;
    do {
        dev->msg[num-1].xferred = 0;
    } while (--num);
    dev->timestamp = systick_uptime();
    dev->state = I2C_STATE_BUSY;

    dev->regs->CR1 = I2C_CR1_PE;    // Enable but reset special flags
    dev->regs->SR1 = 0;             // Reset error/status flags
    i2c_enable_irq(dev, I2C_IRQ_EVENT | I2C_IRQ_ERROR);
    if (dev->msg[0].flags & I2C_MSG_READ) {
        dev->regs->CR1 = I2C_CR1_PE | I2C_CR1_START | I2C_CR1_ACK;
    } else {
        dev->regs->CR1 = I2C_CR1_PE | I2C_CR1_START;
    }

    rc = wait_for_state_change(dev, I2C_STATE_XFER_DONE, timeout);

    i2c_disable_irq(dev, I2C_IRQ_BUFFER | I2C_IRQ_EVENT | I2C_IRQ_ERROR);

    if (rc != 0) {
        // If we had an error, make sure the device state reflects that
        // and make use of the smb I2C_SR1_TIMEOUT flag.  These have to be
        // done here after disabling the IRQ above to avoid a race-condition
        // with state changes in the IRQ handlers:
        dev->state = I2C_STATE_ERROR;
        if (rc == I2C_ERROR_TIMEOUT) dev->error_flags |= I2C_SR1_TIMEOUT;
    } else {
        dev->state = I2C_STATE_IDLE;
    }

    return rc;
}

/**
 * @brief Wait for an I2C event, or time out in case of error.
 * @param dev I2C device
 * @param state I2C_state state to wait for
 * @param timeout Timeout, in milliseconds
 * @return 0 if target state is reached, a negative value on error.
 */
int32 wait_for_state_change(i2c_dev *dev,
                            i2c_state state,
                            uint32 timeout) {
    volatile i2c_state devState;
    volatile uint32 devTimestamp;

    while (1) {
        // Avoid race condition with interrupt handler while reading state and timestamp
        // Note: Since the IRQs have to be enabled in the first place for
        // I2C to even work, we don't need to save/restore the state of IRQ
        // enable here -- a single disable and re-enable is fine:
        nvic_irq_disable(dev->ev_nvic_line);
        nvic_irq_disable(dev->er_nvic_line);
        devState = dev->state;
        devTimestamp = dev->timestamp;
        nvic_irq_enable(dev->er_nvic_line);
        nvic_irq_enable(dev->ev_nvic_line);

        if (devState == I2C_STATE_ERROR) {
            return I2C_ERROR_PROTOCOL;
        }

        if (devState == state) {
            return 0;
        }

        if (timeout) {
            if ((uint32)(systick_uptime() - devTimestamp) > timeout) {
                return I2C_ERROR_TIMEOUT;
            }
        }
    }
}

/*
 * Private API
 */

/*
 * IRQ handler for I2C master and slave.
 * Handles transmission/reception.
 */
void _i2c_irq_handler(i2c_dev *dev) {
    // See Note in ST Specs:
    //  Reading I2C_SR2 after reading I2C_SR1 clears the ADDR flag, even if the ADDR flag was
    //  set after reading I2C_SR1. Consequently, I2C_SR2 must be read only when ADDR is found
    //  set in I2C_SR1 or when the STOPF bit is cleared.

    __IO uint32_t cr1 = dev->regs->CR1;     // initial control register
    __IO uint32_t sr1 = dev->regs->SR1;     // read status register
    __IO uint32_t sr2 = 0;                  // reserved for reading the SR2 register, but save it for latter since reading it clears ADDR

    dev->timestamp = systick_uptime();      // Reset timeout counter

    if (!(dev->config_flags & I2C_SLAVE_MODE)) {    // Handle Master Mode Here:
        int8_t bDone = 0;                       // Set to true when we're done with this transfer unit
        i2c_msg *curMsg = dev->msg;
        if (curMsg != NULL) {
            int todo = curMsg->length;      // Bytes to transfer
            if (curMsg->flags & I2C_MSG_READ) {         // read transaction:
                if (sr1 & I2C_SR1_SB) {	    // start bit
                    // TODO : Add support for 10-bit address
                    i2c_send_slave_addr(dev, curMsg->addr, 1);
                } else {
                    if (sr1 & I2C_SR1_ADDR) { // address sent
                        if (todo <= 1) {
                            dev->regs->CR1 = (cr1 &= ~I2C_CR1_ACK); // Disable ACK
                            sr2 = dev->regs->SR2;                   // Clear ADDR bit
                            dev->regs->CR1 = (cr1 |= I2C_CR1_STOP); // Stop after last byte
                        } else if (todo == 2) {
                            dev->regs->CR1 = (cr1 |= I2C_CR1_POS);  // Enable POS
                            sr2 = dev->regs->SR2;                   // Clear ADDR bit
                            dev->regs->CR1 = (cr1 &= ~I2C_CR1_ACK); // Disable ACK
                        } else {
                            dev->regs->CR1 = (cr1 |= I2C_CR1_ACK);  // Enable ACK
                            sr2 = dev->regs->SR2;                   // Clear ADDR bit
                        }
                        if (todo >= 1) {
                            i2c_enable_irq(dev, I2C_IRQ_BUFFER);        // Enable I2C_SR1_RXNE interrupt
                        } else {
                            bDone = 1;
                        }
                    } else {
                        int8_t bFlgRXNE = ((sr1 & I2C_SR1_RXNE) != 0);
                        int8_t bFlgBTF = ((sr1 & I2C_SR1_BTF) != 0);

                        if (bFlgRXNE && !bFlgBTF) {
                            if ((todo > 3) || (todo == 1)) {
                                curMsg->data[curMsg->xferred++] = (uint8_t)(dev->regs->DR);     // read/save data
                                curMsg->length--;       // Decrement bytes left to read
                            } else if ((todo == 2) || (todo == 3)) {
                                i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_RXNE interrupt and Switch to BTF mode
                            }
                            if (todo <= 1) {
                                bDone = 1;
                            }
                        } else if (bFlgBTF) {
                            if (todo == 3) {
                                dev->regs->CR1 = (cr1 &= ~I2C_CR1_ACK); // Disable ACK
                            } else if (todo == 2) {
                                dev->regs->CR1 = (cr1 |= I2C_CR1_STOP); // Generate stop
                            }

                            curMsg->data[curMsg->xferred++] = (uint8_t)(dev->regs->DR);     // read/save data
                            curMsg->length--;       // Decrement bytes left to read

                            if (todo == 2) {
                                // If there were 2 bytes, read/save last byte -- we're done:
                                curMsg->data[curMsg->xferred++] = (uint8_t)(dev->regs->DR);     // read/save data
                                curMsg->length--;       // Decrement bytes left to read

                                bDone = 1;
                            }
                        }
                    }
                }
            } else { // write transaction
                if (sr1 & I2C_SR1_SB) { // start bit
                    if (todo != 0) {
                        i2c_enable_irq(dev, I2C_IRQ_BUFFER);        // Enable I2C_SR1_TXE interrupt
                    }
                    // TODO : Add support for 10-bit address
                    i2c_send_slave_addr(dev, curMsg->addr, 0);
                } else {
                    int8_t bFlgTXE = 0;
                    int8_t bFlgBTF = 0;

                    if (sr1 & I2C_SR1_ADDR) {
                        sr2 = dev->regs->SR2;                   // Clear ADDR bit
                    }

                    bFlgTXE = ((sr1 & I2C_SR1_TXE) != 0);
                    bFlgBTF = (((sr1 & I2C_SR1_BTF) != 0) || (todo == 0));

                    if (bFlgTXE || bFlgBTF) {
                        if (todo > 0) {
                            // Write data to DR
                            dev->regs->DR = curMsg->data[curMsg->xferred++];
                            curMsg->length = --todo;            // update todo too

                            if (bFlgTXE && (todo == 0)) {
                                i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_TXE interrupt and Switch to BTF mode
                            }
                        } else if (bFlgBTF) {
                            // Generate Stop
                            if ((curMsg->flags & I2C_MSG_NOSTOP) == 0) {
                                dev->regs->CR1 = (cr1 |= I2C_CR1_STOP);
                            }

                            bDone = 1;
                        }
                    }
                }
            }

            if (bDone) {
                if (--dev->msgs_left != 0) {    // Check to see if there's another back-to-back message
                    i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_RXNE/I2C_SR1_TXE interrupt
                    ++dev->msg;
                    curMsg = dev->msg;
                    if (curMsg->flags & I2C_MSG_READ) {     // Send restart, disable POS, and enable ACK as necessary
                        dev->regs->CR1 = I2C_CR1_PE | I2C_CR1_START | I2C_CR1_ACK;
                    } else {
                        dev->regs->CR1 = I2C_CR1_PE | I2C_CR1_START;
                    }
                } else {
                    dev->msg = NULL;
                    dev->state = I2C_STATE_XFER_DONE;
                }
            }
        }   // curMsg != NULL

    } else {    // Handle Slave Mode:
        if (sr1 & I2C_SR1_ADDR) {   // Address Match
            uint16_t nAddr;

            sr2 = dev->regs->SR2;                   // Clear ADDR bit and get remaining status bits

            dev->error_flags = 0;                   // New error flags for the new message

            if (sr2 & I2C_SR2_GENCALL) {            // We matched the General Call Address
                nAddr = 0;
            } else if (sr2 & I2C_SR2_DUALF) {       // We matched the Secondary Slave address
                if (dev->regs->OAR1 & I2C_OAR1_ADDMODE) {
                    // 10-bit address:
                    nAddr = (dev->regs->OAR1 & 0x0301) | (dev->regs->OAR2 & 0xFE);
                } else {
                    // 7-bit address:
                    nAddr = ((dev->regs->OAR2 >> 1) & 0x7F);
                }
            } else {                                // We matched the Primary Slave address
                if (dev->regs->OAR1 & I2C_OAR1_ADDMODE) {
                    // 10-bit address:
                    nAddr = (dev->regs->OAR1 & 0x03FF);
                } else {
                    // 7-bit address:
                    nAddr = ((dev->regs->OAR1 >> 1) & 0x7F);
                }
            }

            if (sr2 & I2C_SR2_TRA) {
                // If we still have data we received in the turn-around, we
                // sometimes get this interrupt ahead of the error interrupt,
                // in which case we need to give the received data to its
                // callback first so that the request data is ready to send:
                if ((dev->i2c_slave_recv_msg != NULL) && (dev->i2c_slave_recv_msg->xferred)) {
                    if (dev->i2c_slave_recv_callback) {
                        dev->i2c_slave_recv_callback(dev->i2c_slave_recv_msg);  // Callback and process the buffer
                    }
                    dev->i2c_slave_recv_msg->xferred = 0;  // Release message buffer for another read
                }

                // Prepare for transmit:
                if (dev->i2c_slave_xmit_msg != NULL) {
                    dev->i2c_slave_xmit_msg->addr = nAddr;
                    dev->i2c_slave_xmit_msg->length = 0;    // Clear length in case we don't have a callback
                    if (dev->i2c_slave_xmit_callback) {
                        dev->i2c_slave_xmit_callback(dev->i2c_slave_xmit_msg);  // Callback and fill buffer to send
                    }
                    dev->i2c_slave_xmit_msg->xferred = 0;
                }
            } else {
                // Prepare for receive:
                if (dev->i2c_slave_recv_msg != NULL) {
                    dev->i2c_slave_recv_msg->addr = nAddr;
                    dev->i2c_slave_recv_msg->xferred = 0;
                }
            }

            i2c_enable_irq(dev, I2C_IRQ_BUFFER);        // Enabled I2C_SR1_RXNE/I2C_SR1_TXE interupt
            dev->state = I2C_STATE_BUSY;
        }
        else if (sr1 & I2C_SR1_STOPF) {
            sr2 = dev->regs->SR2;

            // If we received data we haven't returned, report it to the client,
            // and resume IDLE state.  This handles the case where the master
            // didn't properly send a NACK but sent a STOP instead.  The normal
            // path is handled in the error interrupter with the AF bit.
            if ((dev->i2c_slave_recv_msg != NULL) && (dev->i2c_slave_recv_msg->xferred)) {
                if (dev->i2c_slave_recv_callback) {
                    dev->i2c_slave_recv_callback(dev->i2c_slave_recv_msg);  // Callback and process the buffer
                }
                dev->i2c_slave_recv_msg->xferred = 0;  // Release message buffer for another read
            }

            // Clearing STOPF requires reading SR1 (which we've already done), followed by writing CR1
            dev->regs->CR1 |= (cr1 |= I2C_CR1_ACK);     // Make sure ACK is reenabled for next incoming address
            dev->state = I2C_STATE_IDLE;
        } else {
            int8_t bFlgBTF = ((sr1 & I2C_SR1_BTF) != 0);
            int8_t bFlgTXE = (((sr1 & I2C_SR1_TXE) != 0) && ((dev->regs->CR2 & I2C_CR2_ITBUFEN) != 0));
            int todo = 0;       // Bytes remaining to transfer for Xmit or bytes of remaining buffer for Recv

            sr2 = dev->regs->SR2;

            if (sr2 & I2C_SR2_TRA) {
                // In Transmit Mode:
                if (dev->i2c_slave_xmit_msg) {
                    todo = (dev->i2c_slave_xmit_msg->length - dev->i2c_slave_xmit_msg->xferred);
                }

                if (bFlgTXE || bFlgBTF) {
                    // Send next byte of data:
                    __IO uint8_t nData = 0;
                    if ((dev->i2c_slave_xmit_msg) && (todo)) {
                        nData = dev->i2c_slave_xmit_msg->data[dev->i2c_slave_xmit_msg->xferred++];
                    }
                    dev->regs->DR = nData;

                    if (todo) --todo;
                    if (bFlgTXE && (todo == 0)) {
                        i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_TXE interrupt and switch to BTF mode
                    }
                }
            } else {
                // In Receive Mode:
                __IO uint8_t nData = 0;
                int8_t bFlgRXNE = (((sr1 & I2C_SR1_RXNE) != 0) && ((dev->regs->CR2 & I2C_CR2_ITBUFEN) != 0));

                if (dev->i2c_slave_recv_msg) {
                    todo = (dev->i2c_slave_recv_msg->length - dev->i2c_slave_recv_msg->xferred);
                }

                if (bFlgRXNE && !bFlgBTF) {
                    if (todo > 0) {
                        // Note: We already know the i2c_slave_recv_msg isn't null because todo!=0
                        nData = dev->regs->DR;  // Clears I2C_SR1_RXNE flag
                        dev->i2c_slave_recv_msg->data[dev->i2c_slave_recv_msg->xferred++] = nData;
                    }
                    if (todo <= 3) {
                        i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_RXNE interrupt and Switch to BTF mode
                    }
                } else if (bFlgBTF) {
                    // If our receive buffer is filling up, NAK and/or force Stop:
                    if (todo == 3) {
                        dev->regs->CR1 = (cr1 &= ~I2C_CR1_ACK); // Disable ACK
                    } else if (todo == 2) {
                        dev->regs->CR1 = (cr1 |= I2C_CR1_STOP); // Generate stop
                    }

                    nData = dev->regs->DR;
                    if (todo > 0) {
                        // Note: We already know the i2c_slave_recv_msg isn't null because todo>0
                        dev->i2c_slave_recv_msg->data[dev->i2c_slave_recv_msg->xferred++] = nData;
                    }

                    if (bFlgRXNE) {
                        // If there were 2 bytes (both flags set), read/save last byte:
                        nData = dev->regs->DR;
                        if (todo > 1) {     // Compare with >1 here because we already read one byte above
                            // Note: We already know the i2c_slave_recv_msg isn't null because todo>1
                            dev->i2c_slave_recv_msg->data[dev->i2c_slave_recv_msg->xferred++] = nData;
                        }
                        i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_RXNE/I2C_SR1_TXE interrupt
                    }
                }
            }
        }

    }   // End of Slave Mode
}


/*
 * Interrupt handler for I2C error conditions. Aborts any pending I2C
 * transactions.
 */
void _i2c_irq_error_handler(i2c_dev *dev) {
    __IO uint32_t sr1 = dev->regs->SR1;
    __IO uint32_t sr2 = dev->regs->SR2;

    dev->timestamp = systick_uptime();      // Reset timeout counter

    dev->error_flags = sr1 & (I2C_SR1_BERR |
                              I2C_SR1_ARLO |
                              I2C_SR1_AF |
                              I2C_SR1_OVR);

    if (dev->config_flags & I2C_SLAVE_MODE) {
        // Slave Mode:
        i2c_disable_irq(dev, I2C_IRQ_BUFFER);   // Disable I2C_SR1_RXNE/I2C_SR1_TXE interrupt in case it was still active
        dev->regs->CR1 |= I2C_CR1_ACK;          // Make sure ACK is reenabled for next incoming address

        if (sr1 & I2C_SR1_AF) {
            // If we received data, report it to the client,
            // Clear the NACK for the end of Transmit, and resume IDLE state:
            dev->regs->SR1 = ~I2C_SR1_AF;

            if ((dev->i2c_slave_recv_msg != NULL) && (dev->i2c_slave_recv_msg->xferred)) {
                if (dev->i2c_slave_recv_callback) {
                    dev->i2c_slave_recv_callback(dev->i2c_slave_recv_msg);  // Callback and process the buffer
                }
                dev->i2c_slave_recv_msg->xferred = 0;  // Release message buffer for another read
            }

            dev->state = I2C_STATE_IDLE;
            return;
        }
    } else {
        // Master should send a STOP on NACK:
        if (sr1 & I2C_SR1_AF) {
            dev->regs->CR1 |= I2C_CR1_STOP;
        }
    }

    /* Catch any other strange errors while in slave mode.
     * I have seen BERR caused by an over fast master device
     * as well as several overflows and arbitration failures.
     * We are going to reset SR flags and carry on at this point which
     * is not the best thing to do, but stops the bus locking up completely
     * If we carry on below and send the stop bit, the code spins forever */
    /* Clear flags */
    dev->regs->SR1 = 0;
    dev->regs->SR2 = 0;
    dev->state = I2C_STATE_ERROR;

    UNUSED(sr2);
}


/*
 * CCR/TRISE configuration helper
 */
void i2c_set_ccr_trise(i2c_dev *dev, uint32 flags, uint32 freq) {
    uint32 ccr     = 0;
    uint32 trise   = 0;
    uint32 clk_mhz = _i2c_bus_clk(dev);
    uint32 clk_hz  = clk_mhz * (1000 * 1000);

    i2c_set_input_clk(dev, clk_mhz);

    if (freq > 100000) {        //FAST MODE & FAST MODE+
        ccr |= I2C_CCR_FS;

        if (flags & I2C_DUTY_16_9) {
            /* Tlow/Thigh = 16/9 */
            ccr |= I2C_CCR_DUTY_16_9;
            ccr |= clk_hz / (freq * 25);
        } else {
            /* Tlow/Thigh = 2 */
            ccr |= clk_hz / (freq * 3);
        }

        trise = (300 * clk_mhz / 1000) + 1;
    } else {
        /* Tlow/Thigh = 1 */
        ccr = clk_hz / (freq * 2);
        trise = clk_mhz + 1;
    }

    /* Set minimum required value if CCR < 1*/
    if ((ccr & I2C_CCR_CCR) == 0) {
        ccr |= 0x1;
    }

    i2c_set_clk_control(dev, ccr);
    i2c_set_trise(dev, trise);
}


/**
 * @brief callback for when the device acts as a slave. If using an rx buffer, this is triggered
 * after the last byte, otherwise it is called for every incoming packet.
 * @param dev I2C device
 * @param msg The dev_msg to pass to the slave init code
 * @param func The function pointer to call
 */
void i2c_slave_attach_recv_handler(i2c_dev *dev, i2c_msg *msg, i2c_slave_recv_callback_func func) {
    dev->i2c_slave_recv_callback = func;
    dev->i2c_slave_recv_msg = msg;
    msg->xferred = 0;
}


/**
 * @brief callback for when the device acts as a slave. If using a tx buffer, this is triggered
 * after the device is successsfully addressed with SLA+R.
 * @param dev I2C device
 * @param msg The dev_msg to pass to the slave init code
 * @param func The function pointer to call
 */
void i2c_slave_attach_transmit_handler(i2c_dev *dev, i2c_msg *msg, i2c_slave_xmit_callback_func func) {
    dev->i2c_slave_xmit_callback = func;
    dev->i2c_slave_xmit_msg = msg;
    msg->xferred = 0;
}
