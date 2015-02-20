/******************************************************************************
 * The MIT License
 *
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
 * @file libmaple/stm32f3/i2c.c
 * @author F3-port by Hanspeter Portner <dev@open-music-kontrollers.ch>
 * @brief STM32F3 I2C support
 */

#include "i2c_private.h"
#include <libmaple/i2c.h>
#include <libmaple/systick.h>

/*
 * Devices
 */

/* on GPIO_AF_4 (PA14,PA15) (PB7,PB6) (PB9,PB8)*/
static i2c_dev i2c1 = I2C_DEV_OLD(1, &gpiob, 7, 6);
/** STM32F3 I2C device 1 */
i2c_dev* const I2C1 = &i2c1;

/* on GPIO_AF_4 (PA10,PA9) (PF0,PF1) (PF0,PF6)*/
static i2c_dev i2c2 = I2C_DEV_OLD(2, &gpioa, 10, 9);
/** STM32F3 I2C device 2 */
i2c_dev* const I2C2 = &i2c2;

/*
 * Routines
 */

void i2c_config_gpios(const i2c_dev *dev) {
    gpio_set_modef(sda_port(dev), dev->sda_pin, GPIO_MODE_AF, GPIO_MODEF_TYPE_OD);
    gpio_set_modef(scl_port(dev), dev->scl_pin, GPIO_MODE_AF, GPIO_MODEF_TYPE_OD);

		gpio_set_af(sda_port(dev), dev->sda_pin, GPIO_AF_4);
		gpio_set_af(scl_port(dev), dev->scl_pin, GPIO_AF_4);
}

void i2c_master_release_bus(const i2c_dev *dev) {
    gpio_write_bit(scl_port(dev), dev->scl_pin, 1); /* TODO check this */
    gpio_write_bit(sda_port(dev), dev->sda_pin, 1);

		i2c_config_gpios(dev);
}

int32 wait_for_state_change(i2c_dev *dev,
																	i2c_state state,
																	uint32 timeout) {
	//FIXME not yet used on F3
	return 1;
}

void i2c_bus_reset(const i2c_dev *dev) {
	//FIXME not yet used on F3
}

void _i2c_software_reset(i2c_dev *dev) {
	dev->regs->CR1 &= ~I2C_CR1_PE;	
	while(dev->regs->CR1 & I2C_CR1_PE)
		;
	dev->regs->CR1 |= I2C_CR1_PE;
}

void i2c_master_enable(i2c_dev *dev, uint32 flags) {
	/* Bring the peripheral down for configuration */
	i2c_peripheral_disable(dev);

	/* Reset the bus when requested so */
	if (flags & I2C_BUS_RESET)
		i2c_bus_reset(dev);

	/* Turn on the clock and set GPIO modes */
	i2c_init(dev);
	i2c_config_gpios(dev);
	
	/* Configure analog and digital filters */
	/* TODO ANFOFF, DNF */

	/* Configure the clock and rise time */
	dev->regs->TIMINGR = I2C_TIMING_10_kHz;

	/* Configure NOSTRETCH behaviour */
	/* TODO NOSTRETCH */

	/* Enable all interrupts */ /* FIXME not yet used on F3 */
	//nvic_irq_enable(dev->ev_nvic_line);
	//nvic_irq_enable(dev->er_nvic_line);
	//i2c_enable_irq(dev, 0xFE); /* All interrupts */

	/* Make it go! */
	i2c_peripheral_enable(dev);

	dev->state = I2C_STATE_IDLE;
}

static void _i2c_master_xmit(i2c_dev *dev, i2c_msg *msg) {
	uint32 cr2;
	uint32 isr;
	uint16 remaining = msg->length;
	msg->xferred = 0;
	uint8 *ptr = msg->data; /* pointer to data buffer */

	while (remaining > 0) {
		uint32 reload = remaining > 0xFF ? I2C_CR2_RELOAD : 0U;
		uint16 size = remaining >= 0xFF ? 0xFF : remaining & 0xFF;
		uint32 nbytes = size << I2C_CR2_NBYTES_SHIFT;

		cr2 = dev->regs->CR2;
		cr2 &= ~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
		cr2 |= reload | nbytes;
		dev->regs->CR2 = cr2;

		if (msg->xferred == 0) /* first loop iteration? */
			dev->regs->CR2 |= I2C_CR2_START; /* generate START condition */

		while (ptr < msg->data + msg->xferred + size) {
			while ( !( (isr = dev->regs->ISR) & (I2C_ISR_NACKF | I2C_ISR_TXIS) ) )
				; /* wait until ready for the next byte or NACK*/

			if (isr & I2C_ISR_TXIS)
				dev->regs->TXDR = *ptr++; /* byte to send */
			else { /* NACKF */
				dev->regs->ICR |= I2C_ICR_NACKCF; /* clear NACK flag */
				dev->regs->CR2 |= I2C_CR2_STOP; /* generate STOP condition */
				return; /* STOP condition is automatically generated */
			}
		}

		if (reload)
			while ( !(dev->regs->ISR & I2C_ISR_TCR) )
				; /* wait for transmission reload complete */

		remaining -= size;
		msg->xferred += size;
	}

	while ( !(dev->regs->ISR & I2C_ISR_TC) )
		; /* wait for transmission complete */

	dev->regs->CR2 |= I2C_CR2_STOP; /* generate STOP condition */

	while ( !(dev->regs->ISR & I2C_ISR_STOPF) )
		; /* wait for STOP flag */

	dev->regs->ICR |= I2C_ICR_STOPCF; /* clear STOP flag */
}

