#!/bin/sh

if sudo [ -w /etc/udev/rules.d ]; then
    echo "Copying Maple-specific udev rules..."
    sudo cp -v 45-maple.rules /etc/udev/rules.d/45-maple.rules
    sudo chown root:root /etc/udev/rules.d/45-maple.rules
    sudo chmod 644 /etc/udev/rules.d/45-maple.rules
    sudo cp -v 49-stlinkv1.rules /etc/udev/rules.d/49-stlinkv1.rules
    sudo chown root:root /etc/udev/rules.d/49-stlinkv1.rules
    sudo chmod 644 /etc/udev/rules.d/49-stlinkv1.rules
    sudo cp -v 49-stlinkv2.rules /etc/udev/rules.d/49-stlinkv2.rules
    sudo chown root:root /etc/udev/rules.d/49-stlinkv2.rules
    sudo chmod 644 /etc/udev/rules.d/49-stlinkv2.rules
    sudo cp -v 49-stlinkv2-1.rules /etc/udev/rules.d/49-stlinkv2-1.rules
    sudo chown root:root /etc/udev/rules.d/49-stlinkv2-1.rules
    sudo chmod 644 /etc/udev/rules.d/49-stlinkv2-1.rules
    echo "Reloading udev rules"
    sudo udevadm control --reload-rules
    echo "Adding current user to dialout group"
    sudo adduser $USER dialout
else
    echo "Couldn't copy to /etc/udev/rules.d/; you probably have to run this script as root? Or your distribution of Linux doesn't include udev; try running the IDE itself as root."
fi

