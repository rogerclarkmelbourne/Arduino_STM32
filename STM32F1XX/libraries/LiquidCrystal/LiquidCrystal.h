#ifndef LiquidCrystal_h
#define LiquidCrystal_h

//#include <inttypes.h>
#include <wirish.h>
#include <Print.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal : public Print {
public:
  LiquidCrystal(uint8 rs, uint8 enable,
                uint8 d0, uint8 d1, uint8 d2, uint8 d3,
                uint8 d4, uint8 d5, uint8 d6, uint8 d7);
  LiquidCrystal(uint8 rs, uint8 rw, uint8 enable,
                uint8 d0, uint8 d1, uint8 d2, uint8 d3,
                uint8 d4, uint8 d5, uint8 d6, uint8 d7);
  LiquidCrystal(uint8 rs, uint8 rw, uint8 enable,
                uint8 d0, uint8 d1, uint8 d2, uint8 d3);
  LiquidCrystal(uint8 rs, uint8 enable,
                uint8 d0, uint8 d1, uint8 d2, uint8 d3);

  void init(uint8 fourbitmode, uint8 rs, uint8 rw, uint8 enable,
            uint8 d0, uint8 d1, uint8 d2, uint8 d3,
            uint8 d4, uint8 d5, uint8 d6, uint8 d7);

  void begin(uint8 cols, uint8 rows, uint8 charsize = LCD_5x8DOTS);

  void clear();
  void home();

  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();

  void createChar(uint8, uint8[]);
  void setCursor(uint8, uint8);
  virtual size_t write(uint8);
  void command(uint8);
private:
  void send(uint8, uint8);
  void write4bits(uint8);
  void write8bits(uint8);
  void pulseEnable();
    uint8 displaymode;
  uint8 _rs_pin; // LOW: command.  HIGH: character.
  uint8 _rw_pin; // LOW: write to LCD.  HIGH: read from LCD.
  uint8 _enable_pin; // activated by a HIGH pulse.
  uint8 _data_pins[8];

  uint8 _displayfunction;
  uint8 _displaycontrol;
  uint8 _displaymode;

  uint8 _initialized;

  uint8 _numlines,_currline;
};

#endif
