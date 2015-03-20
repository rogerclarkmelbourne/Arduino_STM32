/*
This sketch is demonstrating loading images from an SD card.
To run this sketch, copy images from the images folder into the root of an SD card
(either those with .bmp extension or those with .565 extension (those will load faster)).
Modify the extension of the files in loop() depending on which files you copied onto SD.

Depending on the length of the cables between Arduino and SD card pins you might want
to change SD_SPI_SPEED to either SPI_FULL_SPEED (if you have a shield directly connected to
Arduino pins) or SPI_QUARTER_SPEED if you have long cables. SPI_HALF_SPEED was the maximum
I could use as I had the display with SD slot on a breadboard.
Obviously you have to modify CS, DC pins for TFT and SD appropriately.

You can generate your own .565 images from 24bit BMPs by using BMP24toILI565 converter which
you can find in the Tools folder on GitHub:
https://github.com/marekburiak/ILI9341_due/tree/master/tools
*/

#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <ILI_SdSpi.h>
#include <ILI_SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

// CS and DC for the LCD
#define LCD_CS 10	// Chip Select for LCD
#define LCD_DC 9	// Command/Data for LCD

#define SD_CS 7		// Chip Select for SD card

#define BUFFPIXELCOUNT 320	// size of the buffer in pixels
#define SD_SPI_SPEED SPI_HALF_SPEED	// SD card SPI speed, try SPI_FULL_SPEED

SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem
//ArduinoOutStream cout(Serial);

ILI9341_due tft(LCD_CS, LCD_DC);

// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))

void setup()
{
	Serial.begin(9600);

	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	progmemPrint(PSTR("Initializing SD card..."));

	if (!sd.begin(SD_CS, SD_SPI_SPEED)){
		progmemPrintln(PSTR("failed!"));
		return;
	}
	progmemPrintln(PSTR("OK!"));
}

void loop()
{
	if (tft.getRotation() == iliRotation90 || tft.getRotation() == iliRotation270){
		bmpDraw("giraffe.565", 0, 0);
		delay(2000);
		bmpDraw("SOLDHO~1.565", 0, 0);
		delay(2000);
		bmpDraw("GLOOMY~1.565", 0, 0);
		delay(2000);
	    bmpDraw("MOTIVA~1.565", 0, 0);
	    delay(2000);
	}
	else
	{
		bmpDraw("smokeP.565", 0, 0);
		delay(2000);
		bmpDraw("origP.565", 0, 0);
		delay(2000);
		bmpDraw("radioP.565", 0, 0);
		delay(2000);
		bmpDraw("stopP.565", 0, 0);
		delay(2000);
	}

}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's RAM but
// makes loading a little faster.  

