//#include <SPI.h> //uncomment when using SPI_MODE_NORMAL or SPI_MODE_EXTENDED
#include <ILI_SdSpi.h>
#include <ILI_SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

#include "fonts\Arial_bold_14.h"

#define TFT_CS 10
#define TFT_DC 9

ILI9341_due myTFT(TFT_CS, TFT_DC);

void setup()
{
	Serial.begin(9600);
	
	bool result = myTFT.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");

	myTFT.fillScreen(ILI9341_BLUE);
	myTFT.setRotation(iliRotation270);

	ILI9341_due_gText t1(&myTFT);
	t1.defineArea(100, 110, 220, 130);
	t1.selectFont(Arial_bold_14);
	t1.setFontLetterSpacing(5);
	t1.setFontColor(ILI9341_WHITE, ILI9341_BLUE);
	t1.drawString("Hello World", gTextAlignMiddleCenter);
}

void loop()
{

	/* add main program code here */

}
