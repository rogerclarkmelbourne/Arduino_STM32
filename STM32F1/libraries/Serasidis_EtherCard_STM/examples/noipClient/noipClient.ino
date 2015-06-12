// NoIP client sketch for ENC28J60 based Ethernet Shield. 
// You need a free account from www.no-ip.com
//
// 1. On NoIP website, create a host of type DNS Host (A) 
// 2. Insert your host name in noIP_host[] variable
// 3. Encode "username:password" in base64 using an online tool like
// 4. Paste the encoded string in noIP_auth[] variable
//
// Contributed by Luca Dentella <luca@dentella.it>
// http://www.lucadentella.it/2012/04/28/enc28j60-e-arduino-6/
//
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

// Finite-State Machine states
#define STATUS_IDLE                   0
#define STATUS_WAITING_FOR_PUBLIC_IP  1
#define STATUS_NOIP_NEEDS_UPDATE      2
#define STATUS_WAITING_FOR_NOIP       3
#define STATUS_ERROR                  99

// The sketch will check your public IP every CHECK_IP_INTERVAL ms,
// and wait for REQUEST_TIMEOUT ms for a response.
// It will retry for maximum of MAX_ATTEMPTS attemps.
#define CHECK_IP_INTERVAL    60000
#define REQUEST_TIMEOUT      10000
#define MAX_ATTEMPTS         3

// MAC Address of Ethernet Shield
static byte mymac[] = {0x74,0x69,0x69,0x2D,0x30,0x31};

// Insert your hostname and authentication string
const char noIP_host[] PROGMEM = "<YourDomainName>"; //e.g. myname.no-ip.info
const char noIP_auth[] PROGMEM = "<YourPassword>";

//----------------------------------------------------------
// Don't change these ones...
const char getIP_web[] PROGMEM = "www.lucadentella.it";
const char noIP_web[] PROGMEM = "dynupdate.no-ip.com";
//----------------------------------------------------------

// Some global variables
byte Ethernet::buffer[700];
byte getIP_address[4];
byte noIP_address[4];
byte actual_status;
static byte session_id;
static uint32_t check_ip_timer;
static uint32_t request_timer;
int attempt;
Stash stash;

void setup () {

  Serial.begin(57600);
  Serial.println(F("NoIP Client Demo"));
  Serial.println();

  if (!ether.begin(sizeof Ethernet::buffer, mymac))
    Serial.println(F( "Failed to access Ethernet controller"));
  else
    Serial.println(F("Ethernet controller initialized"));
  Serial.println();

  if (!ether.dhcpSetup())
    Serial.println(F("Failed to get configuration from DHCP"));
  else
    Serial.println(F("DHCP configuration done"));

  ether.printIp("IP Address:\t", ether.myip);
  ether.printIp("Netmask:\t", ether.netmask);
  ether.printIp("Gateway:\t", ether.gwip);
  Serial.println();

  actual_status = STATUS_IDLE;
  attempt = 0;

  // Resolve IP for getIP_web and noIP_web 
  // and store them into global variables
  if (!ether.dnsLookup(getIP_web)) {
    Serial.print(F("Unable to resolve IP for "));
    SerialPrint_P(getIP_web);
    actual_status = STATUS_ERROR;
  } else {
    ether.copyIp(getIP_address, ether.hisip);
    SerialPrint_P(getIP_web);
    ether.printIp(" resolved to:\t", ether.hisip);
  }
  if (!ether.dnsLookup(noIP_web)) {
    Serial.print(F("Unable to resolve IP for "));
    SerialPrint_P(noIP_web);
    actual_status = STATUS_ERROR;
  } else {
    ether.copyIp(noIP_address, ether.hisip);
    SerialPrint_P(noIP_web);
    ether.printIp(" resolved to:\t", ether.hisip);    
  }

  Serial.println();
}


void loop() {

  ether.packetLoop(ether.packetReceive());

  // FSM
  switch(actual_status) {

  case STATUS_IDLE: checkPublicIP(); break;
  case STATUS_WAITING_FOR_PUBLIC_IP: checkPublicIPResponse(); break;
  case STATUS_NOIP_NEEDS_UPDATE: updateNoIP(); break;
  case STATUS_WAITING_FOR_NOIP: checkNoIPResponse(); break;
  }
}

void checkPublicIP() {

  if(millis() > check_ip_timer) {

    Serial.print(F("Checking public IP... "));

	// Create a request for GetIP service,
	// set destination IP and send request saving session_id
    Stash::prepare(PSTR("GET /demo/getip.php HTTP/1.1" "\r\n" "Host: $F" "\r\n" "\r\n"), getIP_web);
    ether.copyIp(ether.hisip, getIP_address);
    session_id = ether.tcpSend();
	
	// Change FSM state, prepare for timeout and increment attempts counter
    actual_status = STATUS_WAITING_FOR_PUBLIC_IP;
    request_timer = millis() + REQUEST_TIMEOUT;
    attempt++;
  }
}

