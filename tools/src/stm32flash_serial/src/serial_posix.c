/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2010 Geoffrey McRae <geoff@spacevs.com>

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
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "serial.h"
#include "port.h"

struct serial {
	int fd;
	struct termios oldtio;
	struct termios newtio;
	char setup_str[11];
};

static serial_t *serial_open(const char *device)
{
	serial_t *h = calloc(sizeof(serial_t), 1);

	h->fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (h->fd < 0) {
		free(h);
		return NULL;
	}
	fcntl(h->fd, F_SETFL, 0);

	tcgetattr(h->fd, &h->oldtio);
	tcgetattr(h->fd, &h->newtio);

	return h;
}

static void serial_flush(const serial_t *h)
{
	tcflush(h->fd, TCIFLUSH);
}

static void serial_close(serial_t *h)
{
	serial_flush(h);
	tcsetattr(h->fd, TCSANOW, &h->oldtio);
	close(h->fd);
	free(h);
}

static port_err_t serial_setup(serial_t *h, const serial_baud_t baud,
			       const serial_bits_t bits,
			       const serial_parity_t parity,
			       const serial_stopbit_t stopbit)
{
	speed_t	port_baud;
	tcflag_t port_bits;
	tcflag_t port_parity;
	tcflag_t port_stop;
	struct termios settings;

	switch (baud) {
		case SERIAL_BAUD_1200:    port_baud = B1200; break;
		case SERIAL_BAUD_1800:    port_baud = B1800; break;
		case SERIAL_BAUD_2400:    port_baud = B2400; break;
		case SERIAL_BAUD_4800:    port_baud = B4800; break;
		case SERIAL_BAUD_9600:    port_baud = B9600; break;
		case SERIAL_BAUD_19200:   port_baud = B19200; break;
		case SERIAL_BAUD_38400:   port_baud = B38400; break;
		case SERIAL_BAUD_57600:   port_baud = B57600; break;
		case SERIAL_BAUD_115200:  port_baud = B115200; break;
		case SERIAL_BAUD_230400:  port_baud = B230400; break;
#ifdef B460800
		case SERIAL_BAUD_460800:  port_baud = B460800; break;
#endif /* B460800 */
#ifdef B921600
		case SERIAL_BAUD_921600:  port_baud = B921600; break;
#endif /* B921600 */
#ifdef B500000
		case SERIAL_BAUD_500000:  port_baud = B500000; break;
#endif /* B500000 */
#ifdef B576000
		case SERIAL_BAUD_576000:  port_baud = B576000; break;
#endif /* B576000 */
#ifdef B1000000
		case SERIAL_BAUD_1000000: port_baud = B1000000; break;
#endif /* B1000000 */
#ifdef B1500000
		case SERIAL_BAUD_1500000: port_baud = B1500000; break;
#endif /* B1500000 */
#ifdef B2000000
		case SERIAL_BAUD_2000000: port_baud = B2000000; break;
#endif /* B2000000 */

		case SERIAL_BAUD_INVALID:
		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (bits) {
		case SERIAL_BITS_5: port_bits = CS5; break;
		case SERIAL_BITS_6: port_bits = CS6; break;
		case SERIAL_BITS_7: port_bits = CS7; break;
		case SERIAL_BITS_8: port_bits = CS8; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (parity) {
		case SERIAL_PARITY_NONE: port_parity = 0; break;
		case SERIAL_PARITY_EVEN: port_parity = PARENB; break;
		case SERIAL_PARITY_ODD:  port_parity = PARENB | PARODD; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	switch (stopbit) {
		case SERIAL_STOPBIT_1: port_stop = 0;	   break;
		case SERIAL_STOPBIT_2: port_stop = CSTOPB; break;

		default:
			return PORT_ERR_UNKNOWN;
	}

	/* reset the settings */
#ifndef __sun		/* Used by GNU and BSD. Ignore __SVR4 in test. */
	cfmakeraw(&h->newtio);
#else /* __sun */
	h->newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR
			       | IGNCR | ICRNL | IXON);
        if (port_parity)
                h->newtio.c_iflag |= INPCK;

	h->newtio.c_oflag &= ~OPOST;
	h->newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	h->newtio.c_cflag &= ~(CSIZE | PARENB);
	h->newtio.c_cflag |= CS8;
#endif /* __sun */
#ifdef __QNXNTO__
	h->newtio.c_cflag &= ~(CSIZE | IHFLOW | OHFLOW);
#else
	h->newtio.c_cflag &= ~(CSIZE | CRTSCTS);
#endif
	h->newtio.c_cflag &= ~(CSIZE | CRTSCTS);
	h->newtio.c_iflag &= ~(IXON | IXOFF | IXANY | IGNPAR);
	h->newtio.c_lflag &= ~(ECHOK | ECHOCTL | ECHOKE);
	h->newtio.c_oflag &= ~(OPOST | ONLCR);

	/* setup the new settings */
	cfsetispeed(&h->newtio, port_baud);
	cfsetospeed(&h->newtio, port_baud);
	h->newtio.c_cflag |=
		port_parity	|
		port_bits	|
		port_stop	|
		CLOCAL		|
		CREAD;

	h->newtio.c_cc[VMIN] = 0;
	h->newtio.c_cc[VTIME] = 5;	/* in units of 0.1 s */

	/* set the settings */
	serial_flush(h);
	if (tcsetattr(h->fd, TCSANOW, &h->newtio) != 0)
		return PORT_ERR_UNKNOWN;

/* this check fails on CDC-ACM devices, bits 16 and 17 of cflag differ!
 * it has been disabled below for now -jcw, 2015-11-09
    if (settings.c_cflag != h->newtio.c_cflag)
        fprintf(stderr, "c_cflag mismatch %lx\n",
                settings.c_cflag ^ h->newtio.c_cflag);
 */

	/* confirm they were set */
	tcgetattr(h->fd, &settings);
	if (settings.c_iflag != h->newtio.c_iflag ||
	    settings.c_oflag != h->newtio.c_oflag ||
	  //settings.c_cflag != h->newtio.c_cflag ||
	    settings.c_lflag != h->newtio.c_lflag)
		return PORT_ERR_UNKNOWN;

	snprintf(h->setup_str, sizeof(h->setup_str), "%u %d%c%d",
		 serial_get_baud_int(baud),
		 serial_get_bits_int(bits),
		 serial_get_parity_str(parity),
		 serial_get_stopbit_int(stopbit));
	return PORT_ERR_OK;
}

/* 
 * Roger clark.
 * This function is no longer used. But has just been commented out in case it needs 
 * to be reinstated in the future
 
static int startswith(const char *haystack, const char *needle) {
	return strncmp(haystack, needle, strlen(needle)) == 0;
}
*/

static int is_tty(const char *path) {
	char resolved[PATH_MAX];

	if(!realpath(path, resolved)) return 0;
	

	/*
	 * Roger Clark
	 * Commented out this check, because on OSX some devices are /dev/cu
	 * and some users use symbolic links to devices, hence the name may not even start
	 * with /dev
	 
	if(startswith(resolved, "/dev/tty")) return 1;
	
	return 0;
	*/
	
	return 1;
}

static port_err_t serial_posix_open(struct port_interface *port,
				    struct port_options *ops)
{
	serial_t *h;

	/* 1. check device name match */
	if (!is_tty(ops->device))
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

static port_err_t serial_posix_close(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	serial_close(h);
	port->private = NULL;
	return PORT_ERR_OK;
}

static port_err_t serial_posix_read(struct port_interface *port, void *buf,
				     size_t nbyte)
{
	serial_t *h;
	ssize_t r;
	uint8_t *pos = (uint8_t *)buf;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		r = read(h->fd, pos, nbyte);
		if (r == 0)
			return PORT_ERR_TIMEDOUT;
		if (r < 0)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
	}
	return PORT_ERR_OK;
}

static port_err_t serial_posix_write(struct port_interface *port, void *buf,
				      size_t nbyte)
{
	serial_t *h;
	ssize_t r;
	const uint8_t *pos = (const uint8_t *)buf;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	while (nbyte) {
		r = write(h->fd, pos, nbyte);
		if (r < 1)
			return PORT_ERR_UNKNOWN;

		nbyte -= r;
		pos += r;
	}
	return PORT_ERR_OK;
}

static port_err_t serial_posix_gpio(struct port_interface *port,
				    serial_gpio_t n, int level)
{
	serial_t *h;
	int bit, lines;

	h = (serial_t *)port->private;
	if (h == NULL)
		return PORT_ERR_UNKNOWN;

	switch (n) {
	case GPIO_RTS:
		bit = TIOCM_RTS;
		break;

	case GPIO_DTR:
		bit = TIOCM_DTR;
		break;

	case GPIO_BRK:
		if (level == 0)
			return PORT_ERR_OK;
		if (tcsendbreak(h->fd, 1))
			return PORT_ERR_UNKNOWN;
		return PORT_ERR_OK;

	default:
		return PORT_ERR_UNKNOWN;
	}

	/* handle RTS/DTR */
	if (ioctl(h->fd, TIOCMGET, &lines))
		return PORT_ERR_UNKNOWN;
	lines = level ? lines | bit : lines & ~bit;
	if (ioctl(h->fd, TIOCMSET, &lines))
		return PORT_ERR_UNKNOWN;

	return PORT_ERR_OK;
}

static const char *serial_posix_get_cfg_str(struct port_interface *port)
{
	serial_t *h;

	h = (serial_t *)port->private;
	return h ? h->setup_str : "INVALID";
}

struct port_interface port_serial = {
	.name	= "serial_posix",
	.flags	= PORT_BYTE | PORT_GVR_ETX | PORT_CMD_INIT | PORT_RETRY,
	.open	= serial_posix_open,
	.close	= serial_posix_close,
	.read	= serial_posix_read,
	.write	= serial_posix_write,
	.gpio	= serial_posix_gpio,
	.get_cfg_str	= serial_posix_get_cfg_str,
};
