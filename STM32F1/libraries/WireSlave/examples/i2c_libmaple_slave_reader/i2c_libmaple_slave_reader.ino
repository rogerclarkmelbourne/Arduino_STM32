/*
* i2c_slave example.cpp
*
*  You can use this sketch in combination with master_writer.pde
*
*  Created on: 4 Sep 2012
*      Author: Barry Carter <barry.carter@gmail.com>
*
*  Updated 2019 by Donna Whisnant to reflect new WireSlave library updates
*
*  Note: While this example shows how to call the core's low-level I2C
*  functions directly, their API is less stable than the Arduino API
*  and it's easier to get yourself into trouble with the many quirks
*  and peculiarities of the ST I2C peripheral.  Thus, it's recommended
*  that you instead use the WireSlave TwoWire class and the predefined
*  Wire and Wire1 objects, which are ilustrated in the other examples.
*
*/
#include <Wire_slave.h>
#include <Arduino.h>

i2c_msg txMsg = {};
uint8 txBuffer[255];

i2c_msg rxMsg = {};
uint8 rxBuffer[255];

volatile uint32_t lastMessageTime;

void funcrx(i2c_msg *msg __attribute__((unused))){
  // This function is called when data is received:
  //  msg->addr will be our slave address being accessed
  //  msg->data will point to our receive buffer
  //  msg->xferred will be the length of the incoming message (number of bytes received)
  //  msg->length is the overall size of the buffer (as it was initialized)

  if (msg->xferred != 5) {
      Serial.print("Bad data received:\r\n");
  }
  Serial.print("length: ");
  Serial.println(msg->xferred);
  for (int i=0; i<msg->xferred; i++) {
      // Print as char
      Serial.print((char)msg->data[i]);
      Serial.print(' ');
      // Print as byte
      Serial.println(msg->data[i], 16);
  }

  lastMessageTime = millis();
}

void functx(i2c_msg *msg){
  //
  // We will get this callback for each outgoing packet. Fill in the msg buffer
  // with the data to transmit and set the length to the size of the message.
  //
  msg->data[0] = 0x31;
  msg->data[1] = 0x32;
  msg->data[2] = 0x33;
  msg->data[3] = 0x34;
  msg->data[4] = 0x35;
  msg->length = 5;
}

void setup() {
  Serial.begin(115200);
  Serial.println("libmaple I2C slave reader example");

  // Init the message structures
  // and attach the buffers:
  txMsg.data = txBuffer;    // The functx callback will be called with this structure with 'data' pointing to our buffer
  txMsg.length = 0;         // No initial transmit message length

  rxMsg.data = rxBuffer;
  rxMsg.length = sizeof(rxBuffer);      // The rxMsg buffer should reflect the size of the buffer available to receive

  /* Init slave mode. Enables master too
  *
  * We are going to configure the slave device to
  *  - enable fast I2C (400khz)
  *  - dual addresses (can have 2 addresses per module)
  * general call (accepts data writes to 0x00 on a broadcast basis)
  * 
  */
  i2c_slave_enable(I2C1, I2C_FAST_MODE | I2C_SLAVE_DUAL_ADDRESS | I2C_SLAVE_GENERAL_CALL, 400000);

  // attach  receive handler
  i2c_slave_attach_recv_handler(I2C1, &rxMsg, funcrx);
  // attach transmit handler
  i2c_slave_attach_transmit_handler(I2C1, &txMsg, functx);

  // set address #1 to 4
  i2c_slave_set_own_address(I2C1, 4);

  // set address #2 to 5
  i2c_slave_set_own_address2(I2C1, 5);

  // Start clock
  lastMessageTime = millis();
}

void loop() {
  if((millis() - lastMessageTime) > 3000){
    Serial.println("Nothing received in 3 seconds.");
    lastMessageTime = millis();
  }
}