void checkPublicIPResponse() {

  String actualIp, dnsIp;

  const char* reply = ether.tcpReply(session_id);

  // We got a valid response
  if(reply != 0) {

	// Parse response for public IP
    for(int i = 0; i < strlen(reply) - 189; i++) actualIp = actualIp + reply[187 + i];
    Serial.println(actualIp);

	// If we can't resolve actual IP for our hostname on NoIP,
	// return to IDLE state and wait for the next interval
    if(!ether.dnsLookup(noIP_host)) {
      Serial.print(F("Unable to resolve actual IP for "));
      SerialPrint_P(noIP_host);
      Serial.println();
      actual_status = STATUS_IDLE;
      attempt = 0;
      check_ip_timer = millis() + CHECK_IP_INTERVAL;
    
	// If we can resolve the IP for our hostname, save it in xxx.xxx.xxx.xxx form
	// and compare it with our public IP
	} else {
      for(int i = 0; i < 4; i++) {
        dnsIp = dnsIp + String(ether.hisip[i]);
        if(i < 3) dnsIp = dnsIp + ".";
      }
      SerialPrint_P(noIP_host);
      Serial.print(F(" resolved to "));
      Serial.println(dnsIp);
      
	  // If they are the same, we can sit down and wait for the next interval
      if(actualIp.compareTo(dnsIp) == 0) {
        Serial.println(F("No update needed :)"));
        actual_status = STATUS_IDLE;
        attempt = 0;
        check_ip_timer = millis() + CHECK_IP_INTERVAL;        
      
	  // Different? We'd better to update NoIP!
	  } else {
        Serial.println(F("Update needed :("));
        actual_status = STATUS_NOIP_NEEDS_UPDATE;
        attempt = 0;
      }
    }    
  
  // No valid response? Check for timeout
  // If we've already sent a max number of requests, return to IDLE state
  // and wait for the next interval
  } else {
    if(millis() > request_timer) {
      Serial.println(F(" no response :("));    
      actual_status = STATUS_IDLE;
      if(attempt == MAX_ATTEMPTS) {
        Serial.println(F("Max number of attempts reached"));
        attempt = 0;
        check_ip_timer = millis() + CHECK_IP_INTERVAL;
      }
    }
  }
}

void updateNoIP() {

  Serial.print(F("Updating NoIP..."));

  // Create a request for updating NoIP using NoIP API,
  // set destination IP and send request saving session_id
  Stash::prepare(PSTR("GET /nic/update?hostname=$F HTTP/1.0" "\r\n"
    "Host: $F" "\r\n"
    "Authorization: Basic $F" "\r\n"
    "User-Agent: NoIP_Client" "\r\n" "\r\n"),
  noIP_host, noIP_web, noIP_auth);
  ether.copyIp(ether.hisip, noIP_address);
  session_id = ether.tcpSend();
  
  // Wait for response or timeout...
  actual_status = STATUS_WAITING_FOR_NOIP;
  request_timer = millis() + REQUEST_TIMEOUT;
  attempt++;
}

void checkNoIPResponse() {

  const char* reply = ether.tcpReply(session_id);
  boolean done;

  // We got a valid response...
  if(reply != 0) {

	// Parse NoIP response looking for status/error codes...
    if(strstr(reply, "good") != 0) {
      Serial.println(F(" done!"));
      done = true;
    } 
    else if(strstr(reply, "nochg") != 0) {
      Serial.println(F(" no change required!"));
      done = true;
    }
    else if(strstr(reply, "nohost") != 0) Serial.println(F(" host not found :("));
    else if(strstr(reply, "badauth") != 0) Serial.println(F(" invalid username or password :("));
    else if(strstr(reply, "badagent") != 0) Serial.println(F(" agent banned :("));
    else if(strstr(reply, "!donator") != 0) Serial.println(F(" feature not available for specified username :("));
    else if(strstr(reply, "abuse") != 0) Serial.println(F(" username blocked due to abuse :("));
    else Serial.println(F(" generic error :("));
    
	// Record has been updated? Ok wait for next interval...
    if(done) {   
      actual_status = STATUS_IDLE;
      attempt = 0;
      check_ip_timer = millis() + CHECK_IP_INTERVAL;
    }
  
  // No valid response? Check for timeout
  // If we've already sent a max number of requests, return to IDLE state
  // and wait for the next interval
  } else {

    if(millis() > request_timer) {
      Serial.println(F("No response from NoIP"));
      if(attempt == MAX_ATTEMPTS) {
        Serial.println(F("Max number of attempts reached"));
        actual_status = STATUS_IDLE;
        attempt = 0;
        check_ip_timer = millis() + CHECK_IP_INTERVAL;
      } 
      else
        actual_status = STATUS_NOIP_NEEDS_UPDATE;
    }
  }
}

void SerialPrint_P(PGM_P str) {
  for (uint8_t c; (c = pgm_read_byte(str)); str++) Serial.write(c);
}