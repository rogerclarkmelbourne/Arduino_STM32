/*
  ASCII table

  Connect to the Maple Serial using the Serial Monitor, then press
  any key and hit enter.

  Prints out byte values in all possible formats:
  * as raw binary values
  * as ASCII-encoded decimal, hex, octal, and binary values

  For more on ASCII, see:
  http://www.asciitable.com
  http://en.wikipedia.org/wiki/ASCII

  No external hardware needed.

  created 2006
  by Nicholas Zambetti
  modified 18 Jan 2009
  by Tom Igoe

  <http://www.zambetti.com>

  Ported to the Maple 27 May 2010
  by Bryan Newbold
*/

void setup() 
{

	Serial.begin(115200); // Ignored by Maple. But needed by boards using hardware serial via a USB to Serial adaptor
    // Wait for the user to press a key
    while (!Serial.available())
        continue;

    // Prints title with ending line break
    Serial.println("ASCII Table ~ Character Map");
}

// First visible ASCII character: '!' is number 33:
int thisByte = 33;
// You can also write ASCII characters in single quotes.
// for example. '!' is the same as 33, so you could also use this:
//int thisByte = '!';

void loop() {
    // Prints value unaltered, i.e. the raw binary version of the
    // byte. The serial monitor interprets all bytes as
    // ASCII, so 33, the first number,  will show up as '!'
    Serial.write(thisByte);

    Serial.print(", dec: ");
    // Prints value as string as an ASCII-encoded decimal (base 10).
    // Decimal is the default format for Serial.print() and
    // Serial.println(), so no modifier is needed:
    Serial.print(thisByte);
    // But you can declare the modifier for decimal if you want to.
    // This also works if you uncomment it:
    // Serial.print(thisByte, DEC);

    Serial.print(", hex: ");
    // Prints value as string in hexadecimal (base 16):
    Serial.print(thisByte, HEX);

    Serial.print(", oct: ");
    // Prints value as string in octal (base 8);
    Serial.print(thisByte, OCT);

    Serial.print(", bin: ");
    // Prints value as string in binary (base 2); also prints ending
    // line break:
    Serial.println(thisByte, BIN);

    // If printed last visible character '~' or 126, stop:
    if (thisByte == 126) {     // You could also use if (thisByte == '~') {
        // This loops forever and does nothing
        while (true) {
            continue;
        }
    }
    // Go on to the next character
    thisByte++;
}
