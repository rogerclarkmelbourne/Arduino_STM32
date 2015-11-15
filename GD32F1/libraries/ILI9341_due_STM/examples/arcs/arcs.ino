
//#include <SPI.h>
#include <SPI.h> 
#include "ILI_SdSpi.h"
#include "ILI_SdFatConfig.h"
#include "ILI9341_due_gText.h"
#include "ILI9341_due.h"
#include "Arial_bold_14.h"
#include "roboto16.h"
#include "roboto32.h"
#include "roboto70.h"

#define TFT_DC 10
#define TFT_CS 8
#define rst  9

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, rst);

char textBuff[20];

ILI9341_due_gText t1(&tft);
ILI9341_due_gText t2(&tft);
ILI9341_due_gText t3(&tft);
uint16_t colorLightGray = tft.color565(192,192,192);
uint16_t colorGray = tft.color565(127,127,127);
uint16_t colorDarkGray = tft.color565(64,64,64);

void setup()
{
	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor

	tft.begin();
	tft.setRotation(iliRotation270);

	screenIntro();
	delay(2000);
	screenLoading();
	screenClock();
	screenPie();
	delay(2000);
	screenSensors();
}

void screenIntro()
{
	tft.fillScreen(ILI9341_BLUE);
	t1.defineArea(100, 100, 220, 140);
	t1.selectFont(Arial_bold_14);
	t1.setFontLetterSpacing(5);
	t1.setFontColor(ILI9341_WHITE, ILI9341_BLUE);
	strcpy(textBuff, "Arcs demo");
	t1.drawString(textBuff, gTextAlignMiddleCenter);
}

void screenLoading()
{
	const uint16_t x = 159;
	const uint16_t y = 149;
	tft.fillScreen(ILI9341_BLACK);
	t1.selectFont(roboto32);
	t1.setFontLetterSpacing(5);
	t1.setFontColor(ILI9341_WHITE, ILI9341_BLACK);
	t1.defineArea(100, 85, 220, 140);
	strcpy(textBuff, "Loading...");
	t1.drawString(textBuff, gTextAlignTopCenter, 0, 0);
	tft.drawArc(x, y, 10, 5, 0, 360, colorLightGray);

	for(int i=0; i<2880; i+=4)
	{
		tft.drawArc(x, y, 10, 5, (i >> 1)-45, (i >> 1)+45, colorDarkGray);
		tft.drawArc(x, y, 10, 5, (i >> 1)-45-4, (i >> 1)-45, colorLightGray);

		tft.drawArc(x, y, 20, 5, 1440-i-45, 1440-i+45, colorDarkGray);
		tft.drawArc(x, y, 20, 5, 1440-i+45, 1440-i+45+4, colorLightGray);
	}
}

void screenClock()
{
	const uint16_t x = 159;
	const uint16_t y = 119;
	tft.fillScreen(ILI9341_BLACK);
	t1.selectFont(roboto70);
	t1.setFontLetterSpacing(5);
	t1.setFontColor(ILI9341_WHITE, ILI9341_BLACK);
	t1.drawString("15:06", gTextAlignMiddleCenter);
	tft.drawArc(x,y,102,11,0,225, colorLightGray);	// 15 hours
	tft.drawArc(x,y,113,8,0,36, colorGray);	// 6 minutes
	tft.drawArc(x,y,120,5,0,360, colorDarkGray);	// seconds

	for(uint16_t d=324; d<372; d++)
	{
		tft.drawArc(x,y,120,5,d-1,d+1, ILI9341_RED);
		tft.drawArc(x,y,120,5,d-2,d-1, colorDarkGray);	// to erase the red
		if(d == 360)
		{
			t1.drawString("15:07", gTextAlignMiddleCenter);
			tft.drawArc(x,y,113,8,0,42, colorGray);	// 7 minutes
		}
		delay(166);
	}
}

void screenPie()
{
	const uint16_t x = 159;
	const uint16_t y = 119;
	const uint16_t radius = 80;

	tft.fillScreen(ILI9341_BLACK);
	t1.selectFont(roboto16);
	t1.setFontLetterSpacing(2);
	tft.drawArc(x+3,y-2,radius,radius,0,60, tft.color565(198,255,13));
	tft.drawArc(x-3,y+7,radius+10,radius+10,60,340, tft.color565(255,0,54));
	tft.drawArc(x-2,y-3,radius,radius,340,360, tft.color565(0,255,241));

	t1.defineArea(0,0,220,180);
	t1.selectFont(roboto16);
	t1.setFontMode(gTextFontMode_Transparent);
	t1.setFontColor(ILI9341_BLACK);
	t1.drawString("16%",175,70);
	t1.drawString("78%",140,150);
	t1.setFontColor(colorLightGray);
	t1.drawString("6%",132,18);
	delay(2000);
}

