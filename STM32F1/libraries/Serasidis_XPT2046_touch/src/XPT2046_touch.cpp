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

void XPT2046_Touchscreen::update()
{
	int16_t data[6];

	if (!isrWake) return;
	uint32_t now = millis();
	if (now - msraw < MSEC_THRESHOLD) return;
	
	SPI.beginTransaction(SPI_SETTING);
	digitalWrite(csPin, LOW);
	SPI.transfer(0xB1 /* Z1 */);      //ask for Z1 value
	int16_t z1 = SPI.transfer16(0xC1 /* Z2 */) >> 3;      //store Z1 value, ask for Z2
	int z = z1 + 4095;
	int16_t z2 = SPI.transfer16(0x91 /* X */) >> 3;       //store Z2 value, ask for X
	z -= z2;
	if (z >= Z_THRESHOLD) {
		data[0] = SPI.transfer16(0xD1 /* Y */) >> 3;
		data[1] = SPI.transfer16(0x91 /* X */) >> 3; // make 3 x-y measurements
		data[2] = SPI.transfer16(0xD1 /* Y */) >> 3;
		data[3] = SPI.transfer16(0x91 /* X */) >> 3;
	}
	else data[0] = data[1] = data[2] = data[3] = 0;	// Compiler warns these values may be used unset on early exit.
	data[4] = SPI.transfer16(0xD0 /* Y */) >> 3;	// Last Y touch power down
	data[5] = SPI.transfer16(0) >> 3;
	digitalWrite(csPin, HIGH);
	SPI.endTransaction();
	if (z < 0) z = 0;
	if (z < Z_THRESHOLD) { //	if ( !touched ) {
		zraw = 0;
		if (z < Z_THRESHOLD_INT) { //	if ( !touched ) {
			if (255 != tirqPin) isrWake = false;
		}
		return;
	}
	zraw = z;
	
	// Average pair with least distance between each measured x then y
	int16_t x = besttwoavg( data[0], data[2], data[4] );
	int16_t y = besttwoavg( data[1], data[3], data[5] );
	
	if (z >= Z_THRESHOLD) {
		msraw = now;	// good read completed, set wait
          if (alpha_x) {           //if calibration parameters have been defined,
                                   //calculate pixel coordinates
               xraw = alpha_x * x + beta_x * y + delta_x;
               yraw = alpha_y * x + beta_y * y + delta_y;
               if (displ_rot) {    //and if the screen has been rotated
                    uint16_t rot_x;
                    uint16_t rot_y;
                                   //calculate pixel positions for rotated screen
                    if (displ_rot == 1) {
                         rot_x = yraw;
                         rot_y = displ_height - xraw;
                    }
                    else if (displ_rot == 2) {
                         rot_x = displ_width - xraw;
                         rot_y = displ_height - yraw;
                    }
                    else if (displ_rot == 3) {
                         rot_x = displ_width - yraw;
                         rot_y = xraw;
                    }
                    xraw = rot_x;
                    yraw = rot_y;
               }
          }
          else {
     		xraw = x;
     		yraw = y;
          }
	}
}
void XPT2046_touch::calibrate(float ax, float bx, int16_t dx, float ay, float by, int16_t dy, uint16_t dwidth, uint16_t dheight, uint8_t drot) { 
     alpha_x = ax;
     beta_x = bx;
     delta_x = dx;
     alpha_y = ay;
     beta_y = by;
     delta_y = dy;
     displ_width = dwidth;
     displ_height = dheight;
     displ_rot = drot;
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
