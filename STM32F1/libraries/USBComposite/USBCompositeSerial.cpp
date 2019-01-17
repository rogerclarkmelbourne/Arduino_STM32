/* Copyright (c) 2011, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include "USBComposite.h" 

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include <libmaple/usb.h>
#include <string.h>
#include <libmaple/iwdg.h>

#include "usb_composite_serial.h"

#define USB_TIMEOUT 50

#if defined(SERIAL_USB)
static void rxHook(unsigned, void*);
static void ifaceSetupHook(unsigned, void*);
#endif

bool USBCompositeSerial::init(USBCompositeSerial* me) {
#if defined(SERIAL_USB)
	composite_cdcacm_set_hooks(USBHID_CDCACM_HOOK_RX, rxHook);
	composite_cdcacm_set_hooks(USBHID_CDCACM_HOOK_IFACE_SETUP, ifaceSetupHook);
#endif
    composite_cdcacm_setTXEPSize(me->txPacketSize);
    composite_cdcacm_setRXEPSize(me->rxPacketSize);
	return true;
}

void USBCompositeSerial::begin(long speed) {
	(void)speed;
	if (!enabled) {
		USBComposite.clear();
		registerComponent();
		USBComposite.begin();
		enabled = true;
	}
}

void USBCompositeSerial::end() {
	if (enabled) {
		USBComposite.end();
		enabled = false;
	}
}

size_t USBCompositeSerial::write(uint8 ch) {
size_t n = 0;
    this->write(&ch, 1);
		return n;
}

size_t USBCompositeSerial::write(const char *str) {
size_t n = 0;
    this->write((const uint8*)str, strlen(str));
	return n;
}

size_t USBCompositeSerial::write(const uint8 *buf, uint32 len)
{
    size_t n = 0;

    if (!this->isConnected() || !buf) {
        return 0;
    }
    
    uint32 txed = 0;
    while (txed < len) {
        txed += composite_cdcacm_tx((const uint8*)buf + txed, len - txed);
    }

	return n;
}

int USBCompositeSerial::available(void) {
    return composite_cdcacm_data_available();
}

int USBCompositeSerial::peek(void)
{
    uint8 b;
	if (composite_cdcacm_peek(&b, 1)==1)
	{
		return b;
	}
	else
	{
		return -1;
	}
}

bool USBCompositeSerial::registerComponent() {
	return USBComposite.add(&usbSerialPart, this, (USBPartInitializer)&USBCompositeSerial::init);
}

void USBCompositeSerial::flush(void)
{
/*Roger Clark. Rather slow method. Need to improve this */
    uint8 b;
	while(composite_cdcacm_data_available())
	{
		this->read(&b, 1);
	}
    return;
}

uint32 USBCompositeSerial::read(uint8 * buf, uint32 len) {
    uint32 rxed = 0;
    while (rxed < len) {
        rxed += composite_cdcacm_rx(buf + rxed, len - rxed);
    }

    return rxed;
}

/* Blocks forever until 1 byte is received */
int USBCompositeSerial::read(void) {
    uint8 b;
	/*
	    this->read(&b, 1);
    return b;
	*/
	
	if (composite_cdcacm_rx(&b, 1)==0)
	{
		return -1;
	}
	else
	{
		return b;
	}
}

uint8 USBCompositeSerial::pending(void) {
    return composite_cdcacm_get_pending();
}

uint8 USBCompositeSerial::isConnected(void) {
    return usb_is_connected(USBLIB) && usb_is_configured(USBLIB) && composite_cdcacm_get_dtr();
}

uint8 USBCompositeSerial::getDTR(void) {
    return composite_cdcacm_get_dtr();
}

uint8 USBCompositeSerial::getRTS(void) {
    return composite_cdcacm_get_rts();
}

#if defined(SERIAL_USB)

enum reset_state_t {
    DTR_UNSET,
    DTR_HIGH,
    DTR_NEGEDGE,
    DTR_LOW
};

static reset_state_t reset_state = DTR_UNSET;

static void ifaceSetupHook(unsigned hook, void *requestvp) {
    (void)hook;
    uint8 request = *(uint8*)requestvp;

        // Ignore requests we're not interested in.
    if (request != USBHID_CDCACM_SET_CONTROL_LINE_STATE) {
        return;
    }

    // We need to see a negative edge on DTR before we start looking
    // for the in-band magic reset byte sequence.
    uint8 dtr = composite_cdcacm_get_dtr();
    switch (reset_state) {
    case DTR_UNSET:
        reset_state = dtr ? DTR_HIGH : DTR_LOW;
        break;
    case DTR_HIGH:
        reset_state = dtr ? DTR_HIGH : DTR_NEGEDGE;
        break;
    case DTR_NEGEDGE:
        reset_state = dtr ? DTR_HIGH : DTR_LOW;
        break;
    case DTR_LOW:
        reset_state = dtr ? DTR_HIGH : DTR_LOW;
        break;
    }

	if ((composite_cdcacm_get_baud() == 1200) && (reset_state == DTR_NEGEDGE)) {
		iwdg_init(IWDG_PRE_4, 10);
		while (1);
	}
}

#define RESET_DELAY 100000
static void wait_reset(void) {
  delay_us(RESET_DELAY);
  nvic_sys_reset();
}

#define STACK_TOP 0x20000800
#define EXC_RETURN 0xFFFFFFF9
#define DEFAULT_CPSR 0x61000000
static void rxHook(unsigned hook, void *ignored) {
    (void)hook;
    (void)ignored;
    /* FIXME this is mad buggy; we need a new reset sequence. E.g. NAK
     * after each RX means you can't reset if any bytes are waiting. */
    if (reset_state == DTR_NEGEDGE) {
        reset_state = DTR_LOW;

        if (composite_cdcacm_data_available() >= 4) {
            // The magic reset sequence is "1EAF".
            static const uint8 magic[4] = {'1', 'E', 'A', 'F'};	

            uint8 chkBuf[4];

            // Peek at the waiting bytes, looking for reset sequence,
            // bailing on mismatch.
            composite_cdcacm_peek_ex(chkBuf, composite_cdcacm_data_available() - 4, 4);
            for (unsigned i = 0; i < sizeof(magic); i++) {
                if (chkBuf[i] != magic[i]) {
                    return;
                }
            }

            // Got the magic sequence -> reset, presumably into the bootloader.
            // Return address is wait_reset, but we must set the thumb bit.
            uintptr_t target = (uintptr_t)wait_reset | 0x1;
            asm volatile("mov r0, %[stack_top]      \n\t" // Reset stack
                         "mov sp, r0                \n\t"
                         "mov r0, #1                \n\t"
                         "mov r1, %[target_addr]    \n\t"
                         "mov r2, %[cpsr]           \n\t"
                         "push {r2}                 \n\t" // Fake xPSR
                         "push {r1}                 \n\t" // PC target addr
                         "push {r0}                 \n\t" // Fake LR
                         "push {r0}                 \n\t" // Fake R12
                         "push {r0}                 \n\t" // Fake R3
                         "push {r0}                 \n\t" // Fake R2
                         "push {r0}                 \n\t" // Fake R1
                         "push {r0}                 \n\t" // Fake R0
                         "mov lr, %[exc_return]     \n\t"
                         "bx lr"
                         :
                         : [stack_top] "r" (STACK_TOP),
                           [target_addr] "r" (target),
                           [exc_return] "r" (EXC_RETURN),
                           [cpsr] "r" (DEFAULT_CPSR)
                         : "r0", "r1", "r2");

            /* Can't happen. */
            ASSERT_FAULT(0);
        }
    }
}
#endif

