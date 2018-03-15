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
 * @brief Inter-Integrated Circuit (I2C) support.
 *
 * Currently, only master mode is supported.
 */

#include "i2c_private.h"

#include <libmaple/libmaple.h>
#include <libmaple/rcc.h>
#include <libmaple/gpio.h>
#include <libmaple/nvic.h>
#include <libmaple/i2c.h>
#include <libmaple/systick.h>

#include <string.h>

static inline int32 wait_for_state_change(i2c_dev *dev,
                                          i2c_state state,
                                          uint32 timeout);
static void set_ccr_trise(i2c_dev *dev, uint32 flags);

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

#define NR_CRUMBS       128
static struct crumb crumbs[NR_CRUMBS];
static uint32 cur_crumb = 0;

static inline void i2c_drop_crumb(uint32 event, uint32 arg0, uint32 arg1) {
    if (cur_crumb < NR_CRUMBS) {
        struct crumb *crumb = &crumbs[cur_crumb++];
        crumb->event = event;
        crumb->arg0 = arg0;
        crumb->arg1 = arg1;
    }
}
#define I2C_CRUMB(event, arg0, arg1) i2c_drop_crumb(event, arg0, arg1)

#else
#define I2C_CRUMB(event, arg0, arg1)
#endif

struct crumb {
    uint32 event;
    uint32 arg0;
    uint32 arg1;
};

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
    i2c_master_release_bus(dev);

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
}

/**
 * @brief Initialize an I2C device and reset its registers to their
 *        default values.
 * @param dev Device to initialize.
 */
void i2c_init(i2c_dev *dev) {
    rcc_reset_dev(dev->clk_id);
    rcc_clk_enable(dev->clk_id);
}

/* Hack for deprecated bit of STM32F1 functionality */
#ifndef _I2C_HAVE_DEPRECATED_I2C_REMAP
#define _i2c_handle_remap(dev, flags) ((void)0)
#endif

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
void i2c_master_enable(i2c_dev *dev, uint32 flags) {
    /* PE must be disabled to configure the device */
    ASSERT(!(dev->regs->CR1 & I2C_CR1_PE));

    /* Ugh */
    _i2c_handle_remap(dev, flags);

    /* Reset the bus. Clock out any hung slaves. */
    if (flags & I2C_BUS_RESET) {
        i2c_bus_reset(dev);
    }

    /* Turn on clock and set GPIO modes */
    i2c_init(dev);
    i2c_config_gpios(dev);

    /* Configure clock and rise time */
    set_ccr_trise(dev, flags);

    /* Enable event and buffer interrupts */
    nvic_irq_enable(dev->ev_nvic_line);
    nvic_irq_enable(dev->er_nvic_line);
    i2c_enable_irq(dev, I2C_IRQ_EVENT | I2C_IRQ_BUFFER | I2C_IRQ_ERROR);

    /* Make it go! */
    i2c_peripheral_enable(dev);

    dev->state = I2C_STATE_IDLE;
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

    dev->msg = msgs;
    dev->msgs_left = num;
    dev->timestamp = systick_uptime();
    dev->state = I2C_STATE_BUSY;

    i2c_enable_irq(dev, I2C_IRQ_EVENT);
    i2c_start_condition(dev);
    
    rc = wait_for_state_change(dev, I2C_STATE_XFER_DONE, timeout);
    if (rc < 0) {
        goto out;
    }

    dev->state = I2C_STATE_IDLE;
out:
    return rc;
}

/**
 * @brief Wait for an I2C event, or time out in case of error.
 * @param dev I2C device
 * @param state I2C_state state to wait for
 * @param timeout Timeout, in milliseconds
 * @return 0 if target state is reached, a negative value on error.
 */
static inline int32 wait_for_state_change(i2c_dev *dev,
                                          i2c_state state,
                                          uint32 timeout) {
    i2c_state tmp;

    while (1) {
        tmp = dev->state;

        if (tmp == I2C_STATE_ERROR) {
            return I2C_STATE_ERROR;
        }

        if (tmp == state) {
            return 0;
        }

        if (timeout) {
            if (systick_uptime() > (dev->timestamp + timeout)) {
                /* TODO: overflow? */
                /* TODO: racy? */
                return I2C_ERROR_TIMEOUT;
            }
        }
    }
}

