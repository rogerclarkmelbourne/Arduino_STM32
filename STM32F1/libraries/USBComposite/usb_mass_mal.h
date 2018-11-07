#ifndef __USB_MASS_MAL_H
#define __USB_MASS_MAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SCSI_BLOCK_SIZE							     512

#define USB_MASS_MAX_DRIVES  2

typedef bool (*MassStorageWriter)(const uint8_t *writebuff, uint32_t startSector, uint16_t numSectors);
typedef bool (*MassStorageReader)(uint8_t *readbuff, uint32_t startSector, uint16_t numSectors);
typedef bool (*MassStorageStatuser)(void);
typedef bool (*MassStorageInitializer)(void);
typedef bool (*MassStorageFormatter)(void);

typedef struct {
    uint32_t blockCount;
	MassStorageReader read;
	MassStorageWriter write;
	MassStorageStatuser status;
	MassStorageInitializer init;
	MassStorageFormatter format;
} MassStorageDriveInfo;

extern MassStorageDriveInfo usb_mass_drives[USB_MASS_MAX_DRIVES];
uint16_t usb_mass_mal_init(uint8_t lun);
uint16_t usb_mass_mal_get_status(uint8_t lun);
uint16_t usb_mass_mal_read_memory(uint8_t lun, uint8_t *readbuff, uint32_t startSector, uint16_t numSectors);
uint16_t usb_mass_mal_write_memory(uint8_t lun, uint8_t *writebuff, uint32_t startSector, uint16_t numSectors);
void usb_mass_mal_format(uint8_t lun);

#ifdef __cplusplus
}
#endif

#endif
