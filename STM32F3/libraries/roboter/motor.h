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
 *  Das Modul Motor stellt Funktionen und Makros zur Steuerung der
 *  Fahrmotoren zur verfügung sowie Funktionen zur Steuerung und
 *  Abfrage der Wegsensoren.
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define OUT_A   1
#define OUT_B   2
#define OUT_C   4
#define OUT_AB  (OUT_A|OUT_B)
#define OUT_BC  (OUT_B|OUT_C)
#define OUT_AC  (OUT_A|OUT_C)
#define OUT_ABC (OUT_A|OUT_B|OUT_C)


extern void InitMotor(int id, int PinDir, int PinPWM, int PinSensor, int PinLED);

extern void OnFwd(int Motor, int Geschwindigkeit);
extern void OnRev(int Motor, int Geschwindigkeit);
extern void Off (int Motor);

#if 0
extern void DreheRechts(int v, int Winkel);
extern void DreheLinks(int v, int Winkel);

extern void FahreVor(int v, int Strecke);
#endif


#endif /* MOTOR_H_ */