static void _i2c_master_recv(i2c_dev *dev, i2c_msg *msg) {
	uint32 cr2;
	uint16 remaining = msg->length;
	msg->xferred = 0;
	uint8 *ptr = msg->data; /* pointer to data buffer */

	while (remaining > 0) {
		uint32 reload = remaining > 0xFF ? I2C_CR2_RELOAD : 0U;
		uint16 size = remaining >= 0xFF ? 0xFF : remaining & 0xFF;
		uint32 nbytes = size << I2C_CR2_NBYTES_SHIFT;

		cr2 = dev->regs->CR2;
		cr2 &= ~(I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN);
		cr2 |= reload | nbytes | I2C_CR2_RD_WRN;
		dev->regs->CR2 = cr2;

		if (msg->xferred == 0) /* first loop iteration? */
			dev->regs->CR2 |= I2C_CR2_START; /* generate START condition */

		while (ptr < msg->data + msg->xferred + size) {
			while ( !(dev->regs->ISR & I2C_ISR_RXNE) )
				; /* wait for next byte */

			*ptr++ = dev->regs->RXDR & I2C_RXDR_RXDATA; /* read byte */
		}

		if (reload)
			while ( !(dev->regs->ISR & I2C_ISR_TCR) )
				; /* wait for transmission reload complete */

		remaining -= size;
		msg->xferred += size;
	}

	while ( !(dev->regs->ISR & I2C_ISR_TC))
		; /* wait for transmission complete */

	dev->regs->CR2 |= I2C_CR2_STOP; /* generate STOP condition */

	while ( !(dev->regs->ISR & I2C_ISR_STOPF) )
		; /* wait for STOP flag */

	dev->regs->ICR |= I2C_ICR_STOPCF; /* clear STOP flag */
}

int32 i2c_master_xfer(i2c_dev *dev,
                      i2c_msg *msgs,
                      uint16 num,
                      uint32 timeout) {
	i2c_msg *msg;

	ASSERT(dev->state == I2C_STATE_IDLE);

	dev->msg = msgs;
	dev->msgs_left = num;
	dev->timestamp = systick_uptime(); /* FIXME implement handling of timeout */
	dev->state = I2C_STATE_BUSY;

	/* loop over messages */
	for (msg = dev->msg; dev->msgs_left; dev->msgs_left--, msg++) {
		/* set slave address and direction*/
		uint32 addr = msg->flags & I2C_MSG_10BIT_ADDR
									? I2C_CR2_ADD10 | (msg->addr << I2C_CR2_SADD_10_BIT_SHIFT)
									:	msg->addr << I2C_CR2_SADD_7_BIT_SHIFT;
		uint32 rd = msg->flags & I2C_MSG_READ ? I2C_CR2_RD_WRN : 0U;
		uint32 cr2 = dev->regs->CR2;
		cr2 &= ~(I2C_CR2_HEAD10R | I2C_CR2_ADD10 | I2C_CR2_SADD_10_BIT | I2C_CR2_RD_WRN);
		cr2 |= rd | addr;
		dev->regs->CR2 = cr2;

		if (rd)
			_i2c_master_recv(dev, msg);
		else
			_i2c_master_xmit(dev, msg);
	}

	dev->state = I2C_STATE_IDLE;

	return 0; /* FIXME implement error handling*/
}

void i2c_start_condition(i2c_dev *dev) {
	dev->regs->CR2 |= I2C_CR2_START; /* generate START condition */
}

void i2c_stop_condition(i2c_dev *dev) {
	dev->regs->CR2 |= I2C_CR2_STOP; /* generate STOP condition */

	while ( !(dev->regs->ISR & I2C_ISR_STOPF) )
		; /* wait for STOP flag */

	dev->regs->ICR |= I2C_ICR_STOPCF; /* clear STOP flag */
}

void i2c_enable_irq(i2c_dev *dev, uint32 irqs) {
	dev->regs->CR1 |= irqs;
}

void i2c_disable_irq(i2c_dev *dev, uint32 irqs) {
	dev->regs->CR1 &= ~irqs;
}

/* ACK/NACK */
void i2c_enable_ack(i2c_dev *dev) {
	//FIXME nonexistant on F3
}

void i2c_disable_ack(i2c_dev *dev) {
	//FIXME nonexistant on F3
}

void i2c_write(i2c_dev *dev, uint8 byte) {
	//FIXME proper handling of ISR
	dev->regs->TXDR = byte;
}

uint8 i2c_read(i2c_dev *dev) {
	//FIXME proper handling of ISR
	return dev->regs->RXDR & I2C_RXDR_RXDATA;
}

void _i2c_irq_handler(i2c_dev *dev) {
	//FIXME not yet used on F3
}

void _i2c_irq_error_handler(i2c_dev *dev) {
	//FIXME not yet used on F3
}

/*
 * IRQ handlers
 */

void __irq_i2c1_ev(void) {
   _i2c_irq_handler(I2C1);
}

void __irq_i2c2_ev(void) {
   _i2c_irq_handler(I2C2);
}

void __irq_i2c1_er(void) {
    _i2c_irq_error_handler(I2C1);
}

void __irq_i2c2_er(void) {
    _i2c_irq_error_handler(I2C2);
}
