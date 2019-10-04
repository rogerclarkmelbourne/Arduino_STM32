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
 * @file libmaple/include/libmaple/i2c.h
 * @brief Inter-Integrated Circuit (I2C) peripheral support
 *
 * Supports Master and Slave.
 *  Master Usage notes:
 *
 * - Enable an I2C device with i2c_master_enable().
 * - Initialize an array of struct i2c_msg to suit the bus
 *   transactions (reads/writes) you wish to perform.
 * - Call i2c_master_xfer() to do the work.
 *
 * Slave Usage notes:
 * - Enable I2C slave by calling i2c_slave_enable().
 *   Check flags for usage. Enabling master also enabled slave.
 * - initialise the i2c_msg struct and the data buffer
 * - initialise the callback functions
 *
 * I2C slave support added 2012 by Barry Carter. barry.carter@gmail.com, headfuzz.co.uk
 * 
 * Modified 2019 by Donna Whisnant to merge WireSlave changes with the core to
 * make slave mode work and without having conflicting data type defintions,
 * and rewrote logic to function better.
 * 
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
 *   Or define to 0 to completely disable the workaround.
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

/** I2C register map type */
typedef struct i2c_reg_map {
    __IO uint32 CR1;            /**< Control register 1 */
    __IO uint32 CR2;            /**< Control register 2 */
    __IO uint32 OAR1;           /**< Own address register 1 */
    __IO uint32 OAR2;           /**< Own address register 2 */
    __IO uint32 DR;             /**< Data register */
    __IO uint32 SR1;            /**< Status register 1 */
    __IO uint32 SR2;            /**< Status register 2 */
    __IO uint32 CCR;            /**< Clock control register */
    __IO uint32 TRISE;          /**< TRISE (rise time) register */
} i2c_reg_map;

/**
 * @brief I2C message type
 */
typedef struct i2c_msg {
    uint16 addr;                /**< Address */

#define I2C_MSG_READ            0x1
#define I2C_MSG_10BIT_ADDR      0x2
#define I2C_MSG_NOSTOP          0x4

    /**
     * Bitwise OR of:
     * - I2C_MSG_READ (write is default)
     * - I2C_MSG_10BIT_ADDR (7-bit is default) */
    uint16 flags;

    uint16 length;              /**< Message length */
    uint16 xferred;             /**< Messages transferred */
    uint8 *data;                /**< Data */
} i2c_msg;

/*
 * Register bit definitions
 */

/* Control register 1 */

#define I2C_CR1_SWRST           (1U << 15)    // Software reset
#define I2C_CR1_ALERT           (1U << 13)    // SMBus alert
#define I2C_CR1_PEC             (1U << 12)    // Packet error checking
#define I2C_CR1_POS             (1U << 11)    // Acknowledge/PEC position
#define I2C_CR1_ACK             (1U << 10)    // Acknowledge enable
#define I2C_CR1_STOP            (1U << 9)     // Stop generation
#define I2C_CR1_START           (1U << 8)     // Start generation
#define I2C_CR1_NOSTRETCH       (1U << 7)     // Clock stretching disable
#define I2C_CR1_ENGC            (1U << 6)     // General call enable
#define I2C_CR1_ENPEC           (1U << 5)     // PEC enable
#define I2C_CR1_ENARP           (1U << 4)     // ARP enable
#define I2C_CR1_SMBTYPE         (1U << 3)     // SMBus type
#define I2C_CR1_SMBTYPE_DEVICE  (0U << 3)     //     SMBus type: device
#define I2C_CR1_SMBTYPE_HOST    (1U << 3)     //     SMBus type: host
#define I2C_CR1_SMBUS           (1U << 1)     // SMBus mode
#define I2C_CR1_SMBUS_I2C       (0U << 1)     //     SMBus mode: I2C
#define I2C_CR1_SMBUS_SMBUS     (1U << 1)     //     SMBus mode: SMBus
#define I2C_CR1_PE              (1U << 0)     // Peripheral Enable

/* Control register 2 */

#define I2C_CR2_LAST            (1U << 12)    // DMA last transfer
#define I2C_CR2_DMAEN           (1U << 11)    // DMA requests enable
#define I2C_CR2_ITBUFEN         (1U << 10)    // Buffer interrupt enable
#define I2C_CR2_ITEVTEN         (1U << 9)     // Event interupt enable
#define I2C_CR2_ITERREN         (1U << 8)     // Error interupt enable
#define I2C_CR2_FREQ            0x3F          // Peripheral input frequency

/* Own address register 1 */

#define I2C_OAR1_ADDMODE        (1U << 15)    // Addressing mode
#define I2C_OAR1_ADDMODE_7_BIT  (0U << 15)    //     Addressing mode: 7-bit
#define I2C_OAR1_ADDMODE_10_BIT (1U << 15)    //     Addressing mode: 10-bit
#define I2C_OAR1_ADD            0x3FF         // Interface address

