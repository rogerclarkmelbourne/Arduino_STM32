// Example of EtherCard usage, contributed by Will Rose, 2012-07-05.

#include <EtherCard_STM.h>
#include <SPI.h>

#define BUF_SIZE 512

byte mac[] = { 0x00, 0x04, 0xA3, 0x21, 0xCA, 0x38 };   // Nanode MAC address.

uint8_t ip[] = { 192, 168, 1, 203 };          // The fallback board address.
uint8_t dns[] = { 192, 168, 1, 1 };        // The DNS server address.
uint8_t gateway[] = { 192, 168, 1, 1 };    // The gateway router address.
uint8_t subnet[] = { 255, 255, 255, 0 };    // The subnet mask.
byte Ethernet::buffer[BUF_SIZE];
byte fixed;                                 // Address fixed, no DHCP

void setup(void)
{
    Serial.begin(57600);
    delay(2000);

    /* Check that the Ethernet controller exists */
    Serial.println("Initialising the Ethernet controller");
    if (ether.begin(sizeof Ethernet::buffer, mac) == 0) {
        Serial.println( "Ethernet controller NOT initialized");
        while (true)
            /* MT */ ;
    }

    /* Get a DHCP connection */
    Serial.println("Attempting to get an IP address using DHCP");
    fixed = false;
    if (ether.dhcpSetup()) {
        ether.printIp("Got an IP address using DHCP: ", ether.myip);
    }
    /* If DHCP fails, start with a hard-coded address */
    else {
        ether.staticSetup(ip, gateway, dns);
        ether.printIp("DHCP FAILED, using fixed address: ", ether.myip);
        fixed = true;
    }

    return;
}

void loop(void)
{
    word rc;

    /* These function calls are required if ICMP packets are to be accepted */
    rc = ether.packetLoop(ether.packetReceive());
    Serial.print("ether.packetLoop() returned ");
    Serial.println(rc, DEC);

    // For debugging
    delay(5000);

    return;
}