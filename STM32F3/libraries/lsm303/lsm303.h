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
#ifndef LSM303_H
#define LSM303_H


#define ACC_AXIS_X 1
#define ACC_AXIS_Y 2
#define ACC_AXIS_Z 3


/**
 * Initialisiert das spi1 interface des stm32f3discovery und konfiguriert den Gyro.
 * der Gyro ist nach dem Aufruf der Funktion aktiv. Wird er nicht mehr benötigt, kann er
 * mir gyroStop() wieder deaktiviert werden. Für eine Reaktivierung kann die Funktion
 * gyroStart() verwendet werden.
 * 
 * Im aktiven Zustand muss im Abstand von höchstens 120ms die Funktion gyroUpdate() 
 * aufgerufen werden.
 * 
 * Die globalen Variablen winkelx,  winkely und winkelz enthalten den jeweils aktuellen winkel.
 * Die Funktion gyroResetWinkel setzt die globalen Variablen wieder auf 0. 
 */
extern void lsm303Init();


/**
 */
extern void lsm303Calibrate();


/** 
 */
extern void accUpdate();

/**
 */
extern void accStart();

/**
 * Versetzt den Beschleunigungssensor in den Power Down mode
 */
extern void accStop();


//----------------------------------------------------------------------------------
//    gobale Variablen

extern  int accx, accy, accz;


extern void readAccValues();

#endif
