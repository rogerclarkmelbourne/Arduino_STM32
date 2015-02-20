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
#include "motor.h"
#include "../gyro/gyro.h"
#include "debug.h"


#define TB6612

#define MOTOR_ANZ 3


typedef struct {
   int dir;
   int pwm;
   int sensor;
   int led;
} MotorPins_t;

static MotorPins_t mpins[MOTOR_ANZ];

#define IDX_A 0
#define IDX_B 1
#define IDX_C 2

// Umwandlung MotorIds to Index, wenn nur ein Motor addressiert ist
#define MIDX(id)  ((id&OUT_A)?IDX_A:(id&OUT_B)?IDX_B:IDX_C) 


static volatile int RadCount0;
void CountRad0()
{
	RadCount0++;
}

static volatile int RadCount1;
void CountRad1()
{
	RadCount1++;
}

static volatile int RadCount2;
void CountRad2()
{
	RadCount2++;
}


void InitMotor(int id, int PinDir, int PinPWM, int PinSensor, int PinLED)
{
    MotorPins_t *p = mpins+MIDX(id);
	p->dir    = PinDir;
	p->pwm    = PinPWM;
	p->sensor = PinSensor;
	p->led    = PinLED;
	
    pinMode    (p->pwm, PWM);
    pinMode    (p->dir, OUTPUT);
    analogWrite(p->pwm, 0);
	
	if (p->sensor != -1)
	{
	   if (p->led != -1)
	       pinMode(p->led, OUTPUT);
		   
	   pinMode(p->sensor, INPUT);
	}
}


static inline void RadSensorAn(int motor)
{
   MotorPins_t *p = mpins;
    
   for (int idx=0; idx<MOTOR_ANZ; idx++)
   {
      if (motor & (1<<idx)) {
          if (p->led != -1)
              digitalWrite(p->led, HIGH);
		  if (p->sensor != -1) {
		     if (idx == 0)
		      attachInterrupt(p->sensor, CountRad0, CHANGE);
			 else if (idx == 1)  
		      attachInterrupt(p->sensor, CountRad1, CHANGE);
			 else  
		      attachInterrupt(p->sensor, CountRad2, CHANGE);
		  }
      }  	  
	  p++;
   }
   delayMicroseconds(50); // Damit ggf. ein Interrupt beim Einschalten ausgeführt wird
}


static inline void RadSensoren(int motor)
{
   MotorPins_t *p = mpins;
    
   for (int idx=0; idx<MOTOR_ANZ; idx++)
   {
      if (motor & (1<<idx)) {
          if (p->led != -1)
              digitalWrite(p->led, LOW);
		  if (p->sensor != -1) {
		      detachInterrupt(p->sensor);
		  }
      }  	  
	  p++;
   }
}


void OnFwd(int Motor, int Geschwindigkeit)
{
  if(Geschwindigkeit>0)
	Geschwindigkeit+=13;
  else if(Geschwindigkeit<0)
	Geschwindigkeit-=13;

  MotorPins_t *p = mpins;
  Geschwindigkeit = constrain(Geschwindigkeit,-100, 100);  // Geschwindigkeit auf Bereich -100 ... +100 begrenzen
  Geschwindigkeit = (65535*Geschwindigkeit)/100;
  PrintNumber("v ",Geschwindigkeit );
  
  for (int i=0; i<MOTOR_ANZ; i++)
  {
	  if (Motor & (1<<i))
	  {
	#ifdef TB6612
         int v;
		 if (Geschwindigkeit>0)
		 {
			digitalWrite(p->dir, HIGH);
            v = Geschwindigkeit;
		 }
		 else
		 {
			digitalWrite(p->dir, LOW);
			v = -Geschwindigkeit;
		 }   	 
		 analogWrite(p->pwm, v);
	#endif
	   }
	   p++;
  }
}


void OnRev(int m, int v)
{
	OnFwd(m, -v);
}

void Off(int Motor)
{
   MotorPins_t *p = mpins;
   for (int i=0; i<MOTOR_ANZ; i++)
   {
	  if (Motor & (1<<i))
	  {
#ifdef TB6612
         analogWrite(p->pwm, 0);  
#endif
      }
      p++;
   }	  
}



#if 0

// gyro-Funktionen nicht in diesem Modul

void DreheRechts(int v, int Winkel)
{
  gyroResetWinkel();
  OnFwd(OUT_A, v);
  OnFwd(OUT_B, -v);
  while (winkelz>=-Winkel)
      gyroUpdate();
  Off (OUT_AB);

}


void DreheLinks(int v, int Winkel)
{
  gyroResetWinkel();
  OnFwd(OUT_A, -v);
  OnFwd(OUT_B, v);
  while (winkelz<=Winkel)
      gyroUpdate();
  Off (OUT_AB);

}


void FahreVor(int v, int Strecke)
{
  RadSensorenAn();
  RadCountR = 0;     // 0 Setzen erst NACH Einschalten des Sensors, eventuell kommt schon ein Interrupt
//  RadCountL = 0;
  OnFwd(OUT_AB, v);
  while (Strecke>RadCountR)
  {
//	 PrintNumber("RC", RadCountR);
//	 delay (1);
  }
  Off(OUT_AB);
  RadSensorenAus();
}

#endif


#if 0
void TestRadSensoren()
{
   RadSensorenAn();
   RadCountR = 0;
   RadCountL = 0;
   OnFwd(OUT_AB,10);
   while(1)
   {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print(RadCountL);
     lcd.setCursor(4,0);
     lcd.print(RadCountR);
     delay(300);
   }
}

#endif
