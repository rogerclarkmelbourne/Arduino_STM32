/*
Maple Mini StringEx by m. ray burnette: PUBLIC DOMAIN
Arduino 1.6.0rc1
  Sketch uses 18,272 bytes (16%) of program storage space. Maximum is 108,000 bytes.
  Global variables use 3,776 bytes of dynamic memory.
History:
http://forum.arduino.cc/index.php?topic=266669
"How do i get index 6+7 and 8+9 from a stream of 24 chars that come from the serial input into the Arduino?
The connected serial device is in idle mode until i send a command to it. When i send ZD,
it will send a timecode in the format ZD2b102c080c090e05ba0549, where ZD stands for the type of message it returns.
2c represents the minutes and 08 the hours of the the current time (in HEX format). The last 4 chars are a checksum.
There is no CR or NL at the end. I would like to get the minutes and hours as interger values."
*/

#include <Streaming.h>  // get used to this library - it makes serial output easy

char sInput[] = {"ZD2b102c080c090e05ba0549\0"} ;  // this data would come in over the serial connection
String sMM ;          // 0xHH\0 = 5 characters
String sHH ;          // Type String allows for concatenation and substrings
char Mbuf[5] ;        // 5 character user buffer for minutes, same type as serial input
char Hbuf[5] ;        // ditto - this one is for the 2 digit hours (HEX: "0x??\0")
int hours, minutes ;  // This is the final product that we are desiring

void setup(void)
{
  // initialize the digital pin as an output.
  pinMode(33, OUTPUT);
  Serial.begin(9600);
  // wait for serial monitor to be connected.
  while (!Serial)
  {
    digitalWrite(33,!digitalRead(33));// Turn the LED from off to on, or on to off
    delay(100);         // fast blink
  }
  Serial << "Beginning Example Run...\r\n" ;
  Serial << "Original String: " << sInput << "\r\n\r\n" ;
  // alternate http://arduino.cc/en/Reference/StringSubstring
  // Ref:      http://arduino.cc/en/Reference/StringToCharArray
  // Ref:      http://stackoverflow.com/questions/23576827/arduino-convert-a-sting-hex-ffffff-into-3-int
  // 0123456789                  <----- Character count in input array (type char)
  //"ZD2b102c080c090e05ba0549\0" <----- Input stream representation stored in char array sInput[]
  //       mmhh                  <----- HEX values we desire within the stream
  sMM += "0x" ; sMM += sInput[6]; sMM += sInput[7]; sMM += '\0' ;  // concatenate individual string elements
  sHH += "0x" ; sHH += sInput[8]; sHH += sInput[9]; sHH += '\0' ;  // ditto
  Serial << "Substrings: " << sHH << ":" << sMM << "\r\n\r\n" ;    // show it on term
  sMM.toCharArray( Mbuf, 5 ) ;            // <----- convert String to Char Array
  sHH.toCharArray( Hbuf, 5 ) ;            //        ditto
  hours  = (int)strtol(Hbuf, NULL, 16);   // <----- cast to (int) because strtol returns long
  minutes= (int)strtol(Mbuf, NULL, 16);   //        ditto
  Serial << "Hours: " << hours << "  Minutes: " << minutes << "\r \n";  // show it on term

  Serial << "\r\n *****End of run*****\r\n" ;
  Serial.end();
}

void loop( void ) {}

/*  ============================== CONSOLE OUTPUT ==============================
Beginning Example Run...
Original String: ZD2b102c080c090e05ba0549

Substrings: 0x08\0:0x2c\0

Hours: 8  Minutes: 44

 *****End of run*****
*/
