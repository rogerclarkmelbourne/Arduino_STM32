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
#include <wirish/wirish.h>
#include "debug.h"

#define DEBUG

#ifndef DEBUGPORTID
#define DEBUGPORTID 2
// TX-PIN ist PA2
#endif

#ifndef DEBUGPORT
#if DEBUGPORTID == 0
   #define DEBUGPORT SerialUSB
#endif   
#if DEBUGPORTID == 1
   #define DEBUGPORT Serial1
#endif   
#if DEBUGPORTID == 2 
   #define DEBUGPORT Serial2
#endif   
#if DEBUGPORTID == 3
   #define DEBUGPORT Serial3
#endif   
#endif


void debugInit()
{
#ifdef DEBUG
#if (DEBUGPORTID != 0)
   DEBUGPORT.begin(57600);
#endif
   DEBUGPORT.println("Debug Init");
#endif
}


static int printTimeStamp = 0;

void debugSetTimeStamp(int OnOff) {
	printTimeStamp = OnOff;
}

static  inline void addTimeStamp() {
	if (printTimeStamp) {
		DEBUGPORT.print(millis());
		DEBUGPORT.print(" ");
	}
}

void debugPrint(const char *s)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.println(s);
#endif
}

void PrintNumber(const char *name, int num)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.println(num);
#endif
}

void PrintNumber(const char *name, byte num)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.println(num);
#endif
}

void PrintNumber(const char *name, float num)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.println(num);
#endif
}



void Print2Number(const char *name, int num1, int num2)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.print(num1);
	DEBUGPORT.print("\t");
	DEBUGPORT.println(num2);
#endif
}

void Print2Number(const char *name, byte num1, byte num2)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.print(num1);
	DEBUGPORT.print("\t");
	DEBUGPORT.println(num2);
#endif
}

void Print2Number(const char *name, float num1, float num2)
{
#ifdef DEBUG
	addTimeStamp();
	DEBUGPORT.print(name);
	DEBUGPORT.print(" ");
	DEBUGPORT.print(num1);
	DEBUGPORT.print("\t");
	DEBUGPORT.println(num2);
#endif
}

void PrintArray(const char *name, int len, int *arr)
{
#ifdef DEBUG
	int i;
	addTimeStamp();
	DEBUGPORT.print(name);
	for (i=0; i<len; i++)
	{
		DEBUGPORT.print("\t");
		DEBUGPORT.print(arr[i]);
	}
	DEBUGPORT.println("");
#endif
}

void PrintArray(const char *name, int len, byte *arr)
{
#ifdef DEBUG
	int i;
	addTimeStamp();
	DEBUGPORT.print(name);
	for (i=0; i<len; i++)
	{
		DEBUGPORT.print("\t");
		DEBUGPORT.print(arr[i]);
	}
	DEBUGPORT.println("");
#endif
}

void PrintArray(const char *name, int len, float *arr)
{
#ifdef DEBUG
	int i;
	addTimeStamp();
	DEBUGPORT.print(name);
	for (i=0; i<len; i++)
	{
		DEBUGPORT.print("\t");
		DEBUGPORT.print(arr[i]);
	}
	DEBUGPORT.println("");
#endif
}

