/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2010 Geoffrey McRae <geoff@spacevs.com>
  Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <ctype.h>

#include "serial.h"
#include "port.h"

struct serial {
	HANDLE fd;
	DCB oldtio;
	DCB newtio;
	char setup_str[11];
};

static serial_t *serial_open(const char *device)
{
	serial_t *h = calloc(sizeof(serial_t), 1);
	char *devName;

	/* timeout in ms */
	COMMTIMEOUTS timeouts = {MAXDWORD, MAXDWORD, 500, 0, 0};

	/* Fix the device name if required */
	if (strlen(device) > 4 && device[0] != '\\') {
		devName = calloc(1, strlen(device) + 5);
		sprintf(devName, "\\\\.\\%s", device);
	} else {
		devName = (char *)device;
	}

	/* Create file handle for port */
	h->fd = CreateFile(devName, GENERIC_READ | GENERIC_WRITE,
			   0,		/* Exclusive access */
			   NULL,	/* No security */
			   OPEN_EXISTING,
			   0,		/* No overlap */
			   NULL);

	if (devName != device)
		free(devName);

	if (h->fd == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			fprintf(stderr, "File not found: %s\n", device);
		return NULL;
	}

	SetupComm(h->fd, 4096, 4096); /* Set input and output buffer size */

	SetCommTimeouts(h->fd, &timeouts);

	SetCommMask(h->fd, EV_ERR); /* Notify us of error events */

	GetCommState(h->fd, &h->oldtio); /* Retrieve port parameters */
	GetCommState(h->fd, &h->newtio); /* Retrieve port parameters */

	/* PurgeComm(h->fd, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR); */

	return h;
}

static void serial_flush(const serial_t *h)
{
	/* We shouldn't need to flush in non-overlapping (blocking) mode */
	/* tcflush(h->fd, TCIFLUSH); */
}

static void serial_close(serial_t *h)
{
	serial_flush(h);
	SetCommState(h->fd, &h->oldtio);
	CloseHandle(h->fd);
	free(h);
}

