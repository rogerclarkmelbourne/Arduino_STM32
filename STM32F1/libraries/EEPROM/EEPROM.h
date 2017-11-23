#ifndef __EEPROM_H
#define __EEPROM_H

#include <wirish.h>
#include "flash_stm32.h"

/* Page status definitions */
#define EEPROM_ERASED			((uint16)0xFFFF)	/* PAGE is empty */
#define EEPROM_RECEIVE_DATA		((uint16)0xEEEE)	/* PAGE is marked to receive data */
#define EEPROM_VALID_PAGE		((uint16)0x0000)	/* PAGE containing valid data */

/* Page full define */
enum : uint16
	{
	EEPROM_OK				= ((uint16)0x0000),
	EEPROM_OUT_SIZE			= ((uint16)0x0081),
	EEPROM_BAD_ADDRESS		= ((uint16)0x0082),
	EEPROM_BAD_FLASH		= ((uint16)0x0083),
	EEPROM_NOT_INIT			= ((uint16)0x0084),
	EEPROM_SAME_VALUE		= ((uint16)0x0085),
	EEPROM_NO_VALID_PAGE	= ((uint16)0x00AB)
	};

#define EEPROM_DEFAULT_DATA		0xFFFF


class EEPROMClass
{
public:
	EEPROMClass(void);

	uint16 init(void);
	uint16 init(uint32, uint32, uint32);

	uint16 format(void);

	uint16 erases(uint16 *);
	uint16 read (uint16 address);
	uint16 read (uint16 address, uint16 *data);
	uint16 write(uint16 address, uint16 data);
	uint16 update(uint16 address, uint16 data);
	uint16 count(uint16 *);
	uint16 maxcount(void);

	uint32 PageBase0;
	uint32 PageBase1;
	uint32 PageSize;
	uint16 Status;
private:
	FLASH_Status EE_ErasePage(uint32);

	uint16 EE_CheckPage(uint32, uint16);
	uint16 EE_CheckErasePage(uint32, uint16);
	uint16 EE_Format(void);
	uint32 EE_FindValidPage(void);
	uint16 EE_GetVariablesCount(uint32, uint16);
	uint16 EE_PageTransfer(uint32, uint32, uint16);
	uint16 EE_VerifyPageFullWriteVariable(uint16, uint16);
};

extern EEPROMClass EEPROM;

#endif	/* __EEPROM_H */
