// Microchip ENC28J60 Ethernet Interface Driver
// Author: Pascal Stang
// Modified by: Guido Socher
// Copyright: GPL V2
//
// This driver provides initialization and transmit/receive
// functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
// This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
// chip, using an SPI interface to the host processor.
//
// 2010-05-20 <jc@wippler.nl>
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
#ifndef ENC28J60_H
#define ENC28J60_H

/** This class provide low-level interfacing with the ENC28J60 network interface. This is used by the EtherCard class and not intended for use by (normal) end users. */
class ENC28J60 {
public:
    static uint8_t buffer[]; //!< Data buffer (shared by recieve and transmit)
    static uint16_t bufferSize; //!< Size of data buffer
    static bool broadcast_enabled; //!< True if broadcasts enabled (used to allow temporary disable of broadcast for DHCP or other internal functions)

    static uint8_t* tcpOffset () { return buffer + 0x36; } //!< Pointer to the start of TCP payload

    /**   @brief  Initialise SPI interface
    *     @note   Configures Arduino pins as input / output, etc.
    */
    static void initSPI ();

    /**   @brief  Initialise network interface
    *     @param  size Size of data buffer
    *     @param  macaddr Pointer to 6 byte hardware (MAC) address
    *     @param  csPin Arduino pin used for chip select (enable network interface SPI bus). Default = 8 = PA9
    *     @return <i>uint8_t</i> ENC28J60 firmware version or zero on failure.
    */
    static uint8_t initialize (const uint16_t size, const uint8_t* macaddr,
                               uint8_t csPin = PA8);

    /**   @brief  Check if network link is connected
    *     @return <i>bool</i> True if link is up
    */
    static bool isLinkUp ();

    /**   @brief  Sends data to network interface
    *     @param  len Size of data to send
    *     @note   Data buffer is shared by recieve and transmit functions
    */
    static void packetSend (uint16_t len);

    /**   @brief  Copy recieved packets to data buffer
    *     @return <i>uint16_t</i> Size of recieved data
    *     @note   Data buffer is shared by recieve and transmit functions
    */
    static uint16_t packetReceive ();

    /**   @brief  Copy data from ENC28J60 memory
    *     @param  page Data page of memory
    *     @param  data Pointer to buffer to copy data to
    */
    static void copyout (uint8_t page, const uint8_t* data);

    /**   @brief  Copy data to ENC28J60 memory
    *     @param  page Data page of memory
    *     @param  data Pointer to buffer to copy data from
    */
    static void copyin (uint8_t page, uint8_t* data);

    /**   @brief  Get single byte of data from ENC28J60 memory
    *     @param  page Data page of memory
    *     @param  off Offset of data within page
    *     @return Data value
    */
    static uint8_t peekin (uint8_t page, uint8_t off);

    /**   @brief  Put ENC28J60 in sleep mode
    */
    static void powerDown();  // contrib by Alex M.

    /**   @brief  Wake ENC28J60 from sleep mode
    */
    static void powerUp();    // contrib by Alex M.

    /**   @brief  Enable reception of broadcast messages
    *     @param  temporary Set true to temporarily enable broadcast
    *     @note   This will increase load on recieved data handling
    */
    static void enableBroadcast(bool temporary = false);

    /**   @brief  Disable reception of broadcast messages
    *     @param  temporary Set true to only disable if temporarily enabled
    *     @note   This will reduce load on recieved data handling
    */
    static void disableBroadcast(bool temporary = false);

    /**   @brief  Enables reception of mulitcast messages
    *     @note   This will increase load on recieved data handling
    */
    static void enableMulticast ();

    /**   @brief  Disable reception of mulitcast messages
    *     @note   This will reduce load on recieved data handling
    */
    static void disableMulticast();

    /**   @brief  Reset and fully initialise ENC28J60
    *     @param  csPin Arduino pin used for chip select (enable SPI bus)
    *     @return <i>uint8_t</i> 0 on failure
    */
    static uint8_t doBIST(uint8_t csPin = PA8);
};

typedef ENC28J60 Ethernet; //!< Define alias Ethernet for ENC28J60

#endif
