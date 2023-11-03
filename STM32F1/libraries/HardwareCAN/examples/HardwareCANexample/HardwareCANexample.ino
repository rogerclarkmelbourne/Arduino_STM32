#include <HardwareCAN.h>
#include "changes.h"
/*
 * Example of use of the HardwareCAN library
 * This application receives two frames containing various data. It also produces data that are sent periodically using another two frames.
 * Please read the file changes.h to see the changes to be performed to the core in order to use this 
 */
// Define the values of the identifiers
#define GYRO_ID 0x27
#define JOYSTICK_VALUES_ID 0x5A
#define TANK_LEVEL_ID 0x78
#define MOTOR_CONTROL_ID 0x92

// Limit time to flag a CAN error
#define CAN_TIMEOUT 100           
#define CAN_DELAY 10        // ms between two processings of incoming messages
#define CAN_SEND_RATE 200   // ms between two successive sendings

// Message structures. Each message has its own identifier. As many such variables should be defined 
// as the number of different CAN frames the application has to send. Here, they are two.
CanMsg msgGyroscope ;
CanMsg msgMotorControl ;

// Traffic handling data
int CANquietTime ;          // Quiet time counter to detect no activity on CAN bus
bool CANError ;             // Indicates that incoming CAN traffic is missing
int CANsendDivider ;        // Used to send frames once every so many times loop() is called

// Applicaton variables
int Contents[4] ;           // Contents of the four tanks
int JoystickX ;             // Setting of the joystick, X axis
int JoystickY ;             // ... Y axis
int AngularRate ;           // Output of local gyroscope
int Throttle  ;             // Motor control value, produced by some local processing
bool ErreurGyroscope = false ;

// Instanciation of CAN interface
HardwareCAN canBus(CAN1_BASE);


// Note : for the predefined identifiers, please have a look in file can.h

void CANSetup(void)
{
  CAN_STATUS Stat ;

  // Initialize the message structures
  // A CAN structure includes the following fields:
  msgGyroscope.IDE = CAN_ID_STD;          // Indicates a standard identifier ; CAN_ID_EXT would mean this frame uses an extended identifier
  msgGyroscope.RTR = CAN_RTR_DATA;        // Indicated this is a data frame, as opposed to a remote frame (would then be CAN_RTR_REMOTE)
  msgGyroscope.ID = GYRO_ID ;             // Identifier of the frame : 0-2047 (0-0x3ff) for standard idenfiers; 0-0x1fffffff for extended identifiers
  msgGyroscope.DLC = 3;                   // Number of data bytes to follow
  msgGyroscope.Data[0] = 0x0;             // Data bytes, there can be 0 to 8 bytes.
  msgGyroscope.Data[1] = 0x0;
  msgGyroscope.Data[2] = 0x0;
  
  msgMotorControl.IDE = CAN_ID_STD;
  msgMotorControl.RTR = CAN_RTR_DATA;
  msgMotorControl.ID = MOTOR_CONTROL_ID ;
  msgMotorControl.DLC = 2;
  msgMotorControl.Data[0] = 0x0;
  msgMotorControl.Data[1] = 0x0;

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
CAN_TX_MBX CANsend(CanMsg *pmsg)
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

// Process incoming messages
// Note : frames are not fully checked for correctness: DLC value is not checked, neither are the IDE and RTR fields. However, the data is guaranteed to be corrrect.
void ProcessMessages(void)
{
  int Pr = 0 ;
  int i ;
  
  CanMsg *r_msg;

  // Loop for every message in the fifo
  while ((r_msg = canBus.recv()) != NULL)
  {
    CANquietTime = 0 ;              // Reset at each received frame
    CANError = false ;              // Clear CAN silence error
    switch ( r_msg->ID )
    {
      case TANK_LEVEL_ID :                  // This frame contains four 16-bit words, little endian coded
        for ( i = 0 ; i < 4 ; i++ )
          Contents[i] = (int)r_msg->Data[2*i] | ((int)r_msg->Data[(2*i)+1]) << 8 ;
        break ;
    
      case JOYSTICK_VALUES_ID :             // This frame contains two 16-bit words, little endian coded
        Pr = (int)r_msg->Data[0] ;
        Pr |= (int)r_msg->Data[1] << 8 ;
        JoystickX = Pr ;
        
        Pr = (int)r_msg->Data[2] ;
        Pr |= (int)r_msg->Data[3] << 8 ;
        JoystickY = Pr ;
        break ;

      default :                     // Any frame with a different identifier is ignored
        break ;
    }
    
    canBus.free();                          // Remove processed message from buffer, whatever the identifier
#ifdef USE_MULTITASK
    vTaskDelay( 1 ) ;                       // Infinite loops are not multitasking-friendly
#endif
  }
}

// Send messages
// Prepare and send 2 frames containing the value of process variables
// Sending all frames at once is a choice; they could be sent separately, at different times and rates.
void SendCANmessages(void)
{
  // Prepare Gyroscope frame : send angular rate
  msgGyroscope.Data[0] = AngularRate & 0xff ;
  msgGyroscope.Data[1] = ( AngularRate >> 8 ) & 0xff ;
  msgGyroscope.Data[2] = ErreurGyroscope ? 1 : 0 ;
  CANsend(&msgGyroscope) ;      // Send this frame
  
  msgMotorControl.Data[0] = Throttle & 0xff ;
  msgMotorControl.Data[1] = ( Throttle >> 8 ) & 0xff ;
  CANsend(&msgMotorControl) ;
}

// The application program starts here
void setup() {
  // put your setup code here, to run once:
  CANSetup() ;        // Initialize the CAN module and prepare the message structures.
}

void loop() {
  // Process incoming messages periodically (should be often enough to avoid overflowing the fifo)
  ProcessMessages() ;          // Process all incoming messages, update local variables accordingly

  // This is an example of timeout management. Here it is global to all received frames; 
  // it could be on a frame by frame basis, with as many control variables as the number of frames.
  CANquietTime++ ; 
  if ( CANquietTime > CAN_TIMEOUT )
  {
    CANquietTime = CAN_TIMEOUT + 1 ;            // To prevent overflowing this variable if silence prolongs...
    CANError = true ;                           // Flag CAN silence error. Will be cleared at first frame received
  }

  // Send messages containing variables to publish. Sent less frequently than the processing of incoming frames (here, every 200 ms)
  CANsendDivider-- ;
  if ( CANsendDivider < 0 )
  {
    CANsendDivider = CAN_SEND_RATE / CAN_DELAY ;
    SendCANmessages() ;       
  }
  delay(CAN_DELAY) ;    // The delay must not be greater than the time to overflow the incoming fifo (here about 15 ms)
}


