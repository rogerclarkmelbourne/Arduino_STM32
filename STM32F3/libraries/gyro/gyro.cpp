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

/**
 * @brief functions for accessing l3g20 gyro on stm32f3discovery board
 *
 * SPI1 is set up to be a master 
 *      at 9 MHz (SPI_1_125MHZ), 
 *      8 Bit
 *      MSB first in/out  (MSBFIRST)
 *      MODE 0 // following the datasheet it should be MODE 3 but 0 is working too
 *
 * Pin PE3 is used as slave select, as defined for SPI1 in board.h
 *
 * There is no interrupt support because I was not able to get out any int2-output of the chip 
 *
 */

#include <wirish/wirish.h>
#include "l3gd20.h"
#include "../ringbuffer/ringbuffer.h"
#include "gyro.h"


#define NSS PE3
#define GYRO_FIFO_WATERMARK 10

HardwareSPI spi1(1);
static uint8 gyroValuesSingle[6];

static int GYRO_BIAS_X =  -75;
static int GYRO_BIAS_Y =  94;
static int GYRO_BIAS_Z =  0;

float winkelx, winkely, winkelz;
int gyroxRaw, gyroyRaw, gyrozRaw; // bei jedem Update wird ein Datensatz der gelesenen
                                  // Werte in den Raw-Variablen gespeichert um den Bias prüfen zu
                                  // können

#define RB_SIZE 256
#define GYRO_AVERAGE_X 120
#define GYRO_AVERAGE_Y 120
#define GYRO_AVERAGE_Z 120

static int rbvx[RB_SIZE];
static int rbvy[RB_SIZE];
static int rbvz[RB_SIZE];
static RingBufferAverage rbx(RB_SIZE, rbvx, NULL, GYRO_AVERAGE_X);
static RingBufferAverage rby(RB_SIZE, rbvy, NULL, GYRO_AVERAGE_Y);
static RingBufferAverage rbz(RB_SIZE, rbvz, NULL, GYRO_AVERAGE_Z);

static int averageActiveX = 0;
static int averageActiveY = 0;
static int averageActiveZ = 0;

void readGyroValues()
{
	int j;
   digitalWrite(NSS, LOW);
	spi1.transfer(0xA8|0x40);

   for (j=0; j<6; j++)
      gyroValuesSingle[j] =  spi1.transfer(0);
	
   digitalWrite(NSS, HIGH);
}

uint8 gyroReadRegister(uint8 reg)
{
   digitalWrite(NSS, LOW);
   spi1.transfer(reg|0x80);

	uint8 val = spi1.transfer(0);

   digitalWrite(NSS, HIGH);
   return val;
}

void gyroWriteRegister(uint8 reg, uint8 val)
{
   digitalWrite(NSS, LOW);
   spi1.transfer(reg);
   spi1.transfer(val);
   digitalWrite(NSS, HIGH);
}

/**
 * Initialisiert das spi1 interface des stm32f3discovery
 */
void gyroSpiInit()
{
   spi1.begin(SPI_9MHZ, MSBFIRST, 0);
   /* NSS is usually active LOW, so initialize it HIGH */
   pinMode(NSS, OUTPUT);
   digitalWrite(NSS, HIGH);
}

#define GYRO_OP_PARAMS (GYRO_ODR_190|GYRO_LP_BW1|GYRO_ENABLE)

/**
 * konfiguriert den Gyro.
 * der Gyro ist nach dem Aufruf der Funktion aktiv. Wird er nicht mehr benÃ¶tigt, kann er
 * mir gyroStop() wieder deaktiviert werden. FÃ¼r eine Reaktivierung kann die Funktion
 * gyroStart() verwendet werden.
 *
 * Im aktiven Zustand muss im Abstand von hÃ¶chstens 120ms die Funktion gyroUpdate()
 * aufgerufen werden.
 *
 * Die globalen Variablen winkelx,  winkely und winkelz enthalten den jeweils aktuellen winkel.
 * Die Funktion gyroResetWinkel setzt die globalen Variablen wieder auf 0.
 */
void gyroInit()
{
   /* Initialize SPI */
   // gyroSpiInit(); --> called aus dem Anwendungsmodul,
   // da an SPI1 noch weitere Geräte betrieben werden

   // Initialize GyroChip
   // 190Hz data Rate, 50Hz Bandwidth
   gyroWriteRegister(GYRO_REG_CTRL_REG1, GYRO_OP_PARAMS);
   
   // CTRL_REG2 --> no High Pass --> nothing to change
   
   // enable Watermark interrupt on INT2 --> not working no int pulse
   // gyroWriteRegister(GYRO_REG_CTRL_REG3, GYRO_I2_EN|GYRO_I2_WTM);
   
   // set resolution to 500 dps
   gyroWriteRegister(GYRO_REG_CTRL_REG4, GYRO_FULL_SCALE_500);
  
   // enable FIFO, Output after second LP-Filter, no HP
   gyroWriteRegister(GYRO_REG_CTRL_REG5, GYRO_FIFO_ENABLE|GYRO_OUT_SELECT2);
   //gyroWriteRegister(GYRO_REG_CTRL_REG5, GYRO_OUT_SELECT2);

   //Fifomode and watermark
   gyroWriteRegister(GYRO_REG_FIFO_CTRL, GYRO_FIFO_MODE_STREAM|GYRO_FIFO_WATERMARK);
}

/**
 * Es werden 1000 Werte eingelesen (ca. 5,5s) und aus den Werten die Drift-Kompensations-
 * werte gemittelt. Der Gyro darf wÃ¤hrend der Zeit nicht bewegt werden.
 */
