#include <HardwareCAN.h>
//#include "changes.h"
/*
 * Example of use of the HardwareCAN library
 * This application sends two times one frame of data and then blinkes 3 times. Then repeats after 2 seconds. 
 * It also produces data that are sent periodically using another two frames.
 * 
 * Please read the file changes.h to see the changes to be performed to the core in order to use this
 */

byte msgD0 ; // variable to be used in the example.

// Instanciation of CAN interface
HardwareCAN canBus(CAN1_BASE);
CanMsg msg ;

void CANSetup(void)
{
  CAN_STATUS Stat ;

  // Initialize CAN module
  canBus.map(CAN_GPIO_PB8_PB9);       // This setting is already wired in the Olimexino-STM32 board
  Stat = canBus.begin(CAN_SPEED_125, CAN_MODE_NORMAL);    // Other speeds go from 125 kbps to 1000 kbps. CAN allows even more choices.

  canBus.filter(0, 0, 0);
  canBus.set_irq_mode();              // Use irq mode (recommended), so the handling of incoming messages
                                      // will be performed at ease in a task or in the loop. The software fifo is 16 cells long, 
                                      // allowing at least 15 ms before processing the fifo is needed at 125 kbps
  Stat = canBus.status();
  if (Stat != CAN_OK)
     /* Your own error processing here */ ;   // Initialization failed
}


// Send one frame. Parameter is a pointer to a frame structure (above), that has previously been updated with data.
// If no mailbox is available, wait until one becomes empty. There are 3 mailboxes.
CAN_TX_MBX CANsend(CanMsg *pmsg) // Should be moved to the library?!
{
  CAN_TX_MBX mbx;

  do 
  {
    mbx = canBus.send(pmsg) ;
#ifdef USE_MULTITASK
    vTaskDelay( 1 ) ;                 // Infinite loops are not multitasking-friendly
#endif
  }
  while(mbx == CAN_TX_NO_MBX) ;       // Waiting outbound frames will eventually be sent, unless there is a CAN bus failure.
  return mbx ;
}

// Send message
// Prepare and send a frame containing some value 
void SendCANmessage(long id=0x001, byte d0=0x00, byte d1=0x00, byte d2=0x00, byte d3=0x00, byte d4=0x00, byte d5=0x00, byte d6=0x00, byte d7=0x00)
{
  // Initialize the message structure
  // A CAN structure includes the following fields:
  msg.IDE = CAN_ID_STD;          // Indicates a standard identifier ; CAN_ID_EXT would mean this frame uses an extended identifier
  msg.RTR = CAN_RTR_DATA;        // Indicated this is a data frame, as opposed to a remote frame (would then be CAN_RTR_REMOTE)
  msg.ID = id ;                  // Identifier of the frame : 0-2047 (0-0x3ff) for standard idenfiers; 0-0x1fffffff for extended identifiers
  msg.DLC = 8;                   // Number of data bytes to follow

  // Prepare frame : send something
  msg.Data[0] = d0 ;
  msg.Data[1] = d1 ;
  msg.Data[2] = d2 ;
  msg.Data[3] = d3 ;
  msg.Data[4] = d4 ;
  msg.Data[5] = d5 ;
  msg.Data[6] = d6 ;
  msg.Data[7] = d7 ;

  digitalWrite(PC13, LOW);    // turn the onboard LED on
  CANsend(&msg) ;      // Send this frame
  delay(180);              
  digitalWrite(PC13, HIGH);   // turn the LED off 
  delay(100);  
}

// The application program starts here
void setup() {
  // put your setup code here, to run once:
  CANSetup() ;        // Initialize the CAN module and prepare the message structures.
  pinMode(PC13, OUTPUT);
  msgD0 = 0x01;
}

void loop() {
  delay(1000);
  long msgID = 0x101 ;
  SendCANmessage(msgID, msgD0) ;       
  msgD0++;
}
