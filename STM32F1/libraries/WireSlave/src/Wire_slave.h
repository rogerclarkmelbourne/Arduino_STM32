/*
 TwoWire.h - TWI/I2C library for Arduino & Wiring
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

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include <Stream.h>
#include <Arduino.h>
#include <libmaple/i2c.h>


// WIRE_HAS_END means Wire has end()
#ifndef WIRE_HAS_END
#define WIRE_HAS_END 1
#endif

#ifndef WIRE_INTERFACES_COUNT
#define WIRE_INTERFACES_COUNT 2
#endif

/* return codes from endTransmission() */
typedef enum EndTranmissionCodes {
	SUCCESS = 0,	/* transmission was successful */
	EDATA = 1,		/* too much data */
	ENACKADDR = 2,	/* received nack on transmit of address */
	ENACKTRNS = 3,	/* received nack on transmit of data */
	EOTHER = 4,		/* other error */
} EndTranmissionCodes;

class TwoWire: public Stream
{
private:
	i2c_dev* sel_hard;			// Pointer to the I2C device for this interface

	uint8_t *rxBuffer;			// Receive Buffer -- lazily allocated
	size_t rxBufferAllocated;	// Receive Buffer bytes allocated
	size_t rxBufferIndex;		// Receive Buffer index for next byte to read
	size_t rxBufferLength;		// Receive Buffer bytes stored within the space allocated

	uint8_t *txBuffer;			// Transmit Buffer -- lazily allocated
	size_t txBufferAllocated;	// Transmit Buffer bytes allocated
	size_t txBufferIndex;		// Transmit Buffer index of next byte to write (will always equal txBufferLength since we aren't doing random access, kept in case random access is added)
	size_t txBufferLength;		// Transmit Buffer bytes stored within the space allocated

	bool transmitting;			// Flag indicating a transmitting master (doesn't track slave transmissions)
	bool haveReset;				// Flag indicating initial power-on I2C reset status
	bool useGeneralCall;		// Flag indicating if I2C General Call slave address will be processed by slave

	uint32_t dev_flags;			// Flags used for enabling master or slave
	i2c_msg itc_msg;			// Master Tx/Rx Message and Slave Tx Message
	i2c_msg itc_slave_msg;		// Slave Rx Message (since it's completely asynchronous)

	uint32_t frequency;			// Frequency to use for I2C Master (defaults to 100000)

	void (*user_onRequest)(void);
	void (*user_onReceive)(int quantity);

protected:
	bool isMaster() const { return ((dev_flags & I2C_SLAVE_MODE) == 0); }
	void allocateRxBuffer(size_t length);
	void allocateTxBuffer(size_t length);

	uint8 process(bool stop = true);	// wrapper for i2c_master_xfer

	inline void __attribute__((always_inline)) onReceiveService(i2c_msg* msg);
	inline void __attribute__((always_inline)) onRequestService(i2c_msg* msg);

	static void onRequestService1(i2c_msg*);
	static void onReceiveService1(i2c_msg*);
#if WIRE_INTERFACES_COUNT > 1
	static void onRequestService2(i2c_msg*);
	static void onReceiveService2(i2c_msg*);
#endif

	TwoWire(i2c_dev* i2cDevice, uint32_t flags = I2C_SLAVE_GENERAL_CALL, uint32_t frequencyHz = 100000);
	TwoWire() = delete;
	TwoWire(const TwoWire&) = delete;
	TwoWire& operator=(const TwoWire&) = delete;
	TwoWire(TwoWire&&) = delete;
	TwoWire& operator=(TwoWire&&) = delete;

	static TwoWire g_Wire;
	#if WIRE_INTERFACES_COUNT > 1
	static TwoWire g_Wire1;
	#endif

public:
	// return the 'Wire' instance (using I2C1)
	static inline TwoWire& getInstance()
	{
		return g_Wire;
	}

#if WIRE_INTERFACES_COUNT > 1
	// return the 'Wire1' instance (using I2C2)
	static inline TwoWire& getInstance1()
	{
		return g_Wire1;
	}
#endif

	void begin();											// Master mode
	void begin(uint16_t myAddress);							// Single-Adddress Slave mode
	void begin(uint16_t myAddress1, uint16_t myAddress2);	// Dual-Address Slave mode
	void end();												// End either Master or Slave
	void setClock(uint32_t frequencyHz);					// Set Master clock speed
	uint32_t getClock() const { return frequency; }			// Get Master clock speed
	void beginTransmission(uint16_t slaveAddress);			// Master transmission start
	uint8_t endTransmission(bool sendStop = true);			// Master transmission end

	uint8_t requestFrom(uint16_t slaveAddress, uint8_t quantity);	// Master Request From Slave
	uint8_t requestFrom(uint16_t slaveAddress, uint8_t quantity, bool sendStop);	// Master Request From Slave
	uint8_t requestFrom(uint16_t slaveAddress, uint8_t num_bytes, uint32_t iaddress, uint8_t isize, bool sendStop);		// Master Request From Slave

	virtual size_t write(uint8_t data) override;	// Write functions for either Master transmission or Slave onRequest handler
	virtual size_t write(const void *data, uint32_t quantity) override;	// Note: this is the signature of the base Print::write function

	virtual int available(void) override;		// Bytes available in the receive buffer from either Master Request From or Slave onReceive handler
	virtual int read(void) override;			// Read byte from receive buffer from either Master Request From or Slave onReceive handler
	virtual int peek(void) override;			// Peek at next byte in receive buffer from either Master Request From or Slave onReceive handler
	virtual void flush(void) override;			// Finish the last transmission and wait for I2C to go idle.   Used to switch Master->Slave or Slave->Master modes
	void onReceive(void (*)(int quantity));		// Set Slave Receive Handler function
	void onRequest(void (*)(void));				// Set Slave Request Handler function

	// recvSlaveAddress : Can be called from the user-defined onReceive handler
	//	function to provide the slave address that the message was sent to.
	//	This will be either the primary or secondary address used in the call
	//	to begin() or will be '0' if it was a General Call (broadcast) message.
	uint16_t recvSlaveAddress() const { return itc_slave_msg.addr; }

	// setGeneralCall : Set whether or not the I2C General Call Slave address (00)
	//	will be processed.  Call this BEFORE calling begin() to activate a slave.
	//	If begin() was already called, call end(), change the state, and call
	//	begin() again, otherwise the state will not change.
	//	The default for this option is 'true'.
	void setGeneralCall(bool bEnable)
	{
		if (sel_hard->state == I2C_STATE_DISABLED) useGeneralCall = bEnable;	// Only allow change when disabled
	}

	// generalCall : Returns whether or not General Call Slave address (00)
	//	has been (or will be) enabled on the port configured as a slave.
	bool getGeneralCall() const { return useGeneralCall; }

	using Print::write;

	// Minimum I2C Buffer Length:
	static constexpr size_t BUFFER_LENGTH  = 36;		// Use 36 instead of 32 for minimum size so we can transfer 32 bytes of data in addition to addresses for EEPROM, etc.

private:
	uint8_t slaveRxBuffer[BUFFER_LENGTH];				// Fixed length slave IRQ receive buffer
};

extern TwoWire& Wire;
#if WIRE_INTERFACES_COUNT > 1
extern TwoWire& Wire1;
#endif

#endif // TwoWire_h
