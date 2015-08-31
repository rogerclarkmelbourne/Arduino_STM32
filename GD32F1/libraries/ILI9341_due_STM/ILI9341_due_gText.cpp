/*
ILI9341_due_gText.cpp - Used for rendering GLCD fonts on Arduino Due

Copyright (c) 2014  Marek Buriak

The library is based on GLCD library by Bill Perry and Michael Margolis: 
https://code.google.com/p/glcd-arduino

This file is part of the Arduino ILI9341_due library. 
Sources for this library can be found at https://github.com/marekburiak/ILI9341_Due.

Fonts .h files can be created with GLCDFontCreator2 (not the one MikroElektronika):
https://code.google.com/p/glcd-arduino/downloads/detail?name=GLCDFontCreator2.zip&can=2&q=


ILI9341_due is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

ILI9341_due is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ILI9341_due.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ILI9341_due.h"
#include "ILI9341_due_gText.h"

//#include "..\Streaming\Streaming.h"

//extern glcd_Device GLCD; // this is the global GLCD instance, here upcast to the base glcd_Device class 

// This constructor creates a text area using the entire display
// The device pointer is initialized using the global GLCD instance
// New constuctors can be added to take an exlicit glcd instance pointer
// if multiple glcd instances need to be supported
ILI9341_due_gText::ILI9341_due_gText(ILI9341_due *ili)
{
	// device = (glcd_Device*)&GLCD; 
	_ili = ili;
	_fontMode = gTextFontMode_Solid;
	_fontBgColor = ILI9341_BLACK;
	_fontColor = ILI9341_WHITE;
	_letterSpacing = 2;
	_isLastChar = false;
	defineArea(0,0, _ili->width() -1, _ili->height() -1, DEFAULT_SCROLLDIR); // this should never fail

}

// This constructor creates a text area with the given coordinates
// full display area is used if any coordinate is invalid
ILI9341_due_gText::ILI9341_due_gText(ILI9341_due *ili, int16_t x1, int16_t y1, int16_t x2, int16_t y2) //, textMode mode) 
{
	//device = (glcd_Device*)&GLCD; 
	_ili = ili;
	if( ! defineArea(x1,y1,x2,y2)) //,mode))
		defineArea(0,0, _ili->width() -1,_ili->height() -1); //,mode); // this should never fail
}

#if INCLUDE_NOT_WORKING_YET
ILI9341_due_gText::ILI9341_due_gText(ILI9341_due *ili, predefinedArea selection) //, textMode mode)
{
	//device = (glcd_Device*)&GLCD; 
	_ili = ili;
	if( ! defineArea(selection)) //,mode))
		defineArea(0,0, _ili->width() -1,_ili->height() -1,mode); // this should never fail

}
#endif

ILI9341_due_gText::ILI9341_due_gText(ILI9341_due *ili, int16_t x1, int16_t y1, int16_t columns, int16_t rows, gTextFont font) //, textMode mode)
{
	//device = (glcd_Device*)&GLCD; 
	_ili = ili;
	if( ! defineArea(x1,y1,columns,rows,font)) //, mode))
	{
		defineArea(0,0, _ili->width() -1,_ili->height() -1); //,mode); // this should never fail
		selectFont(font);
	}
}

/**
* Clear text area with the current font background color
* and home the cursor to upper left corner of the text area.
*
* @see defineArea()
*/
void ILI9341_due_gText::clearArea(uint16_t color)
{
	/*
	* fill the area with font background color
	*/
	//Serial << "clearing area from " << _area.x1 << " to " << _area.x2 << endl;
	_ili->fillRect(_area.x1, _area.y1, _area.x2 - _area.x1+1, _area.y2-_area.y1+1, color);

	/*glcd_Device::SetPixels(_area.x1, _area.y1, 
	_area.x2, _area.y2, 
	_fontColor == BLACK ? WHITE : BLACK);*/
	/*
	* put cursor at home position of text area to ensure we are always inside area.
	*/

	//cursorToXY(0,0);
}

/**
* Define a Text area by columns and rows
*
* @param x X coordinate of upper left corner
* @param y Y coordinate of upper left corner
* @param columns number of text columns
* @param rows number of text rows
* @param font a font definition
* @param mode constants SCROLL_DOWN and SCROLL_UP control scroll direction
*
*
* Defines a text area sized to hold columns characters across and rows characters tall.
* It is properly sized for the specified font.
* 
* The area within the newly defined text area is intentionally not cleared.
*
* While intended for fixed width fonts, sizing will work for variable
* width fonts.
*
* When variable width fonts are used, the column is based on assuming a width
* of the widest character.
*
* x,y is an absolute coordinate and is relateive to the 0,0 origin of the
* display.
*
* mode is an optional parameter and defaults to normal/up scrolling
*
* @note
* Upon defining the text area, the cursor position for the text area will be set to x,y
*
* @see ClearArea()
*/

bool ILI9341_due_gText::defineArea(int16_t x, int16_t y, int16_t columns, int16_t rows, gTextFont font) //, textMode mode)
{
	textMode mode = DEFAULT_SCROLLDIR;
	uint16_t x2,y2;

	selectFont(font);

	x2 = x + columns * (pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH)+1) -1;
	y2 = y + rows * (fontHeight()+1) -1;

	return defineArea(x, y, x2, y2); //, mode);
}

/**
* Define a text area by absolute coordinates
*
* @param x1 X coordinate of upper left corner
* @param y1 Y coordinate of upper left corner
* @param x2 X coordinate of lower right corner
* @param y2 Y coordinate of lower right corner
* @param mode constants SCROLL_DOWN and SCROLL_UP control scroll direction
*
* Defines a text area based on absolute coordinates.
* The pixel coordinates for the text area are inclusive so x2,y2 is the lower right
* pixel of the text area.
*
* x1,y1 and x2,y2 are an absolute coordinates and are relateive to the 0,0 origin of the
* display.
*
* The area within the newly defined text area is intentionally not cleared.
*
* mode is an optional parameter and defaults to normal/up scrolling
*
* @returns true with the given area selected if all the coordinates are valid,
* otherwise returns returns false with the area set to the full display
*
* @note
* Upon creation of the text area, the cursor position for the text area will be set to x1, y1
*
* @see ClearArea()
*
*/

