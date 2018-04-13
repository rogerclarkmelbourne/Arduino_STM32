/*--------------------------------------------------------------------------MidiSpecs.h
 *
 * These defines are based on specs created by the USB and MMA standards organizations. 
 * There are not a lot of other ways to code them so licensing this is rather ludicrous.
 * However, in order to be able to embed this in client projects, and avoid the stupidity 
 * of enforced open everything I will declare the following about this file.
 *
 * Copyright (c) 2011 Donald Delmar Davis, Suspect Devices
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this 
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, 
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */
 
/*
 *  USB midi commands from 
 *  MIDI10.pdf "Universal Serial Bus Device Class Definition for MIDI Devices"
 *  REV 1. (1999)
 *  http://www.usb.org/developers/devclass_docs/midi10.pdf
 *
 */

#ifndef __LETS_MIDI_SPECS_H__
#define __LETS_MIDI_SPECS_H__
#include <stdint.h>
#include <stdbool.h>
// rework this for the different architectures....
#if defined(__GNUC__)
typedef struct 
{
    unsigned cin : 4;  // this is the low nibble.
    unsigned cable  : 4;
//    uint8_t  cin;
    uint8_t  midi0;
    uint8_t  midi1; 
    uint8_t  midi2; 
}  __attribute__ ((__packed__)) MIDI_EVENT_PACKET_t ;
#else
typedef struct // may need to be adjusted for other compilers and bitfield order...
{
    unsigned cable : 4; 
    unsigned cin   : 4; 
    uint8_t  midi0; 
    uint8_t  midi1; 
    uint8_t  midi2; 
} MIDI_EVENT_PACKET_t ;
#endif


#define CIN_MISC_FUNCTION    0x00 /* Reserved for future extension. */
#define CIN_CABLE_EVENT      0x01 /* Reserved for future extension. */
#define CIN_2BYTE_SYS_COMMON 0x02 /* 2Bytes -- MTC, SongSelect, etc. */ 
#define CIN_3BYTE_SYS_COMMON 0x03 /* 3Bytes -- SPP, etc. */
#define CIN_SYSEX			 0x04 /* 3Bytes */
#define CIN_SYSEX_ENDS_IN_1  0x05 /* 1Bytes */
#define CIN_SYSEX_ENDS_IN_2  0x06 /* 2Bytes */
#define CIN_SYSEX_ENDS_IN_3  0x07 /* 3Bytes */
#define CIN_NOTE_OFF         0x08 /* 3Bytes */
#define CIN_NOTE_ON          0x09 /* 3Bytes */
#define CIN_AFTER_TOUCH      0x0A /* 3Bytes */
#define CIN_CONTROL_CHANGE   0x0B /* 3Bytes */
#define CIN_PROGRAM_CHANGE   0x0C /* 2Bytes */
#define CIN_CHANNEL_PRESSURE 0x0D /* 2Bytes */
#define CIN_PITCH_WHEEL      0x0E /* 3Bytes */
#define CIN_1BYTE            0x0F /* 1Bytes */
//#define CIN_IS_SYSEX(cin) ((cin == CIN_SYSEX)||(cin == CIN_SYSEX_ENDS_IN_1)||(cin == CIN_SYSEX_ENDS_IN_2)||(cin == CIN_SYSEX_ENDS_IN_3))
#define CIN_IS_SYSEX(cin) ( ((cin) & ~(0x08)) && ((cin) &0x04) )

/* 
 * MIDI V1 message definitions these are from the MMA document
 * http://www.midi.org/techspecs/midimessages.php
 */
#define MIDIv1_BAUD_RATE 31250

/*
 * parse midi (v1) message macros
 */
#define MIDIv1_IS_STATUS(b)     ((b) & 0x80)
#define MIDIv1_IS_VOICE(b)      ((b) <= 0xEF)
#define MIDIv1_VOICE_COMMAND(b) ((b) & 0xF0)
#define MIDIv1_VOICE_CHANNEL(b) ((b) & 0x0F)
#define MIDIv1_IS_SYSCOMMON(b)  (((b) >= 0xF0) & ((b) < 0xF8))
#define MIDIv1_IS_REALTIME(b)   ((b) >= 0xF8)

/*
 * Voice category messages
 */
#define MIDIv1_NOTE_OFF         0x80 /* 2 bytes data -- CIN_NOTE_OFF */
#define MIDIv1_NOTE_ON          0x90 /* 2 bytes data -- CIN_NOTE_ON */
#define MIDIv1_AFTER_TOUCH      0xA0 /* 2 bytes data -- CIN_AFTER_TOUCH */
#define MIDIv1_CONTROL_CHANGE   0xB0 /* 2 bytes data -- CIN_CONTROL_CHANGE */
#define MIDIv1_PROGRAM_CHANGE   0xC0 /* 1 byte data  -- CIN_PROGRAM_CHANGE */
#define MIDIv1_CHANNEL_PRESSURE 0xD0 /* 1 byte data  -- CIN_CHANNEL_PRESSURE */
#define MIDIv1_PITCH_WHEEL      0xE0 /* 2 bytes data -- CIN_PITCH_WHEEL */

/*
 * System common category messages
 */
#define MIDIv1_SYSEX_START       0xF0
#define MIDIv1_SYSEX_END         0xF7
#define MIDIv1_MTC_QUARTER_FRAME 0xF1 /* 1 byte data -- CIN_2BYTE_SYS_COMMON */
#define MIDIv1_SONG_POSITION_PTR 0xF2 /* 2 bytes data -- CIN_3BYTE_SYS_COMMON */
#define MIDIv1_SONG_SELECT       0xF3 /* 1 byte data -- CIN_2BYTE_SYS_COMMON */
#define MIDIv1_TUNE_REQUEST      0xF6 /* no data -- CIN_1BYTE */

/*
 * Realtime category messages, can be sent anytime
 */
#define MIDIv1_CLOCK             0xF8 /* no data -- CIN_1BYTE */
#define MIDIv1_TICK              0xF9 /* no data -- CIN_1BYTE */
#define MIDIv1_START             0xFA /* no data -- CIN_1BYTE */
#define MIDIv1_CONTINUE          0xFB /* no data -- CIN_1BYTE */
#define MIDIv1_STOP              0xFC /* no data -- CIN_1BYTE */
#define MIDIv1_ACTIVE_SENSE      0xFE /* no data -- CIN_1BYTE */
#define MIDIv1_RESET             0xFF /* no data -- CIN_1BYTE */

/*
 * sysex universal id's 
 */
#define MIDIv1_UNIVERSAL_REALTIME_ID     0x7F
#define MIDIv1_UNIVERSAL_NON_REALTIME_ID 0x7E
#define MIDIv1_UNIVERSAL_ALL_CHANNELS    0x7F
/*
 * Susbset of universal sysex (general info request)
 * As described http://www.blitter.com/~russtopia/MIDI/~jglatt/tech/midispec.htm
 */
#define USYSEX_NON_REAL_TIME    0x7E
#define USYSEX_REAL_TIME        0x7F
#define USYSEX_ALL_CHANNELS     0x7F
#define USYSEX_GENERAL_INFO     0x06
#define USYSEX_GI_ID_REQUEST    0x01
#define USYSEX_GI_ID_RESPONSE   0x02


#endif


