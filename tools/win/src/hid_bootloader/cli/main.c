/*
* STM32 HID Bootloader - USB HID bootloader for STM32F10X
* Copyright (c) 2018 Bruno Freitas - bruno@brunofreitas.com
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Modified 20 April 2018
*	by Vassilis Serasidis <info@serasidis.gr>
*	Now, this HID bootloader work with bluepill + STM32duino + Arduino IDE <http://www.stm32duino.com/>
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "rs232.h"
#include "hidapi.h"

int serial_init(int ser_num) ;


static int usb_write(hid_device *device, uint8_t *buffer, int len) {
	int retries = 20;
	int retval;

	while(((retval = hid_write(device, buffer, len)) < len) && --retries) {
		if(retval < 0) {
			usleep(100 * 1000); // No data has been sent here. Delay and retry.
			//sleep(1);
		} else {
			return 0; // Partial data has been sent. Firmware will be corrupted. Abort process.
		}
	}

	if(retries <= 0) {
		return 0;
	}

	return 1;
}

int main(int argc, char *argv[]) {
	uint8_t page_data[1024];
	uint8_t hid_buffer[129];
	uint8_t CMD_RESET_PAGES[8] = {'B','T','L','D','C','M','D', 0x00};
	hid_device *handle = NULL;
	size_t read_bytes;
	FILE *config_file = NULL, *firmware_file = NULL;
	int error = 0;

	setbuf(stdout, NULL);

	printf("HID-Flash v1.0h - STM32 HID Bootloader Flash Tool\n");
	printf("(c) 04/2018 - Bruno Freitas - http://www.brunofreitas.com/\n\n");
	
	if(argc != 3) {
		printf("Usage: hid-flash <firmware_bin_file> <COM port>\n");

		return 1;
	}
	
	int ser_num = atoi(argv[2]);
	serial_init(ser_num);
	hid_init();

	handle = hid_open(0xF055, 0x0001, NULL);

	if (!handle) {
		printf("Unable to open device.\n");
		error = 1;
		goto exit;
	}

	firmware_file = fopen(argv[1], "rb");
	if(!firmware_file) {
		printf("Error opening firmware file: %s\n", argv[2]);
		error = 1;
		goto exit;
	}

	// Send RESET PAGES command to put HID bootloader in initial stage...
	memset(hid_buffer, 0, sizeof(hid_buffer));
	memcpy(&hid_buffer[1], CMD_RESET_PAGES, sizeof(CMD_RESET_PAGES));

	printf("Sending reset pages command...\n");

	// Flash is unavailable when writing to it, so USB interrupt may fail here
	if(!usb_write(handle, hid_buffer, 129)) {
		printf("Error while sending reset pages command.\n");
		error = 1;
		goto exit;
	}

	memset(hid_buffer, 0, sizeof(hid_buffer));

	fseek(firmware_file, 0, SEEK_SET);

	// Send Firmware File data
	printf("Flashing firmware...\n");

	memset(page_data, 0, sizeof(page_data));
	read_bytes = fread(page_data, 1, sizeof(page_data), firmware_file);

	while(read_bytes > 0) {

		for(int i = 0; i < 1024; i += 128) {
			memcpy(&hid_buffer[1], page_data + i, 128);

			// Flash is unavailable when writing to it, so USB interrupt may fail here
			if(!usb_write(handle, hid_buffer, 129)) {
				printf("Error while flashing firmware data.\n");
				error = 1;
				goto exit;
			}
		}

		memset(page_data, 0, sizeof(page_data));
		read_bytes = fread(page_data, 1, sizeof(page_data), firmware_file);
	}

	printf("Ok!\n");

	exit:

	if(handle) {
		hid_close(handle);
	}

	hid_exit();

	if(firmware_file) {
		fclose(firmware_file);
	}

	return error;
}

int serial_init(int ser_num) {
	//printf("aaa %i",ser_num);
  int cport_nr = ser_num - 1;			/* COM port number (ex COM6 must be number 5*/
  int	bdrate=9600;		/* 9600 baud */

  char mode[]={'8','N','1',0};
	unsigned char magic[4] = "1EAF";
	
  //printf("hello\n");
  if(RS232_OpenComport(cport_nr, bdrate, mode)){
    printf("Can not open com-port\n");

    return(0);
  }
	
	printf("Sending Magic sequence \"1EAF\"\n");
	RS232_disableRTS(cport_nr);
	RS232_disableDTR(cport_nr);
  RS232_enableDTR(cport_nr);
  usleep(50000L);
	RS232_disableDTR(cport_nr);
	RS232_enableRTS(cport_nr);
  RS232_enableDTR(cport_nr);
	usleep(50000L);
	RS232_disableDTR(cport_nr);
	usleep(50000L);
	RS232_SendBuf(cport_nr, magic, 4); //Magic sequence
	RS232_CloseComport(cport_nr);
	sleep(3);
	
	return 0;
}