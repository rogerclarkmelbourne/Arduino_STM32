#include <HardwareCAN.h>
/*
 *
 */

#define T_DELAY 10
// Instanciation of CAN interface
HardwareCAN canBus(CAN1_BASE);
CanMsg msg ;


void CAN_a_33_Setup(void)
{

  CAN_STATUS Stat ;
  afio_init(); // this will restart subsystem and make it work!
  canBus.map(CAN_GPIO_PA11_PA12);  
  Stat = canBus.begin(CAN_SPEED_33, CAN_MODE_NORMAL);
  canBus.filter(0, 0, 0);
  canBus.set_irq_mode();
  Stat = canBus.status();
  if (Stat != CAN_OK)
  {digitalWrite(PC13, LOW);
   }
// /* Your own error processing here */  ;  // Initialization failed
 }

void CAN_b_95_Setup(void)
{
  CAN_STATUS Stat ;
  canBus.map(CAN_GPIO_PB8_PB9);
  Stat = canBus.begin(CAN_SPEED_95, CAN_MODE_NORMAL);
  canBus.filter(0, 0, 0);
  canBus.set_irq_mode();
  Stat = canBus.status();
  if (Stat != CAN_OK)
  {digitalWrite(PC13, LOW);
   }
//     /* Your own error processing here */ ;   // Initialization failed
//  delay(T_DELAY);
}


CAN_TX_MBX CANsend(CanMsg *pmsg) // Should be moved to the library?!
{
  CAN_TX_MBX mbx;

//  do 
//  {
    mbx = canBus.send(pmsg) ;
#ifdef USE_MULTITASK
    vTaskDelay( 1 ) ;                 // Infinite loops are not multitasking-friendly
#endif
//  }
//  while(mbx == CAN_TX_NO_MBX) ;       // Waiting outbound frames will eventually be sent, unless there is a CAN bus failure.
  return mbx ;
}

// Send message
// Prepare and send a frame containing some value 
void SendCANmessage(long id=0x001, byte dlength=8, byte d0=0x00, byte d1=0x00, byte d2=0x00, byte d3=0x00, byte d4=0x00, byte d5=0x00, byte d6=0x00, byte d7=0x00)
{
  // Initialize the message structure
  // A CAN structure includes the following fields:
  msg.IDE = CAN_ID_STD;          // Indicates a standard identifier ; CAN_ID_EXT would mean this frame uses an extended identifier
  msg.RTR = CAN_RTR_DATA;        // Indicated this is a data frame, as opposed to a remote frame (would then be CAN_RTR_REMOTE)
  msg.ID = id ;                  // Identifier of the frame : 0-2047 (0-0x3ff) for standard idenfiers; 0-0x1fffffff for extended identifiers
  msg.DLC = dlength;                   // Number of data bytes to follow

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
  digitalWrite(PC13, HIGH);   // turn the LED off 
  delay(T_DELAY);  
}


// The application program starts here
byte msgD0 = 0x00;
void setup() {        // Initialize the CAN module and prepare the message structures.
  pinMode(PC13, OUTPUT);
  digitalWrite(PC13, HIGH);
  delay(10);
  digitalWrite(PC13, LOW);
  delay(200);
  digitalWrite(PC13, HIGH);
  delay(200);

}


void loop() {
/**/
	CAN_a_33_Setup();
	SendCANmessage(0x108,8,0x03,msgD0,msgD0,0x00,msgD0,msgD0,0x00,0x00);       
	SendCANmessage(0x5e8,8,0x81,msgD0,msgD0,msgD0);
	delay(30);
/**/
	CAN_b_95_Setup();
	for (byte msgD1=1;msgD1<3;msgD1++)
	{	    
		delay(T_DELAY);
		SendCANmessage(0x201,3,0x01,0xff,msgD0);       
		delay(T_DELAY);
		SendCANmessage(0x201,3,0x00,0xff,msgD0);       
		delay(T_DELAY);
		SendCANmessage(0x201,3,0x01,0xff,msgD1);       
		delay(T_DELAY);
		SendCANmessage(0x201,3,0x00,0xff,msgD1);       
		delay(T_DELAY);
	}  
/**/
msgD0++;
}