static port_err_t serial_setup(serial_t *h,
			       const serial_baud_t baud,
			       const serial_bits_t bits,
			       const serial_parity_t parity,
			       const serial_stopbit_t stopbit)
{
	switch (baud) {
		case SERIAL_BAUD_1200:    h->newtio.BaudRate = CBR_1200; break;
		/* case SERIAL_BAUD_1800: h->newtio.BaudRate = CBR_1800; break; */
		case SERIAL_BAUD_2400:    h->newtio.BaudRate = CBR_2400; break;
		case SERIAL_BAUD_4800:    h->newtio.BaudRate = CBR_4800; break;
		case SERIAL_BAUD_9600:    h->newtio.BaudRate = CBR_9600; break;
		case SERIAL_BAUD_19200:   h->newtio.BaudRate = CBR_19200; break;
		case SERIAL_BAUD_38400:   h->newtio.BaudRate = CBR_38400; break;
		case SERIAL_BAUD_57600:   h->newtio.BaudRate = CBR_57600; break;
		case SERIAL_BAUD_115200:  h->newtio.BaudRate = CBR_115200; break;
		case SERIAL_BAUD_128000:  h->newtio.BaudRate = CBR_128000; break;
		case SERIAL_BAUD_256000:  h->newtio.BaudRate = CBR_256000; break;
		/* These are not defined in WinBase.h and might work or not */
		case SERIAL_BAUD_230400:  h->newtio.BaudRate = 230400; break;
		case SERIAL_BAUD_460800:  h->newtio.BaudRate = 460800; break;
		case SERIAL_BAUD_500000:  h->newtio.BaudRate = 500000; break;
		case SERIAL_BAUD_576000:  h->newtio.BaudRate = 576000; break;
		case SERIAL_BAUD_921600:  h->newtio.BaudRate = 921600; break;
		case SERIAL_BAUD_1000000: h->newtio.BaudRate = 1000000; break;
		case SERIAL_BAUD_1500000: h->newtio.BaudRate = 1500000; break;
		case SERIAL_BAUD_2000000: h->newtio.BaudRate = 2000000; break;
		case SERIAL_BAUD_INVALID:

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (bits) {
		case SERIAL_BITS_5: h->newtio.ByteSize = 5; break;
		case SERIAL_BITS_6: h->newtio.ByteSize = 6; break;
		case SERIAL_BITS_7: h->newtio.ByteSize = 7; break;
		case SERIAL_BITS_8: h->newtio.ByteSize = 8; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (parity) {
		case SERIAL_PARITY_NONE: h->newtio.Parity = NOPARITY;   break;
		case SERIAL_PARITY_EVEN: h->newtio.Parity = EVENPARITY; break;
		case SERIAL_PARITY_ODD:  h->newtio.Parity = ODDPARITY;  break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (stopbit) {
		case SERIAL_STOPBIT_1: h->newtio.StopBits = ONESTOPBIT;	 break;
		case SERIAL_STOPBIT_2: h->newtio.StopBits = TWOSTOPBITS; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	/* reset the settings */
	h->newtio.fOutxCtsFlow = FALSE;
	h->newtio.fOutxDsrFlow = FALSE;
	h->newtio.fOutX = FALSE;
	h->newtio.fInX = FALSE;
	h->newtio.fNull = 0;
	h->newtio.fAbortOnError = 0;

	/* set the settings */
	serial_flush(h);
	if (!SetCommState(h->fd, &h->newtio))
		return PORT_ERR_UNKNOWN;

	snprintf(h->setup_str, sizeof(h->setup_str), "%u %d%c%d",
		serial_get_baud_int(baud),
		serial_get_bits_int(bits),
		serial_get_parity_str(parity),
		serial_get_stopbit_int(stopbit)
	);
	return PORT_ERR_OK;
}

static port_err_t serial_w32_open(struct port_interface *port,
				  struct port_options *ops)
{
	serial_t *h;

	/* 1. check device name match */
	if (!((strlen(ops->device) == 4 || strlen(ops->device) == 5)
	      && !strncmp(ops->device, "COM", 3) && isdigit(ops->device[3]))
	    && !(!strncmp(ops->device, "\\\\.\\COM", strlen("\\\\.\\COM"))
		 && isdigit(ops->device[strlen("\\\\.\\COM")])))
		return PORT_ERR_NODEV;

	/* 2. check options */
	if (ops->baudRate == SERIAL_BAUD_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_bits(ops->serial_mode) == SERIAL_BITS_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_parity(ops->serial_mode) == SERIAL_PARITY_INVALID)
		return PORT_ERR_UNKNOWN;
	if (serial_get_stopbit(ops->serial_mode) == SERIAL_STOPBIT_INVALID)
		return PORT_ERR_UNKNOWN;

	/* 3. open it */
	h = serial_open(ops->device);
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	/* 4. set options */
	if (serial_setup(h, ops->baudRate,
	    serial_get_bits(ops->serial_mode),
	    serial_get_parity(ops->serial_mode),
	    serial_get_stopbit(ops->serial_mode)
	   ) != PORT_ERR_OK) {
		serial_close(h);
		return PORT_ERR_UNKNOWN;
	}

	port->private = h;
	return PORT_ERR_OK;
}

static port_err_t serial_w32_close(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	serial_close(h);
	port->private = NULL;
	return PORT_ERR_OK;
}

static port_err_t serial_w32_read(struct port_interface *port, void *buf,
				  size_t nbyte)
{
	serial_t *h;
	DWORD r;
	uint8_t *pos = (uint8_t *)buf;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		ReadFile(h->fd, pos, nbyte, &r, NULL);
		if (r == 0)
			return PORT_ERR_TIMEDOUT;
		if (r < 0)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
	}
	return PORT_ERR_OK;
}

static port_err_t serial_w32_write(struct port_interface *port, void *buf,
				   size_t nbyte)
{
	serial_t *h;
	DWORD r;
	uint8_t *pos = (uint8_t *)buf;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		if (!WriteFile(h->fd, pos, nbyte, &r, NULL))
			return PORT_ERR_UNKNOWN;
		if (r < 1)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
	}
	return PORT_ERR_OK;
}

static port_err_t serial_w32_gpio(struct port_interface *port,
				  serial_gpio_t n, int level)
{
	serial_t *h;
	int bit;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	switch (n) {
	case GPIO_RTS:
		bit = level ? SETRTS : CLRRTS;
		break;

	case GPIO_DTR:
		bit = level ? SETDTR : CLRDTR;
		break;

	case GPIO_BRK:
		if (level == 0)
			return PORT_ERR_OK;
		if (EscapeCommFunction(h->fd, SETBREAK) == 0)
			return PORT_ERR_UNKNOWN;
		usleep(500000);
		if (EscapeCommFunction(h->fd, CLRBREAK) == 0)
			return PORT_ERR_UNKNOWN;
		return PORT_ERR_OK;

	default:
		return PORT_ERR_UNKNOWN;
	}

	/* handle RTS/DTR */
	if (EscapeCommFunction(h->fd, bit) == 0)
		return PORT_ERR_UNKNOWN;

	return PORT_ERR_OK;
}

static const char *serial_w32_get_cfg_str(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	return h ? h->setup_str : "INVALID";
}

struct port_interface port_serial = {
	.name	= "serial_w32",
	.flags	= PORT_BYTE | PORT_GVR_ETX | PORT_CMD_INIT | PORT_RETRY,
	.open	= serial_w32_open,
	.close	= serial_w32_close,
	.read	= serial_w32_read,
	.write	= serial_w32_write,
	.gpio	= serial_w32_gpio,
	.get_cfg_str	= serial_w32_get_cfg_str,
};
