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

#define Z_THRESHOLD   3500 // Note: reversed for backwards compatiblity: 4095-x
#define MAX_OVERSAMPLING 32

// Pre-defined touch screen calibration for using the 2.4" ILI9341 LCD 
#define X_MIN          830
#define X_MAX         3800
#define Y_MIN          550
#define Y_MAX         3550


class TS_Point {
public:
    TS_Point(void) : x(0), y(0), z(0) {}
    TS_Point(int16_t x, int16_t y, int16_t z) : x(x), y(y), z(z) {}
    bool operator==(TS_Point p) { return ((p.x == x) && (p.y == y) && (p.z == z)); }
    bool operator!=(TS_Point p) { return ((p.x != x) || (p.y != y) || (p.z != z)); }
    int16_t x, y, z;
};

/**
 *
 */
class XPT2046_touch {
  private:
    uint8_t  cs_pin;
    SPIClass my_SPI;
    uint8_t  _rowButtons = 1;
    uint8_t  _columnButtons = 1;
    uint8_t  oversampling;
    uint16_t threshold;
    uint16_t gatherSamples(uint8_t command);

  public:
      /** c'tor. Note that no IRQ pin is supported, here. You can easily do that yourself:
       *  \code if(digitalRead(irq_pin)) {
       *           // no press: skip
       *        } else {
       *           // _may_ be touched, but not necessarily reach the threshold
       *           TS_Point p = ts.getPoint();
       *           if (p.z > 0) {
       *               // do something
       *           }
       *        }
       *  \endcode */
    XPT2046_touch(uint8_t _cs_pin, SPIClass _spiChan); //Contructor
    void begin();
    void setButtonsNumber(byte rowButtons, byte columnButtons);
    /** Number of samples to average per point 0..32 */
    void setOversampling(uint8_t num_readings = 7) {
        oversampling = max(1, min(num_readings, MAX_OVERSAMPLING));
    }
    void setThreshold(uint16_t threshold = 4095 - Z_THRESHOLD) {
        XPT2046_touch::threshold = threshold;
    }
    uint8_t getButtonNumber();
    boolean read_XY(uint16_t *xy);
    TS_Point getPoint();
};

#endif

