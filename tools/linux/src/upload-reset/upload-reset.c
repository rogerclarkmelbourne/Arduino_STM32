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
 * Terminal control code by Heiko Noordhof (see copyright below)
 */



/* Copyright (C) 2003 Heiko Noordhof <heikyAusers.sf.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdbool.h>

/* Function prototypes (belong in a seperate header file) */
int   openserial(char *devicename);
void  closeserial(void);
int   setDTR(unsigned short level);
int   setRTS(unsigned short level); 


/* Two globals for use by this module only */
static int fd;
static struct termios oldterminfo;


void closeserial(void)
{
     tcsetattr(fd, TCSANOW, &oldterminfo);
     close(fd);
}


int openserial(char *devicename) 
{
     struct termios attr;

     if ((fd = open(devicename, O_RDWR)) == -1) return 0; /* Error */ 
     atexit(closeserial);

     if (tcgetattr(fd, &oldterminfo) == -1) return 0; /* Error */
     attr = oldterminfo;
     attr.c_cflag |= CRTSCTS | CLOCAL;
     attr.c_oflag = 0;
     if (tcflush(fd, TCIOFLUSH) == -1) return 0; /* Error */
     if (tcsetattr(fd, TCSANOW, &attr) == -1) return 0; /* Error */ 

     /* Set the lines to a known state, and */
     /* finally return non-zero is successful. */
     return setRTS(0) && setDTR(0);
}


/* For the two functions below:
 *     level=0 to set line to LOW
 *     level=1 to set line to HIGH
 */

int setRTS(unsigned short level)
{
     int status;

     if (ioctl(fd, TIOCMGET, &status) == -1) {
	  perror("setRTS(): TIOCMGET");
	  return 0;
     }
     if (level) status |= TIOCM_RTS;
     else status &= ~TIOCM_RTS;
     if (ioctl(fd, TIOCMSET, &status) == -1) {
	  perror("setRTS(): TIOCMSET");
	  return 0;
     }
     return 1;
}


int setDTR(unsigned short level)
{
     int status;

     if (ioctl(fd, TIOCMGET, &status) == -1) {
	  perror("setDTR(): TIOCMGET");
	  return 0;
     }
     if (level) status |= TIOCM_DTR;
     else status &= ~TIOCM_DTR;
     if (ioctl(fd, TIOCMSET, &status) == -1) {
	  perror("setDTR: TIOCMSET");
	  return 0;
     }
     return 1;
}

/* This portion of code was written by Roger Clark
 * It was informed by various other pieces of code written by Leaflabs to reset their 
 * Maple and Maple mini boards 
 */

main(int argc, char *argv[])
{
 	
	if (argc<2 || argc >3)
	{
		printf("Usage upload-reset <serial_device> <Optional_delay_in_milliseconds>\n\r");
		return;
	}

 	if (openserial(argv[1]))
	{
		// Send magic sequence of DTR and RTS followed by the magic word "1EAF"
		setRTS(false);
 		setDTR(false);
 		setDTR(true);

		usleep(50000L);

		setDTR(false);
		setRTS(true);
		setDTR(true);

		usleep(50000L);

		setDTR(false);

		usleep(50000L);

		write(fd,"1EAF",4);
 		
		closeserial();
		if (argc==3)
		{
			usleep(atol(argv[2])*1000L);
		}
	}
	else
	{
		printf("Failed to open serial device.\n\r");
	}
}
