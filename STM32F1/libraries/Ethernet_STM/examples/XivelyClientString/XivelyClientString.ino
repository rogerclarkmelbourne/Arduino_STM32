/*
  Xively(Cosm, Pachube) sensor client
 
 This sketch connects an analog sensor to Xively (http://xively.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 This example has been updated to use the Xively.com API. 
 To make it work, create a feed with two datastreams, and give them the IDs
 sensor1 and sensor2. Or change the code below to match your feed.
 
 This example uses the String library, which is part of the Arduino core from
 version 0019.  
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 modified 9 Apr 2012
 by Tom Igoe with input from Usman Haque and Joe Saavedra
 modified 8 September 2012
 by Scott Fitzgerald
 modified 12 Aug 2013
 by Soohwan Kim
 Modified 18 Aug 2015
 by Vassilis Serasidis
 
 =========================================================
 Ported to STM32F103 on 18 Aug 2015 by Vassilis Serasidis

 <---- Pinout ---->
 W5x00 <--> STM32F103
 SS    <-->  PA4 <-->  BOARD_SPI1_NSS_PIN
 SCK   <-->  PA5 <-->  BOARD_SPI1_SCK_PIN
 MISO  <-->  PA6 <-->  BOARD_SPI1_MISO_PIN
 MOSI  <-->  PA7 <-->  BOARD_SPI1_MOSI_PIN
 =========================================================
 
 
 http://arduino.cc/en/Tutorial/PachubeClientString
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet_STM.h>


#define APIKEY         "YOUR API KEY GOES HERE" // replace your pachube api key here
#define FEEDID         00000 // replace your feed ID
#define USERAGENT      "My Project" // user agent is the project name


// assign a MAC address for the ethernet controller.
// fill in your address here:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif  

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192,168,1,177);
IPAddress gw(0,0,0,0);
IPAddress snip(0,0,0,0);
IPAddress dnsip(0,0,0,0);
// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(173,203,98,29);      // In cases where it is not possible to use DNS, you can use the following bare-IP address alternative
char server[] = "api.xively.com";   // name address for cosm API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000; //delay between updates to xively.com

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection:
#if defined(WIZ550io_WITH_MACADDRESS)
  if (Ethernet.begin() == 0) {
#else
  if (Ethernet.begin(mac) == 0) {
#endif  
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
#if defined(WIZ550io_WITH_MACADDRESS)
    Ethernet.begin(ip, dnsip, gw,snip);
#else
    Ethernet.begin(mac, ip, dnsip, gw,snip);
#endif  
  }
}

void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);   
  // convert the data to a String to send it:

  String dataString = "sensor1,";
  dataString += sensorReading;

  // you can append multiple readings to this String if your
  // pachube feed is set up to handle multiple values:
  int otherSensorReading = analogRead(A1);
  dataString += "\nsensor2,";
  dataString += otherSensorReading;

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data: 
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sendData(dataString);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(String thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");
    client.println(thisData.length());

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.println(thisData);
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}

