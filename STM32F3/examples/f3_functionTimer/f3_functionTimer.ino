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
#include <functiontimer.h>

/*
 * Demo program  function timer
 *
 * You can define functions register those functions at the timer system
 * to be called after the time in ms given at timer start 
 *
 * - there are 10 timers with id from 0 to 9
 * - the id has to be given at timer start
 * - there can only be running one instance of a timer id at one time
 * - at each timer start another function can be registered
 * - time resolution is 5ms
 *
 * The registered function will be executed in interrupt context.
 * Therefore the timer funtion shall be execute very fast.
 *
 * If using functtion timer and ultrasonic sensors in parallel the measurement results of
 * the sensors can be slightly wrong.
 *
 * This demo controls three LEDs with different blinking rythms
 */


/*
 * Demoprogramm fuer Funktions-Timer
 *
 * Es koennen Funktionen definiert und dem Timersystem-uebergeben werden,
 * die nach einer uebergebenen Zeit aufgerufen werden.
 *
 * - Jeder Timer hat eine id zwischen 0 und 9, die beim Start angegeben werden muss.
 * - Es kann nur ein Timer pro Id zu gleichen Zeit laufen
 * - Es kann bei jedem Start eines Timers eine andere Funktion uebergeben werden.
 *
 * Die uebergebene Funktion wird im Interrupt-Kontext ausgefuehrt und unterbricht das
 * normal in setup/loop ausgefuehrte Programm an beliebiger Stelle.
 * Es ist darauf zu achten, dass die Timerfuntion moeglichst kurz, d.h. schnell ausfuehrbar, ist.
 *
 * Die parallele Ausfuehrung von Funktionstimern kann die Messergebnisse von Ultraschallsensoren
 * leicht verfaelschen!
 *
 * Das Demoprogramm steuert drei LEDs mit unterschiedlichen Blink-Zeiten an.
 * 
 */



// 500ms Bink mit Steuerung ueber Timer-Id 0
#define LED PE9
int LEDState = 0;
void TimerFunction(int arg)
{
	if (LEDState)
		LEDState = 0;
	else
		LEDState = 1;
	digitalWrite(LED, LEDState);
	timerStart(0, 500, TimerFunction, 0 ); // restart Timer
}


// 900ms Bink mit Steuerung ueber Timer-Id 1
#define LED1 PE10
int LED1State = 0;
void Timer1Function(int arg)
{
	if (LED1State)
		LED1State = 0;
	else
		LED1State = 1;
	digitalWrite(LED1, LED1State);
	timerStart(1, 900, Timer1Function, 0 ); // restart Timer
}


// 1300ms Bink mit Steuerung ueber Timer-Id 2
#define LED2 PE11
int LED2State = 0;
void Timer2Function(int arg)
{
	if (LED2State)
		LED2State = 0;
	else
		LED2State = 1;
	digitalWrite(LED2, LED2State);
	timerStart(2, 1300, Timer2Function, 0 ); // restart Timer
}




void setup()       
{
	pinMode(LED, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);

    // Timer-System initialisieren
	timerInit();

	// Timer starten
	timerStart(0, 500, TimerFunction, 0 );
	timerStart(1, 900, Timer1Function, 0 );
	timerStart(2, 1300, Timer2Function, 0 );
}

void loop()
{
	// Die Programmabarbeitung erfolgt komplett im Interrupt,
	// daher muss in loop nichts getan werden
	while (1)
		;
}



