
#ifndef PORTABLE_H
#define PORTABLE_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef HAVE_USLEEP
# include <unistd.h>
# define milli_sleep(msec) usleep(1000 * (msec))
#elif defined HAVE_WINDOWS_H
# define milli_sleep(msec) Sleep(msec)
#else
# error "Can't get no sleep! Please report"
#endif /* HAVE_USLEEP */

#endif /* PORTABLE_H */
