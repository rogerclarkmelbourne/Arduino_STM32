// Arduino demo sketch for testing RFM12B + ethernet
// 2010-05-20 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

// Listens for RF12 messages and displays valid messages on a webpage
// Memory usage exceeds 1K, so use Atmega328 or decrease history/buffers
//
// This sketch is derived from RF12eth.pde:
// May 2010, Andras Tucsni, http://opensource.org/licenses/mit-license.php
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
#include <JeeLib.h>
#include <avr/eeprom.h>

#define DEBUG   1   // set to 1 to display free RAM on web page
#define SERIAL  0   // set to 1 to show incoming requests on serial port

#define CONFIG_EEPROM_ADDR ((byte*) 0x10)

// configuration, as stored in EEPROM
struct Config {
    byte band;
    byte group;
    byte collect;
    word refresh;
    byte valid; // keep this as last byte
} config;

// ethernet interface mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

// buffer for an outgoing data packet
static byte outBuf[RF12_MAXDATA], outDest;
static char outCount = -1;

#define NUM_MESSAGES  10    // Number of messages saved in history
#define MESSAGE_TRUNC 15    // Truncate message payload to reduce memory use

static BufferFiller bfill;  // used as cursor while filling the buffer

static byte history_rcvd[NUM_MESSAGES][MESSAGE_TRUNC+1]; //history record
static byte history_len[NUM_MESSAGES]; // # of RF12 messages+header in history
static byte next_msg;       // pointer to next rf12rcvd line
static word msgs_rcvd;      // total number of lines received modulo 10,000

byte Ethernet::buffer[1000];   // tcp/ip send and receive buffer

static void loadConfig() {
    for (byte i = 0; i < sizeof config; ++i)
        ((byte*) &config)[i] = eeprom_read_byte(CONFIG_EEPROM_ADDR + i);
    if (config.valid != 253) {
        config.valid = 253;
        config.band = 8;
        config.group = 1;
        config.collect = 1;
        config.refresh = 5;
    }
    byte freq = config.band == 4 ? RF12_433MHZ :
                config.band == 8 ? RF12_868MHZ :
                                   RF12_915MHZ;
    rf12_initialize(31, freq, config.group);
}

static void saveConfig() {
    for (byte i = 0; i < sizeof config; ++i)
        eeprom_write_byte(CONFIG_EEPROM_ADDR + i, ((byte*) &config)[i]);
}

#if DEBUG
static int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
#endif

void setup(){
#if SERIAL
    Serial.begin(57600);
    Serial.println("\n[etherNode]");
#endif
    loadConfig();
    
    if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) 
      Serial.println( "Failed to access Ethernet controller");
    if (!ether.dhcpSetup())
      Serial.println("DHCP failed");
#if SERIAL
    ether.printIp("IP: ", ether.myip);
#endif
}

const char okHeader[] PROGMEM = 
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
;

static void homePage(BufferFiller& buf) {
    word mhz = config.band == 4 ? 433 : config.band == 8 ? 868 : 915;
    buf.emit_p(PSTR("$F\r\n"
        "<meta http-equiv='refresh' content='$D'/>"
        "<title>RF12 etherNode - $D MHz, group $D</title>" 
        "RF12 etherNode - $D MHz, group $D "
            "- <a href='c'>configure</a> - <a href='s'>send packet</a>"
        "<h3>Last $D messages:</h3>"
        "<pre>"), okHeader, config.refresh, mhz, config.group,
                                            mhz, config.group, NUM_MESSAGES);
    for (byte i = 0; i < NUM_MESSAGES; ++i) {
        byte j = (next_msg + i) % NUM_MESSAGES;
        if (history_len[j] > 0) {
            word n = msgs_rcvd - NUM_MESSAGES + i;
            buf.emit_p(PSTR("\n$D$D$D$D: OK"), // hack, to show leading zero's
                                n/1000, (n/100) % 10, (n/10) % 10, n % 10);
            for (byte k = 0; k < history_len[j]; ++k)
                buf.emit_p(PSTR(" $D"), history_rcvd[j][k]);
        }
    }
    long t = millis() / 1000;
    word h = t / 3600;
    byte m = (t / 60) % 60;
    byte s = t % 60;
    buf.emit_p(PSTR(
        "</pre>"
        "Uptime is $D$D:$D$D:$D$D"), h/10, h%10, m/10, m%10, s/10, s%10);
#if DEBUG
    buf.emit_p(PSTR(" ($D bytes free)"), freeRam());
#endif
}

static int getIntArg(const char* data, const char* key, int value =-1) {
    char temp[10];
    if (ether.findKeyVal(data + 7, temp, sizeof temp, key) > 0)
        value = atoi(temp);
    return value;
}

