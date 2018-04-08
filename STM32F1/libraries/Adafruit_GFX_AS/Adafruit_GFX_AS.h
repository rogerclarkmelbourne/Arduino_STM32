#ifndef _ADAFRUIT_GFX_AS_H
#define _ADAFRUIT_GFX_AS_H

#include "Load_fonts.h"

#include <Adafruit_GFX.h>

#define swap(a, b) { int16_t t = a; a = b; b = t; }

/** This class provides a few extensions to Adafruit_GFX, mostly for compatibility with
 *  existing code. Note that the fonts ("size" parameter) are not the same ones use as the
 *  ones provided by Adafruit_GFX. Using any of the functions defined in this class will
 *  therefore pull additional font tables into flash. If that is an issue, try to stick
 *  to the base class, or trim down the fonts loaded in Load_fonts.h . */
class Adafruit_GFX_AS : public Adafruit_GFX {
public:
    Adafruit_GFX_AS(int16_t w, int16_t h); // Constructor
    int16_t drawUnicode(uint16_t uniCode, int16_t x, int16_t y, int16_t size);
    int16_t drawNumber(long long_num,int16_t poX, int16_t poY, int16_t size);
    int16_t drawChar(char c, int16_t x, int16_t y, int16_t size);
    int16_t drawString(char *string, int16_t poX, int16_t poY, int16_t size);
    int16_t drawCentreString(char *string, int16_t dX, int16_t poY, int16_t size);
    int16_t drawRightString(char *string, int16_t dX, int16_t poY, int16_t size);
    int16_t drawFloat(float floatNumber,int16_t decimal,int16_t poX, int16_t poY, int16_t size);
};

#endif // _ADAFRUIT_GFX_AS_H
