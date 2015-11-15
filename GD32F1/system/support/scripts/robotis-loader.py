#!/usr/bin/python

# This script sends a program on a robotis board (OpenCM9.04 or CM900)
# using the robotis bootloader (used in OpenCM IDE)
# 
# Usage:
# python robotis-loader.py <serial port> <binary>
#
# Example:
# python robotis-loader.py /dev/ttyACM0 firmware.bin
#
# https://github.com/Gregwar/robotis-loader

import serial, sys, os, time

print('~~ Robotis loader ~~')
print('')

# Reading command line
if len(sys.argv) != 3:
    exit('! Usage: robotis-loader.py <serial-port> <binary>')
pgm, port, binary = sys.argv

# Helper to prints a progress bar
def progressBar(percent, precision=65):
    threshold=precision*percent/100.0
    sys.stdout.write('[ ')
    for x in xrange(0, precision):
        if x < threshold: sys.stdout.write('#')
        else: sys.stdout.write(' ')
    sys.stdout.write(' ] ')
    sys.stdout.flush()

# Opening the firmware file
try:
    stat = os.stat(binary)
    size = stat.st_size
    firmware = file(binary, 'rb')
    print('* Opening %s, size=%d' % (binary, size))
except:
    exit('! Unable to open file %s' % binary)

# Opening serial port
try:
    s = serial.Serial(port, baudrate=115200)
except:
    exit('! Unable to open serial port %s' % port)

print('* Resetting the board')
s.setRTS(True)
s.setDTR(False)
time.sleep(0.1)
s.setRTS(False)
s.write('CM9X')
s.close()
time.sleep(1.0);

print('* Connecting...')
s = serial.Serial(port, baudrate=115200)
s.write('AT&LD')
print('* Download signal transmitted, waiting...')

# Entering bootloader sequence
while True:
    line = s.readline().strip()
    if line.endswith('Ready..'):
        print('* Board ready, sending data')
        cs = 0
        pos = 0
        while True:
            c = firmware.read(2048)
            if len(c):
                pos += len(c)
                sys.stdout.write("\r")
                progressBar(100*float(pos)/float(size))
                s.write(c)
                for k in range(0,len(c)):
                    cs = (cs+ord(c[k]))%256
            else:
                break
        print('')
        s.setDTR(True)
        print('* Checksum: %d' % (cs))
        s.write(chr(cs))
        print('* Firmware was sent')
    else:
        if line == 'Success..':
            print('* Success, running the code')
            print('')
            s.write('AT&RST')
            s.close()
            exit()
        else:
            print('Board -> '+line)
