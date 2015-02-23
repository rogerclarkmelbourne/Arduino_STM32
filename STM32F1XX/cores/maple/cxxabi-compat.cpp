/* We compile with nodefaultlibs, so we need to provide an error
 * handler for an empty pure virtual function */
extern "C" void __cxa_pure_virtual(void) {
    while(1);
}
/* Don't suck in exceptions, they bloat the code size */
namespace __gnu_cxx {
  void __verbose_terminate_handler() {
      while(1);
  }
}
