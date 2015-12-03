/**
 * XPT2046 touch screen buttons.
 *
 * It divides the touch screen into COLUMNS * LINES areas (4*2=8 buttons) and creates virtual buttons.   
 * if the touch screen area is pressed and prints on Serial Port 1 the X,Y coordinates.
 *
 * Copyright (c) 02 Dec 2015 by Vassilis Serasidis
 * Home: http://www.serasidis.gr
 * email: avrsite@yahoo.gr
 *
 * The sketch example has been created for using it with STM32Duino (http://www.stm32duino.com)
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "XPT2046_touch.h"
#include <SPI.h>;

#define CS_PIN  PA3  // Chip Select pin
#define LINES     2
#define COLUMNS   4

SPIClass mySPI(1); //Create an SPI instance on SPI1 port.
XPT2046_touch ts(CS_PIN, mySPI); // Chip Select pin, SPI port

uint16_t xy[2];

void setup() {
  Serial1.begin(9600);
  Serial1.println("-------------------------------------------------");
  Serial1.println("XPT2046 touch screen buttons");
  Serial1.println("Copyright (c) 02 Dec 2015 by Vassilis Serasidis");
  Serial1.println("Home: http://www.serasidis.gr");
  Serial1.println("-------------------------------------------------");
  ts.begin(); //Begin TouchScreen.
  ts.setButtonsNumber(COLUMNS, LINES); //Divide the Touch screen area into 4 columns and 2 lines and make them act as buttons. 
}

void loop() {
  if(ts.read_XY(xy)){ //If the touch screen is preesed, read the X,Y coordinates and print them on Serial port.
    uint8_t buttonNumber = ts.getButtonNumber();
    if(buttonNumber > 0){
      Serial1.print("Button: ");
      Serial1.println(buttonNumber);
      
      Serial1.print("X: ");
      Serial1.println(xy[0]); //Print X value
      Serial1.print("Y: ");
      Serial1.println(xy[1]); //Print Y value
      Serial1.println();
    }
    delay(500);
  }
}