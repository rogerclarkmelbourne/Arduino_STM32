#ifndef USBMASSSTORAGE_H
#define	USBMASSSTORAGE_H

#include <boards.h>
#include "USBComposite.h"
#include "usb_generic.h"
#include "usb_mass_mal.h"

class USBMassStorageDevice {
private:
  bool enabled = false;
public:
  void begin();
  void end();
  void loop();
  void clearDrives(void);
  bool registerComponent();
  void setDrive(uint32 driveNumber, uint32 byteSize, MassStorageReader reader,
	MassStorageWriter writer = NULL, MassStorageStatuser = NULL, MassStorageInitializer = NULL);
};

extern USBMassStorageDevice MassStorage;

#endif	/* USBMASSSTORAGE_H */

