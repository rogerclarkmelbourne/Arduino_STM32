#!/bin/sh

#Arch uses the uucp group, dialout is depricated
retUUCP=false
getent group uucp $1 >/dev/null 2>&1 && retUUCP=true

if sudo [ -w /etc/udev/rules.d ]; then
    echo "Copying Maple-specific udev rules..."
    if $retUUCP
    then
        echo "Adding current user to uucp group"
        sudo usermod -a -G uucp $USER
        sudo cp -v 45-maple.rules.arch /etc/udev/rules.d/45-maple.rules
    else
        echo "Adding current user to dialout group"
        sudo adduser $USER dialout
        sudo cp -v 45-maple.rules /etc/udev/rules.d/45-maple.rules
    fi
    sudo chown root:root /etc/udev/rules.d/45-maple.rules
    sudo chmod 644 /etc/udev/rules.d/45-maple.rules
    echo "Reloading udev rules"
    sudo udevadm control --reload-rules
else
    echo "Couldn't copy to /etc/udev/rules.d/; you probably have to run this script as root? Or your distribution of Linux doesn't include udev; try running the IDE itself as root."
fi