/*
 * Private API
 */

/*
 * IRQ handler for I2C master. Handles transmission/reception.
 */
void _i2c_irq_handler(i2c_dev *dev) {
    /* WTFs:
     * - Where is I2C_MSG_10BIT_ADDR handled?
     */
    i2c_msg *msg = dev->msg;

    uint8 read = msg->flags & I2C_MSG_READ;

    uint32 sr1 = dev->regs->SR1;
    uint32 sr2 = dev->regs->SR2;
    I2C_CRUMB(IRQ_ENTRY, sr1, sr2);

    /*
     * Reset timeout counter
     */
    dev->timestamp = systick_uptime();

    /*
     * EV5: Start condition sent
     */
    if (sr1 & I2C_SR1_SB) {
        msg->xferred = 0;
        i2c_enable_irq(dev, I2C_IRQ_BUFFER);

        /*
         * Master receiver
         */
        if (read) {
            i2c_enable_ack(dev);
        }

        i2c_send_slave_addr(dev, msg->addr, read);
        sr1 = sr2 = 0;
    }

    /*
     * EV6: Slave address sent
     */
    if (sr1 & (I2C_SR1_ADDR|I2C_SR1_ADD10)) {
        /*
         * Special case event EV6_1 for master receiver.
         * Generate NACK and restart/stop condition after ADDR
         * is cleared.
         */
        if (read) {
            if (msg->length == 1) {
                i2c_disable_ack(dev);
                if (dev->msgs_left > 1) {
                    i2c_start_condition(dev);
                    I2C_CRUMB(RX_ADDR_START, 0, 0);
                } else {
                    i2c_stop_condition(dev);
                    I2C_CRUMB(RX_ADDR_STOP, 0, 0);
                }
            }
        } else {
            /*
             * Master transmitter: write first byte to fill shift
             * register.  We should get another TXE interrupt
             * immediately to fill DR again.
             */
            if (msg->length > 1) {
                i2c_write(dev, msg->data[msg->xferred++]);
            } else if (msg->length == 0) { /* We're just sending an address */
                i2c_stop_condition(dev);
              /*
               * Turn off event interrupts to keep BTF from firing until
               * the end of the stop condition. Why on earth they didn't
               * have a start/stop condition request clear BTF is beyond
               * me.
               */
              i2c_disable_irq(dev, I2C_IRQ_EVENT);
              I2C_CRUMB(STOP_SENT, 0, 0);
              dev->state = I2C_STATE_XFER_DONE;
            } /* else we're just sending one byte */
        }
        sr1 = sr2 = 0;
    }

    /*
     * EV8: Master transmitter
     * Transmit buffer empty, but we haven't finished transmitting the last
     * byte written.
     */
    if ((sr1 & I2C_SR1_TXE) && !(sr1 & I2C_SR1_BTF)) {
        I2C_CRUMB(TXE_ONLY, 0, 0);
        if (dev->msgs_left) {
            i2c_write(dev, msg->data[msg->xferred++]);
            if (msg->xferred == msg->length) {
                /*
                 * End of this message. Turn off TXE/RXNE and wait for
                 * BTF to send repeated start or stop condition.
                 */
                i2c_disable_irq(dev, I2C_IRQ_BUFFER);
                dev->msgs_left--;
            }
        } else {
            /*
             * This should be impossible...
             */
            ASSERT(0);
        }
        sr1 = sr2 = 0;
    }

    /*
     * EV8_2: Master transmitter
     * Last byte sent, program repeated start/stop
     */
    if ((sr1 & I2C_SR1_TXE) && (sr1 & I2C_SR1_BTF)) {
        I2C_CRUMB(TXE_BTF, 0, 0);
        if (dev->msgs_left) {
            I2C_CRUMB(TEST, 0, 0);
            /*
             * Repeated start insanity: We can't disable ITEVTEN or else SB
             * won't interrupt, but if we don't disable ITEVTEN, BTF will
             * continually interrupt us. What the fuck ST?
             */
            i2c_start_condition(dev);
            while (!(dev->regs->SR1 & I2C_SR1_SB))
                ;
            dev->msg++;
        } else {
            i2c_stop_condition(dev);

            /*
             * Turn off event interrupts to keep BTF from firing until
             * the end of the stop condition. Why on earth they didn't
             * have a start/stop condition request clear BTF is beyond
             * me.
             */
            i2c_disable_irq(dev, I2C_IRQ_EVENT);
            I2C_CRUMB(STOP_SENT, 0, 0);
            dev->state = I2C_STATE_XFER_DONE;
        }
        sr1 = sr2 = 0;
    }

    /*
     * EV7: Master Receiver
     */
    if (sr1 & I2C_SR1_RXNE) {
        I2C_CRUMB(RXNE_ONLY, 0, 0);
        msg->data[msg->xferred++] = dev->regs->DR;

        /*
         * EV7_1: Second to last byte in the reception? Set NACK and generate
         * stop/restart condition in time for the last byte. We'll get one more
         * RXNE interrupt before shutting things down.
         */
        if (msg->xferred == (msg->length - 1)) {
            i2c_disable_ack(dev);
            if (dev->msgs_left > 2) {
                i2c_start_condition(dev);
                I2C_CRUMB(RXNE_START_SENT, 0, 0);
            } else {
                i2c_stop_condition(dev);
                I2C_CRUMB(RXNE_STOP_SENT, 0, 0);
            }
        } else if (msg->xferred == msg->length) {
            dev->msgs_left--;
            if (dev->msgs_left == 0) {
                /*
                 * We're done.
                 */
                I2C_CRUMB(RXNE_DONE, 0, 0);
                dev->state = I2C_STATE_XFER_DONE;
            } else {
                dev->msg++;
            }
        }
    }
}

