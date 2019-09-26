#include "USBComposite.h" 
#include "usb_mass.h"
#include "usb_mass_mal.h"
#include "usb_composite_serial.h"
#include <string.h>

void USBMassStorage::begin() {
	if(!enabled) {
		USBComposite.clear();
		registerComponent();
		USBComposite.begin();

		enabled = true;
	}
}

void USBMassStorage::end() {
	USBComposite.end();
}

void USBMassStorage::loop() {
	usb_mass_loop();
}

bool USBMassStorage::registerComponent() {
	return USBComposite.add(&usbMassPart, this);
}

void USBMassStorage::setDriveData(uint32 driveNumber, uint32 numSectors, MassStorageReader reader,
	MassStorageWriter writer, MassStorageStatuser statuser, MassStorageInitializer initializer) {
	if (driveNumber >= USB_MASS_MAX_DRIVES)
		return;
	usb_mass_drives[driveNumber].blockCount = numSectors;
	usb_mass_drives[driveNumber].read = reader;
	usb_mass_drives[driveNumber].write = writer;
	usb_mass_drives[driveNumber].status = statuser;
	usb_mass_drives[driveNumber].init = initializer;
	usb_mass_drives[driveNumber].format = initializer;
}

void USBMassStorage::clearDrives() {
	memset(usb_mass_drives, 0, sizeof(usb_mass_drives));
}