/* Own address register 2 */

#define I2C_OAR2_ADD2           0xFE          // Interface address
#define I2C_OAR2_ENDUAL         1U            // Dual addressing mode enable

/* Status register 1 */

#define I2C_SR1_SMBALERT        (1U << 15)    // SMBus alert
#define I2C_SR1_TIMEOUT         (1U << 14)    // Timeout or Tlow error
#define I2C_SR1_PECERR          (1U << 12)    // PEC Error in reception
#define I2C_SR1_OVR             (1U << 11)    // Overrun/underrun
#define I2C_SR1_AF              (1U << 10)    // Acknowledge failure
#define I2C_SR1_ARLO            (1U << 9)     // Arbitration lost
#define I2C_SR1_BERR            (1U << 8)     // Bus error
#define I2C_SR1_TXE             (1U << 7)     // Data register empty
#define I2C_SR1_RXNE            (1U << 6)     // Data register not empty
#define I2C_SR1_STOPF           (1U << 4)     // Stop detection
#define I2C_SR1_ADD10           (1U << 3)     // 10-bit header sent
#define I2C_SR1_BTF             (1U << 2)     // Byte transfer finished
#define I2C_SR1_ADDR            (1U << 1)     // Address sent/matched
#define I2C_SR1_SB              (1U << 0)     // Start bit

/* Status register 2 */

#define I2C_SR2_PEC             0xFF00        // Packet error checking register
#define I2C_SR2_DUALF           (1U << 7)     // Dual flag
#define I2C_SR2_SMBHOST         (1U << 6)     // SMBus host header
#define I2C_SR2_SMBDEFAULT      (1U << 5)     // SMBus device default address
#define I2C_SR2_GENCALL         (1U << 4)     // General call address
#define I2C_SR2_TRA             (1U << 2)     // Transmitter/receiver
#define I2C_SR2_BUSY            (1U << 1)     // Bus busy
#define I2C_SR2_MSL             (1U << 0)     // Master/slave

/* Clock control register */

#define I2C_CCR_FS              (1U << 15)    // Fast mode selection
#define I2C_CCR_DUTY            (1U << 14)    // Fast mode duty cycle
#define I2C_CCR_DUTY_2_1        (0U << 14)    //     Fast mode duty: 2/1
#define I2C_CCR_DUTY_16_9       (1U << 14)    //     Fast mode duty: 16/9
#define I2C_CCR_CCR             0xFFF         // Clock control bits

/*
 * Convenience routines
 */

/* Main I2C API */

/* I2C enable options */
#define I2C_FAST_MODE           0x1           // 400 khz
#define I2C_DUTY_16_9           0x2           // 16/9 duty ratio
#define I2C_REMAP               0x4           // Deprecated: I2C_REMAP for I2C1
#define I2C_BUS_RESET           0x8           // Perform a bus reset
#define I2C_10BIT_ADDRESSING    0x10          // Enable 10-bit address mode
#define I2C_SLAVE_MODE          0x20          // Set if configured for slave mode instead of master mode
#define I2C_SLAVE_DUAL_ADDRESS  0x40          // Enable the dual slave address scheme
#define I2C_SLAVE_GENERAL_CALL  0x80          // Enable the general call on address 0x00
#define I2C_PUP_RESET           0x100         // Power-Up Reset

void i2c_master_enable(i2c_dev *dev, uint32 flags, uint32 freq);
void i2c_slave_enable(i2c_dev *dev, uint32 flags, uint32 freq);

#define I2C_ERROR_PROTOCOL      (-1)
#define I2C_ERROR_TIMEOUT       (-2)
int32 i2c_master_xfer(i2c_dev *dev, i2c_msg *msgs, uint16 num, uint32 timeout);
int32 wait_for_state_change(i2c_dev *dev, i2c_state state, uint32 timeout);

void i2c_bus_reset(const i2c_dev *dev);

/* Auxiliary procedure for enabling an I2C peripheral; `flags' as for
 * i2c_master_enable(). */
void i2c_set_ccr_trise(i2c_dev *dev, uint32 flags, uint32 freq);


/* IRQ enable/disable */

#if !defined(_I2C_HAVE_IRQ_FIXUP) || (_I2C_HAVE_IRQ_FIXUP == 0)
/* The series header provides this if _I2C_HAVE_IRQ_FIXUP is defined,
 * but we need it either way. */
#define _i2c_irq_priority_fixup(dev) ((void)0)
#endif

