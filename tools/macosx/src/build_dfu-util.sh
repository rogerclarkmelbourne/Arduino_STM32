#!/bin/bash

sudo apt-get build-dep dfu-util
sudo apt-get install build-essentials
sudo apt-get install libusb-1.0-0-dev
sudo apt-get install autoconf automake autotools-dev

cd dfu-util
./autogen.sh
./configure
make
cp src/dfu-util ../../linux/dfu-util
cp src/dfu-suffix ../../linux/dfu-util
cp src/dfu-prefix ../../linux/dfu-util