void screenSensors()
{
	const uint16_t radius = 55;
	float temp=22.4;
	uint16_t hum=73, lux=1154;

	const uint16_t s1x = 0;
	const uint16_t s1y = 10;

	const uint16_t s2x = 110;
	const uint16_t s2y = 70;

	const uint16_t s3x = 210;
	const uint16_t s3y = 130;

	tft.fillScreen(ILI9341_BLACK);

	t1.setFontMode(gTextFontMode_Solid);
	t1.setFontLetterSpacing(3);
	t2.setFontMode(gTextFontMode_Solid);
	t2.setFontLetterSpacing(3);
	t3.setFontMode(gTextFontMode_Solid);
	t3.setFontLetterSpacing(3);

	t1.defineArea(s1x,s1y,s1x+2*radius,s1y+2*radius);	
	t2.defineArea(s2x,s2y,s2x+2*radius,s2y+2*radius);	
	t3.defineArea(s3x,s3y,s3x+2*radius,s3y+2*radius);	

	t1.setFontColor(ILI9341_WHITE);
	tft.drawArc(s1x+radius,s1y+radius,radius,10,-3,3, tft.color565(127,0,27));
	t1.selectFont(roboto16);
	t1.drawStringOffseted("C", gTextAlignMiddleCenter, 0, 25);
	t1.selectFont(roboto32);
	sprintf(textBuff, "%4.1f", temp);
	t1.drawString(textBuff, gTextAlignMiddleCenter);
	tft.fillRect(s1x+radius-7,s1y+radius+17,2,2,ILI9341_WHITE);	// degrees symbol

	for(uint16_t d=1; d<temp*10; d++)
	{
		tft.drawArc(s1x+radius,s1y+radius,radius,10,d,d+3, tft.color565(127,0,27));
		tft.drawArc(s1x+radius,s1y+radius,radius-3,4,d-1,d, tft.color565(255,0,54));
	}

	t2.setFontColor(ILI9341_WHITE);
	tft.drawArc(s2x+radius,s2y+radius,radius,10,-3,3, tft.color565(0,43,127));
	t2.selectFont(roboto16);	
	t2.drawStringOffseted("%", gTextAlignMiddleCenter, 0, 25);
	t2.selectFont(roboto32);
	sprintf(textBuff, "%d", hum);
	t2.drawString(textBuff, gTextAlignMiddleCenter);

	for(uint16_t d=1; d<263; d++)
	{
		tft.drawArc(s2x+radius,s2y+radius,radius,10,d,d+3, tft.color565(0,43,127));
		tft.drawArc(s2x+radius,s2y+radius,radius-3,4,d-1,d, tft.color565(0,86,255));
	}

	t3.setFontColor(ILI9341_WHITE);
	tft.drawArc(s3x+radius,s3y+radius,radius,10,-3,3, tft.color565(127,103,6));
	t3.selectFont(roboto16);
	t3.drawStringOffseted("lux", gTextAlignMiddleCenter, 0, 25);
	t3.selectFont(roboto32);
	sprintf(textBuff, "%d", lux);
	t3.drawString(textBuff, gTextAlignMiddleCenter);

	for(uint16_t d=1; d<42; d++)
	{
		tft.drawArc(s3x+radius,s3y+radius,radius,10,d,d+3, tft.color565(127,103,6));
		tft.drawArc(s3x+radius,s3y+radius,radius-3,4,d-1,d,  tft.color565(255,206,13));
	}

	delay(1000);
	for(uint16_t d=1; d<660; d++)
	{
		if(d<220 )
		{
			lux+=21;

			sprintf(textBuff, "%d", lux);
			t3.drawString(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.drawArc(s3x+radius,s3y+radius,radius,10, ((float)lux/(float)10000)*360,((float)lux/(float)10000)*360+3, tft.color565(127,103,6));
			tft.drawArc(s3x+radius,s3y+radius,radius-3,4,((float)lux/(float)10000)*360-4,((float)lux/(float)10000)*360, tft.color565(255,206,13));
		}

		if(d == 220)
			delay(1000);

		if(d>220 && d % 15 == 0)
		{
			temp+=0.1;
			sprintf(textBuff, "%4.1f", temp);
			t1.drawString(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.drawArc(s1x+radius,s1y+radius,radius,10,temp*10,temp*10+3, tft.color565(127,0,27));
			tft.drawArc(s1x+radius,s1y+radius,radius-3,4,temp*10-2,temp*10, tft.color565(255,0,54));
			delay(random(350, 700));
		}
		if(d>300 && d<600 && d % 38 == 0)
		{
			hum+=1;
			sprintf(textBuff, "%d", hum);
			t2.drawString(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.drawArc(s2x+radius,s2y+radius,radius,10,(float)hum*3.6-4,(float)hum*3.6+3, tft.color565(0,43,127));
			tft.drawArc(s2x+radius,s2y+radius,radius-3,4,(float)hum*3.6-5,(float)hum*3.6, tft.color565(0,86,255));
		}
	}
}


void loop()
{
  setup ();
	/* add main program code here */

}
