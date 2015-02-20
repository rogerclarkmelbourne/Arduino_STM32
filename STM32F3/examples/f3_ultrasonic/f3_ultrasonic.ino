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
#include <debug.h>
#include <ultraschall.h>

/*
 * Demoprogramm für den Betrieb von Ultraschallsensoren über Interrupts.
 *
 * Diese Betriebsart hat den Vorteil, dass während der Messung (die einige 10ms dauern kann)
 * andere Dinge erledigt werden können.
 *
 * Es kann ständig im Hintergrund gemessen werden und der letzte gültige oder der aktuelle
 * Messwert per Funktion abgefragt werden.
 *
 * Außerdem kann eine Callbackfunktion pro Sensor registriert werden, die nach einer erfolgten
 * Messung aufgerufen wird. ZU beachten ist, dass diese Funktion im Interrupt-Kontext aufgerufen wird
 * und daher möglichst kurz (d.h. schnell abarbeitbar) sein.
 * Bei Debug-Ausgaben in der Handler-Funktion können sich die Ausgaben in Ausgaben aus loop() heraus "hineindrängeln"
 *
 */


//----------------- Ultraschall -----------------------
//                   3x HC-SR04

#define US1_ECHO      PD4
#define US1_TRIGGER   PB4

#define US2_ECHO      PD5
#define US2_TRIGGER   PB5

#define US3_ECHO      PD6
#define US3_TRIGGER   PB8



void UsHandler(int id, int entfernung)
{
	Print2Number("Sensor", id, entfernung);
}


void setup() 
{
	delay(150);
	debugInit();
	debugSetTimeStamp(DDEBUG_TIME_STAMP_ON);

	// Ultraschallsensoren definieren
	addUSSensor(0, US1_TRIGGER, US1_ECHO, 220);
	addUSSensor(1, US2_TRIGGER, US2_ECHO, 220);
	addUSSensor(2, US3_TRIGGER, US3_ECHO);

	UsAttachHandler(0, UsHandler);
	UsSetMessPause(100);  // Pause von 100 ms nach jeder Messung
	                      // damit die Ausgabe im Terminal nicht zu schnell wird
	                      // Im Normalfall würde keine so lange Pause programmiert werden.
	                      // Die Pause ist nur nötig, wenn festgestellt wird, dass sich
                          // die Sensoren gegenseitig durch nachfolgende mehrfache Echos beeinflussen.
	                      // Das ist Umgebungsabhängig.

	debugPrint("activate Sensor 0 ---------------");
	aktiviereUSSensor(0);
	PrintUsSensorList();
	debugPrint("activate Sensor 1 ---------------");
	aktiviereUSSensor(1);
	PrintUsSensorList();

	debugPrint("activate Sensor 2 ---------------");
	aktiviereUSSensor(2);
	PrintUsSensorList();

	// ab jetzt wird im Hintergrund gemessen

	delay(1000);
	debugPrint("deactivate Sensor 2 ---------------");
	deaktiviereUSSensor(2);
	PrintUsSensorList();

	delay(1000);
	debugPrint("deactivate Sensor 1 ---------------");
	deaktiviereUSSensor(1);
	PrintUsSensorList();

	delay(1000);
	debugPrint("deactivate Sensor 0 ---------------");
	deaktiviereUSSensor(0);
	PrintUsSensorList();

	delay(1000);
	debugPrint("activate Sensor 0 ---------------");
	aktiviereUSSensor(0);
	PrintUsSensorList();

	delay(1000);
    debugPrint("activate Sensor 1 ---------------");
	aktiviereUSSensor(1);
	PrintUsSensorList();


	debugPrint("Programm gestartet");
}

void loop()
{
	Print2Number("Sensor 1", UsLetzterGueltigerMesswert(1), UsLetzterGueltigerMesswertAlter(1));
    delay(300);
}




