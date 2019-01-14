# Windows program for listing COM (serial) ports.
#
# enumerate_serial_ports() is by Eli Bendersky:
#
# http://eli.thegreenplace.net/2009/07/31/listing-all-serial-ports-on-windows-with-python/

import _winreg as winreg
import itertools

def enumerate_serial_ports():
    """ Uses the Win32 registry to return an
        iterator of serial (COM) ports
        existing on this computer.
    """
    path = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
    try:
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, path)
    except WindowsError:
        raise IterationError

    for i in itertools.count():
        try:
            val = winreg.EnumValue(key, i)
            yield str(val[1])
        except EnvironmentError:
            break

for com in enumerate_serial_ports():
    print com
