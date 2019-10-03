#include <stdlib.h>
#include <stdio.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GFX.h>
#include "XPT2046_touch.h"
#include <SPI.h>
#include <Adafruit_TFTLCD_16bit_STM32.h> // Hardware-specific library
Adafruit_TFTLCD_16bit_STM32 tft;

//definitions, initialize parameters

#define CS_PIN PA3 // Chip Select pin

SPIClass mySPI(1); //Create an SPI instance on SPI1 port.
XPT2046_touch ts(CS_PIN, mySPI); // Chip Select pin, SPI port


char s[30];                             //character arrays for text strings
char s2[30];

uint16_t calA[] = {15, 10};             //calibration points must be independent, i.e. not line up
uint16_t calB[] = {80, 280};
uint16_t calC[] = {200, 170};

uint16_t calA_raw[] = {0, 0};           //store raw touch data for calibration points
uint16_t calB_raw[] = {0, 0};
uint16_t calC_raw[] = {0, 0};

uint8_t calCount = 0;                   //track which point has been activated


uint16_t xy[2];

//SETUP

void setup() {
  pinMode(1, OUTPUT);
  

  tft.begin(0x9341);                          //set up our screen
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  tft.setTextSize(1);                   //these should be self-explanatory
  tft.setCursor(20,43);
  tft.print("touch stylus to target centre");

  tft.setTextSize(2);
  tft.setCursor(20,25);
  tft.print("Calibrate Touch:");
  tft.setCursor(35,63);
  tft.print("X:");
  tft.setCursor(35,82);
  tft.print("Y:");
  tft.setCursor(35,101);
  tft.print("Z:");

  calibrationPoint(calA[0], calA[1]);   //display first calibration point

  ts.begin();                           //initialize touch screen
  ts.calibrate(-.00114, -.0653, 254, -.0885, -.00125, 348, 240, 320);
}


//MAIN LOOP

void loop() {
  if (ts.read_XY(xy)) {                   //check for screen touch
    tft.fillRect(59, 63, 48, 54, BLACK);
    char buf[30];
                                        //black out X, Y, Z values
    TS_Point p = ts.getPoint();         //get touch data
    sprintf(s, "%d", p.x);              //convert X data to char array
    tft.setCursor(59,63);
    tft.print(s);                       //and display
    sprintf(s, "%d", p.y);              //same for Y value
    //itoa(p.y);                   
    tft.setCursor(59,82);
    tft.print(s);
    sprintf(s, "%d", p.z);              //and Z (pressure)
    tft.setCursor(59,101);
    tft.print(s);

    if (calCount == 0) {                //first calibration point?
      calA_raw[0] = p.x;                //store values
      calA_raw[1] = p.y; 

      tft.fillRect(calA[0]-6, calA[1]-6, 13, 13, BLACK);
                                        //black out calibration point
      delay(500);                       //give user time to take stylus off screen so as not to
                                        //read same values into next calibration point
      calibrationPoint(calB[0], calB[1]);
                                        //and display the next calibration point
    }
    else if (calCount == 1) {           //do the same for the second calibration point
      calB_raw[0] = p.x;
      calB_raw[1] = p.y;

      tft.fillRect(calB[0]-6, calB[1]-6, 13, 13, BLACK);
      delay(500);
      calibrationPoint(calC[0], calC[1]);
    }
    else if (calCount == 2) {           //and third
      calC_raw[0] = p.x;
      calC_raw[1] = p.y;

      tft.fillRect(calC[0]-6, calC[1]-6, 13, 13, BLACK);

      tft.setTextSize(2);               //get ready to display calibration parameters
      tft.setCursor(20,130);
      tft.print("Parameters:");
      tft.setCursor(35,155);
      tft.print("X:");
      tft.setCursor(35,209);
      tft.print("Y:");

      //calculate calibration parameters

      int32_t delta = (calA_raw[0]-calC_raw[0])*(calB_raw[1]-calC_raw[1]) -
            (calB_raw[0]-calC_raw[0])*(calA_raw[1]-calC_raw[1]); 
      float alpha_x = (float)((calA[0]-calC[0])*(calB_raw[1]-calC_raw[1]) -
            (calB[0]-calC[0])*(calA_raw[1]-calC_raw[1])) / delta; 
      float beta_x = (float)((calA_raw[0]-calC_raw[0])*(calB[0]-calC[0]) -
            (calB_raw[0]-calC_raw[0])*(calA[0]-calC[0])) / delta;
      uint16_t delta_x = ((uint64_t)calA[0]*(calB_raw[0]*calC_raw[1]-calC_raw[0]*calB_raw[1]) -
            (uint64_t)calB[0]*(calA_raw[0]*calC_raw[1]-calC_raw[0]*calA_raw[1]) + 
            (uint64_t)calC[0]*(calA_raw[0]*calB_raw[1]-calB_raw[0]*calA_raw[1])) / delta;
      float alpha_y = (float)((calA[1]-calC[1])*(calB_raw[1]-calC_raw[1]) -
            (calB[1]-calC[1])*(calA_raw[1]-calC_raw[1])) / delta; 
      float beta_y = (float)((calA_raw[0]-calC_raw[0])*(calB[1]-calC[1]) -
            (calB_raw[0]-calC_raw[0])*(calA[1]-calC[1])) / delta;
      uint16_t delta_y = ((uint64_t)calA[1]*(calB_raw[0]*calC_raw[1]-calC_raw[0]*calB_raw[1]) -
            (uint64_t)calB[1]*(calA_raw[0]*calC_raw[1]-calC_raw[0]*calA_raw[1]) +
            (uint64_t)calC[1]*(calA_raw[0]*calB_raw[1]-calB_raw[0]*calA_raw[1])) / delta;

      ftoa(alpha_x, s, 3);              //convert first parameter, a float value, to char array
      tft.setCursor(59,155);
      tft.print(s);                     //and display
      ftoa(beta_x, s, 3);
      tft.setCursor(59,173);
      tft.print(s);  
      sprintf(s, "%d", delta_x);        //etc.
      tft.setCursor(59,191);
      tft.print(s);  
      ftoa(alpha_y, s, 3);
      tft.setCursor(59,209);
      tft.print(s);  
      ftoa(beta_y, s, 3);
      tft.setCursor(59,227);
      tft.print(s);  
      sprintf(s, "%d", delta_y);
      tft.setCursor(59,245);
      tft.print(s);  

      tft.setCursor(20,275);
      tft.print("[ Restart ]");
    }

    calCount++;
  }

  if (calCount == 4) {                  //finally, if another touch is detected
    tft.fillRect(0, 130, 240, 190, BLACK);
                                        //clear parameters from screen
    calibrationPoint(calA[0], calA[1]); //and start over
    calCount = 0;
  }

  delay(200);
}


