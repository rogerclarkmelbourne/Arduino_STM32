void OLED::_convert_float(char *buf, double num, int width, byte prec)
{
	char format[10];
	
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}


void OLED::_initTWI()
{
	if ((_sda_pin == SDA) and (_scl_pin == SCL))
	{
		twi = TWI1;
		pmc_enable_periph_clk(WIRE_INTERFACE_ID);
		PIO_Configure(g_APinDescription[PIN_WIRE_SDA].pPort, g_APinDescription[PIN_WIRE_SDA].ulPinType, g_APinDescription[PIN_WIRE_SDA].ulPin, g_APinDescription[PIN_WIRE_SDA].ulPinConfiguration);
		PIO_Configure(g_APinDescription[PIN_WIRE_SCL].pPort, g_APinDescription[PIN_WIRE_SCL].ulPinType, g_APinDescription[PIN_WIRE_SCL].ulPin, g_APinDescription[PIN_WIRE_SCL].ulPinConfiguration);
		NVIC_DisableIRQ(TWI1_IRQn);
		NVIC_ClearPendingIRQ(TWI1_IRQn);
		NVIC_SetPriority(TWI1_IRQn, 0);
		NVIC_EnableIRQ(TWI1_IRQn);

	}
	else if ((_sda_pin == SDA1) and (_scl_pin == SCL1))
	{
		twi = TWI0;
		pmc_enable_periph_clk(WIRE1_INTERFACE_ID);
		PIO_Configure(g_APinDescription[PIN_WIRE1_SDA].pPort, g_APinDescription[PIN_WIRE1_SDA].ulPinType, g_APinDescription[PIN_WIRE1_SDA].ulPin, g_APinDescription[PIN_WIRE1_SDA].ulPinConfiguration);
		PIO_Configure(g_APinDescription[PIN_WIRE1_SCL].pPort, g_APinDescription[PIN_WIRE1_SCL].ulPinType, g_APinDescription[PIN_WIRE1_SCL].ulPin, g_APinDescription[PIN_WIRE1_SCL].ulPinConfiguration);
		NVIC_DisableIRQ(TWI0_IRQn);
		NVIC_ClearPendingIRQ(TWI0_IRQn);
		NVIC_SetPriority(TWI0_IRQn, 0);
		NVIC_EnableIRQ(TWI0_IRQn);
	}

	// activate internal pullups for twi.
	digitalWrite(SDA, 1);
	digitalWrite(SCL, 1);

	// Reset the TWI
	twi->TWI_CR = TWI_CR_SWRST;
	// TWI Slave Mode Disabled, TWI Master Mode Disabled.
	twi->TWI_CR = TWI_CR_SVDIS;
	twi->TWI_CR = TWI_CR_MSDIS;
	// Set TWI Speed
	twi->TWI_CWGR = (TWI_DIV << 16) | (TWI_SPEED << 8) | TWI_SPEED;
	// Set master mode
	twi->TWI_CR = TWI_CR_MSEN;
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
		// Set slave address and number of internal address bytes.
		twi->TWI_MMR = (1 << 8) | (SSD1306_ADDR << 16);
		// Set internal address bytes
		twi->TWI_IADR = SSD1306_DATA_CONTINUE;
		// Send start address
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
			twi->TWI_THR = scrbuf[b];
			while ((twi->TWI_SR & TWI_SR_TXRDY) != TWI_SR_TXRDY) {};
		}
		else
		{
			_writeByte(scrbuf[b]);
			_waitForAck();
		}

	if (_use_hw)					// Send TWI Stop
	{
		// Send STOP condition
		twi->TWI_CR = TWI_CR_STOP;
		while ((twi->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {};
	}
	else
		_sendStop();
	interrupts();
}

void OLED::_sendTWIcommand(uint8_t value)
{
	if (_use_hw)
	{
		// Set slave address and number of internal address bytes.
		twi->TWI_MMR = (1 << 8) | (SSD1306_ADDR << 16);
		// Set internal address bytes
		twi->TWI_IADR = SSD1306_COMMAND;

		twi->TWI_THR = value;
		while ((twi->TWI_SR & TWI_SR_TXRDY) != TWI_SR_TXRDY) {};

		// Send STOP condition
		twi->TWI_CR = TWI_CR_STOP;
		while ((twi->TWI_SR & TWI_SR_TXCOMP) != TWI_SR_TXCOMP) {};
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