bool ILI9341_due_gText::defineArea(int16_t x1, int16_t y1, int16_t x2, int16_t y2) //, textMode mode)
{
	textMode mode = DEFAULT_SCROLLDIR;
	//Serial << "defineArea input " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
	uint8_t ret = false;

	int16_t disp_width = _ili->width();
	int16_t disp_height = _ili->height();

	//if(		(x1 >= x2)
	//	||	(y1 >= y2)
	//	||	(x1 >= disp_width)
	//	||	(y1 >= disp_height)
	//	||	(x2 >= disp_width)
	//	||	(y2 >= disp_height)
	//	)
	//{
	//	// failed sanity check so set defaults and return false 
	//	_area.x1 = 0;
	//	_area.y1 = 0;
	//	_area.x2 = disp_width -1;
	//	_area.y2 = disp_height -1;
	//	_area.mode = DEFAULT_SCROLLDIR;
	//} 		
	//else
	{  
		_area.x1 = x1; 
		_area.y1 = y1; 
		_area.x2 = x2; 
		_area.y2 = y2; 		
		_area.mode = mode; // not yet sanity checked
		ret = true;
	}		
	//Serial << "defineArea set " << _area.x1 << " " << _area.y1 << " " << _area.x2 << " " << _area.y2 << endl;
	/*
	* set cursor position for the area
	*/
	_x = x1;
	_y = y1;	
	_scale = 1;

#ifndef GLCD_NODEFER_SCROLL
	/*
	* Make sure to clear a deferred scroll operation when re defining areas.
	*/
	_needScroll = 0;
#endif
	return ret;
}

/**
* Define a predefined generic text area
*
* @param selection a value from @ref predefinedArea
* @param mode constants SCROLL_DOWN and SCROLL_UP control scroll direction
*
* Defines a text area using a selection form a set of predefined areas.
*
* The area within the newly defined text area is intentionally not cleared.
*
* mode is an optional parameter and defaults to normal/up scrolling
*
* @return returns @em true if successful.
*
*
* @note
* Upon defining the text area, the cursor position for the text area will be set to 
* the upper left coordinate of the given predefined area
*
* @see ClearArea()
* @see predefinedArea
*
*/
#if INCLUDE_NOT_WORKING_YET
bool ILI9341_due_gText::defineArea(predefinedArea selection, textMode mode)
{
	int16_t x1,y1,x2,y2;
	TareaToken tok;

	tok.token = selection;

	x1 =  tok.coord.x1;
	y1 =  tok.coord.y1;
	x2 =  tok.coord.x2;
	y2 =  tok.coord.y2;

	return defineArea(x1,y1,x2,y2, mode);
}
#endif
/*
* Scroll a pixel region up.
* 	Area scrolled is defined by x1,y1 through x2,y2 inclusive.
*  x1,y1 is upper left corder, x2,y2 is lower right corner.
*
*	color is the color to be used for the created space along the
*	bottom.
*
*	pixels is the *exact* pixels to scroll. 1 is 1 and 9 is 9 it is
*  not 1 less or 1 more than what you want. It is *exact*.
*/

//void ILI9341_due_gText::ScrollUp(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, 
//	uint8_t pixels, uint8_t color)
//{
//uint8_t dy;
//uint8_t dbyte;
//uint8_t sy;
//uint8_t sbyte;
//uint8_t col;
//
//	/*
//	 * Scrolling up more than area height?
//	 */
//	if(y1 + pixels > y2)
//	{
//		/*
//		 * fill the region with "whitespace" because
//		 * it is being totally scrolled out.
//		 */
//		_ili->fillRect(x1, y1, x2 - x1, y2 - y1, color);
//		return;
//	}
//
//	for(col = x1; col <= x2; col++)
//	{
//		dy = y1;
//		GotoXY(col, dy & ~7);
//		dbyte = glcd_Device::ReadData();
//
//
//		/*
//		 * preserve bits outside/above scroll region
//		 */
//
//		dbyte &= (_BV((dy & 7)) - 1);
//
//		sy = dy + pixels;
//		glcd_Device::GotoXY(col, sy & ~7);
//		sbyte = glcd_Device::ReadData();
//
//		while(sy <= y2)
//		{
//			if(sbyte & _BV(sy & 7))
//			{
//				dbyte |= _BV(dy & 7);
//			}
//
//			sy++;
//			if((sy & 7) == 0)
//			{
//				/*
//				 * If we just crossed over, then we should be done.
//				 */
//				if(sy < DISPLAY_HEIGHT)
//				{
//					glcd_Device::GotoXY(col, sy & ~7);
//					sbyte = glcd_Device::ReadData();
//				}
//			}
//
//			if((dy & 7) == 7)
//			{
//				glcd_Device::GotoXY(col, dy & ~7);	// Should be able to remove this
//				glcd_Device::WriteData(dbyte);
//				dbyte = 0;
//			}
//			dy++;
//		}
//
//		/*
//		 * Handle the new area at the bottom of the region
//		 */
//
//		for(uint8_t p = pixels; p; p--)
//		{
//			if(color == BLACK)
//			{
//				dbyte |= _BV(dy & 7);
//			}
//			else
//			{
//				dbyte &= ~_BV(dy & 7);
//			}
//
//			if((dy & 7) == 7)
//			{
//				glcd_Device::GotoXY(col, dy & ~7); // should be able to remove this.
//				glcd_Device::WriteData(dbyte);
//				dbyte = 0;
//			}
//			dy++;
//		}
//
//		/*
//		 * Flush out the final destination byte
//		 */
//
//
//		if(dy & 7)
//		{
//			dy--;
//
//			glcd_Device::GotoXY(col, dy & ~7);
//			sbyte = glcd_Device::ReadData();
//			/*
//			 * Preserver bits outside/below region
//			 */
//
//			dy++;
//			sbyte &= ~(_BV((dy & 7)) - 1);
//			dbyte |= sbyte;
//
//			glcd_Device::WriteData(dbyte);
//		}
//	}
//
//}

