#!/bin/bash
$(dirname $0)/stlink/st-flash write "$1" 0x8000000
exit 0

## Remove the lines 2 and 3 (above) if you want this script to wait until the Serial device has been enumerated and loaded before the script exits

# Check for leaf device.
function leaf_status()
{

this_leaf_status=$(lsusb |grep "1eaf" | awk '{ print $NF}')
# Find the mode of the leaf bootloader
case $this_leaf_status in
   "1eaf:0003")
      echo "dfu"
   ;;
   "1eaf:0004")
      echo "ttyACMx"
   ;;
   *)
      #echo "$this_leaf_status"
      echo "unknown"
   ;;
esac
}

# You will need the usb-reset code, see https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Using-a-generic-stm32-board-on-linux-with-Maple-bootloader
#
USBRESET=$(which usb-reset) || USBRESET="./usb-reset"

# Check to see if a maple compatible board is attached
LEAF_STATUS=$(leaf_status)
echo "USB Status [$LEAF_STATUS]"

$(dirname $0)/stlink/st-flash write "$4" 0x8000000

sleep 4
# Reset the usb device to bring up the tty rather than DFU
"$USBRESET" "/dev/bus/usb/$(lsusb |grep "1eaf" |awk '{print $2,$4}'|sed 's/\://g'|sed 's/ /\//g')" >/dev/null 2>&1
# Check to see if a maple compatible board is attached
LEAF_STATUS=$(leaf_status)
echo "USB Status [$LEAF_STATUS]"
# Check to see if the tty came up
TTY_DEV=$(find /dev -cmin -2 |grep ttyAC)
echo -e "Waiting for tty device $TTY_DEV \n"
sleep 20
echo -e "$TTY_DEV should now be available.\n"
exit 0

