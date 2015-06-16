// Simple demo for feeding some random data to Pachube.
// 2011-07-08 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Handle returning code and reset ethernet module if needed
// 2013-10-22 hneiraf@gmail.com
//
//-----------------------------------------------------------------
//  Ported to STM32F103 by Vassilis Serasidis on 21 May 2015
//  Home:  http://www.serasidis.gr
//  email: avrsite@yahoo.gr
//
// PIN Connections (Using STM32F103):
//
// ENC28J60 -  STM32F103
//   VCC    -    3.3V
//   GND    -    GND
//   SCK    -    Pin PA5
//   SO     -    Pin PA6
//   SI     -    Pin PA7
//   CS     -    Pin PA8
//----------------------------------------------------------------- 
//
//

#include <EtherCard_STM.h>
#include <SPI.h>

// change these settings to match your own setup
#define FEED "12345678" //Write here your FEED key (look at "Feed Keys" on xively.
#define APIKEY "asgEEwewtwerweTRRWER323SSDFfds" //Write your own APIKEY.

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

const char website[] PROGMEM = "api.xively.com";

byte Ethernet::buffer[350];
uint32_t timer;
Stash stash;
byte session;

//timing variable
int res = 0;



void setup () {
  Serial.begin(57600);
  Serial.println("\n[Xively example]");

  //Initialize Ethernet
  initialize_ethernet();
}


void loop () { 
  
  //if correct answer is not received then re-initialize ethernet module
  if (res > 220){
    initialize_ethernet(); 
  }
  
  res = res + 1;
  
  ether.packetLoop(ether.packetReceive());
  
  //200 res = 10 seconds (50ms each res)
  if (res == 200) {

    
    //Generate random info
    float demo = random(0,500);
    word one = random(0,500);
    String msje;

    if (demo < 250){
      msje = "low";
    }
    else{
      msje = "high";
    }


    // generate two fake values as payload - by using a separate stash,
    // we can determine the size of the generated message ahead of time
    byte sd = stash.create();
    stash.print("demo,");
    stash.println(demo);
    stash.print("one,");
    stash.println(one);
    stash.print("mensaje,");
    stash.println(msje);
    stash.save();

    //Display data to be sent
    Serial.println(demo);
    Serial.println(one);


    // generate the header with payload - note that the stash size is used,
    // and that a "stash descriptor" is passed in as argument using "$H"
    Stash::prepare(PSTR("PUT http://$F/v2/feeds/$F.csv HTTP/1.0" "\r\n"
      "Host: $F" "\r\n"
      "X-PachubeApiKey: $F" "\r\n"
      "Content-Length: $D" "\r\n"
      "\r\n"
      "$H"),
    website, PSTR(FEED), website, PSTR(APIKEY), stash.size(), sd);

    // send the packet - this also releases all stash buffers once done
    session = ether.tcpSend(); 
  }
  
   const char* reply = ether.tcpReply(session);
   
   if (reply != 0) {
     res = 0;
     Serial.println(reply);
   }
   delay(50);
}



void initialize_ethernet(void){  
  for(;;){ // keep trying until you succeed 
    //Reinitialize ethernet module
    pinMode(5, OUTPUT);
    Serial.println("Reseting Ethernet...");
    digitalWrite(5, LOW);
    delay(1000);
    digitalWrite(5, HIGH);
    delay(500);

    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0){ 
      Serial.println( "Failed to access Ethernet controller");
      continue;
    }
    
    if (!ether.dhcpSetup()){
      Serial.println("DHCP failed");
      continue;
    }

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);  
    ether.printIp("DNS: ", ether.dnsip);  

    if (!ether.dnsLookup(website))
      Serial.println("DNS failed");

    ether.printIp("SRV: ", ether.hisip);

    //reset init value
    res = 0;
    break;
  }
}