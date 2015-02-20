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
#ifndef GYRO_H
#define GYRO_H


#define GYRO_AXIS_X 1
#define GYRO_AXIS_Y 2
#define GYRO_AXIS_Z 3

/**
 * Initialisiert das spi1 interface des stm32f3discovery
 */
extern void gyroSpiInit();

/**
 * konfiguriert den Gyro.
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
extern void gyroInit();


/**
 * Es werden 1000 Werte eingelesen (ca. 5,5s) und aus den Werten die Drift-Kompensations-
 * werte gemittelt. Der Gyro darf während der Zeit nicht bewegt werden. 
 */
extern void gyroCalibrate();

/**
 *  Funktionen zum manuellen Setzen der Gyro-Bias-Werte, z.B. aus gespeicherten EEPROM-Werten
 */
extern void gyroSetBiasX(int bias);
extern void gyroSetBiasY(int bias);
extern void gyroSetBiasZ(int bias);


/**
 * Die Winkel-Variablen werden auf 0 zurückgesetzt
 */
extern void gyroResetWinkel();

/** 
 * Diese Funktion muss innehalb von 120ms mindestens einmal aufgerufen werden. 
 * Es werden die vorhandenen Messwerte aus dem Fifo-Speicher des Chips ausgelesen und
 * die aktuellen Winkel berechnet
 */
extern void gyroUpdate();

/**
 * Versetzt den GyroChip in den aktiven Zustand, leert den Fifo und setzt die Winkel zurueck
 */
extern void gyroStart();

/**
 * Versetzt den gyro in den Power Down mode
 */
extern void gyroStop();


//----------------------------------------------------------------------------------
//    gobale Variablen

extern  float winkelx, winkely, winkelz;

extern int gyroxRaw, gyroyRaw, gyrozRaw; // bei jedem Update wird ein Datensatz der gelesenen
                                  // Werte in den Raw-Variablen gespeichert um den Bias pr�fen zu
                                  // k�nnen



//----------------------------------------------------------------------------------
// some low level functions

extern uint8 gyroReadRegister(uint8 reg);
extern void gyroWriteRegister(uint8 reg, uint8 val);

extern void readGyroValues();


extern void gyroSetActualAngleHistory(int axis, int len);

extern float gyroGetShortValueX();
extern float gyroGetShortValueY();
extern float gyroGetShortValueZ();


#endif
