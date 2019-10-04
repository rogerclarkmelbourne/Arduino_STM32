/*
  TwoWire.cpp - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts

  Modified 2019 by Donna Whisnant to merge WireSlave changes with the core to
  make slave mode work and without having conflicting data type defintions and
  to improve the WireSlave functionality, completing some of the missing pieces.

 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "Wire_slave.h"
#include <wirish.h>
#include <libmaple/i2c.h>

static constexpr uint16_t MASTER_ADDRESS = 0x01;			// Use 0x01 since it's an invalid device address. 0x00 = General Call, but 0x01 is reserved for Start Byte

// Constructors ////////////////////////////////////////////////////////////////

TwoWire::TwoWire(i2c_dev* i2cDevice, uint32_t flags, uint32_t frequencyHz)
	:	sel_hard(i2cDevice),
		rxBuffer(nullptr),
		rxBufferAllocated(0),
		rxBufferIndex(0),
		rxBufferLength(0),
		txBuffer(nullptr),
		txBufferAllocated(0),
		txBufferIndex(0),
		txBufferLength(0),
		transmitting(false),
		haveReset(false),
		useGeneralCall((flags & I2C_SLAVE_GENERAL_CALL) ? true : false),
		dev_flags(flags),
		itc_msg({}),
		itc_slave_msg({}),
		frequency((flags & I2C_FAST_MODE) ? 400000 : frequencyHz),
		user_onRequest(nullptr),
		user_onReceive(nullptr)
{
}

// Public Methods //////////////////////////////////////////////////////////////

void TwoWire::begin(void)
{
	begin(MASTER_ADDRESS, MASTER_ADDRESS);
}

void TwoWire::begin(uint16_t address)
{
	begin(address, MASTER_ADDRESS);
}

void TwoWire::begin(uint16_t myAddress1, uint16_t myAddress2)
{
	rxBufferIndex = 0;
	rxBufferLength = 0;
	allocateRxBuffer(BUFFER_LENGTH);

	txBufferIndex = 0;
	txBufferLength = 0;
	allocateTxBuffer(BUFFER_LENGTH);

	transmitting = false;

	// Note: Leave speed set as configured in dev_flags
	//	in case user calls setClock() before begin().
	if (myAddress1 == MASTER_ADDRESS) {
		dev_flags &= (I2C_FAST_MODE | I2C_DUTY_16_9);		// Note: Clear I2C_SLAVE_MODE flag
		i2c_master_enable(sel_hard, dev_flags | (!haveReset ? I2C_PUP_RESET : 0), frequency);
	} else {
		dev_flags &= (I2C_FAST_MODE | I2C_DUTY_16_9);
		dev_flags |= I2C_SLAVE_MODE;
		if (myAddress2 != MASTER_ADDRESS) dev_flags |= I2C_SLAVE_DUAL_ADDRESS;
		if (useGeneralCall) dev_flags |= I2C_SLAVE_GENERAL_CALL;

		itc_slave_msg.addr = myAddress1;
		itc_slave_msg.flags = 0;
		itc_slave_msg.data = slaveRxBuffer;
		itc_slave_msg.length = sizeof(slaveRxBuffer);	// length will be the size of the receive buffer that the IRQ routine can use
		itc_slave_msg.xferred = 0;

		itc_msg.addr = 0;
		itc_msg.flags = 0;
		itc_msg.data = txBuffer;
		itc_msg.length = 0;
		itc_msg.xferred = 0;

		if (sel_hard->regs == I2C1_BASE) {
			// attach  receive handler
			i2c_slave_attach_recv_handler(sel_hard, &itc_slave_msg, onReceiveService1);
			// attach transmit handler
			i2c_slave_attach_transmit_handler(sel_hard, &itc_msg, onRequestService1);
		}
		#if WIRE_INTERFACES_COUNT > 1
		else if (sel_hard->regs == I2C2_BASE) {
			// attach  receive handler
			i2c_slave_attach_recv_handler(sel_hard, &itc_slave_msg, onReceiveService2);
			// attach transmit handler
			i2c_slave_attach_transmit_handler(sel_hard, &itc_msg, onRequestService2);
		}
		#endif

		i2c_slave_enable(sel_hard, dev_flags | (!haveReset ? I2C_PUP_RESET : 0), frequency);		// Must enable first so that RCC, etc, get configured

		// TODO : Add support for 10-bit addresses	
		i2c_slave_set_own_address(sel_hard, myAddress1);
		if (dev_flags & I2C_SLAVE_DUAL_ADDRESS) {
			i2c_slave_set_own_address2(sel_hard, myAddress2);
		}
	}

	haveReset = true;
}

void TwoWire::end(void)
{
	flush();							// Make sure all transmissions are complete
	i2c_disable(sel_hard);				// Disable peripheral
	i2c_master_release_bus(sel_hard);	// Release pins and switch the port pins from AF back to GPIO
	free(txBuffer);
	txBuffer = nullptr;
	txBufferAllocated = 0;
	free(rxBuffer);
	rxBuffer = nullptr;
	rxBufferAllocated = 0;
}

void TwoWire::setClock(uint32_t frequencyHz)
{
	switch (frequencyHz) {
		case 400000:
			dev_flags |= I2C_FAST_MODE;		// set FAST_MODE bit
			break;
		case 100000:
		default:
			dev_flags &= ~I2C_FAST_MODE;	// clear FAST_MODE bit
			break;
	}
	frequency = frequencyHz;
	if (sel_hard->regs->CR1 & I2C_CR1_PE) {
		i2c_master_enable(sel_hard, dev_flags, frequency);
	}
}

uint8 TwoWire::process(bool stop)
{
	if (!stop) itc_msg.flags |= I2C_MSG_NOSTOP;
	int8 res = i2c_master_xfer(sel_hard, &itc_msg, 1, getTimeout());
	itc_msg.flags &= ~I2C_MSG_NOSTOP;

	if (res == I2C_ERROR_PROTOCOL) {
		if (sel_hard->error_flags & I2C_SR1_AF) { /* NACK */
			res = (sel_hard->error_flags & I2C_SR1_ADDR ? ENACKADDR : ENACKTRNS);
		} else if (sel_hard->error_flags & I2C_SR1_OVR) { /* Over/Underrun */
			res = EDATA;
		} else { /* Bus or Arbitration error */
			res = EOTHER;
		}
	} else if (res == I2C_ERROR_TIMEOUT) {
		res = EOTHER;		// Arduino API spec doesn't have a special error for timeout so use EOTHER
	} else if (res != 0) {
		res = EOTHER;
	}

	if (sel_hard->state == I2C_STATE_ERROR) {
		// All errors should reset the bus:
		i2c_master_enable(sel_hard, (I2C_BUS_RESET | dev_flags), frequency);	// This will transition the state back to IDLE
	}

	return res;
}

