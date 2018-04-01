/**
 *
 *
 */



#include "Arduino.h"
#include "XPT2046_touch.h"

XPT2046_touch::XPT2046_touch(uint8_t _cs_pin, SPIClass _spiChan) : cs_pin(_cs_pin), my_SPI(_spiChan){
    setOversampling();
    setThreshold();
}

void XPT2046_touch::begin(){
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    my_SPI.begin();
}

uint16_t XPT2046_touch::gatherSamples(uint8_t command) {
    uint32_t sample_sum = 0;
    uint16_t samples[MAX_OVERSAMPLING];

    my_SPI.transfer16(command); // discard first reading after switching MUX
    for (int i = 0; i < oversampling; ++i) {
        samples[i] = my_SPI.transfer16(command);
        sample_sum += samples[i];
    }
    int32_t avg = sample_sum / oversampling;
    if (oversampling < 3) {
        return avg >> 3;
    }

    // occasionally, we get a reading that is a _far_ outlier.
    // checking for, and removing those improves quality a lot.
    uint8_t n = oversampling;
    for (int i = 0; i < oversampling; ++i) {
        if (abs(avg - samples[i]) > 80) {   // NOTE: data is left shifted by 3 at this point. I.e. the test is for 10+ pixels deviation from average
            sample_sum -= samples[i];
            --n;
        }
    }
    if (n < 2) return avg >> 3;
    else return (sample_sum / n) >> 3;
}

TS_Point XPT2046_touch::getPoint() {
    uint16_t z1, z2;

    my_SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));  // max clock freq for XPT2046
    digitalWrite(cs_pin, LOW);
    TS_Point ret;

    //Check if touch screen is pressed.
    my_SPI.transfer(B10110011); // trigger Z1 reading
    z1 = my_SPI.transfer16(B11000011) >> 3; // read Z1, and trigger Z2 reading
    z2 = my_SPI.transfer16(B10010011) >> 3; // read Z2, and trigger Y reading
    ret.z = z1 + 4095 - z2;

    if(ret.z >= threshold){ //If the touch screen is pressed, read the X,Y  coordinates from XPT2046.
        ret.x = gatherSamples(B10010011);
        ret.y = gatherSamples(B11010011);
    } else {
        ret.z = 0;
    }
    my_SPI.transfer(B00000000); // enter power saving (and IRQ enable)

    digitalWrite(cs_pin, HIGH);
    my_SPI.endTransaction();
    return ret;
}

boolean XPT2046_touch::read_XY(uint16_t *xy) {
    TS_Point p = getPoint();
    xy[0] = p.x;
    xy[1] = p.y;
    return p.z > 0;
}

////////////////// Buttons ////////////////
void XPT2046_touch::setButtonsNumber(byte columnButtons, byte rowButtons ){
    _rowButtons = rowButtons;
    _columnButtons = columnButtons;
}

uint8_t  XPT2046_touch::getButtonNumber(){
    uint16_t xy[2];
    uint8_t tmp, buttonNum;
    int div;
    
    if(read_XY(xy)){
        div = (X_MAX + X_MIN) / (_columnButtons + 1);
        buttonNum = ((xy[1] / div));

        div  = (Y_MAX + Y_MIN) / _rowButtons;
        tmp = ((xy[0] / div));

        return ((buttonNum * _rowButtons)  + tmp + 1); //Return the button number.
    }

    return 0; //Touch screen is not pressed.
}
