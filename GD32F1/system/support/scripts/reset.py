#!/usr/bin/env python

from __future__ import print_function

import serial
import os
import platform
import sys
import time
from struct import pack

def unix_get_maple_path(file_prefix, dev_is_maple=lambda dev: True):
    """Try to find the device file for the Maple on *nix.

    This function works assuming that the device file globs like
    '/dev/<file_prefix>*'. The caller may pass an additional
    dev_is_maple predicate if the platform supports additional tests
    to determine if a device is a Maple.

    If there are multiple possibilities, ask the user what to do.  If
    the user chooses not to say, returns None."""
    possible_paths = [os.path.join('/dev', x) for x in os.listdir('/dev') \
                      if x.startswith(file_prefix) and dev_is_maple(x)]
    return choose_path(possible_paths)

def linux_get_maple_path(file_prefix='ttyACM'):
    """Specialized unix_get_maple_path() for Linux.

    Attempts to check that a candidate device has the correct ID in
    the /sys tree when deciding if it's a Maple."""
    return unix_get_maple_path(file_prefix, linux_tty_is_maple)

def linux_tty_is_maple(device):
    try:
        sysfile = open("/sys/class/tty/%s/device/uevent" % device, "r")
        text = "".join(sysfile.readlines())
        return "PRODUCT=1eaf/4" in text
    except IOError: # no udev info available
        return True

def windows_get_maple_path():
    """Similar to unix_get_maple_path(), but on Windows."""
    import _winreg as reg
    p = 'HARDWARE\\DEVICEMAP\\SERIALCOMM'
    k = reg.OpenKey(reg.HKEY_LOCAL_MACHINE, p)
    possible_paths = []
    i = 0
    while True:
        try:
            possible_paths.append(reg.EnumValue(k, i)[1])
            i += 1
        except WindowsError:
            break
    return choose_path(possible_paths)

def choose_path(possible_paths):
    if len(possible_paths) == 0:
        return None
    elif len(possible_paths) == 1:
        return possible_paths[0]
    else:
        print('Found multiple candidates for the Maple device:')
        return choose_among_options(possible_paths)

def choose_among_options(options):
    for (i,p) in enumerate(options):
        print('\t%d. %s' % (i+1, p))

    prompt = 'Enter a number to select one, or q to quit: '
    while True:
        resp = raw_input(prompt).strip().lower()
        if resp == 'q': sys.exit()

        try:
            i = int(resp, 10)
        except ValueError:
            pass
        else:
            if 0 <= i-1 < len(options):
                return options[i-1]

        prompt = 'Please enter a number from the list, or q to quit: '

plat_sys = platform.system()
plat_bits = platform.architecture()[0]
if plat_sys == 'Linux':
    maple_path = linux_get_maple_path()
    # fall back on /dev/maple if that doesn't work
    if maple_path is None:
        maple_path = '/dev/maple'
        print('Could not find Maple serial port; defaulting to /dev/maple.')
elif plat_sys == 'Darwin':
    maple_path = unix_get_maple_path('tty.usbmodem')
elif plat_sys == 'Windows':
    maple_path = windows_get_maple_path()
else:
    maple_path = raw_input('Unrecognized platform.  Please enter '
                           "the path to the Maple's serial port device file:")

if maple_path is None:
    print('Could not find the Maple serial port for reset.',
          'Perhaps this is your first upload, or the board is already',
          'in bootloader mode.')
    print()
    print("If your sketch doesn't upload, try putting your Maple",
          'into bootloader mode manually by pressing the RESET button',
          'then letting it go and quickly pressing button BUT',
          '(hold for several seconds).')
    sys.exit()

print('Using %s as Maple serial port' % maple_path)

try:
    ser = serial.Serial(maple_path, baudrate=115200, xonxoff=1)

    try:
        # try to toggle DTR/RTS (old scheme)
        ser.setRTS(0)
        time.sleep(0.01)
        ser.setDTR(0)
        time.sleep(0.01)
        ser.setDTR(1)
        time.sleep(0.01)
        ser.setDTR(0)

        # try magic number
        ser.setRTS(1)
        time.sleep(0.01)
        ser.setDTR(1)
        time.sleep(0.01)
        ser.setDTR(0)
        time.sleep(0.01)
        ser.write("1EAF".encode("ascii"))
        ser.flush()

        # Delay a bit before proceeding
        time.sleep(0.1)
    finally:
        # ok we're done here
        ser.close()

except Exception as e:
    print('Failed to open serial port %s for reset' % maple_path)
    sys.exit()

