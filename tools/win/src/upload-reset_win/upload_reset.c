#include <libserialport.h>
#include <stdio.h>
#include <windows.h>
/*
 * Utility to send the reset sequence on RTS and DTR and chars
 * which resets the libmaple and causes the bootloader to be run
 */

/* Helper function for error handling. */
int check(enum sp_return result)
{
  char *error_message;

  switch (result) {
  case SP_ERR_ARG:
    printf("Error: Invalid argument.\n");
    exit(SP_ERR_ARG);
  case SP_ERR_FAIL:
    error_message = sp_last_error_message();
    printf("Error: Failed: %s\n", error_message);
    sp_free_error_message(error_message);
    exit(SP_ERR_FAIL);
  case SP_ERR_SUPP:
    printf("Error: Not supported.\n");
    exit(SP_ERR_SUPP);
  case SP_ERR_MEM:
    printf("Error: Couldn't allocate memory.\n");
    exit(SP_ERR_MEM);
  case SP_OK:
  default:
    return result;
  }
}

int main(int argc, char **argv)
{
  if (argc<2 || argc >3)
  {
    printf("Usage %s <serial port> <Optional delay in milliseconds>\n", argv[0]);
    return -1;
  }

  char *port_name = argv[1];
  /* A pointer to a struct sp_port, which will refer to
   * the port found. */
  struct sp_port *port;
  printf("Looking for port %s.\n", port_name);
  /* Call sp_get_port_by_name() to find the port. The port
   * pointer will be updated to refer to the port found. */
  check(sp_get_port_by_name(port_name, &port));

  /* Display some basic information about the port. */
  printf("Port name: %s\n", sp_get_port_name(port));
  printf("Description: %s\n", sp_get_port_description(port));

  /* The port must be open to access its configuration. */
  printf("Opening port.\n");
  check(sp_open(port, SP_MODE_READ_WRITE));
  check(sp_set_flowcontrol(port, SP_FLOWCONTROL_RTSCTS));
  check(sp_flush(port, SP_BUF_BOTH));
  // check(sp_set_rts(port, SP_RTS_OFF));
  // check(sp_set_dtr(port, SP_DTR_OFF));

  // Send magic sequence of DTR and RTS followed by the magic word "1EAF"
  check(sp_set_rts(port, SP_RTS_OFF));
  check(sp_set_dtr(port, SP_DTR_OFF));
  check(sp_set_dtr(port, SP_DTR_ON));
  Sleep(50);
  check(sp_set_dtr(port, SP_DTR_OFF));
  check(sp_set_rts(port, SP_RTS_ON));
  check(sp_set_dtr(port, SP_DTR_ON));
  Sleep(50);
  check(sp_set_dtr(port, SP_DTR_OFF));
  Sleep(50);
  check(sp_blocking_write(port, "1EAF", 4, 1000));

  /* Now clean up by closing the port and freeing structures. */
  check(sp_close(port));
  sp_free_port(port);
  if (argc==3)
  {
    Sleep(atol(argv[2]));
  }
  return 0;
}
