#include "USBMassStorage.h"
#include "usb_mass.h"
#include "usb_mass_mal.h"
#include "usb_composite_serial.h"
#include <string.h>

void USBMassStorageDevice::begin() {
	if(!enabled) {
		USBComposite.clear();
		registerComponent();
		USBComposite.begin();

		enabled = true;
	}
}

void USBMassStorageDevice::end() {
	USBComposite.end();
}

void USBMassStorageDevice::loop() {
	usb_mass_loop();
}

bool USBMassStorageDevice::registerComponent() {
	return USBComposite.add(&usbMassPart, this);
}

void USBMassStorageDevice::setDrive(uint32 driveNumber, uint32 byteSize, MassStorageReader reader,
	MassStorageWriter writer, MassStorageStatuser statuser, MassStorageInitializer initializer) {
	if (driveNumber >= USB_MASS_MAX_DRIVES)
		return;
	usb_mass_drives[driveNumber].blockCount = byteSize/512;
	usb_mass_drives[driveNumber].read = reader;
	usb_mass_drives[driveNumber].write = writer;
	usb_mass_drives[driveNumber].status = statuser;
	usb_mass_drives[driveNumber].init = initializer;
	usb_mass_drives[driveNumber].format = initializer;
}

void USBMassStorageDevice::clearDrives() {
	memset(usb_mass_drives, 0, sizeof(usb_mass_drives));
}

USBMassStorageDevice MassStorage;
