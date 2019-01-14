#ifndef USBMASSSTORAGE_H
#define	USBMASSSTORAGE_H

#include <boards.h>
#include "USBComposite.h"
#include "usb_generic.h"
#include "usb_mass_mal.h"

class USBMassStorage {
private:
  bool enabled = false;
public:
  void begin();
  void end();
  void loop();
  void clearDrives(void);
  bool registerComponent();
  void setDriveData(uint32 driveNumber, uint32 numSectors, MassStorageReader reader,
	MassStorageWriter writer = NULL, MassStorageStatuser = NULL, MassStorageInitializer = NULL);
};

#endif	/* USBMASSSTORAGE_H */