static void configPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    if (data[6] == '?') {
        byte b = getIntArg(data, "b");
        byte g = getIntArg(data, "g");
        byte c = getIntArg(data, "c", 0);
        word r = getIntArg(data, "r");
        if (1 <= g && g <= 250 && 1 <= r && r <= 3600) {
            // store values as new settings
            config.band = b;
            config.group = g;
            config.collect = c;
            config.refresh = r;
            saveConfig();
            // re-init RF12 driver
            loadConfig();
            // clear history
            memset(history_len, 0, sizeof history_len);
            // redirect to the home page
            buf.emit_p(PSTR(
                "HTTP/1.0 302 found\r\n"
                "Location: /\r\n"
                "\r\n"));
            return;
        }
    }
    // else show a configuration form
    buf.emit_p(PSTR("$F\r\n"
        "<h3>Server node configuration</h3>"
        "<form>"
          "<p>"
    "Freq band <input type=text name=b value='$D' size=1> (4, 8, or 9)<br>"
    "Net group <input type=text name=g value='$D' size=3> (1..250)<br>"
    "Collect mode: <input type=checkbox name=c value='1' $S> "
        "Don't send ACKs<br><br>"
    "Refresh rate <input type=text name=r value='$D' size=4> (1..3600 seconds)"
          "</p>"
          "<input type=submit value=Set>"
        "</form>"), okHeader, config.band, config.group,
                    config.collect ? "CHECKED" : "",
                    config.refresh);
}

static void sendPage(const char* data, BufferFiller& buf) {
    // pick up submitted data, if present
    const char* p = strstr(data, "b=");
    byte d = getIntArg(data, "d");
    if (data[6] == '?' && p != 0 && 0 <= d && d <= 31) {
        // prepare to send data as soon as possible in loop()
        outDest = d & RF12_HDR_MASK ? RF12_HDR_DST | d : 0;
        outCount = 0;
        // convert the input string to a number of decimal data bytes in outBuf
        ++p;
        while (*p != 0 && *p != '&') {
            outBuf[outCount] = 0;
            while ('0' <= *++p && *p <= '9')
                outBuf[outCount] = 10 * outBuf[outCount] + (*p - '0');
            ++outCount;
        }
#if SERIAL
        Serial.print("Send to ");
        Serial.print(outDest, DEC);
        Serial.print(':');
        for (byte i = 0; i < outCount; ++i) {
            Serial.print(' ');
            Serial.print(outBuf[i], DEC);
        }
        Serial.println();
#endif
        // redirect to home page
        buf.emit_p(PSTR(
            "HTTP/1.0 302 found\r\n"
            "Location: /\r\n"
            "\r\n"));
        return;
    }
    // else show a send form
    buf.emit_p(PSTR("$F\r\n"
        "<h3>Send a wireless data packet</h3>"
        "<form>"
          "<p>"
    "Data bytes <input type=text name=b size=50> (decimal)<br>"
    "Destination node <input type=text name=d size=3> "
        "(1..31, or 0 to broadcast)<br>"
          "</p>"
          "<input type=submit value=Send>"
        "</form>"), okHeader);
}

void loop(){
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
    // check if valid tcp data is received
    if (pos) {
        bfill = ether.tcpOffset();
        char* data = (char *) Ethernet::buffer + pos;
#if SERIAL
        Serial.println(data);
#endif
        // receive buf hasn't been clobbered by reply yet
        if (strncmp("GET / ", data, 6) == 0)
            homePage(bfill);
        else if (strncmp("GET /c", data, 6) == 0)
            configPage(data, bfill);
        else if (strncmp("GET /s", data, 6) == 0)
            sendPage(data, bfill);
        else
            bfill.emit_p(PSTR(
                "HTTP/1.0 401 Unauthorized\r\n"
                "Content-Type: text/html\r\n"
                "\r\n"
                "<h1>401 Unauthorized</h1>"));  
        ether.httpServerReply(bfill.position()); // send web page data
    }

    // RFM12 loop runner, don't report acks
    if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) {
        history_rcvd[next_msg][0] = rf12_hdr;
        for (byte i = 0; i < rf12_len; ++i)
            if (i < MESSAGE_TRUNC) 
                history_rcvd[next_msg][i+1] = rf12_data[i];
        history_len[next_msg] = rf12_len < MESSAGE_TRUNC ? rf12_len+1
                                                         : MESSAGE_TRUNC+1;
        next_msg = (next_msg + 1) % NUM_MESSAGES;
        msgs_rcvd = (msgs_rcvd + 1) % 10000;

        if (RF12_WANTS_ACK && !config.collect) {
#if SERIAL
            Serial.println(" -> ack");
#endif
            rf12_sendStart(RF12_ACK_REPLY, 0, 0);
        }
    }
    
    // send a data packet out if requested
    if (outCount >= 0 && rf12_canSend()) {
        rf12_sendStart(outDest, outBuf, outCount, 1);
        outCount = -1;
    }
}