//#ifndef GLCD_NO_SCROLLDOWN
//
//void ILI9341_due_gText::ScrollDown(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, 
//	uint8_t pixels, uint8_t color)
//{
//uint8_t dy;
//uint8_t dbyte;
//uint8_t sy;
//uint8_t sbyte;
//uint8_t col;
//
//	/*
//	 * Scrolling up more than area height?
//	 */
//	if(y1 + pixels > y2)
//	{
//		/*
//		 * fill the region with "whitespace" because
//		 * it is being totally scrolled out.
//		 */
//		glcd_Device::SetPixels(x1, y1, x2, y2, color);
//		return;
//	}
//
//	/*
//	 * Process region from left to right
//	 */
//	for(col = x1; col <= x2; col++)
//	{
//		dy = y2;
//		glcd_Device::GotoXY(col, dy & ~7);
//		dbyte = glcd_Device::ReadData();
//
//		/*
//		 * preserve bits outside/below scroll region
//		 */
//
//		dbyte &= ~(_BV(((dy & 7)+1)) - 1);
//		sy = dy - pixels;
//		glcd_Device::GotoXY(col, sy & ~7);
//		sbyte = glcd_Device::ReadData();
//
//		while(sy >= y1)
//		{
//			if(sbyte & _BV(sy & 7))
//			{
//				dbyte |= _BV(dy & 7);
//			}
//			if((dy & 7) == 0)
//			{
//				glcd_Device::GotoXY(col, dy & ~7);	// Should be able to remove this
//				glcd_Device::WriteData(dbyte);
//				dbyte = 0;
//			}
//			dy--;
//
//			if(!sy)
//				break; /* if we bottomed out, we are done */
//			sy--;
//			if((sy & 7) == 7)
//			{
//				glcd_Device::GotoXY(col, sy & ~7);
//				sbyte = glcd_Device::ReadData();
//			}
//
//		}
//
//		/*
//		 * Handle the new area at the top of the column
//		 */
//
//		for(uint8_t p = pixels; p; p--)
//		{
//			if(color == BLACK)
//			{
//				dbyte |= _BV(dy & 7);
//			}
//			else
//			{
//				dbyte &= ~_BV(dy & 7);
//			}
//
//			if((dy & 7) == 0)
//			{
//				glcd_Device::GotoXY(col, dy & ~7); // should be able to remove this.
//				glcd_Device::WriteData(dbyte);
//				dbyte = 0;
//			}
//			dy--;
//		}
//
//		dy++; /* point dy back to last destination row */
//
//		/*
//		 * Flush out the final destination byte
//		 */
//
//		if(dy & 7)
//		{
//			glcd_Device::GotoXY(col, dy & ~7);
//			sbyte = glcd_Device::ReadData();
//			/*
//			 * Preserve bits outside/above region
//			 */
//
//			sbyte &= (_BV((dy & 7)) - 1);
//			dbyte |= sbyte;
//			glcd_Device::WriteData(dbyte);
//		}
//
//	}
//
//}
//#endif //GLCD_NO_SCROLLDOWN





/*
* Handle all special processing characters
*/
void ILI9341_due_gText::specialChar(uint8_t c)
{


	if(c == '\n')
	{
		uint8_t height = fontHeight();

		/*
		* Erase all pixels remaining to edge of text area.on all wraps
		* It looks better when using inverted (WHITE) text, on proportional fonts, and
		* doing WHITE scroll fills.
		*
		*/


		if(_x < _area.x2)
			_ili->fillRect(_x, _y, _area.x2 - _x, height, _fontBgColor);
		//glcd_Device::SetPixels(_x, _y, _area.x2, _y+height, _fontColor == BLACK ? WHITE : BLACK);

		/*
		* Check for scroll up vs scroll down (scrollup is normal)
		*/
#ifndef GLCD_NO_SCROLLDOWN
		if(_area.mode == SCROLL_UP)
#endif
		{

			/*
			* Normal/up scroll
			*/

			/*
			* Note this comparison and the pixel calcuation below takes into 
			* consideration that fonts
			* are atually 1 pixel taller when rendered. 
			* This extra pixel is along the bottom for a "gap" between the character below.
			*/
			if(_y + 2*height >= _area.y2)
			{
#ifndef GLCD_NODEFER_SCROLL
				if(!_needScroll)
				{
					_needScroll = 1;
					return;
				}
#endif

				/*
				* forumula for pixels to scroll is:
				*	(assumes "height" is one less than rendered height)
				*
				*		pixels = height - ((_area.y2 - _y)  - height) +1;
				*
				*		The forumala below is unchanged 
				*		But has been re-written/simplified in hopes of better code
				*
				*/

				uint8_t pixels = 2*height + _y - _area.y2 +1;

				/*
				* Scroll everything to make room
				* * NOTE: (FIXME, slight "bug")
				* When less than the full character height of pixels is scrolled,
				* There can be an issue with the newly created empty line.
				* This is because only the # of pixels scrolled will be colored.
				* What it means is that if the area starts off as white and the text
				* color is also white, the newly created empty text line after a scroll 
				* operation will not be colored BLACK for the full height of the character.
				* The only way to fix this would be alter the code use a "move pixels"
				* rather than a scroll pixels, and then do a clear to end line immediately
				* after the move and wrap.
				*
				* Currently this only shows up when
				* there are are less than 2xheight pixels below the current Y coordinate to
				* the bottom of the text area
				* and the current background of the pixels below the current text line
				* matches the text color
				* and  a wrap was just completed.
				*
				* After a full row of text is printed, the issue will resolve itself.
				* 
				* 
				*/
				//ScrollUp(_area.x1, _area.y1, _area.x2, _area.y2, pixels, _fontBgColor);

				_x = _area.x1;
				_y = _area.y2 - height;
			}
			else
			{
				/*
				* Room for simple wrap
				*/

				_x = _area.x1;
				_y = _y+height+1;
			}
		}
#ifndef GLCD_NO_SCROLLDOWN
		else
		{
			/*
			* Reverse/Down scroll
			*/

			/*
			* Check for Wrap vs scroll.
			*
			* Note this comparison and the pixel calcuation below takes into 
			* consideration that fonts
			* are atually 1 pixel taller when rendered. 
			*
			*/
			if(_y > _area.y1 + height)
			{
				/*
				* There is room so just do a simple wrap
				*/
				_x = _area.x1;
				_y = _y - (height+1);
			}
			else
			{
#ifndef GLCD_NODEFER_SCROLL
				if(!_needScroll)
				{
					_needScroll = 1;
					return;
				}
#endif

				/*
				* Scroll down everything to make room for new line
				*	(assumes "height" is one less than rendered height)
				*/

				uint8_t pixels = height+1 - (_area.y1 - _y);

				//ScrollDown(_area.x1, _area.y1, _area.x2, _area.y2, pixels, _fontBgColor);

				_x = _area.x1;
				_y = _area.y1;
			}
		}
#endif
	}

}

/**
* output a character
*
* @param c the character to output
*
* If the character will not fit on the current text line
* inside the text area,
* the text position is wrapped to the next line. This might be
* the next lower or the next higher line depending on the
* scroll direction.
*
* If there is not enough room to fit a full line of new text after
* wrapping, the entire text area will be scrolled to make room for a new
* line of text. The scroll direction will be up or down
* depending on the scroll direction for the text area.
*/