//TODO: Add the ability to queue messages (adding a boolean to end of function
// call, allows for the Arduino style to stay while also giving the flexibility
// to bulk send
uint8 TwoWire::requestFrom(uint16_t address, uint8_t num_bytes, uint32_t iaddress, uint8_t isize, bool sendStop)
{
	if (!isMaster()) return 0;

	allocateRxBuffer(num_bytes);
	// error if no memory block available to allocate the buffer
	if (rxBuffer == nullptr) {
		return 0;		// This function returns number of bytes returned from slave (so can't return an error code value!)
	}

	// reset rx buffer iterator vars
	rxBufferIndex = 0;
	rxBufferLength = 0;

	itc_msg.addr = address;
	itc_msg.flags = I2C_MSG_READ;
	itc_msg.length = num_bytes;
	itc_msg.data = rxBuffer;
	if (process(sendStop) != 0) {
		itc_msg.flags = 0;		// Clear message flags because we are no longer reading
		return 0;				// Failure means we received no data back
	}

	// TODO handle iaddress & isize
	rxBufferLength += itc_msg.xferred;
	itc_msg.flags = 0;

	return itc_msg.xferred;
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity, bool sendStop)
{
	return requestFrom(address, quantity, (uint32_t) 0, (uint8_t) 0, sendStop);
}

