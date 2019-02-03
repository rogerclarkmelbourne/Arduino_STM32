#!/bin/bash

# Get the directory where the script is running.
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

binfile="$1"
com_port="$2"

"$DIR/hid_flash/hid-flash" ${binfile} ${com_port}