int ILI9341_due_gText::putChar(uint8_t c)
{
	if(_font == 0)
	{
		Serial.println(F("No font selected"));
		return 0; // no font selected
	}

	/*
	* check for special character processing
	*/

	if(c < 0x20)
	{
		//specialChar(c);
		return 1;
	}
	uint16_t charWidth = 0;
	uint16_t charHeight = fontHeight();
	uint8_t charHeightInBytes = (charHeight+7)/8; /* calculates height in rounded up bytes */

	uint8_t firstChar = pgm_read_byte(_font+GTEXT_FONT_FIRST_CHAR);
	uint8_t charCount = pgm_read_byte(_font+GTEXT_FONT_CHAR_COUNT);

	uint16_t index = 0;
	uint8_t thielefont;

	if(c < firstChar || c >= (firstChar+charCount)) {
		return 0; // invalid char
	}
	c-= firstChar;

	if( isFixedWidthFont(_font) {
		thielefont = 0;
		charWidth = pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH); 
		index = c*charHeightInBytes*charWidth+GTEXT_FONT_WIDTH_TABLE;
	}
	else{
		// variable width font, read width data, to get the index
		thielefont = 1;
		/*
		* Because there is no table for the offset of where the data
		* for each character glyph starts, run the table and add up all the
		* widths of all the characters prior to the character we
		* need to locate.
		*/
		for(uint8_t i=0; i<c; i++) {  
			index += pgm_read_byte(_font+GTEXT_FONT_WIDTH_TABLE+i);
		}
		/*
		* Calculate the offset of where the font data
		* for our character starts.
		* The index value from above has to be adjusted because
		* there is potentialy more than 1 byte per column in the glyph,
		* when the characgter is taller than 8 bits.
		* To account for this, index has to be multiplied
		* by the height in bytes because there is one byte of font
		* data for each vertical 8 pixels.
		* The index is then adjusted to skip over the font width data
		* and the font header information.
		*/

		index = index*charHeightInBytes+charCount+GTEXT_FONT_WIDTH_TABLE;

		/*
		* Finally, fetch the width of our character
		*/
		charWidth = pgm_read_byte(_font+GTEXT_FONT_WIDTH_TABLE+c);
	}

#ifndef GLCD_NODEFER_SCROLL
	/*
	* check for a defered scroll
	* If there is a deferred scroll,
	* Fake a newline to complete it.
	*/

	if(_needScroll)
	{
		putChar('\n'); // fake a newline to cause wrap/scroll
		_needScroll = 0;
	}
#endif

	/*
	* If the character won't fit in the text area,
	* fake a newline to get the text area to wrap and 
	* scroll if necessary.
	* NOTE/WARNING: the below calculation assumes a 1 pixel pad.
	* This will need to be changed if/when configurable pixel padding is supported.
	*/
	if(_x + charWidth > _area.x2)
	{
		putChar('\n'); // fake a newline to cause wrap/scroll
#ifndef GLCD_NODEFER_SCROLL
		/*
		* We can't defer a scroll at this point since we need to ouput
		* a character right now.
		*/
		if(_needScroll)
		{
			putChar('\n'); // fake a newline to cause wrap/scroll
			_needScroll = 0;
		}
#endif
	}

	if(_fontMode == gTextFontMode_Solid)
		drawSolidChar(c, index, charWidth, charHeight);
	else if(_fontMode == gTextFontMode_Transparent)
		drawTransparentChar(c, index, charWidth, charHeight);

	// last but not least, draw the character

	//glcd_Device::GotoXY(_x, _y);


	/*
	* Draw each column of the glyph (character) horizontally
	* 8 bits (1 page) at a time.
	* i.e. if a font is taller than 8 bits, draw upper 8 bits first,
	* Then drop down and draw next 8 bits and so on, until done.
	* This code depends on WriteData() doing writes that span LCD
	* memory pages, which has issues because the font data isn't
	* always a multiple of 8 bits.
	*/


	return 1; // valid char
}

void ILI9341_due_gText::drawSolidChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight)
{
	uint8_t bitId=0;
	int16_t cx = _x;
	int16_t cy = _y;
#if SPI_MODE_DMA
	uint8_t* scanlineBuff = _ili->_scanlineBuffer;
	uint8_t fontColorHi = highByte(_fontColor);
	uint8_t fontColorLo = lowByte(_fontColor);
	uint8_t fontBgColorHi = highByte(_fontBgColor);
	uint8_t fontBgColorLo = lowByte(_fontBgColor);
#endif
	uint8_t charHeightInBytes = (charHeight+7)/8; /* calculates height in rounded up bytes */

	uint16_t lineId=0;
	uint8_t numRenderBits = 8;
	const uint8_t numRemainingBits = charHeight % 8;
	//_ili->enableCS();
	for(uint16_t j=0; j<charWidth; j++) /* each column */
	{
		//Serial << "Printing row" << endl;
		lineId = 0;
		numRenderBits = 8;
		_ili->setAddrAndRW_cont(cx, cy, cx, cy+charHeight -1);
		//_ili->setDCForData();
		for(uint8_t i=0; i<charHeightInBytes; i++)	/* each vertical byte */
		{
			uint16_t page = i*charWidth; // page must be 16 bit to prevent overflow
			uint8_t data = pgm_read_byte(_font+index+page+j);

			/*
			* This funkyness is because when the character glyph is not a
			* multiple of 8 in height, the residual bits in the font data
			* were aligned to the incorrect end of the byte with respect 
			* to the GLCD. I believe that this was an initial oversight (bug)
			* in Thieles font creator program. It is easily fixed
			* in the font program but then creates a potential backward
			* compatiblity problem.
			*	--- bperrybap
			*/
			if(charHeight > 8 && charHeight < (i+1)*8)	/* is it last byte of multibyte tall font? */
			{
				data >>= ((i+1) << 3) - charHeight;	// (i+1)*8
			}
			//Serial << "data:" <<data << " x:" << cx << " y:" << cy << endl;


			if(i == charHeightInBytes - 1)	// last byte in column
				numRenderBits = numRemainingBits;

			for( bitId = 0; bitId < numRenderBits; bitId++ )
			{
				if ( (data & 0x01) == 0 )
				{
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
					_ili->writedata16_cont(_fontBgColor);
#elif SPI_MODE_DMA
					scanlineBuff[lineId++] = fontBgColorHi;
					scanlineBuff[lineId++] = fontBgColorLo;
#endif
				}
				else
				{
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
					_ili->writedata16_cont(_fontColor);
#elif SPI_MODE_DMA
					scanlineBuff[lineId++] = fontColorHi;
					scanlineBuff[lineId++] = fontColorLo;
#endif
				}
				data>>=1;
			} 
			//delay(50);
		}
		//Serial << endl;
		cx++;
#if SPI_MODE_DMA
		_ili->writeScanline_cont(charHeight);	// height in px * 2 bytes per px
#endif

	}
	_ili->disableCS();	// to put CS line back up

	_x += charWidth;

	//Serial << " ending at " << _x  << " lastChar " << _lastChar <<endl;

	if(_letterSpacing > 0 && !_isLastChar)
	{
		_ili->fillRect(_x, _y, _letterSpacing, charHeight, _fontBgColor);
		_x+=_letterSpacing;
	}
	//Serial << "letterSpacing " << _letterSpacing <<" x: " << _x <<endl;
}

