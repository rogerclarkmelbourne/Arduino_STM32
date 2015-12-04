/**
 *
 *
 *
 *
 */


#ifndef XPT2046_touch_h
#define XPT2046_touch_h
#include <Arduino.h>
#include <SPI.h>

#define Z_THRESHOLD   3000

// Pre-defined touch screen calibration for using the 2.4" ILI9341 LCD 
#define X_MIN          830
#define X_MAX         3800
#define Y_MIN          550
#define Y_MAX         3550

/**
 *
 */

class XPT2046_touch{
  private:
    uint8_t  cs_pin;
    SPIClass my_SPI;
    uint8_t  _rowButtons = 1;
    uint8_t  _columnButtons = 1;

  public:
    XPT2046_touch(uint8_t _cs_pin, SPIClass _spiChan); //Contructor.
    void begin();
    void setButtonsNumber(byte rowButtons, byte columnButtons);
    uint8_t getButtonNumber();
    boolean read_XY(uint16_t *xy);
};

#endif