void gyroCalibrate() {
	int count = 0;
	int sumx=0,sumy=0,sumz=0;

	#ifdef DEBUG_GYRO
	   SerialUSB.print("Biaswerte ermitteln ");
	#endif   
	while (count < 1000)
	{
	   delay(100);
		uint8 anz = gyroReadRegister(GYRO_REG_FIFO_SRC) & 0x1F;
		for (int i=0; i<anz; i++)
		{
			readGyroValues();
			int16 *p = (int16*)gyroValuesSingle;
			sumx += *p++;
			sumy += *p++;
			sumz += *p;
		}
		#ifdef DEBUG_GYRO
		SerialUSB.print("*");
		#endif
		count += anz;
	}
	GYRO_BIAS_X = sumx/count;
	GYRO_BIAS_Y = sumy/count;
	GYRO_BIAS_Z = sumz/count;

	#ifdef DEBUG_GYRO
	SerialUSB.println("");
	SerialUSB.print("BIAS ");
	SerialUSB.print(GYRO_BIAS_X); SerialUSB.print(" ");
	SerialUSB.print(GYRO_BIAS_Y); SerialUSB.print(" ");
	SerialUSB.println(GYRO_BIAS_Z);
	#endif
}

void gyroSetBiasX(int bias) { GYRO_BIAS_X = bias; }
void gyroSetBiasY(int bias) { GYRO_BIAS_Y = bias; }
void gyroSetBiasZ(int bias) { GYRO_BIAS_Z = bias; }



/**
 * Die Winkel-Variablen werden auf 0 zurÃ¼ckgesetzt
 */
void gyroResetWinkel()
{
   winkelx = 0;
   winkely = 0;
   winkelz = 0;
}


/**
 * Neben der "unendlichen" Integration über die Winkelgeschwindigkeitsmesswerte
 * kann eine zeitlich begrenzte Integration über die letzten n Messwerte eingestellt
 * werden. Damit erhält man den Winkel, um den die Achse in z.B. der letzten Sekunde
 * gedreht wurde. Der additive Fehler der "unendlichen" Integration entfällt hierbei.
 *
 * Außerdem kann eine positive  und eine negative Schwelle eingestellt werden, bei
 * deren Überschreiten je eine Variable incrementiert wird.
 */
void gyroSetActualAngleHistory(int axis, int len )
{
   if (axis == GYRO_AXIS_X)
   {
	   rbx.setAverageLen(len);
	   if (len != 0)
		   averageActiveX = 1;
	   else
		   averageActiveX = 0;
   }
   else if (axis == GYRO_AXIS_Y)
   {
	   rby.setAverageLen(len);
	   if (len != 0)
		   averageActiveY = 1;
	   else
		   averageActiveY = 0;
   }
   else if (axis == GYRO_AXIS_Z)
   {
	   rbz.setAverageLen(len);
	   if (len != 0)
		   averageActiveZ = 1;
	   else
		   averageActiveZ = 0;
   }
}


/**
 * Diese Funktion muss innehalb von 120ms mindestens einmal aufgerufen werden.
 * Es werden die vorhandenen Messwerte aus dem Fifo-Speicher des Chips ausgelesen und
 * die aktuellen Winkel berechnet
 */
void gyroUpdate()
{
	uint8 gyroFifoStatus = gyroReadRegister(GYRO_REG_FIFO_SRC);
	uint8 anz = gyroFifoStatus & 0x1F;

	#ifdef DEBUG_GYRO
	SerialUSB.print("fifo anz = ");
	SerialUSB.print(anz);
    SerialUSB.print(" ");
	#endif

	int16* p;
    int sumx=0,sumy=0,sumz=0;

	for (int i=0; i<anz; i++)
	{
		readGyroValues();
		p = (int16*)gyroValuesSingle;
		gyroxRaw = *p++ - GYRO_BIAS_X;
		gyroyRaw = *p++ - GYRO_BIAS_Y;
		gyrozRaw = *p++ - GYRO_BIAS_Z;
		sumx += gyroxRaw;
		sumy += gyroyRaw;
		sumz += gyrozRaw;

		if (averageActiveX)
			rbx.addValue(gyroxRaw);
	}

	winkelx += (sumx*500.0F)/(160.0F*32767.0F);
	winkely += (sumy*500.0F)/(160.0F*32767.0F);
	winkelz += (sumz*500.0F)/(160.0F*32767.0F);

    #ifdef DEBUG_GYRO
	SerialUSB.print(waktX); SerialUSB.print(" ");
	SerialUSB.print(waktY); SerialUSB.print(" ");
	SerialUSB.print(waktZ); SerialUSB.print(" ");
	SerialUSB.println("");
	#endif
}

/**
 * Versetzt den GyroChip in den aktiven Zustand, leert den Fifo und setzt die Winkel zurueck
 */
void gyroStart()
{
   gyroWriteRegister(GYRO_REG_CTRL_REG1, GYRO_OP_PARAMS);
   // Empty Fifo
   gyroUpdate();
   gyroResetWinkel();
}

/**
 * Versetzt den gyro in den Power Down mode
 */
void gyroStop()
{
   gyroWriteRegister(GYRO_REG_CTRL_REG1, 0);
}


float gyroGetShortValueX()
{
	return (rbx.getAverageSum()*500.0F)/(160.0F*32767.0F);
}

float gyroGetShortValueY()
{
	return (rby.getAverageSum()*500.0F)/(160.0F*32767.0F);
}

float gyroGetShortValueZ()
{
	return (rby.getAverageSum()*500.0F)/(160.0F*32767.0F);
}
 
