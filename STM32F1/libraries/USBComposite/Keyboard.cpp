#include "USBHID.h"
#include <string.h>

//================================================================================
//================================================================================
//	Keyboard

#define SHIFT 0x80
static const uint8_t ascii_to_hid[128] =
{
	0x00,             // NUL
	0x00,             // SOH
	0x00,             // STX
	0x00,             // ETX
	0x00,             // EOT
	0x00,             // ENQ
	0x00,             // ACK
	0x00,             // BEL
	0x2a,             // BS	Backspace
	0x2b,             // TAB	Tab
	0x28,             // LF	Enter
	0x00,             // VT
	0x00,             // FF
	0x00,             // CR
	0x00,             // SO
	0x00,             // SI
	0x00,             // DEL
	0x00,             // DC1
	0x00,             // DC2
	0x00,             // DC3
	0x00,             // DC4
	0x00,             // NAK
	0x00,             // SYN
	0x00,             // ETB
	0x00,             // CAN
	0x00,             // EM
	0x00,             // SUB
	0x00,             // ESC
	0x00,             // FS
	0x00,             // GS
	0x00,             // RS
	0x00,             // US

	0x2c,		   //  ' '
	0x1e|SHIFT,	   // !
	0x34|SHIFT,	   // "
	0x20|SHIFT,    // #
	0x21|SHIFT,    // $
	0x22|SHIFT,    // %
	0x24|SHIFT,    // &
	0x34,          // '
	0x26|SHIFT,    // (
	0x27|SHIFT,    // )
	0x25|SHIFT,    // *
	0x2e|SHIFT,    // +
	0x36,          // ,
	0x2d,          // -
	0x37,          // .
	0x38,          // /
	0x27,          // 0
	0x1e,          // 1
	0x1f,          // 2
	0x20,          // 3
	0x21,          // 4
	0x22,          // 5
	0x23,          // 6
	0x24,          // 7
	0x25,          // 8
	0x26,          // 9
	0x33|SHIFT,      // :
	0x33,          // ;
	0x36|SHIFT,      // <
	0x2e,          // =
	0x37|SHIFT,      // >
	0x38|SHIFT,      // ?
	0x1f|SHIFT,      // @
	0x04|SHIFT,      // A
	0x05|SHIFT,      // B
	0x06|SHIFT,      // C
	0x07|SHIFT,      // D
	0x08|SHIFT,      // E
	0x09|SHIFT,      // F
	0x0a|SHIFT,      // G
	0x0b|SHIFT,      // H
	0x0c|SHIFT,      // I
	0x0d|SHIFT,      // J
	0x0e|SHIFT,      // K
	0x0f|SHIFT,      // L
	0x10|SHIFT,      // M
	0x11|SHIFT,      // N
	0x12|SHIFT,      // O
	0x13|SHIFT,      // P
	0x14|SHIFT,      // Q
	0x15|SHIFT,      // R
	0x16|SHIFT,      // S
	0x17|SHIFT,      // T
	0x18|SHIFT,      // U
	0x19|SHIFT,      // V
	0x1a|SHIFT,      // W
	0x1b|SHIFT,      // X
	0x1c|SHIFT,      // Y
	0x1d|SHIFT,      // Z
	0x2f,          // [
	0x31,          // bslash
	0x30,          // ]
	0x23|SHIFT,    // ^
	0x2d|SHIFT,    // _
	0x35,          // `
	0x04,          // a
	0x05,          // b
	0x06,          // c
	0x07,          // d
	0x08,          // e
	0x09,          // f
	0x0a,          // g
	0x0b,          // h
	0x0c,          // i
	0x0d,          // j
	0x0e,          // k
	0x0f,          // l
	0x10,          // m
	0x11,          // n
	0x12,          // o
	0x13,          // p
	0x14,          // q
	0x15,          // r
	0x16,          // s
	0x17,          // t
	0x18,          // u
	0x19,          // v
	0x1a,          // w
	0x1b,          // x
	0x1c,          // y
	0x1d,          // z
	0x2f|SHIFT,    //
	0x31|SHIFT,    // |
	0x30|SHIFT,    // }
	0x35|SHIFT,    // ~
	0				// DEL
};

void HIDKeyboard::begin(void){
    USBHID.addOutputBuffer(&ledData);
}

void HIDKeyboard::end(void) {
}

// 136: non-printing key
// shift -> 0x02
// modifiers: 128 --> bit shift

uint8_t HIDKeyboard::getKeyCode(uint8_t k, uint8_t* modifiersP)
{
    *modifiersP = 0;
    
    if (adjustForHostCapsLock && (getLEDs() & 0x02)) { // capslock is down on host OS, so host will reverse
        if ('a' <= k && k <= 'z') 
            k += 'A'-'a';
        else if ('A' <= k && k <= 'Z') 
            k += 'a'-'A';
    }
    
    if (k < 0x80) {
        k = ascii_to_hid[k];
        if (k & SHIFT) {
            k &= 0x7f;
            *modifiersP = 0x02;
        }
        return k;
    }
    if (k >= 0x88) { // non-printing key, Arduino format
        return k - 0x88;
    }
    else { // shift key
        *modifiersP = 1<<(k-0x80);
        return 0;
    }    
}

size_t HIDKeyboard::press(uint8_t k) {
    uint8_t modifiers;
    
    k = getKeyCode(k, &modifiers);
    
    if (k == 0) {
        if (modifiers == 0) {
            return 0;
        }
    }
    else {
        for (unsigned i = 0; i<HID_KEYBOARD_ROLLOVER; i++) {
            if (keyReport.keys[i] == k) {
                goto SEND;
            }
        }
        for (unsigned i = 0; i<HID_KEYBOARD_ROLLOVER; i++) {
            if (keyReport.keys[i] == 0) {
                keyReport.keys[i] = k;
                goto SEND;
            }
        }
        return 0;
    }

SEND:
    keyReport.modifiers |= modifiers;
    sendReport();
    return 1;
}

// release() takes the specified key out of the persistent key report and
// sends the report.  This tells the OS the key is no longer pressed and that
// it shouldn't be repeated any more.
size_t HIDKeyboard::release(uint8_t k)
{
    uint8_t modifiers;
    k = getKeyCode(k, &modifiers);
    
    if (k != 0) {
        for (unsigned i=0; i<HID_KEYBOARD_ROLLOVER; i++) {
             if (keyReport.keys[i] == k) {
                 keyReport.keys[i] = 0;
                 break;
             }
        }
    }
    else {
        if (modifiers == 0)
            return 0;
    }
    
    keyReport.modifiers &= ~modifiers;
    
	sendReport();
	return 1;
}

void HIDKeyboard::releaseAll(void)
{
    memset(keyReport.keys, 0, HID_KEYBOARD_ROLLOVER);
	keyReport.modifiers = 0;
	
	sendReport();
}

size_t HIDKeyboard::write(uint8_t c)
{
    if (press(c)) {
        release(c);		// Keyup
        return 1;
    }
    else {
        return 0;
    }
}

HIDKeyboard Keyboard;
