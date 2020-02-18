/*
Streaming.h - Arduino library for supporting the << streaming operator
Copyright (c) 2010-2012 Mikal Hart.  All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* 12-05-2015 GgR
    added _FILL and _TIME classes from web page 
*/

#ifndef ARDUINO_STREAMING
#define ARDUINO_STREAMING

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define STREAMING_LIBRARY_VERSION 5.1

// Generic template
template<class T> 
inline Print &operator <<(Print &stream, T arg) 
{ stream.print(arg); return stream; }

struct _BASED 
{ 
  long val; 
  int base;
  _BASED(long v, int b): val(v), base(b) 
  {}
};

#if ARDUINO >= 100

struct _BYTE_CODE
{
	byte val;
	_BYTE_CODE(byte v) : val(v)
	{}
};
#define _BYTE(a)    _BYTE_CODE(a)

inline Print &operator <<(Print &obj, const _BYTE_CODE &arg)
{ obj.write(arg.val); return obj; } 

#else

#define _BYTE(a)    _BASED(a, BYTE)

#endif

#define _HEX(a)     _BASED(a, HEX)
#define _DEC(a)     _BASED(a, DEC)
#define _OCT(a)     _BASED(a, OCT)
#define _BIN(a)     _BASED(a, BIN)

// Specialization for class _BASED
// Thanks to Arduino forum user Ben Combee who suggested this 
// clever technique to allow for expressions like
//   Serial << _HEX(a);

inline Print &operator <<(Print &obj, const _BASED &arg)
{ obj.print(arg.val, arg.base); return obj; } 

#if ARDUINO >= 18
// Specialization for class _FLOAT
// Thanks to Michael Margolis for suggesting a way
// to accommodate Arduino 0018's floating point precision
// feature like this:
//   Serial << _FLOAT(gps_latitude, 6); // 6 digits of precision

struct _FLOAT
{
  float val;
  int digits;
  _FLOAT(double v, int d): val(v), digits(d)
  {}
};

inline Print &operator <<(Print &obj, const _FLOAT &arg)
{ obj.print(arg.val, arg.digits); return obj; }
#endif

// Specialization for enum _EndLineCode
// Thanks to Arduino forum user Paul V. who suggested this
// clever technique to allow for expressions like
//   Serial << "Hello!" << endl;

enum _EndLineCode { endl };

inline Print &operator <<(Print &obj, _EndLineCode arg) 
{ (void)arg; obj.println(); return obj; }


// _Fill struct from web page
//  from user Rob Tillaart
//  Serial << _FILL('=', 20) << endl;

struct _FILL
{
char ch;
int len;
_FILL(char c, int l): ch(c), len(l)
{}
};

inline Print &operator <<(Print &obj, const _FILL &arg)
{ for (int i=0; i< arg.len; i++) obj.write(arg.ch); return obj; }

// _TIME struct from web page
//  from user Rob Tillaart
//  Serial << _TIME(1,2,3) << endl;
struct _TIME
{
uint8_t hour;
uint8_t minu;
uint8_t sec;
_TIME(uint8_t h, uint8_t m, uint8_t s): hour(h), minu(m), sec(s)
{}
};

inline Print &operator <<(Print &obj, const _TIME &arg)
{ obj.print(((arg.hour<10)?"0":"")); obj.print(int(arg.hour));
obj.print(((arg.minu<10)?":0":":")); obj.print(int(arg.minu));
obj.print(((arg.sec<10)?":0":":")); obj.print(int(arg.sec));
return obj; } 
#endif
