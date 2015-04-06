#if defined(__WIN32__) || defined(__CYGWIN__)
#   include "serial_w32.c"
#else
#   include "serial_posix.c"
#endif
