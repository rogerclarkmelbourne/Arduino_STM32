#include <EEPROM.h>

int ledPin =  13;    // LED connected to digital pin 13
const char HELP_MSG[] = "Press :\r\n" \
			" 0 display configuration\r\n" \
			" 1 set configuration to 0x801F000 / 0x801F800 / 0x400 (RB MCU)\r\n" \
			" 2 set configuration to 0x801F000 / 0x801F800 / 0x800 (ZE/RE MCU)\r\n" \
			" 3 write/read variable\r\n" \
			" 4 increment address\r\n" \
			" 5 display pages top/bottom\r\n" \
			" 6 initialize EEPROM\r\n" \
			" 7 format EEPROM\r\n";
uint16 DataWrite = 0;
uint16 AddressWrite = 0x10;

void setup()
{
	// initialize the digital pin as an output:
	pinMode(ledPin, OUTPUT);
	SerialUSB.print(HELP_MSG);
}

void loop()
{
	uint16 Status;
	uint16 Data;

	while (SerialUSB.available())
	{
		char cmd = (char)SerialUSB.read();
		SerialUSB.println(cmd);
		if (cmd == '0')
		{
			DisplayConfig();
		}
		else if (cmd == '1')
		{
			EEPROM.PageBase0 = 0x801F000;
			EEPROM.PageBase1 = 0x801F800;
			EEPROM.PageSize  = 0x400;
			DisplayConfig();
		}
		else if (cmd == '2')
		{
			EEPROM.PageBase0 = 0x801F000;
			EEPROM.PageBase1 = 0x801F800;
			EEPROM.PageSize  = 0x800;
			DisplayConfig();
		}
		else if (cmd == '3')
		{
			Status = EEPROM.write(AddressWrite, DataWrite);
			SerialUSB.print("EEPROM.write(0x");
			SerialUSB.print(AddressWrite, HEX);
			SerialUSB.print(", 0x");
			SerialUSB.print(DataWrite, HEX);
			SerialUSB.print(") : Status : ");
			SerialUSB.println(Status, HEX);

			Status = EEPROM.read(AddressWrite, &Data);
			SerialUSB.print("EEPROM.read(0x");
			SerialUSB.print(AddressWrite, HEX);
			SerialUSB.print(", &..) = 0x");
			SerialUSB.print(Data, HEX);
			SerialUSB.print(" : Status : ");
			SerialUSB.println(Status, HEX);

			++DataWrite;
		}
		else if (cmd == '4')
		{
			++AddressWrite;
		}
		else if (cmd == '5')
		{
			DisplayPages(0x20);
			DisplayPagesEnd(0x20);
		}
		else if (cmd == '6')
		{
			Status = EEPROM.init();
			SerialUSB.print("EEPROM.init() : ");
			SerialUSB.println(Status, HEX);
			SerialUSB.println();
		}
		else if (cmd == '7')
		{
			Status = EEPROM.format();
			SerialUSB.print("EEPROM.format() : ");
			SerialUSB.println(Status, HEX);
		}
		else
			SerialUSB.print(HELP_MSG);
	}
	digitalWrite(ledPin, HIGH);
	delay(500);
	digitalWrite(ledPin, LOW);
	delay(500);
}

void DisplayConfig(void)
{
	SerialUSB.print  ("EEPROM.PageBase0 : 0x");
	SerialUSB.println(EEPROM.PageBase0, HEX);
	SerialUSB.print  ("EEPROM.PageBase1 : 0x");
	SerialUSB.println(EEPROM.PageBase1, HEX);
	SerialUSB.print  ("EEPROM.PageSize  : 0x");
	SerialUSB.print  (EEPROM.PageSize, HEX);
	SerialUSB.print  (" (");
	SerialUSB.print  (EEPROM.PageSize, DEC);
	SerialUSB.println(")");
}

void DisplayHex(uint16 value)
{
	if (value <= 0xF)
		SerialUSB.print("000");
	else if (value <= 0xFF)
		SerialUSB.print("00");
	else if (value <= 0xFFF)
		SerialUSB.print("0");
	SerialUSB.print(value, HEX);
}

void DisplayPages(uint32 endIndex)
{
	SerialUSB.println("Page 0     Top         Page 1");

	for (uint32 idx = 0; idx < endIndex; idx += 4)
	{
		SerialUSB.print  (EEPROM.PageBase0 + idx, HEX);
		SerialUSB.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		SerialUSB.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		SerialUSB.print  ("    ");
		SerialUSB.print  (EEPROM.PageBase1 + idx, HEX);
		SerialUSB.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		SerialUSB.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		SerialUSB.println();
	}
}

void DisplayPagesEnd(uint32 endIndex)
{
	SerialUSB.println("Page 0     Bottom      Page 1");

	for (uint32 idx = EEPROM.PageSize - endIndex; idx < EEPROM.PageSize; idx += 4)
	{
		SerialUSB.print  (EEPROM.PageBase0 + idx, HEX);
		SerialUSB.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		SerialUSB.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		SerialUSB.print  ("    ");
		SerialUSB.print  (EEPROM.PageBase1 + idx, HEX);
		SerialUSB.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		SerialUSB.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		SerialUSB.println();
	}
}
