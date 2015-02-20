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
#include <libmaple/i2c.h>
#include "lsm303reg.h"
#include "../ringbuffer/ringbuffer.h"
#include "lsm303.h"

#define ACC_FIFO_WATERMARK 10

static int ACC_BIAS_X;
static int ACC_BIAS_Y;
static int ACC_BIAS_Z;


int accx, accy, accz;

#define RB_SIZE 64
#define ACC_AVERAGE_X 60
#define ACC_AVERAGE_Y 60
#define ACC_AVERAGE_Z 60

static int rbvx[RB_SIZE];
static long unsigned int rbtx[RB_SIZE];
static int rbvy[RB_SIZE];
static long unsigned int rbty[RB_SIZE];
static int rbvz[RB_SIZE];
static long unsigned int rbtz[RB_SIZE];
static RingBuffer rbx(RB_SIZE, rbvx, rbtx);
static RingBuffer rby(RB_SIZE, rbvy, rbty);
static RingBuffer rbz(RB_SIZE, rbvz, rbtz);

//static int averageActiveX = 0;
//static int averageActiveY = 0;
//static int averageActiveZ = 0;


static uint8 write_msg_data[3];
static i2c_msg write_msg;

static uint8 read_msg_data[7];
static i2c_msg read_msg;


void readAccValues()
{
	int16 *p;
    write_msg_data[0] = LSM303_REG_OUT_X_L_A|LSM303_READ_MULTI_BYTES; // 0x80 -> AutoIncrement address on read
	write_msg.length = 1;
    i2c_master_xfer(I2C1, &write_msg, 1, 0);

	read_msg.length = 6;
    i2c_master_xfer(I2C1, &read_msg, 1, 2);

	p = (int16*)read_msg_data;
	accx = *p++ - ACC_BIAS_X;
	accy = *p++ - ACC_BIAS_Y;
	accz = *p++ - ACC_BIAS_Z;
}

uint8 accReadRegister(uint8 reg)
{
    write_msg_data[0] = reg;
 	write_msg.length = 1;
    i2c_master_xfer(I2C1, &write_msg, 1, 0);

	read_msg.length = 1;
    i2c_master_xfer(I2C1, &read_msg, 1, 2);

    return read_msg_data[0];
}

void accWriteRegister(uint8 reg, uint8 val)
{
    write_msg_data[0] = reg;
    write_msg_data[1] = val;
	write_msg.length = 2;
    i2c_master_xfer(I2C1, &write_msg, 1, 0);
}


void lsm303I2CInit()
{
    //i2c_master_enable(I2C1, 0);
	// Aktivierung des I2C-Busses muss im Hauptprogramm erfolgen,
	// da mehrere Komponenten das I2C-Interface benutzen können
	// und eine mehrfache Initialisierung nicht funktioniert

    write_msg.addr = LSM303_ADDR_A;
    write_msg.flags = 0; // write, 7 bit address
    write_msg.length = sizeof(write_msg_data);
    write_msg.xferred = 0;
    write_msg.data = write_msg_data;

    read_msg.addr = LSM303_ADDR_A;
    read_msg.flags = I2C_MSG_READ;
    read_msg.length = sizeof(read_msg_data);
    read_msg.xferred = 0;
    read_msg.data = read_msg_data;

}

/**
 * Initialisiert und aktiviert den Beschleunigungssensor
 * Aktivierung des I2C-Busses muss im Hauptprogramm erfolgen,
 * da mehrere Komponenten das I2C-Interface benutzen können
 * und eine mehrfache Initialisierung nicht funktioniert
 */
void lsm303Init()
{
   /* Initialize I2C Transfer-Strukturen für den lsm303*/
   lsm303I2CInit();

   // Initialize ACC-Chip
   // 50Hz data Rate
   accWriteRegister(LSM303_REG_CTRL_REG1_A, LSM303_A_ODR_50HZ|LSM303_A_ALL_AXIS_EN);
   
   // CTRL_REG2 --> no High Pass --> nothing to change
   
   // enable Watermark interrupt on INT2 --> not working no int pulse
   // gyroWriteRegister(GYRO_REG_CTRL_REG3, GYRO_I2_EN|GYRO_I2_WTM);
   
   // sensitivity 2G is default
  
   // enable FIFO ?

   //Fifomode and watermark
}


void accCalibrate() {
	int count = 0;
	int sumx=0,sumy=0,sumz=0;

	#ifdef DEBUG_ACC
	   SerialUSB.print("Biaswerte ermitteln ");
	#endif   
	while (count < 100)
	{
	   delay(3);
   	   readAccValues();
   	   sumx += accx;
	   sumy += accy;
	   sumz += accz;
   	   #ifdef DEBUG_ACC
	   SerialUSB.print("*");
	   #endif
	   count += 1;
	}
	ACC_BIAS_X = sumx/count;
	ACC_BIAS_Y = sumy/count;
	ACC_BIAS_Z = sumz/count;

	#ifdef DEBUG_ACC
	SerialUSB.println("");
	SerialUSB.print("ACC-BIAS ");
	SerialUSB.print(ACC_BIAS_X); SerialUSB.print(" ");
	SerialUSB.print(ACC_BIAS_Y); SerialUSB.print(" ");
	SerialUSB.println(ACC_BIAS_Z);
	#endif
}



void accUpdate()
{
	//uint8 gyroFifoStatus = gyroReadRegister(GYRO_REG_FIFO_SRC);
	//uint8 anz = gyroFifoStatus & 0x1F;

	#ifdef DEBUG_ACC
//	SerialUSB.print("fifo anz = ");
//	SerialUSB.print(anz);
//    SerialUSB.print(" ");
	#endif

	readAccValues();

    rbx.addValue(accx);
    rby.addValue(accy);
    rbz.addValue(accz);

    #ifdef DEBUG_ACC
	SerialUSB.print(accx); SerialUSB.print(" ");
	SerialUSB.print(accy); SerialUSB.print(" ");
	SerialUSB.print(accz); SerialUSB.print(" ");
	SerialUSB.println("");
	#endif
}

void accStart()
{
	rbx.reset();
	rby.reset();
	rbz.reset();
}

void accStop()
{
}

