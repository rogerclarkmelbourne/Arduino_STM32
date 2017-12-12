/*
* i2c_slave example.cpp
*
*  Created on: 4 Sep 2012
*      Author: Barry Carter <barry.carter@gmail.com>
*/
#include <Arduino.h>
#include <libmaple/i2c.h>

#define USE_BUFFERED_EXAMPLE 1

i2c_msg msg;
uint8 buffer[255];

volatile uint8 value_to_print = 'A';
volatile bool newMessage = false;

void funcrx(i2c_msg *msg){
  // Received length will be in msg->length
  char return_data = msg->data[0];
  value_to_print = return_data;
  newMessage = true;
}

#if USE_BUFFERED_EXAMPLE == 1
/* We ARE using a buffer to transmit the data out.
* Make sure you fill the buffer with the data AND you set the length correctly
*/
void functx(i2c_msg *msg){
  // Cheeky. We are using the received byte of the data which is currently in
  // byte 0 to echo it back to the master device
  //msg->data[0] = 0x01;    // We are re-using the rx buffer here to echo the request back
  msg->data[1] = 0x02;
  msg->data[2] = 0x03;
  msg->data[3] = 0x04;
  msg->data[4] = 0x05;
  msg->length = 5;
}

#else

/* We are NOT using the buffered data transmission
* We will get this callback for each outgoing packet. Make sure to call i2c_write
* Strickly speaking, we should be sending a NACk on the last byte we want to send
* but for this test example I am going to assume the master will NACK it when it
* wants to stop.
*/
void functx(i2c_msg *msg){
  i2c_write(I2C1, msg->data[0]);
}

#endif

#define Serial Serial1

void setup() {
  Serial.begin(115200);
  while(!Serial)
  ;
  Serial.println("I2C Slave example");

  // attach the buffer
  msg.data = buffer;

  /* Init slave mode. Enables master too
  * We are going to configure the slave device to
  *  - enable fast I2C (400khz)
  *  - dual addresses (can have 2 addresses per module)
  * general call (accepts data writes to 0x00 on a broadcast basis)
  *
  * If the buffered example is enabled, then we also enable the
  * buffer for rx and tx.
  * Note you can independently enable/disable RX and TX buffers to
  * allow a buffered read and direct writes. Useful if you don't know how
  * much the master will read.
  */
  #if USE_BUFFERED_EXAMPLE == 1
  i2c_slave_enable(I2C1, I2C_FAST_MODE | I2C_SLAVE_DUAL_ADDRESS | I2C_SLAVE_GENERAL_CALL | I2C_SLAVE_USE_RX_BUFFER | I2C_SLAVE_USE_TX_BUFFER);
  #else
  i2c_slave_enable(I2C1, I2C_FAST_MODE | I2C_SLAVE_DUAL_ADDRESS | I2C_SLAVE_GENERAL_CALL);
  #endif

  // attach  receive handler
  i2c_slave_attach_recv_handler(I2C1, &msg, funcrx);
  // attach transmit handler
  i2c_slave_attach_transmit_handler(I2C1, &msg, functx);

  // set addresss to 4
  i2c_slave_set_own_address(I2C1, 4);
}

void loop() {
  if (newMessage) {
    Serial.println("");
    Serial.print("Last byte: ");
    Serial.println(value_to_print);
    Serial.println((char*)buffer);
    newMessage = false;
  } else {
    static uint32_t lastMessage = millis();
    if(millis() -lastMessage > 2000){
      Serial.print("n");
      lastMessage = millis();
    }
  }
}