#define I2C_IRQ_ERROR              I2C_CR2_ITERREN
#define I2C_IRQ_EVENT              I2C_CR2_ITEVTEN
#define I2C_IRQ_BUFFER             I2C_CR2_ITBUFEN
/**
 * @brief Enable one or more I2C interrupts
 * @param dev I2C device
 * @param irqs Bitwise or of:
 *             I2C_IRQ_ERROR (error interrupt),
 *             I2C_IRQ_EVENT (event interrupt), and
 *             I2C_IRQ_BUFFER (buffer interrupt).
 */
static inline void i2c_enable_irq(i2c_dev *dev, uint32 irqs) {
    dev->regs->CR2 |= irqs;
}

/**
 * @brief Disable one or more I2C interrupts
 * @param dev I2C device
 * @param irqs Bitwise or of:
 *             I2C_IRQ_ERROR (error interrupt),
 *             I2C_IRQ_EVENT (event interrupt), and
 *             I2C_IRQ_BUFFER (buffer interrupt).
 */
static inline void i2c_disable_irq(i2c_dev *dev, uint32 irqs) {
    dev->regs->CR2 &= ~irqs;
}

/**
 * @brief Disable an I2C device
 *
 * This function disables the corresponding peripheral and marks dev's
 * state as I2C_STATE_DISABLED.
 *
 * @param dev Device to disable.
 */
static inline void i2c_disable(i2c_dev *dev) {
    i2c_disable_irq(dev, I2C_IRQ_BUFFER | I2C_IRQ_EVENT | I2C_IRQ_ERROR);   // Make sure IRQs are disabled in case we are switching master<->slave modes
    dev->regs->CR1 &= ~I2C_CR1_PE;
    dev->state = I2C_STATE_DISABLED;
}

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

void i2c_init(i2c_dev *dev);

/**
 * @brief Set input clock frequency, in MHz
 * @param dev I2C device
 * @param freq Frequency, in MHz. This must be at least 2, and at most
 *             the APB frequency of dev's bus. (For example, if
 *             rcc_dev_clk(dev) == RCC_APB1, freq must be at most
 *             PCLK1, in MHz). There is an additional limit of 36 MHz.
 */
static inline void i2c_set_input_clk(i2c_dev *dev, uint32 freq) {
// TODO : I2C_MAX_FREQ_MHZ should be in the series header, as it's different on STM32F4 and others!
#define I2C_MAX_FREQ_MHZ 36     // Limit on STM32F1 is 36 MHz (See ST Spec, which says: Higher than 0b100100 not allowed)
    if (freq < 2) freq = 2;
    if (freq > _i2c_bus_clk(dev)) freq = _i2c_bus_clk(dev);
    if (freq > I2C_MAX_FREQ_MHZ) freq = I2C_MAX_FREQ_MHZ;
    uint32 cr2 = dev->regs->CR2;
    cr2 &= ~I2C_CR2_FREQ;
    cr2 |= freq;
    dev->regs->CR2 = freq;
#undef I2C_MAX_FREQ_MHZ
}

/**
 * @brief Set I2C clock control register.
 *
 * See the chip reference manual for the details.
 *
 * @param dev I2C device
 * @param val Value to use for clock control register (in
 *            Fast/Standard mode)
 */
static inline void i2c_set_clk_control(i2c_dev *dev, uint32 val) {
    dev->regs->CCR = val;
}

/**
 * @brief Set SCL rise time
 * @param dev I2C device
 * @param trise Maximum rise time in fast/standard mode (see chip
 *              reference manual for the relevant formulas).
 */
static inline void i2c_set_trise(i2c_dev *dev, uint32 trise) {
    dev->regs->TRISE = trise;
}


/*
 * Slave support
 */

/* callback functions */
/* Callback handler for data received over the bus */
void i2c_slave_attach_recv_handler(i2c_dev *dev, i2c_msg *msg, i2c_slave_recv_callback_func func);

/* Callback handler for data being requested over the bus
 * The callback function must call i2c_write to get the data over the bus
 */
void i2c_slave_attach_transmit_handler(i2c_dev *dev, i2c_msg *msg, i2c_slave_xmit_callback_func func);

/**
 * @brief Set the primary I2c slave address
 * @param dev I2C device
 * @param address the 7 or 10 bit i2c address
  */
static inline void i2c_slave_set_own_address(i2c_dev *dev, uint16 address)
{
    // TODO : Add support for 10-bit addresses
    dev->regs->OAR1 = (address << 1) | 0x4000;     // According to ST Docs: Note: Bit 14 should always be kept at 1 by software!
}

/**
 * @brief Set the secondary I2c slave address
 * @param dev I2C device
 * @param address the 7 or 10 bit i2c address
  */
static inline void i2c_slave_set_own_address2(i2c_dev *dev, uint16 address)
{
    // TODO : Add support for 10-bit addresses
    dev->regs->OAR2 = (address << 1) | I2C_OAR2_ENDUAL;
}

#ifdef __cplusplus
}
#endif

#endif
