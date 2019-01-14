#ifndef _USB_MASS_INTERNAL_H_
#define _USB_MASS_INTERNAL_H_

extern USBEndpointInfo usbMassEndpoints[];

#define MASS_ENDPOINT_TX 0
#define MASS_ENDPOINT_RX 1
#define USB_MASS_RX_ENDP (usbMassEndpoints[MASS_ENDPOINT_RX].address)
#define USB_MASS_TX_ENDP (usbMassEndpoints[MASS_ENDPOINT_TX].address)
#define USB_MASS_RX_ADDR (usbMassEndpoints[MASS_ENDPOINT_RX].pmaAddress)
#define USB_MASS_TX_ADDR (usbMassEndpoints[MASS_ENDPOINT_TX].pmaAddress)
#endif