void ILI9341_due_gText::drawTransparentChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight)
{
	uint8_t bitId=0;
	uint8_t bit=0, lastBit=0;
	int16_t cx = _x;
	int16_t cy = _y;
	uint16_t lineStart = 0;
	uint16_t lineEnd = 0;

#if SPI_MODE_DMA
	_ili->fillScanline(_fontColor);	//pre-fill the scanline, we will be drawing different lenghts of it
#endif

	uint8_t charHeightInBytes = (charHeight+7)/8; /* calculates height in rounded up bytes */

	uint16_t lineId=0;
	uint8_t numRenderBits = 8;
	const uint8_t numRemainingBits = charHeight % 8;

	_ili->enableCS();

	for(uint8_t j=0; j<charWidth; j++) /* each column */
	{
		//Serial << "Printing row" << endl;
		lineId = 0;
		numRenderBits = 8;

		for(uint8_t i=0; i<charHeightInBytes; i++)	/* each vertical byte */
		{
			uint16_t page = i*charWidth; // page must be 16 bit to prevent overflow
			uint8_t data = pgm_read_byte(_font+index+page+j);

			/*
			* This funkyness is because when the character glyph is not a
			* multiple of 8 in height, the residual bits in the font data
			* were aligned to the incorrect end of the byte with respect 
			* to the GLCD. I believe that this was an initial oversight (bug)
			* in Thieles font creator program. It is easily fixed
			* in the font program but then creates a potential backward
			* compatiblity problem.
			*	--- bperrybap
			*/
			if(charHeight > 8 && charHeight < (i+1)*8)	/* is it last byte of multibyte tall font? */
			{
				data >>= ((i+1) << 3) - charHeight;	// (i+1)*8
			}
			//Serial << "data:" <<data << " x:" << cx << " y:" << cy << endl;

			if(i == 0)
			{
				bit = lastBit = lineStart = 0;
			}
			else if(i == charHeightInBytes - 1)	// last byte in column
				numRenderBits = numRemainingBits;

			for( bitId = 0; bitId < numRenderBits; bitId++ )
			{
				bit = data & 0x01;
				if(bit ^ lastBit)	// same as bit != lastBit
				{
					if(bit ^ 0x00) // if bit != 0 (so it's 1)
					{	
						lineStart = lineEnd = i*8+bitId;
					} 
					else 
					{
						_ili->setAddrAndRW_cont(cx, cy+lineStart, cx, cy+lineEnd);

#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
						_ili->setDCForData();
						for(int p=0; p<lineEnd-lineStart+1; p++)
						{
							_ili->write16_cont(_fontColor);
						}
#elif SPI_MODE_DMA
						_ili->writeScanline_cont(lineEnd-lineStart+1);
#endif
					}
					lastBit = bit;
				} 
				else if(bit ^ 0x00)	// increment only if bit is 1
				{
					lineEnd++;
				}

				data>>=1;
			}

			if(lineEnd == charHeight - 1)	// we have a line that goes all the way to the bottom
			{
				_ili->setAddrAndRW_cont(cx, cy+lineStart, cx, cy+lineEnd);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
				_ili->setDCForData();
				for(int p=0; p<lineEnd-lineStart+1; p++)
				{
					_ili->write16_cont(_fontColor);
				}
#elif SPI_MODE_DMA
				_ili->writeScanline_cont(lineEnd-lineStart+1);
#endif
			}
			//delay(25);
		}
		//Serial << endl;
		cx++;
	}
	_ili->disableCS();	// to put CS line back up

	_x += charWidth;

	if(_letterSpacing > 0 && !_isLastChar)
	{
		_x+=_letterSpacing;
	}
}

/**
* output a character string
*
* @param str pointer to a null terminated character string.
*
* Outputs all the characters in the string to the text area. 
* See putChar() for a full description of how characters are
* written to the text area.
*/

void ILI9341_due_gText::puts(char *str)
{
	while(*str)
	{
		if(*(str+1) == 0)
			_isLastChar = true;
		putChar((uint8_t)*str);
		str++;
	}
	_isLastChar = false;
}

/**
* output a String class string
*
* @param str String class string
*
* Outputs all the characters in the string to the text area. 
* See putChar() for a full description of how characters are
* written to the text area.
*
*/
void ILI9341_due_gText::puts(const String &str)
{
	for (int i = 0; i < str.length(); i++)
	{
		write(str[i]);
	}
}

/**
* output a program memory character string
*
* @param str pointer to a null terminated character string stored in program memory
*
* Outputs all the characters in the string to the text area. 
* See putChar() for a full description of how characters are
* written to the text area.
*
* @see putChar()
* @see puts()
* @see drawString()
* @see drawString_P()
* @see write()
*/

void ILI9341_due_gText::puts_P(PGM_P str)
{
	uint8_t c;

	while((c = pgm_read_byte(str)) != 0)
	{
		putChar(c);
		str++;
	}
}

/**
* output a String class string at x,y coordinate
*
* @param str pointer to a null terminated character string
* @param x specifies the horizontal location
* @param y specifies the vertical location
*
*
* Outputs all the characters in the string to the text area. 
* X & Y are zero based pixel coordinates and are relative to 
* the upper left corner of the text area.
*
* See putChar() for a full description of how characters are
* written to the text area.
*/
void ILI9341_due_gText::drawString(String &str, int16_t x, int16_t y)
{
	cursorToXY(x,y);
	puts(str);
}

/**
* output a program memory character string at x,y coordinate
*
* @param str pointer to a null terminated character string stored in program memory
* @param x specifies the horizontal location
* @param y specifies the vertical location
*
*
* Outputs all the characters in the string to the text area. 
* X & Y are zero based pixel coordinates and are relative to 
* the upper left corner of the text area.
*
* See putChar() for a full description of how characters are
* written to the text area.
*/

void ILI9341_due_gText::drawString_P(PGM_P str, int16_t x, int16_t y)
{
	cursorToXY(x,y);
	puts_P(str);
}

/**
* output a character string at x,y coordinate
*
* @param str String class string
* @param x specifies the horizontal location
* @param y specifies the vertical location
*
*
* Outputs all the characters in the string to the text area. 
* X & Y are zero based pixel coordinates and are relative to 
* the upper left corner of the text area.
*
* See putChar() for a full description of how characters are
* written to the text area.
*/

void ILI9341_due_gText::drawString(char *str, int16_t x, int16_t y)
{
	cursorToXY(x,y);
	puts(str);
}

