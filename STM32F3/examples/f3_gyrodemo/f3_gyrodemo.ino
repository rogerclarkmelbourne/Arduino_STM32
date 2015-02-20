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
#include <Arduino.h>

#include <ringbuffer.h>
#include <gyro.h>



/**
 * Dieses Beispiel benutzt die Z-Achse des Gyro-Sensors, um eine Drehung des Boards
 * zu messen und anzuzeigen.
 * Wenn das Board um seine Z-Achse gedreht wird, so zeigt die jeweils aktive LED
 * immer in die gleiche Richtung.
 * Mit der Zeit ändert sich die angezeigte Richtung, da die Messwerte kleine
 * Fehler haben, die sich mit der Zeit aufsummieren und das Programm keine
 * Kalibrierung und Fehlerkorrektur enthält. Die Größe des Fehlers ist von
 * Board zu Board unterschiedlich.
 *
 */
 
// Array mit den LED-Pins
int leds[] = { PE9, PE10, PE11, PE12, PE13, PE14, PE15, PE8 };


void setup()            // Init-Funktion 
{
   // Die 8 LED-Pins auf Ausgang setzen
   pinMode (leds[0], OUTPUT);
   pinMode (leds[1], OUTPUT);
   pinMode (leds[2], OUTPUT);
   pinMode (leds[3], OUTPUT);
   pinMode (leds[4], OUTPUT);
   pinMode (leds[5], OUTPUT);
   pinMode (leds[6], OUTPUT);
   pinMode (leds[7], OUTPUT);

   // SPI1-Interface initialisieren
   gyroSpiInit();

   // den Gyrochip initialisieren
   gyroInit();

   // Gyro aktivieren
   gyroStart();
}


int led_idx;

void loop()
{
	// Auslesen der Gyro-Messwerte und Berechnen des aktuellen Drehwinkels
	gyroUpdate();

	// Wenn der Winkel 360° überschreitet oder 0° unterschreitet
	// werden 360° abgezogen bzw. addiert
	if (winkelz>360)
	   winkelz -= 360;
	else if (winkelz<0)
	   winkelz += 360;

    // aktuell aktive LED abschalten
	digitalWrite(leds[led_idx], LOW);
	
	// anhand des Winkels die neue Aktive LED bestimmen.
	// Dazu wird mit Dreisatz der Winkel (0..360) auf die LED (0..7) abgebildet
	led_idx = (winkelz*8)/360;

	// aktive LED anschalten
	digitalWrite(leds[led_idx], HIGH);

	delay(50);
}



