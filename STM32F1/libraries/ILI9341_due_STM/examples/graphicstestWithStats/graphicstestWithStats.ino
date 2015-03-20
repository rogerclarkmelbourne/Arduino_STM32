/***************************************************
This is our GFX example for the Adafruit ILI9341 Breakout and Shield
----> http://www.adafruit.com/products/1651

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
****************************************************/
#include <SPI.h> 
#include <ILI_SdSpi.h>
#include <ILI_SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>
#include "fonts\Arial_bold_14.h"


#define TFT_DC 10
#define TFT_CS 8
#define TFT_RST 9

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

ILI9341_due_gText t1(&tft);
long Screenfill,
	Text,
	Lines,
	HorizVertLines,
	Rectanglesoutline,
	Rectanglesfilled,
	Circlesfilled,
	Circlesoutline,
	Trianglesoutline,
	Trianglesfilled,
	Roundedrectsoutline,
	Roundedrectsfilled;

void setup() {


	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor
	Serial.println("ILI9341 Test!"); 

	tft.begin();
	tft.setRotation(iliRotation270);
	tft.fillScreen(ILI9341_BLUE);
	t1.defineArea(30, 30, 26, 12, Arial_bold_14);
	t1.selectFont(Arial_bold_14);
	t1.setFontLetterSpacing(2);
	t1.setFontColor(ILI9341_WHITE, ILI9341_BLUE);

#if SPI_MODE_NORMAL
	char mode[] = "SPI_MODE_NORMAL";
#elif SPI_MODE_EXTENDED
	char mode[] = "SPI_MODE_EXTENDED";
#elif SPI_MODE_DMA
	char mode[] = "SPI_MODE_DMA";

#endif
	t1.drawString(mode, tft.width()/2 - t1.stringWidth(mode) / 2 - 30, 80);

	delay(3000);
	tft.setRotation(iliRotation0);
	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 

	Serial.println(F("Benchmark                Time (microseconds)"));

	Serial.print(F("Screen fill              "));
	unsigned long start = micros();
	Serial.println(Screenfill = testFillScreen());
	// delay(200);

	Serial.print(F("Text                     "));
	Serial.println(Text = testText());
	// delay(200);

	Serial.print(F("Lines                    "));
	Serial.println(Lines = testLines(ILI9341_CYAN));
	// delay(200);

	Serial.print(F("Horiz/Vert Lines         "));
	Serial.println(HorizVertLines = testFastLines(ILI9341_RED, ILI9341_BLUE));
	// delay(200);

	Serial.print(F("Rectangles (outline)     "));
	Serial.println(Rectanglesoutline = testRects(ILI9341_GREEN));
	// delay(200);

	Serial.print(F("Rectangles (filled)      "));
	Serial.println(Rectanglesfilled = testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
	// delay(200);

	Serial.print(F("Circles (filled)         "));
	Serial.println(Circlesfilled = testFilledCircles(10, ILI9341_MAGENTA));

	Serial.print(F("Circles (outline)        "));
	Serial.println(Circlesoutline = testCircles(10, ILI9341_WHITE));
	// delay(200);

	Serial.print(F("Triangles (outline)      "));
	Serial.println(Trianglesoutline = testTriangles());
	// delay(200);

	Serial.print(F("Triangles (filled)       "));
	Serial.println(Trianglesfilled = testFilledTriangles());
	// delay(200);

	Serial.print(F("Rounded rects (outline)  "));
	Serial.println(Roundedrectsoutline = testRoundRects());
	// delay(200);

	Serial.print(F("Rounded rects (filled)   "));
	Serial.println(Roundedrectsfilled = testFilledRoundRects());
	// delay(200);


	Serial.println(F("Done!"));

	tft.fillScreen(ILI9341_BLUE);
	tft.setRotation(iliRotation270);
	t1.cursorToXY(45, 80);
	t1.print("Total time: ");
	t1.print((micros() - start)/1000);
	t1.print(" ms");

	delay(2000);
	printStats();

}

void printStats()
{
	tft.setRotation(iliRotation270);
	tft.fillScreen(ILI9341_BLUE);
	t1.cursorTo(0, 0);
	t1.print("Screen fill              "); t1.cursorTo(18); t1.print(Screenfill);
	t1.cursorTo(0, 1);
	t1.print("Text                     "); t1.cursorTo(18); t1.print(Text);
	t1.cursorTo(0, 2);
	t1.print("Lines                    "); t1.cursorTo(18); t1.print(Lines);
	t1.cursorTo(0, 3);
	t1.print("Horiz/Vert Lines         "); t1.cursorTo(18); t1.print(HorizVertLines);
	t1.cursorTo(0, 4);
	t1.print("Rectangles (outline)     "); t1.cursorTo(18); t1.print(Rectanglesoutline);
	t1.cursorTo(0, 5);
	t1.print("Rectangles (filled)      "); t1.cursorTo(18); t1.print(Rectanglesfilled);
	t1.cursorTo(0, 6);
	t1.print("Circles (filled)         "); t1.cursorTo(18); t1.print(Circlesfilled);
	t1.cursorTo(0, 7);
	t1.print("Circles (outline)        "); t1.cursorTo(18); t1.print(Circlesoutline);
	t1.cursorTo(0, 8);
	t1.print("Triangles (outline)      "); t1.cursorTo(18); t1.print(Trianglesoutline);
	t1.cursorTo(0, 9);
	t1.print("Triangles (filled)       "); t1.cursorTo(18); t1.print(Trianglesfilled);
	t1.cursorTo(0, 10);
	t1.print("Rounded rects (outline)  "); t1.cursorTo(18); t1.print(Roundedrectsoutline);
	t1.cursorTo(0, 11);
	t1.print("Rounded rects (filled)   "); t1.cursorTo(18); t1.print(Roundedrectsfilled);

}

void loop(void) {
	/*for(uint8_t rotation=0; rotation<4; rotation++) {
	tft.setRotation(rotation);
	testText();
	delay(1000);
	}*/
}

unsigned long testFillScreen() {
	unsigned long start = micros();
	tft.fillScreen(ILI9341_BLACK);
	tft.fillScreen(ILI9341_RED);
	tft.fillScreen(ILI9341_GREEN);
	tft.fillScreen(ILI9341_BLUE);
	tft.fillScreen(ILI9341_BLACK);
	return micros() - start;
}

unsigned long testText() {
	tft.fillScreen(ILI9341_BLACK);
	unsigned long start = micros();
	tft.setCursor(0, 0);
	tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
	tft.println("Hello World!");
	tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
	tft.println(1234.56);
	tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
	tft.println(0xDEADBEEF, HEX);
	tft.println();
	tft.setTextColor(ILI9341_GREEN);
	tft.setTextSize(5);
	tft.println("Groop");
	tft.setTextSize(2);
	tft.println("I implore thee,");
	tft.setTextSize(1);
	tft.println("my foonting turlingdromes.");
	tft.println("And hooptiously drangle me");
	tft.println("with crinkly bindlewurdles,");
	tft.println("Or I will rend thee");
	tft.println("in the gobberwarts");
	tft.println("with my blurglecruncheon,");
	tft.println("see if I don't!");
	return micros() - start;
}

unsigned long testLines(uint16_t color) {
	unsigned long start, t;
	int           x1, y1, x2, y2,
		w = tft.width(),
		h = tft.height();

	tft.fillScreen(ILI9341_BLACK);

	x1 = y1 = 0;
	y2    = h - 1;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t     = micros() - start; // fillScreen doesn't count against timing

	tft.fillScreen(ILI9341_BLACK);

	x1    = w - 1;
	y1    = 0;
	y2    = h - 1;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t    += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1    = 0;
	y1    = h - 1;
	y2    = 0;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
	t    += micros() - start;

	tft.fillScreen(ILI9341_BLACK);

	x1    = w - 1;
	y1    = h - 1;
	y2    = 0;
	start = micros();
	for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

	return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
	unsigned long start;
	int           x, y, w = tft.width(), h = tft.height();

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
	for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

	return micros() - start;
}

unsigned long testRects(uint16_t color) {
	unsigned long start;
	int           n, i, i2,
		cx = tft.width()  / 2,
		cy = tft.height() / 2;

	tft.fillScreen(ILI9341_BLACK);
	n     = min(tft.width(), tft.height());
	start = micros();
	for(i=2; i<n; i+=6) {
		i2 = i / 2;
		tft.drawRect(cx-i2, cy-i2, i, i, color);
	}

	return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
	unsigned long start, t = 0;
	int           n, i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n = min(tft.width(), tft.height());
	for(i=n; i>0; i-=6) {
		i2    = i / 2;
		start = micros();
		tft.fillRect(cx-i2, cy-i2, i, i, color1);
		t    += micros() - start;
		// Outlines are not included in timing results
		tft.drawRect(cx-i2, cy-i2, i, i, color2);
	}

	return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(x=radius; x<w; x+=r2) {
		for(y=radius; y<h; y+=r2) {
			tft.fillCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
	unsigned long start;
	int           x, y, r2 = radius * 2,
		w = tft.width()  + radius,
		h = tft.height() + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = micros();
	for(x=0; x<w; x+=r2) {
		for(y=0; y<h; y+=r2) {
			tft.drawCircle(x, y, radius, color);
		}
	}

	return micros() - start;
}

unsigned long testTriangles() {
	unsigned long start;
	int           n, i, cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	n     = min(cx, cy);
	start = micros();
	for(i=0; i<n; i+=5) {
		tft.drawTriangle(
			cx    , cy - i, // peak
			cx - i, cy + i, // bottom left
			cx + i, cy + i, // bottom right
			tft.color565(0, 0, i));
	}

	return micros() - start;
}

unsigned long testFilledTriangles() {
	unsigned long start, t = 0;
	int           i, cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();
	for(i=min(cx,cy); i>10; i-=5) {
		start = micros();
		tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(0, i, i));
		t += micros() - start;
		tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
			tft.color565(i, i, 0));
	}

	return t;
}

unsigned long testRoundRects() {
	unsigned long start;
	int           w, i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	w     = min(tft.width(), tft.height());
	start = micros();
	for(i=0; i<w; i+=6) {
		i2 = i / 2;
		tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
	}

	return micros() - start;
}

unsigned long testFilledRoundRects() {
	unsigned long start;
	int           i, i2,
		cx = tft.width()  / 2 - 1,
		cy = tft.height() / 2 - 1;

	tft.fillScreen(ILI9341_BLACK);
	start = micros();

	for(i=min(tft.width(), tft.height()); i>20; i-=6) {
		i2 = i / 2;
		tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
	}

	return micros() - start;
}