//calibrationPoint() displays a calibration target on screen, using the Adafruit graphics functions

void calibrationPoint(uint16_t calX, uint16_t calY) {
  tft.drawCircle(calX, calY, 6, WHITE);
  tft.drawFastHLine(calX-4, calY, 9, WHITE);
  tft.drawFastVLine(calX, calY-4, 9, WHITE);
}



void ftoa(float f, char *str, uint8_t precision) {
  uint8_t i, j, divisor = 1;
  int8_t log_f;
  int32_t int_digits = (int)f;             //store the integer digits
  float decimals;
  char s1[12];

  memset(str, 0, sizeof(s));  
  memset(s1, 0, 10);

  if (f < 0) {                             //if a negative number 
    str[0] = '-';                          //start the char array with '-'
    f = abs(f);                            //store its positive absolute value
  }
  log_f = ceil(log10(f));                  //get number of digits before the decimal
  if (log_f > 0) {                         //log value > 0 indicates a number > 1
    if (log_f == precision) {              //if number of digits = significant figures
      f += 0.5;                            //add 0.5 to round up decimals >= 0.5
      sprintf(s1, "%d", f);                     //itoa converts the number to a char array
      strcat(str, s1);                     //add to the number string
    }
    else if ((log_f - precision) > 0) {    //if more integer digits than significant digits
      i = log_f - precision;               //count digits to discard
      divisor = 10;
      for (j = 0; j < i; j++) divisor *= 10;    //divisor isolates our desired integer digits 
      f /= divisor;                             //divide
      f += 0.5;                            //round when converting to int
      int_digits = (int)f;
      int_digits *= divisor;               //and multiply back to the adjusted value
      sprintf(s1, "%d", int_digits);
      strcat(str, s1);
    }
    else {                                 //if more precision specified than integer digits,
      sprintf(s1, "%d", int_digits);       //convert
      strcat(str, s1);                     //and append
    }
  }

  else {                                   //decimal fractions between 0 and 1: leading 0
    s1[0] = '0';
    strcat(str, s1);
  }

  if (log_f < precision) {                 //if precision exceeds number of integer digits,
    decimals = f - (int)f;                 //get decimal value as float
    strcat(str, ".");                      //append decimal point to char array

    i = precision - log_f;                 //number of decimals to read
    for (j = 0; j < i; j++) {              //for each,
      decimals *= 10;                      //multiply decimals by 10
      if (j == (i-1)) decimals += 0.5;     //and if it's the last, add 0.5 to round it
      sprintf(s1, "%d", (int)decimals);    //convert as integer to character array
      strcat(str, s1);                     //append to string
      decimals -= (int)decimals;           //and remove, moving to the next
    }
  }
}