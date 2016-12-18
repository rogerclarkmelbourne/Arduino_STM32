#ifdef CHANGES_INCLUDE

****** DETAILS OF THE CHANGES TO BE DONE TO THE CORE TO BE ABLE TO USE THE LIBRARY HardwareCAN   ******

1) History
The Hardware CAN library was originally published in the Maple Leaflabs forum by X893. 
I tested it, and found bugs, which I fixed in the code. My fixes are commented with the initials JMD.
The most important things that missed was to connect the interrupt service routine to the CAN interrupt vector.
The problem is that in the F1 family, this vector is shared with the USB vector, as is some of the memory. Thus,
when one wants to use the CAN, the USB becomes unavailable. This is a severe drawback of this chip, but one has to cope with this.

2) Changes performed

2.1) In file C:\ArduinoForSTM32\arduino-1.6.9\hardware\Arduino_STM32-master\STM32F1\cores\maple\libmaple\rcc_f1.c
inserted 1 line, position 96:
	[RCC_CAN]	= { .clk_domain = APB1, .line_num = 25 },	//! JMD after X893

2.2) In file C:\ArduinoForSTM32\arduino-1.6.9\hardware\Arduino_STM32-master\STM32F1\system\libmaple\stm32f1\include\series\rcc.h
inserted 1 line, position 442:
	RCC_CAN,	//! JMD after X893    
    
2.3) In file C:\ArduinoForSTM32\arduino-1.6.9\hardware\Arduino_STM32-master\STM32F1\cores\maple\libmaple\usb\stm32f1\usb.c
2.3.1) inserted 12 lines, position 186
// JMD : default ISRs of CAN, to be overridden if HardwareCAN library is used in sketch
void __attribute__((weak)) USB_HP_CAN_TX_IRQHandler(void) 
{ ; }      // Dummy ISR

void __irq_usb_hp_can_tx(void)
{
  USB_HP_CAN_TX_IRQHandler () ;
}

uint8 __attribute__((weak)) CAN_RX0_IRQ_Handler(void)
{ return 1 ; }      // Dummy ISR 

2.3.2) and altered function void __irq_usb_lp_can_rx0(void)
 Was
 
void __irq_usb_lp_can_rx0(void) {
    uint16 istr = USB_BASE->ISTR;

    /* Use USB_ISR_MSK to only include code for bits we care about. */
    
Becomes

void __irq_usb_lp_can_rx0(void) {
    uint16 istr = USB_BASE->ISTR;

	if (CAN_RX0_IRQ_Handler())			//! JMD : Call to CAN ISR, returns 1 CAN is active
		return;							//! JMD

    /* Use USB_ISR_MSK to only include code for bits we care about. */
 #endif
