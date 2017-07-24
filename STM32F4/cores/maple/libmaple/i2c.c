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
 * @file i2c.c
 * @brief Inter-Integrated Circuit (I2C) support.
 *
 * Currently, only master mode is supported.
 */

#include "libmaple.h"
#include "rcc.h"
#include "gpio.h"
#include "nvic.h"
#include "i2c.h"
#include "string.h"
#include "systick.h"

static i2c_dev i2c_dev1 = {
    .regs         = I2C1_BASE,
    //.gpio_port    = &gpiob,
    .sda_pin      = PB7,
    .scl_pin      = PB6,
    .clk_id       = RCC_I2C1,
    .ev_nvic_line = NVIC_I2C1_EV,
    .er_nvic_line = NVIC_I2C1_ER,
    .state        = I2C_STATE_DISABLED
};
/** I2C1 device */
i2c_dev* const I2C1 = &i2c_dev1;

static i2c_dev i2c_dev2 = {
    .regs         = I2C2_BASE,
    //.gpio_port    = &gpiob,
    .sda_pin      = PB11,
    .scl_pin      = PB10,
    .clk_id       = RCC_I2C2,
    .ev_nvic_line = NVIC_I2C2_EV,
    .er_nvic_line = NVIC_I2C2_ER,
    .state        = I2C_STATE_DISABLED
};
/** I2C2 device */
i2c_dev* const I2C2 = &i2c_dev2;

static inline int32 wait_for_state_change(i2c_dev *dev,
                                          i2c_state state,
                                          uint32 timeout);

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
 * @brief IRQ handler for I2C master. Handles transmission/reception.
 * @param dev I2C device
 */
static void i2c_irq_handler(i2c_dev *dev) {
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
    if (sr1 & I2C_SR1_ADDR) {
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
            if (msg->length != 1) {
                i2c_write(dev, msg->data[msg->xferred++]);
            }
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
            throb();
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

void __irq_i2c1_ev(void) {
   i2c_irq_handler(&i2c_dev1);
}

void __irq_i2c2_ev(void) {
   i2c_irq_handler(&i2c_dev2);
}

/**
 * @brief Interrupt handler for I2C error conditions
 * @param dev I2C device
 * @sideeffect Aborts any pending I2C transactions
 */
static void i2c_irq_error_handler(i2c_dev *dev) {
    I2C_CRUMB(ERROR_ENTRY, dev->regs->SR1, dev->regs->SR2);

    dev->error_flags = dev->regs->SR2 & (I2C_SR1_BERR |
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

void __irq_i2c1_er(void) {
    i2c_irq_error_handler(&i2c_dev1);
}

void __irq_i2c2_er(void) {
    i2c_irq_error_handler(&i2c_dev2);
}

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
    gpio_set_pin(dev->scl_pin);
    gpio_set_pin(dev->sda_pin);
    gpio_set_mode(dev->scl_pin, GPIO_OUTPUT_OD);
    gpio_set_mode(dev->sda_pin, GPIO_OUTPUT_OD);

    /*
     * Make sure the bus is free by clocking it until any slaves release the
     * bus.
     */
    while (!gpio_read_pin(dev->sda_pin)) {
        /* Wait for any clock stretching to finish */
        while (!gpio_read_pin(dev->scl_pin))
            ;
        delay_us(10);

        /* Pull low */
        gpio_clear_pin(dev->scl_pin);
        delay_us(10);

        /* Release high again */
        gpio_set_pin(dev->scl_pin);
        delay_us(10);
    }

    /* Generate start then stop condition */
    gpio_clear_pin(dev->sda_pin);
    delay_us(10);
    gpio_clear_pin(dev->scl_pin);
    delay_us(10);
    gpio_set_pin(dev->scl_pin);
    delay_us(10);
    gpio_set_pin(dev->sda_pin);
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
 *              I2C_REMAP: Remap I2C1 to SCL/PB8 SDA/PB9.
 */
void i2c_master_enable(i2c_dev *dev, uint32 flags) {
#define I2C_CLK                (STM32_PCLK1/1000000)
    uint32 ccr   = 0;
    uint32 trise = 0;

    /* PE must be disabled to configure the device */
    ASSERT(!(dev->regs->CR1 & I2C_CR1_PE));

    if ((dev == I2C1) && (flags & I2C_REMAP)) {
        afio_remap(AFIO_REMAP_I2C1);
        I2C1->sda_pin = 9;
        I2C1->scl_pin = 8;
    }

    /* Reset the bus. Clock out any hung slaves. */
    if (flags & I2C_BUS_RESET) {
        i2c_bus_reset(dev);
    }

    /* Turn on clock and set GPIO modes */
    i2c_init(dev);
    gpio_set_mode(dev->sda_pin, GPIO_AF_OUTPUT_OD);
    gpio_set_mode(dev->scl_pin, GPIO_AF_OUTPUT_OD);

    /* I2C1 and I2C2 are fed from APB1, clocked at 36MHz */
    i2c_set_input_clk(dev, I2C_CLK);

    if (flags & I2C_FAST_MODE) {
        ccr |= I2C_CCR_FS;

        if (flags & I2C_DUTY_16_9) {
            /* Tlow/Thigh = 16/9 */
            ccr |= I2C_CCR_DUTY;
            ccr |= STM32_PCLK1/(400000 * 25);
        } else {
            /* Tlow/Thigh = 2 */
            ccr |= STM32_PCLK1/(400000 * 3);
        }

        trise = (300 * (I2C_CLK)/1000) + 1;
    } else {
        /* Tlow/Thigh = 1 */
        ccr = STM32_PCLK1/(100000 * 2);
        trise = I2C_CLK + 1;
    }

    /* Set minimum required value if CCR < 1*/
    if ((ccr & I2C_CCR_CCR) == 0) {
        ccr |= 0x1;
    }

    i2c_set_clk_control(dev, ccr);
    i2c_set_trise(dev, trise);

    /* Enable event and buffer interrupts */
    nvic_irq_enable(dev->ev_nvic_line);
    nvic_irq_enable(dev->er_nvic_line);
    i2c_enable_irq(dev, I2C_IRQ_EVENT | I2C_IRQ_BUFFER | I2C_IRQ_ERROR);

    /*
     * Important STM32 Errata:
     *
     * See STM32F10xx8 and STM32F10xxB Errata sheet (Doc ID 14574 Rev 8),
     * Section 2.11.1, 2.11.2.
     *
     * 2.11.1:
     * When the EV7, EV7_1, EV6_1, EV6_3, EV2, EV8, and EV3 events are not
     * managed before the current byte is being transferred, problems may be
     * encountered such as receiving an extra byte, reading the same data twice
     * or missing data.
     *
     * 2.11.2:
     * In Master Receiver mode, when closing the communication using
     * method 2, the content of the last read data can be corrupted.
     *
     * If the user software is not able to read the data N-1 before the STOP
     * condition is generated on the bus, the content of the shift register
     * (data N) will be corrupted. (data N is shifted 1-bit to the left).
     *
     * ----------------------------------------------------------------------
     *
     * In order to ensure that events are not missed, the i2c interrupt must
     * not be preempted. We set the i2c interrupt priority to be the highest
     * interrupt in the system (priority level 0). All other interrupts have
     * been initialized to priority level 16. See nvic_init().
     */
    nvic_irq_set_priority(dev->ev_nvic_line, 0);
    nvic_irq_set_priority(dev->er_nvic_line, 0);

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
