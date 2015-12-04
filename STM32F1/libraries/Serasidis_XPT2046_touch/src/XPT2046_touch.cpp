/**
 *
 *
 */



#include "Arduino.h"
#include "XPT2046_touch.h"


/****************************************************************************/
  XPT2046_touch::XPT2046_touch(uint8_t _cs_pin, SPIClass _spiChan) : cs_pin(_cs_pin), my_SPI(_spiChan){
  }


/****************************************************************************/

  void XPT2046_touch::begin(){
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    my_SPI.begin();
  }

  /****************************************************************************/

  boolean XPT2046_touch::read_XY(uint16_t *xy){
    int z1, z2, tmpH, tmpL;
    digitalWrite(cs_pin, LOW);

    //Check if touch screen is pressed.
    SPI.transfer(B10110011); // Z1
    delay(10);
    tmpH = (my_SPI.transfer(0) << 5);
    tmpL = (my_SPI.transfer(0) >> 3);
    z1 = tmpH | tmpL;

    SPI.transfer(B11000011); // Z2
    delay(10);
    tmpH = (my_SPI.transfer(0) << 5);
    tmpL = (my_SPI.transfer(0) >> 3);
    z2 = tmpH | tmpL;
   
    if((z2 - z1) < Z_THRESHOLD){ //If the touch screen is pressed, read the X,Y  coordinates from XPT2046.
      my_SPI.transfer(B11010011); // X
      delay(10);
      tmpH = (my_SPI.transfer(0) << 5);
      tmpL = (my_SPI.transfer(0) >> 3);
      xy[0] =  tmpH | tmpL;
     
      my_SPI.transfer(B10010011); // Y
      delay(10);
      tmpH = (my_SPI.transfer(0) << 5);
      tmpL = (my_SPI.transfer(0) >> 3);
      xy[1] =  tmpH | tmpL;
      digitalWrite(cs_pin, HIGH);
      return true;
    }
    digitalWrite(cs_pin, HIGH);
    return false;
  }
  
  /****************************************************************************/
  void XPT2046_touch::setButtonsNumber(byte columnButtons, byte rowButtons ){
      _rowButtons = rowButtons;
      _columnButtons = columnButtons;     
  }
  
  /****************************************************************************/
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
/****************************************************************************/
