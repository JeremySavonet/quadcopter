/*
 * CLI over USB driver
 */

#include "ch.h"
#include "hal.h"

#ifndef _USBCONFIG_H_
#define _USBCONFIG_H_

// Endpoints to be used for USBD2.
#define USBD2_DATA_REQUEST_EP           1
#define USBD2_DATA_AVAILABLE_EP         1
#define USBD2_INTERRUPT_REQUEST_EP      2

// Serial over USB Driver structure.
extern SerialUSBDriver SDU2;
extern const USBConfig usbcfg;
extern const SerialUSBConfig serusbcfg;

void usb_manager_init( void );

#endif /* _USBCONFIG_H_ */
