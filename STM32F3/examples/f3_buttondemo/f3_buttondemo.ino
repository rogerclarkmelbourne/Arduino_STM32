/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2015 Frank-Michael Krause
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/


/*
 *  This demo shows how to handle the blue button of the stm32f3discovery board
 *
 *  Dieses Beispiel zeigt, wie die blaue Taste des Boards abgefragt werden kann.
 *  Es wird eine LED ein- bzw. ausgeschaltet.
 */


void setup()            // Init-Funktion 
{
	// BOARD_LED_PIN ist PE10
	pinMode(BOARD_LED_PIN, OUTPUT);

	// BOARD_BUTTTON_PIN ist PA0
	pinMode(BOARD_BUTTON_PIN, INPUT);

}


void loop()
{
	if (isButtonPressed())
		togglePin(BOARD_LED_PIN);
}



