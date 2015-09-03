#include <SPI.h> //uncomment when using SPI_MODE_NORMAL or SPI_MODE_EXTENDED
#include <ILI_SdSpi.h>
#include <ILI_SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

#include "fonts\jokerman_255.h"

#define TFT_DC 10
#define TFT_CS 8
#define TFT_RST  9

ILI9341_due myTFT(TFT_CS, TFT_DC, TFT_RST);
ILI9341_due_gText t1(&myTFT);

void setup()
{
	Serial.begin(9600);
	
	bool result = myTFT.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");
	myTFT.fillScreen(ILI9341_BLACK);

	t1.defineArea(18, 32, 222, 287);
	t1.selectFont(jokerman_255);
	//t1.setFontMode(gTextFontMode_Transparent);
}

void loop()
{
	t1.setFontColor(255,230,0,0,0,0);
	t1.drawString("0", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(232,157,12);
	t1.drawString("1", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(255,88,0);
	t1.drawString("2", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(232,12,15);
	t1.drawString("3", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(227,13,255);
	t1.drawString("4", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(57,0,255);
	t1.drawString("5", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(12,103,232);
	t1.drawString("6", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(0,255,243);
	t1.drawString("7", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(12,232,73);
	t1.drawString("8", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	t1.setFontColor(132,255,13);
	t1.drawString("9", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
}
