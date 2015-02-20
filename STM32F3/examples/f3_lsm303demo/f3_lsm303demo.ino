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
#include <lsm303.h>

#include <libmaple/i2c.h>
#include <math.h>

 
/**
 * Dieses Beispiel wertet die x und y Werte des Beschleunigungssensors aus
 * und zeigt über die 8 LEDs an, in welche Richtung das Board gekippt wurde.
 * Die z-Komponente wird nicht ausgewertet.
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

   // I2C-Interface initialisieren, mit dem der
   // Beschleunigungssensor verbunden ist
   i2c_master_enable(I2C1, 0);

   // Beschleunigungssensor initialisieren
   lsm303Init();

}

// Speicher für die aktuell aktive LED
int led_idx;

void loop()
{
	float winkel;

	// Beschleunigungssensor lesen
	readAccValues();

	// Winkel entsprechend der gemessenen x und y Komponente
	// berechnen
	// atan2 gibt den Winkel im Bogenmass zurueck, 2*Pi entsprechen 360°
	winkel = atan2(accy*-1.0,accx*1.0);

    // Umrechnung des Winkels auf die 8 LEDs, aus denen der Vollkreis (2*Pi)
	// besteht (Dreisatz, Umrechnung mit 4 und Pi)
	winkel = (winkel*4)/3.14;

	// Falls der Winkel negativ ist, eine Vollkreis-Wert hinzurechnen
	// für unser Beispiel 8
	if (winkel<0)
		winkel += 8;

	// alte aktive LED ausschalten
	digitalWrite(leds[led_idx], LOW);

	// der Ganzzahl-Anteil des Winkels ist der Index der neuen LED
	// Das (int) vor winkel führt die Umwandlung der Kommanzahl in einen Integer-Wert
	// durch. Das nennt man einen Type-Cast. Der ist hier nötig, da eine
	// float Variable nicht direkt einer Integervariable zugewiesen werden kann
	led_idx = (int)winkel;

	// nochmal prüfen, ob der neue Led-Index nicht zu groß ist
	// wenn ja auf maximalwert setzen
	if (led_idx>7)
		led_idx=7;

	// neue LED anschalten
	digitalWrite(leds[led_idx], HIGH);
	
    delay(50);	
}



