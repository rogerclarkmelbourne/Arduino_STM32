// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#include <Wire_slave.h>
//#define Serial Serial1

#define MasterWire Wire
#define SlaveWire Wire1
#define ENABLE_SLAVE
#define ENABLE_MASTER

void receiveEvent(int howMany);

void setup(){
  Serial.begin(115200);         // start serial for output
  
  #ifdef ENABLE_MASTER
  MasterWire.begin();     // join i2c bus #1 as master
  Serial.println("Done with MasterWire.begin ");
  #endif

  #ifdef ENABLE_SLAVE
  SlaveWire.begin(4);     // join i2c bus #2 as slave with address #4
  Serial.println("Done with SlaveWire.begin ");
  #endif

  #ifdef ENABLE_SLAVE
  SlaveWire.onReceive(receiveEvent); // register event
  #endif
}

void loop(){
  #ifdef ENABLE_MASTER
  static byte x = 0;
  Serial.print("Master writing ");
  Serial.println(x);
  
  MasterWire.beginTransmission(4); // transmit to device #4
  MasterWire.write("x is ");       // sends five bytes
  MasterWire.write(x);             // sends one byte
  MasterWire.endTransmission();    // stop transmitting

  x++;
  #endif
  delay(500);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
// Note that it is not advicable to call Serial.print() from within an ISR
void receiveEvent(int howMany){
  //Serial.print("Slave receving ");
  while(1 < SlaveWire.available()){ // loop through all but the last
    char c = SlaveWire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = SlaveWire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}

