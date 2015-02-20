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
 *  Debug stellt Funktionen und Makros zur Ausgabe von Debug-Informationen
 *  über UART/Bluetooth zur Verfügung
 */

#ifndef DEBUG_H_
#define DEBUG_H_


#define DDEBUG_TIME_STAMP_OFF 0
#define DDEBUG_TIME_STAMP_ON  1


extern void debugInit();

extern void debugSetTimeStamp(int OnOff);

extern void debugPrint(const char *s);

extern void PrintNumber(const char *name, int num);
extern void PrintNumber(const char *name, byte num);
extern void PrintNumber(const char *name, float num);

extern void Print2Number(const char *name, int num1, int num2);
extern void Print2Number(const char *name, byte num1, byte num2);
extern void Print2Number(const char *name, float num1, float num2);

extern void PrintArray(const char *name, int len, int *arr);
extern void PrintArray(const char *name, int len, byte *arr);
extern void PrintArray(const char *name, int len, float *arr);


#endif /* DEBUG_H_ */