uint8_t TwoWire::requestFrom(uint16_t address, uint8_t quantity)
{
	return requestFrom(address, quantity, (uint32_t) 0, (uint8_t) 0, true);
}

void TwoWire::beginTransmission(uint16_t address)
{
	if (isMaster()) {
		// indicate that we are transmitting
		transmitting = true;

		// reset tx buffer iterator vars
		txBufferIndex = 0;
		txBufferLength = 0;

		itc_msg.addr = address;
		itc_msg.data = txBuffer;
		itc_msg.length = 0;
		itc_msg.flags = 0;
	}
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
uint8_t TwoWire::endTransmission(bool sendStop)
{
	uint8_t ret = EOTHER;

	if (isMaster() && transmitting) {
		itc_msg.data = txBuffer;
		itc_msg.length = txBufferLength;
		itc_msg.flags = 0;
		ret = process(sendStop);

		// reset tx buffer iterator vars
		txBufferIndex = 0;
		txBufferLength = 0;

		// indicate that we are done transmitting
		transmitting = false;
	}

	return ret;
}


// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t TwoWire::write(uint8_t data)
{
	if (!transmitting && isMaster()) {
		return 0;
	} else {
		// in master transmitter mode or slave tx event callback
		allocateTxBuffer(txBufferLength + 1);
		// error if no memory block available to allocate the buffer
		if (txBuffer == nullptr) {
			setWriteError();
			return 0;
		}
		// put byte in tx buffer
		txBuffer[txBufferIndex] = data;
		++txBufferIndex;
		// update amount in buffer
		txBufferLength = txBufferIndex;
	}
	return 1;
}

/**
 * @brief  This function must be called in slave Tx event callback or after
 *         beginTransmission() and before endTransmission().
 * @param  pdata: pointer to the buffer data
 * @param  quantity: number of bytes to write
 * @retval number of bytes ready to write.
 */
size_t TwoWire::write(const void *data, uint32_t quantity)
{
	if (!transmitting && isMaster()) {
		return 0;
	} else {
		// in master transmitter mode or slave Tx event callback
		allocateTxBuffer(txBufferLength + quantity);
		// error if no memory block available to allocate the buffer
		if (txBuffer == nullptr) {
			setWriteError();
			return 0;
		}
		// put bytes in tx buffer
		memcpy(&txBuffer[txBufferIndex], data, quantity);
		txBufferIndex = txBufferIndex + quantity;
		// update amount in buffer
		txBufferLength = txBufferIndex;
		return quantity;
	}
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::available(void)
{
	return rxBufferLength - rxBufferIndex;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::read(void)
{
	int value = -1;

	// get each successive byte on each call
	if (rxBufferIndex < rxBufferLength) {
		value = rxBuffer[rxBufferIndex++];
	}

	return value;
}

// must be called in:
// slave rx event callback
// or after requestFrom(address, numBytes)
int TwoWire::peek(void)
{
	int value = -1;

	if (rxBufferIndex < rxBufferLength) {
		value = rxBuffer[rxBufferIndex];
	}

	return value;
}

void TwoWire::flush(void)
{
	if (sel_hard->state != I2C_STATE_DISABLED) {		// Can't flush if we are disabled
		if (!isMaster()) {
			if (sel_hard->state == I2C_STATE_BUSY) {	// It should already be in the busy state, but check in case there was an error and no timeout
				// If this is a slave, let flush make sure
				//	any pending onRequestService has completed
				//	so we can turn around and reconfigure as a
				//	master for multi-master bus transmission:
				wait_for_state_change(sel_hard, I2C_STATE_IDLE, getTimeout());
			}

			// Since this function has no return value and is
			//	an override of the underlying flush() function,
			//	then force-change our state to idle to cover
			//	error and timeout conditions:
			sel_hard->state = I2C_STATE_IDLE;
		} else {
			// If this is a master, finish any transmission
			//	that hasn't been ended:
			endTransmission(true);
		}
	}
}

// behind the scenes function that is called when data is received
void __attribute__((always_inline)) TwoWire::onReceiveService(i2c_msg* msg)
{
	// don't bother if user hasn't registered a callback
	if (!user_onReceive) {
		return;
	}

	// copy twi rx buffer into local read buffer, enabling new
	//	reads to happen in parallel
	allocateRxBuffer(msg->xferred);
	rxBufferIndex = 0;
	if (rxBuffer != nullptr) {
		memcpy(rxBuffer, msg->data, msg->xferred);
		rxBufferLength = msg->xferred;
		// alert user program
		user_onReceive(msg->xferred);
	} else {
		rxBufferLength = 0;
		user_onReceive(0);		// Alert the user program that we received something, but couldn't transfer the data
	}
}

// behind the scenes function that is called when data is requested
void __attribute__((always_inline)) TwoWire::onRequestService(i2c_msg* msg)
{
	// don't bother if user hasn't registered a callback
	if (!user_onRequest) return;

	// reset tx buffer iterator vars for slave data write
	// !!! this will kill any pending pre-master sendTo() activity
	txBufferIndex = 0;
	txBufferLength = 0;
	// alert user program
	user_onRequest();

	// update i2c_msg
	msg->data = txBuffer;				// This assignment is necessary as the writes in user function may change the pointer
	msg->length = txBufferLength;

	// Note: Caller can't do Master transmission in multi-master until Slave
	//	transmission is complete! (they should call flush() before switching to master!)
}

// sets function called on slave write
void TwoWire::onReceive(void (*function)(int))
{
	user_onReceive = function;
}

// sets function called on slave read
void TwoWire::onRequest(void (*function)(void))
{
	user_onRequest = function;
}

/**
 * @brief  Allocate the Rx/Tx buffer to the requested length if needed
 * @note   Minimum allocated size is BUFFER_LENGTH)
 * @param  length: number of bytes to allocate
 */
inline void TwoWire::allocateRxBuffer(size_t length)
{
	// By default we allocate BUFFER_LENGTH bytes. It is the min size of the buffer.
	if (length < BUFFER_LENGTH) length = BUFFER_LENGTH;
	// Allocate in increments of BUFFER_LENGTH so we don't fragment the heap:
	if ((length % BUFFER_LENGTH) != 0) length += (BUFFER_LENGTH - (length % BUFFER_LENGTH));
	if (rxBufferAllocated < length) {
		rxBuffer = (uint8_t *) realloc(rxBuffer, length * sizeof(uint8_t));
		rxBufferAllocated = (rxBuffer != nullptr) ? length : 0;
	}
}

inline void TwoWire::allocateTxBuffer(size_t length)
{
	// By default we allocate BUFFER_LENGTH bytes. It is the min size of the buffer.
	if (length < BUFFER_LENGTH) length = BUFFER_LENGTH;
	// Allocate in increments of BUFFER_LENGTH so we don't fragment the heap:
	if ((length % BUFFER_LENGTH) != 0) length += (BUFFER_LENGTH - (length % BUFFER_LENGTH));
	if (txBufferAllocated < length) {
		txBuffer = (uint8_t *) realloc(txBuffer, length * sizeof(uint8_t));
		txBufferAllocated = (txBuffer != nullptr) ? length : 0;
	}
}


// Preinstantiate Objects //////////////////////////////////////////////////////

TwoWire TwoWire::g_Wire(I2C1);
#if WIRE_INTERFACES_COUNT > 1
TwoWire TwoWire::g_Wire1(I2C2);
#endif

TwoWire& Wire = TwoWire::getInstance();
#if WIRE_INTERFACES_COUNT > 1
TwoWire& Wire1 = TwoWire::getInstance1();
#endif


// onRequestServiceX and onReceiveServiceX can't be inline since they
// are exclusively called via a function pointer
void TwoWire::onRequestService1(i2c_msg* msg)
{
	Wire.onRequestService(msg);
}

void TwoWire::onReceiveService1(i2c_msg* msg)
{
	Wire.onReceiveService(msg);
}

#if WIRE_INTERFACES_COUNT > 1
void TwoWire::onRequestService2(i2c_msg* msg)
{
	Wire1.onRequestService(msg);
}
void TwoWire::onReceiveService2(i2c_msg* msg)
{
	Wire1.onReceiveService(msg);
}
#endif