/*
 * Interrupt handler for I2C error conditions. Aborts any pending I2C
 * transactions.
 */
void _i2c_irq_error_handler(i2c_dev *dev) {
    I2C_CRUMB(ERROR_ENTRY, dev->regs->SR1, dev->regs->SR2);

    dev->error_flags = dev->regs->SR1 & (I2C_SR1_BERR |
                                         I2C_SR1_ARLO |
                                         I2C_SR1_AF |
                                         I2C_SR1_OVR);
    /* Clear flags */
    dev->regs->SR1 = 0;
    dev->regs->SR2 = 0;

    i2c_stop_condition(dev);
    i2c_disable_irq(dev, I2C_IRQ_BUFFER | I2C_IRQ_EVENT | I2C_IRQ_ERROR);
    dev->state = I2C_STATE_ERROR;
}

/*
 * CCR/TRISE configuration helper
 */
static void set_ccr_trise(i2c_dev *dev, uint32 flags) {
    uint32 ccr     = 0;
    uint32 trise   = 0;
    uint32 clk_mhz = _i2c_bus_clk(dev);
    uint32 clk_hz  = clk_mhz * (1000 * 1000);

    i2c_set_input_clk(dev, clk_mhz);

    if (flags & I2C_FAST_MODE) {
        ccr |= I2C_CCR_FS;

        if (flags & I2C_DUTY_16_9) {
            /* Tlow/Thigh = 16/9 */
            ccr |= I2C_CCR_DUTY_16_9;
            ccr |= clk_hz / (400000 * 25);
        } else {
            /* Tlow/Thigh = 2 */
            ccr |= clk_hz / (400000 * 3);
        }

        trise = (300 * clk_mhz / 1000) + 1;
    } else {
        /* Tlow/Thigh = 1 */
        ccr = clk_hz / (100000 * 2);
        trise = clk_mhz + 1;
    }

    /* Set minimum required value if CCR < 1*/
    if ((ccr & I2C_CCR_CCR) == 0) {
        ccr |= 0x1;
    }

    i2c_set_clk_control(dev, ccr);
    i2c_set_trise(dev, trise);
}
