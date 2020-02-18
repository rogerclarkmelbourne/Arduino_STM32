#include <stdlib.h>

int __wrap_atexit(void (*function)(void) __attribute__((unused))) { return 0; }

int __wrap___cxa_atexit(void (*func) (void *) __attribute__((unused)), void * arg __attribute__((unused)), void * dso_handle __attribute__((unused))) { return 0; }

__attribute__((noreturn)) void __wrap_exit(int status __attribute__((unused))) { while(1); }
