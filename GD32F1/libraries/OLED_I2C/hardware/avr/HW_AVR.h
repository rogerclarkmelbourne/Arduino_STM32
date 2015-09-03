void OLED::_convert_float(char *buf, double num, int width, byte prec)
{
	dtostrf(num, width, prec, buf);
}

void OLED::_initTWI()
{
	// activate internal pullups for twi.
	digitalWrite(SDA, HIGH);
	digitalWrite(SCL, HIGH);
	//delay(1);  // Workaround for a linker bug

	// initialize twi prescaler and bit rate
	__cbi2(TWSR, TWPS0);
	__cbi2(TWSR, TWPS1);
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

	// enable twi module, acks, and twi interrupt
	TWCR = _BV(TWEN) | _BV(TWIE)/* | _BV(TWEA)*/;
}

void OLED::update()
{
	noInterrupts();
	_sendTWIcommand(SSD1306_SET_COLUMN_ADDR);
	_sendTWIcommand(0);
	_sendTWIcommand(127);

	_sendTWIcommand(SSD1306_SET_PAGE_ADDR);
	_sendTWIcommand(0);
	_sendTWIcommand(7);
	
	if (_use_hw)					// Send TWI Start
	{
		// Send start address
		TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);
		while ((TWCR & _BV(TWINT)) == 0) {};
		TWDR = SSD1306_ADDR<<1;
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);
		while ((TWCR & _BV(TWINT)) == 0) {};
		TWDR = SSD1306_DATA_CONTINUE;
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);
		while ((TWCR & _BV(TWINT)) == 0) {};
	}
	else
	{
		_sendStart(SSD1306_ADDR<<1);
		_waitForAck();
		_writeByte(SSD1306_DATA_CONTINUE);
		_waitForAck();
	}

	for (int b=0; b<1024; b++)		// Send data
		if (_use_hw)
		{
			TWDR = scrbuf[b];
			TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);									// Clear TWINT to proceed
			while ((TWCR & _BV(TWINT)) == 0) {};										// Wait for TWI to be ready
		}
		else
		{
			_writeByte(scrbuf[b]);
			_waitForAck();
		}

	if (_use_hw)					// Send TWI Stop
		TWCR = _BV(TWEN)| _BV(TWINT) | _BV(TWSTO);									// Send STOP
	else
		_sendStop();
	interrupts();
}

void OLED::_sendTWIcommand(uint8_t value)
{
	if (_use_hw)
	{
		// Send start address
		TWCR = _BV(TWEN) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);						// Send START
		while ((TWCR & _BV(TWINT)) == 0) {};										// Wait for TWI to be ready
		TWDR = SSD1306_ADDR<<1;
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);									// Clear TWINT to proceed
		while ((TWCR & _BV(TWINT)) == 0) {};										// Wait for TWI to be ready

		TWDR = SSD1306_COMMAND;
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);									// Clear TWINT to proceed
		while ((TWCR & _BV(TWINT)) == 0) {};										// Wait for TWI to be ready
		TWDR = value;
		TWCR = _BV(TWEN) | _BV(TWINT) | _BV(TWEA);									// Clear TWINT to proceed
		while ((TWCR & _BV(TWINT)) == 0) {};										// Wait for TWI to be ready

		TWCR = _BV(TWEN)| _BV(TWINT) | _BV(TWSTO);									// Send STOP
	}
	else
	{
		_sendStart(SSD1306_ADDR<<1);
		_waitForAck();
		_writeByte(SSD1306_COMMAND);
		_waitForAck();
		_writeByte(value);
		_waitForAck();
		_sendStop();
	}
}
