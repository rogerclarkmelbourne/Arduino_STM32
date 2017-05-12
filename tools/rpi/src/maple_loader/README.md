These files build the maple_loader.jar file used on Windows to reset the Sketch via USB Serial, so that the bootloader will run in dfu upload mode, ready for a new sketch to be uploaded

The files were written by @bobC (github) and have been slightly modified by me (Roger Clark), so that dfu-util no longer attempts to reset the board after upload.
This change to dfu-util's reset command line argument, was required because dfu-util was showing errors on some Windows systems, because the bootloader had reset its self after upload, 
before dfu-util had chance to tell it to reset.