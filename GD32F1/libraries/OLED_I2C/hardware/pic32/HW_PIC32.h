void OLED::_convert_float(char *buf, double num, int width, byte prec)
{
	char format[10];
	
	sprintf(format, "%%%i.%if", width, prec);
	sprintf(buf, format, num);
}

inline void _waitForIdleBus() { while (I2C1CON & 0x1f) {} }

void OLED::_initTWI()
{
	uint32_t	tpgd;

	pinMode(SDA, OUTPUT);
	digitalWrite(SDA, HIGH);
	IFS0CLR = 0xE0000000;									// Clear Interrupt Flag
	IEC0CLR = 0xE0000000;									// Disable Interrupt
	I2C1CONCLR = (1 << _I2CCON_ON);							// Disable I2C interface
	tpgd = ((F_CPU / 8) * 104) / 125000000;
	I2C1BRG = (F_CPU / (2 * TWI_FREQ) - tpgd) - 2;			// Set I2C Speed
	I2C1ADD = SSD1306_ADDR;									// Set I2C device address
	I2C1CONSET = (1 << _I2CCON_ON) | (1 << _I2CCON_STREN);	// Enable I2C Interface
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
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
		I2C1TRN = (SSD1306_ADDR<<1);						// Send device Write address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (SSD1306_ADDR<<1);					// Retry send device Write address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = SSD1306_DATA_CONTINUE;					// Send the command for continous data
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
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
			I2C1TRN = scrbuf[b];
			while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}
			while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}
		}
		else
		{
			_writeByte(scrbuf[b]);
			_waitForAck();
		}

	if (_use_hw)					// Send TWI Stop
	{
		I2C1CONSET = (1 << _I2CCON_PEN);					// Send stop condition
		while (I2C1CON & (1 << _I2CCON_PEN)) {}				// Wait for stop condition to finish
	}
	else
		_sendStop();
	interrupts();
}

void OLED::_sendTWIcommand(uint8_t value)
{
	if (_use_hw)
	{
		_waitForIdleBus();									// Wait for I2C bus to be Idle before starting
		I2C1CONSET = (1 << _I2CCON_SEN);					// Send start condition
		if (I2C1STAT & (1 << _I2CSTAT_BCL)) { return; }		// Check if there is a bus collision
		while (I2C1CON & (1 << _I2CCON_SEN)) {}				// Wait for start condition to finish
		I2C1TRN = (SSD1306_ADDR<<1);						// Send device Write address
		while (I2C1STAT & (1 << _I2CSTAT_IWCOL))			// Check if there is a Write collision
		{
			I2C1STATCLR = (1 << _I2CSTAT_IWCOL);			// Clear Write collision flag
			I2C1TRN = (SSD1306_ADDR<<1);					// Retry send device Write address
		}
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = SSD1306_COMMAND;							// Send the 1st data byte
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1TRN = value;									// Send the 2nd data byte
		while (I2C1STAT & (1 << _I2CSTAT_TRSTAT)) {}		// Wait for transmit to finish
		while (I2C1STAT & (1 << _I2CSTAT_ACKSTAT)) {}		// Wait for ACK
		I2C1CONSET = (1 << _I2CCON_PEN);					// Send stop condition
		while (I2C1CON & (1 << _I2CCON_PEN)) {}				// Wait for stop condition to finish
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