void ILI9341_due_gText::drawString(char *str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	cursorToXY(x,y);

	// CLEAR PIXELS ON THE LEFT
	if(pixelsClearedOnLeft > 0)
		_ili->fillRect(_x-pixelsClearedOnLeft, _y, pixelsClearedOnLeft, fontHeight(), _fontBgColor); 

	puts(str);

	// CLEAR PIXELS ON THE RIGHT
	if(pixelsClearedOnRight > 0)
		_ili->fillRect(_x, _y, pixelsClearedOnRight, fontHeight(), _fontBgColor); 
}

void ILI9341_due_gText::drawString(char *str, int16_t x, int16_t y, gTextEraseLine eraseLine)
{
	cursorToXY(x,y);

	// CLEAR PIXELS ON THE LEFT
	if(eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
	{
		uint16_t clearX1 = max(min(_x, _area.x1), _x-1024);		
		_ili->fillRect(clearX1, _y, _x - clearX1, fontHeight(), _fontBgColor); 
	}

	puts(str);

	// CLEAR PIXELS ON THE RIGHT
	if(eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
	{
		uint16_t clearX2 = min(max(_x,_area.x2), _x+1024);
		_ili->fillRect(_x, _y, clearX2 - _x, fontHeight(), _fontBgColor); 
	}
}

void ILI9341_due_gText::drawString(char *str, gTextAlign align)
{
	drawStringPivotedOffseted(str, align, gTextPivotDefault, 0, 0, 0, 0);
}

void ILI9341_due_gText::drawString(char *str, gTextAlign align, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if(eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if(eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	drawStringPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawString(char *str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	drawStringPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringOffseted(char *str, gTextAlign align, uint16_t offsetX, uint16_t offsetY)
{
	drawStringPivotedOffseted(str, align, gTextPivotDefault,offsetX, offsetY, 0, 0);
}

void ILI9341_due_gText::drawStringOffseted(char *str, gTextAlign align, uint16_t offsetX, uint16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if(eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if(eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	drawStringPivotedOffseted(str, align, gTextPivotDefault,offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringOffseted(char *str, gTextAlign align, uint16_t offsetX, uint16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	drawStringPivotedOffseted(str, align, gTextPivotDefault,offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringPivoted(char *str, int16_t x, int16_t y, gTextPivot pivot)
{
	cursorToXY(x,y);

	if(pivot != gTextPivotTopLeft && pivot != gTextPivotDefault)
		applyPivot(str, pivot);

	puts(str);
}

void ILI9341_due_gText::drawStringPivoted(char *str, gTextAlign align, gTextPivot pivot)
{
	drawStringPivotedOffseted(str, align, pivot, 0, 0, 0, 0);
}

void ILI9341_due_gText::drawStringPivoted(char *str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if(eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if(eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	drawStringPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringPivoted(char *str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	drawStringPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringPivotedOffseted(char *str, gTextAlign align, gTextPivot pivot, uint16_t offsetX, uint16_t offsetY)
{
	drawStringPivotedOffseted(str, align, pivot, offsetX, offsetY, 0, 0);
}

void ILI9341_due_gText::drawStringPivotedOffseted(char *str, gTextAlign align, gTextPivot pivot, uint16_t offsetX, uint16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if(eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if(eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	drawStringPivotedOffseted(str, align, pivot, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due_gText::drawStringPivotedOffseted(char *str, gTextAlign align, gTextPivot pivot, uint16_t offsetX, uint16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	//Serial << pixelsClearedOnLeft << " " << pixelsClearedOnRight << endl;
	_x = _area.x1;
	_y = _area.y1;

	//PIVOT
	if(pivot == gTextPivotDefault)
	{
		switch(align)
		{
		case gTextAlignTopLeft: { pivot = gTextPivotTopLeft; break;	}
		case gTextAlignTopCenter: { pivot = gTextPivotTopCenter; break;	}
		case gTextAlignTopRight: { pivot = gTextPivotTopRight; break; }
		case gTextAlignMiddleLeft: { pivot = gTextPivotMiddleLeft; break; }
		case gTextAlignMiddleCenter: { pivot = gTextPivotMiddleCenter; break; }
		case gTextAlignMiddleRight: { pivot = gTextPivotMiddleRight; break; }
		case gTextAlignBottomLeft: { pivot = gTextPivotBottomLeft; break; }
		case gTextAlignBottomCenter: { pivot = gTextPivotBottomCenter; break; }
		case gTextAlignBottomRight: { pivot = gTextPivotBottomRight; break; }
		}
	}

	if(pivot != gTextPivotTopLeft)
		applyPivot(str, pivot);

	// ALIGN
	if(align != gTextAlignTopLeft)
	{
		switch(align)
		{
		case gTextAlignTopCenter:
			{
				_x += (_area.x2 - _area.x1)/2;
				break;
			}
		case gTextAlignTopRight:
			{
				_x += _area.x2 - _area.x1;
				break;
			}
		case gTextAlignMiddleLeft:
			{
				_y += (_area.y2-_area.y1)/2;
				break;
			}
		case gTextAlignMiddleCenter:
			{
				_x += (_area.x2 - _area.x1)/2;
				_y += (_area.y2 - _area.y1)/2;
				break;
			}
		case gTextAlignMiddleRight:
			{
				_x += _area.x2 - _area.x1;
				_y += (_area.y2 - _area.y1)/2;
				break;
			}
		case gTextAlignBottomLeft:
			{
				_y += _area.y2 - _area.y1;
				break;
			}
		case gTextAlignBottomCenter:
			{
				_x += (_area.x2 - _area.x1)/2;
				_y += _area.y2 - _area.y1;
				break;
			}
		case gTextAlignBottomRight:
			{
				_x += _area.x2 - _area.x1;
				_y += _area.y2 - _area.y1;
				break;
			}
		}
	}

	// OFFSET
	_x+=offsetX;
	_y+=offsetY;

	// CLEAR PIXELS ON THE LEFT
	if(pixelsClearedOnLeft > 0)
	{
		int16_t clearX1 = max(min(_x, (int16_t)_area.x1), _x-(int16_t)pixelsClearedOnLeft);		
		//Serial.println(clearX1);
		_ili->fillRect(clearX1, _y, _x - clearX1, fontHeight(), _fontBgColor); 
	}

	puts(str);

	// CLEAR PIXELS ON THE RIGHT
	if(pixelsClearedOnRight > 0)
	{
		int16_t clearX2 = min(max(_x, _area.x2), _x+pixelsClearedOnRight);
		//Serial << "area from " << _area.x1 << " to " << _area.x2 << endl;
		//Serial << "clearing on right from " << _x << " to " << clearX2 << endl;
		_ili->fillRect(_x, _y, clearX2 - _x, fontHeight(), _fontBgColor); 
	}
}

void ILI9341_due_gText::applyPivot(char *str, gTextPivot pivot)
{
	switch(pivot)
	{
	case gTextPivotTopCenter:
		{
			_x -= stringWidth(str)/2;
			break;
		}
	case gTextPivotTopRight:
		{
			_x -= stringWidth(str);
			break;
		}
	case gTextPivotMiddleLeft:
		{
			_y -= fontHeight()/2;
			break;
		}
	case gTextPivotMiddleCenter:
		{
			_x -= stringWidth(str)/2;
			_y -= fontHeight()/2;			
			break;
		}
	case gTextPivotMiddleRight:
		{
			_x -= stringWidth(str);
			_y -= fontHeight()/2;
			break;
		}
	case gTextPivotBottomLeft:
		{
			_y -= fontHeight();
			break;
		}
	case gTextPivotBottomCenter:
		{
			_x -= stringWidth(str)/2;
			_y -= fontHeight();
			break;
		}
	case gTextPivotBottomRight:
		{
			_x -= stringWidth(str);
			_y -= fontHeight();
			break;
		}
	}
}

/**
* Positions cursor to a character based column and row.
*
* @param column specifies the horizontal position 
* @param row  specifies the vertical position
*
*	Column and Row are zero based character positions
*	and are relative the the upper left corner of the
*	text area base on the size of the currently selected font.
*
* While intended for fixed width fonts, positioning will work for variable
* width fonts.
*
* When variable width fonts are used, the column is based on assuming a width
* of the widest character.
*
* @see cursorToXY()
*/

void ILI9341_due_gText::cursorTo( uint8_t column, uint8_t row)
{
	if(_font == 0)
		return; // no font selected

	/*
	* Text position is relative to current text area
	*/

	_x = column * (pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH)+1) + _area.x1;
	_y = row * (fontHeight()+1) + _area.y1;

#ifndef GLCD_NODEFER_SCROLL
	/*
	* Make sure to clear a deferred scroll operation when repositioning
	*/
	_needScroll = 0;
#endif
}

// Bill, I think the following would be a useful addition to the API
// Should we add a sanity check to these?
/**
* Positions cursor to a character based column on the current row.
*
* @param column specifies the horizontal position 
*
*	Column is a 0 based character position
*	based on the size of the currently selected font.
*
* If column is negative then the column position is relative to the current cursor
* position.
*
* @warning
* While intended only for fixed width fonts, cursor repositioning will be done for variable
* width fonts.
* When variable width fonts are used, the column is based on assuming a width
* of the widest character in the font.
* Because the widest character is used for the amount of cursor movement, the amount
* of cursor movement when using relative positioning will often not be consistent with
* the number characters previously rendered. For example, if a letter "l" was written
* and the cursor was reposisitioned with a -1, the amount backed up will be much larger
* than the width of the "l".
*
*
* @see cursorToXY()
*/
void ILI9341_due_gText::cursorTo(int8_t column)
{
	if(_font == 0)
		return; // no font selected
	/*
	* Text position is relative to current text area
	* negative value moves the cursor backwards
	*/
	if(column >= 0) 
		_x = column * (pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH)+1) + _area.x1;
	else
		_x -= column * (pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH)+1);   	

#ifndef GLCD_NODEFER_SCROLL
	/*
	* Make sure to clear a deferred scroll operation when repositioning
	*/
	_needScroll = 0;
#endif
}


/**
* Positions cursor to a X,Y position
*
* @param x specifies the horizontal location
* @param y specifies the vertical location
*
*	X & Y are zero based pixel coordinates and are relative to 
*	the upper left corner of the text area.
*
* @see cursorTo()
*/

void ILI9341_due_gText::cursorToXY( int16_t x, int16_t y)
{

	/*
	* Text position is relative to current text area
	*/

	_x = _area.x1 + x;
	_y = _area.y1 + y;
	//Serial << F("cursorToXY x:") << x << F(" y:") << y << endl;

#ifndef GLCD_NODEFER_SCROLL
	/*
	* Make sure to clear a deferred scroll operation when repositioning
	*/
	_needScroll = 0;
#endif
}

/**
* Erase in Line
*
* @param type type of line erase
*
* @arg \ref eraseTO_EOL Erase from cursor to end of line
* @arg \ref eraseFROM_BOL Erase from beginning of line to cursor
* @arg \ref eraseFULL_LINE Erase entire line
*
* Erases all or part of a line of text depending on the type
* of erase specified.
*
* If type is not specified it is assumed to be \ref eraseTO_EOL
*
* The cursor position does not change.
*
* @see ClearArea()
* @see eraseLine_t
*/

void ILI9341_due_gText::eraseTextLine(uint16_t color, gTextEraseLine type) 
{
	int16_t x = _x;
	int16_t y = _y;
	int16_t height = fontHeight();
	//uint8_t color = (_fontColor == BLACK) ? WHITE : BLACK;

	switch(type)
	{
	case gTextEraseToEOL:
		_ili->fillRect(x, y, _area.x2-x, height, color);
		//glcd_Device::SetPixels(x, y, _area.x2, y+height, color);
		break;
	case gTextEraseFromBOL:
		_ili->fillRect(_area.x1, y, x - _area.x1, height, color);
		//glcd_Device::SetPixels(_area.x1, y, x, y+height, color);
		break;
	case gTextEraseFullLine:
		_ili->fillRect(_area.x1, y, _area.x2 - _area.x1, height, color);
		//glcd_Device::SetPixels(_area.x1, y, _area.x2, y+height, color);
		break;
	}

	/*
	* restore cursor position
	*/
	cursorToXY(x,y);
}

/**
* Erase Text Line
*
* @param row row # of text to earase
*
* Erases a line of text and moves the cursor
* to the begining of the line. Rows are zero based so
* the top line/row of a text area is 0.
*
* @see ClearArea()
*/

void ILI9341_due_gText::eraseTextLine(uint16_t color, uint8_t row)
{
	cursorTo(0, row);
	eraseTextLine(color, gTextEraseToEOL);	
}


/**
* Select a Font and font color
*
* @param font a font definition
* @param color  can be WHITE or BLACK and defaults to black
* @param callback optional font read routine
*
*
* Selects the font definition as the current font for the text area.
*
* All subsequent printing functions will use this font. 
*
* Font definitions from included font definition files are stored in program memory 
* You can have as many fonts defines as will fit in program memory up to 64k and can
* switch between them with this function.
*
* If the optional callback argument is ommitted, a default routine
* is selected that assumes that the font is in program memory (flash).
*
* @note
* When the display is initilized in normal mode, BLACK renders dark 
* pixels on a white background and WHITE renders white pixels on 
* black background; however, if the display is set to INVERTED mode
* all colors are inverted. 
*
* @see setFontColor()
* @see SetTextMode()
*/

void ILI9341_due_gText::selectFont(gTextFont font)
{
	_font = font;
}

void ILI9341_due_gText::selectFont(gTextFont font, uint16_t fontColor)
{
	_font = font;
	_fontColor = fontColor;
}

void ILI9341_due_gText::selectFont(gTextFont font, uint16_t fontColor, uint16_t backgroundColor)
{
	_font = font;
	_fontColor = fontColor;
	_fontBgColor = backgroundColor;
}

/**
* Select a font color
*
* @param color  can be WHITE or BLACK
*
*
* @see selectFont()
* @see SetTextMode()
*/

void ILI9341_due_gText::setFontColor(uint16_t color)
{
	_fontColor = color;
}

void ILI9341_due_gText::setFontColor(uint8_t R, uint8_t G, uint8_t B)
{
	_fontColor = _ili->color565(R,G,B);
}

void ILI9341_due_gText::setFontColor(uint16_t color, uint16_t backgroundColor)
{
	_fontColor = color;
	_fontBgColor = backgroundColor;
}

void ILI9341_due_gText::setFontColor(uint8_t R, uint8_t G, uint8_t B, uint8_t bgR, uint8_t bgG, uint8_t bgB)
{
	_fontColor = _ili->color565(R,G,B);
	_fontBgColor = _ili->color565(bgR,bgG,bgB);
}


void ILI9341_due_gText::setFontLetterSpacing(uint8_t letterSpacing)
{
	_letterSpacing = letterSpacing;
}


void ILI9341_due_gText::setFontMode(gTextFontMode fontMode)
{
	if(fontMode == gTextFontMode_Solid || fontMode == gTextFontMode_Transparent)
		_fontMode = fontMode;
}

/**
* Set TextArea mode
*
* @param mode  text area mode
*
* Currently mode is a scroll direction
* @arg SCROLL_UP
* @arg SCROLL_DOWN
*
* @see selectFont()
* @see setFontColor()
* @see defineArea()
*/
/*
*/
#if INLCUDE_NOT_WORKING_YET
void ILI9341_due_gText::SetTextMode(textMode mode)
{

	/*
	* when other modes are added the tarea.mode variable will hold a bitmask or enum for the modde and should be renamed
	*/
	_area.mode = mode; 
} 
#endif




/**
* Returns the pixel width of a character
*
* @param c character to be sized
*
* @return The width in pixels of the given character
* including any inter-character gap pixels following the character when the character is
* rendered on the display.
*
* @note The font for the character is the most recently selected font.
*
* @see StringWidth()
* @see StringWidth_P()
*/

uint16_t ILI9341_due_gText::charWidth(uint8_t c)
{
	int16_t width = 0;

	if(isFixedWidthFont(_font){
		width = pgm_read_byte(_font+GTEXT_FONT_FIXED_WIDTH);
	} 
	else{ 
		// variable width font 
		uint8_t firstChar = pgm_read_byte(_font+GTEXT_FONT_FIRST_CHAR);
		uint8_t charCount = pgm_read_byte(_font+GTEXT_FONT_CHAR_COUNT);

		// read width data
		if(c >= firstChar && c < (firstChar+charCount)) {
			c -= firstChar;
			width = pgm_read_byte(_font+GTEXT_FONT_WIDTH_TABLE+c);
			//Serial << "strWidth of " << c << " : " << width << endl;
		}
	}	
	return width;
}

/**
* Returns the pixel width of a string
*
* @param str pointer to string stored in RAM
*
* @return the width in pixels of the sum of all the characters in the
* the string pointed to by str. 
*
* @see charWidth()
* @see StringWidth_P()
*/

uint16_t ILI9341_due_gText::stringWidth(const char* str)
{
	uint16_t width = 0;

	while(*str != 0) {
		width += charWidth(*str++) + _letterSpacing;
	}
	if(width > 0)
		width -= _letterSpacing;
	return width;
}

/**
* Returns the pixel width of a character
*
* @param str pointer to string stored in program memory
*
* @return the width in pixels of the sum of all the characters in the
* the string pointed to by str. 
*
* @see charWidth()
* @see StringWidth()
*/

uint16_t ILI9341_due_gText::stringWidth_P(PGM_P str)
{
	uint16_t width = 0;

	while(pgm_read_byte(str) != 0) {
		width += charWidth(pgm_read_byte(str++)) + _letterSpacing;
	}
	width -= _letterSpacing;
	return width;
}

/**
* Returns the pixel width of a character
*
* @param str String class string
*
* @return the width in pixels of the sum of all the characters in the
* the string pointed to by str. 
*
* @see charWidth()
* @see StringWidth()
*/

uint16_t ILI9341_due_gText::stringWidth_P(String &str)
{
	uint16_t width = 0;

	for (int i = 0; i < str.length(); i++)
	{
		width += charWidth(str[i]) + _letterSpacing;
	}
	width -= _letterSpacing;
	return width;
}


/**
* Legacy function to print a number
*
* @param n is the number to print
*
*
* @see print(n)
*/
void ILI9341_due_gText::printNumber(long n)
{
	uint8_t buf[10];  // prints up to 10 digits  
	uint8_t i=0;
	if(n==0)
		putChar('0');
	else{
		if(n < 0){
			putChar('-');
			n = -n;
		}
		while(n>0 && i <= 10){
			buf[i++] = n % 10;  // n % base
			n /= 10;   // n/= base
		}
		for(; i >0; i--)
			putChar((char) (buf[i-1] < 10 ? '0' + buf[i-1] : 'A' + buf[i-1] - 10));	  
	}
}

/**
* output a character to the text area
* @param c the character to output
*
* This method is needed for the Print base class
* @see putChar()
*/


size_t ILI9341_due_gText::write(uint8_t c) 
{
	return(putChar(c));
} 

//
//#ifndef USE_ARDUINO_FLASHSTR
//// functions to store and print strings in Progmem
//// these should be removed when Arduino supports FLASH strings in the base print class
///**
//* print a flash based string
//* @param str pointer to a null terminated character string stored in program memory
//*
//* @see puts_P()
//*/
//void ILI9341_due_gText::printFlash(FLASHSTRING str)
//{
//	puts_P((PGM_P) str);
//}
//
///**
//* print a flash based string
//* @param str pointer to a null terminated character string stored in program memory
//*
//* The string is output followed by a newline.
//*
//* @see puts_P()
//*/
//void ILI9341_due_gText::printFlashln(FLASHSTRING str)
//{
//	printFlash(str);
//	write('\n');
//}
//#endif


