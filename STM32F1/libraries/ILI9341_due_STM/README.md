ILI9341_due
===========

Please see http://marekburiak.github.io/ILI9341_due


Version History:
```
v0.94.000 - Added AVR compatibility, the library can now be also used on Uno, Mega and alike.
			Please check the github.io page for more information (especially if you want to
			use gText)
v0.93.000 - Breaking changes:
                - setRotation now needs iliRotation enum as a parameter (instead of an int)
                - the meaning of some gText drawString parameters have changed 
                  (event though the parameter type is the same)
          - new additions:
                - gText drawString with new parameters
                - new gText drawStringOffseted, drawStringPivoted, drawStringPivotedOffseted 
                  functions
          - gText fontLetterSpacing default value is now 2 (previously 0)
          - examples updated

v0.92.002 - Fixed drawArc
		  - Added setArcParams function to change maxArcAngle and arcAngleOffset at runtime

v0.92.001 - Added fontHeight function in gText
          - fixes for NORMAL and EXTENDED mode

v0.92.000 - Added drawArc function for drawing arcs and circles with thickness and pies
          - Added screenshotToConsole method and ILIScreenShotViewer app for taking 
            screenshots from the display
          - Added alignment options for drawString in ILI9341_due_gText
	  
v0.91.002 - Updated graphicstestWithStats example sketch so it does not use Streaming.h

v0.91.001 - Performance improvements (especially fill circle)
          - Fixed a scanline fill bug, clean up commented out code

v0.91.000 - Added functions for controlling TFT power levels (sleep, idle, setPowerLevel)

v0.90.026 - Fixed fillScreen after recent changes

v0.90.025 - Added PushColors565 function
          - Added BMP24toILI565 tool
          - Updated sdFatTftBitmap example

v0.90.010 - Initial version
```