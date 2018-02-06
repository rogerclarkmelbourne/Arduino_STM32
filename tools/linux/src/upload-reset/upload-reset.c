/* Copyright (C) 2015 Roger Clark <www.rogerclark.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 *
 * Utility to send the reset sequence on RTS and DTR and chars
 * which resets the libmaple and causes the bootloader to be run
 *
 *
 *
 * Terminal control code by Kir Kolyshkin <kolyshkin@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

static int fd = -1;

int openserial(char *devicename) 
{
	if ((fd = open(devicename, O_RDWR | O_NOCTTY)) == -1) {
		fprintf(stderr, "Can't open %s: %m\n\r", devicename);
		return -1;
	}

	return 0; // all good
}


/* flag can be TIOCM_RTS or TIOCM_DTR
 * level can be 1 (set) or 0 (clear)
 */
int setFlag(int flag, short level)
{
	int op = TIOCMBIC;
	if (level)
		op = TIOCMBIS;

	if (ioctl(fd, op, &flag) == -1) {
		perror("setFlag(): ioctl failed");
		return -1;
	}

	return 0; // all good
}

/* This portion of code was written by Roger Clark.
 * It was informed by various other pieces of code written
 * by Leaflabs to reset their Maple and Maple mini boards.
 * For example, see https://goo.gl/qJi16x
 */
int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3) {
		printf("Usage: upload-reset <serial_device> [<delay_in_milliseconds>]\n\r");
		return 1;
	}

	if (openserial(argv[1]) < 0) {
		fprintf(stderr, "Can't open serial %s\n\r", argv[1]);
		return 1;
	}

	// Old style magic sequence
	setFlag(TIOCM_RTS, 0);
	setFlag(TIOCM_DTR, 0);
	setFlag(TIOCM_DTR, 1);
	usleep(50000L);
	setFlag(TIOCM_DTR, 0);

	// New style
	setFlag(TIOCM_RTS, 1);
	setFlag(TIOCM_DTR, 1);
	usleep(50000L);
	setFlag(TIOCM_DTR, 0);
	if (write(fd, "1EAF", 4) != 4) {
		perror("write()");
	}

	if (argc == 3) {
		usleep(atol(argv[2])*1000L);
	}

	return 0;
}
