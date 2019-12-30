#include <cstdlib>
#include <cstring>

/* We compile with nodefaultlibs, so we need to provide an error
 * handler for an empty pure virtual function */
extern "C" void __cxa_pure_virtual(void) {
    while(1)
        ;
}

/* We need to override it in order to avoid bloat binaries
 * due to using demangling of symbols in backtraces */
extern "C" char * __cxa_demangle (const char *mangled_name,
                                  char *output_buffer,
                                  size_t *length,
                                  int *status) {
  strcpy((char*)mangled_name, output_buffer);
  return output_buffer;
}