void bmpDraw(char* filename, int x, int y) {

	SdFile   bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint8_t	 headerSize;
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;     // Not always = bmpWidth; may have padding
	uint32_t fileSize;
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip = true;        // BMP is stored bottom-to-top
	uint16_t w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime;

	if ((x >= tft.width()) || (y >= tft.height())) return;

	progmemPrint(PSTR("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');
	startTime = millis();
	// Open requested file on SD card
	if (!bmpFile.open(filename, O_READ)) {
		Serial.println("File open failed.");
		return;
	}
	else {
		//Serial.println("File opened.");
	}

	// Parse BMP header
	if (read16(bmpFile) == 0x4D42) { // BMP signature
		fileSize = read32(bmpFile);
		//progmemPrint(PSTR("File size: ")); Serial.println(fileSize);
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		//progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		headerSize = read32(bmpFile);
		//progmemPrint(PSTR("Header size: ")); Serial.println(headerSize);
		bmpWidth = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if (read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			//progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
			if (read32(bmpFile) == 0) // 0 = uncompressed
			{
				//progmemPrint(PSTR("Image size: "));
				//Serial.print(bmpWidth);
				//Serial.print('x');
				//Serial.println(bmpHeight);

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if (bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if ((x + w - 1) >= tft.width())  w = tft.width() - x;
				if ((y + h - 1) >= tft.height()) h = tft.height() - y;

				// Set TFT address window to clipped image bounds
				tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

				if (bmpDepth == 16)	//565 format
				{
					goodBmp = true; // Supported BMP format -- proceed!

					uint8_t buffer[2 * BUFFPIXELCOUNT]; // pixel buffer (contains already formatted data for ILI9341 display)

					bmpFile.seekSet(54);	//skip header
					uint32_t totalPixels = bmpWidth*bmpHeight;
					uint16_t numFullBufferRuns = totalPixels / BUFFPIXELCOUNT;
					for (uint32_t p = 0; p < numFullBufferRuns; p++) {
						// read pixels into the buffer
						bmpFile.read(buffer, 2 * BUFFPIXELCOUNT);
						// push them to the diplay
						tft.pushColors565(buffer, 0, 2 * BUFFPIXELCOUNT);
					}

					// render any remaining pixels that did not fully fit the buffer
					uint32_t remainingPixels = totalPixels % BUFFPIXELCOUNT;
					if (remainingPixels > 0)
					{
						bmpFile.read(buffer, 2 * remainingPixels);
						tft.pushColors565(buffer, 0, 2 * remainingPixels);
					}

				}
				else if (bmpDepth == 24)	// standard 24bit bmp
				{
					goodBmp = true; // Supported BMP format -- proceed!
					uint16_t bufferSize = min(w, BUFFPIXELCOUNT);
					uint8_t  sdbuffer[3 * bufferSize]; // pixel in buffer (R+G+B per pixel)
					uint16_t lcdbuffer[bufferSize];  // pixel out buffer (16-bit per pixel)

					// BMP rows are padded (if needed) to 4-byte boundary
					rowSize = (bmpWidth * 3 + 3) & ~3;

					for (row = 0; row < h; row++) { // For each scanline...
						// Seek to start of scan line.  It might seem labor-
						// intensive to be doing this on every line, but this
						// method covers a lot of gritty details like cropping
						// and scanline padding.  Also, the seek only takes
						// place if the file position actually needs to change
						// (avoids a lot of cluster math in SD library).

						if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
							pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
						else     // Bitmap is stored top-to-bottom
							pos = bmpImageoffset + row * rowSize;
						if (bmpFile.curPosition() != pos) { // Need seek?
							bmpFile.seekSet(pos);
						}

						for (col = 0; col < w; col += bufferSize)
						{
							// read pixels into the buffer
							bmpFile.read(sdbuffer, 3 * bufferSize);

							// convert color
							for (int p = 0; p < bufferSize; p++)
							{
								b = sdbuffer[3 * p];
								g = sdbuffer[3 * p + 1];
								r = sdbuffer[3 * p + 2];
								lcdbuffer[p] = tft.color565(r, g, b);
							}
							// push buffer to TFT
							tft.pushColors(lcdbuffer, 0, bufferSize);
						}

						// render any remaining pixels that did not fully fit the buffer
						uint16_t remainingPixels = w % bufferSize;
						if (remainingPixels > 0)
						{
							bmpFile.read(sdbuffer, 3 * remainingPixels);

							for (int p = 0; p < remainingPixels; p++)
							{
								b = sdbuffer[3 * p];
								g = sdbuffer[3 * p + 1];
								r = sdbuffer[3 * p + 2];
								lcdbuffer[p] = tft.color565(r, g, b);
							}

							tft.pushColors(lcdbuffer, 0, remainingPixels);
						}
					}
				}
				else
				{
					progmemPrint(PSTR("Unsupported Bit Depth."));
				}

				if (goodBmp)
				{
					progmemPrint(PSTR("Loaded in "));
					Serial.print(millis() - startTime);
					Serial.println(" ms");
				}
			}
		}
	}

	bmpFile.close();
	if (!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(SdFile& f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
	char c;
	while (c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
	progmemPrint(str);
	Serial.println();
}

//void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
//{
//	char tmp[length*5+1];
//	byte first;
//	byte second;
//	for (int i=0; i<length; i++) {
//		first = (data[i] >> 4) & 0x0f;
//		second = data[i] & 0x0f;
//		// base for converting single digit numbers to ASCII is 48
//		// base for 10-16 to become upper-case characters A-F is 55
//		// note: difference is 7
//		tmp[i*5] = 48; // add leading 0
//		tmp[i*5+1] = 120; // add leading x
//		tmp[i*5+2] = first+48;
//		tmp[i*5+3] = second+48;
//		tmp[i*5+4] = 32; // add trailing space
//		if (first > 9) tmp[i*5+2] += 7;
//		if (second > 9) tmp[i*5+3] += 7;
//	}
//	tmp[length*5] = 0;
//	Serial.print(tmp);
//}