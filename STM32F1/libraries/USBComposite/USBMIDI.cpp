/******************************************************************************
 *  This started out as a munging of Tymm Twillman's arduino Midi Library into the Libusb class,
 *  though by now very little of the original code is left, except for the class API and
 *  comments. Tymm Twillman kindly gave Alexander Pruss permission to relicense his code under the MIT
 *  license, which fixed a nasty licensing mess.
 *
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2013 Magnus Lundin.
 * Copyright (c) 2013 Donald Delmar Davis, Suspect Devices.
 * (c) 2003-2008 Tymm Twillman <tymm@booyaka.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 *****************************************************************************/

/**
 * @brief USB MIDI device with a class compatible with maplemidi
 */

#include "USBComposite.h" 

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include <wirish.h>
#include "usb_midi_device.h"
#include <libmaple/usb.h>
#include "usb_generic.h"


/*
 * USBMIDI interface
 */

#define USB_TIMEOUT 50

void USBMIDI::setChannel(unsigned int channel) {
	channelIn_ = channel;

}

bool USBMIDI::init(USBMIDI* me) {
    usb_midi_setTXEPSize(me->txPacketSize);
    usb_midi_setRXEPSize(me->rxPacketSize);
	return true;
}

bool USBMIDI::registerComponent() {
    return USBComposite.add(&usbMIDIPart, this, (USBPartInitializer)&USBMIDI::init); 
}

void USBMIDI::begin(unsigned channel) {
	setChannel(channel);
	
	if (enabled)
		return;

	USBComposite.clear();
	registerComponent();
	USBComposite.begin();
	
	enabled = true;	
}

void USBMIDI::end(void) {
	if (enabled) {
		USBComposite.end();
		enabled = false;
	}
}

void USBMIDI::writePacket(uint32 p) {
    this->writePackets(&p, 1);
}

void USBMIDI::writePackets(const void *buf, uint32 len) {
    if (!this->isConnected() || !buf) {
        return;
    }

    uint32 txed = 0;
    uint32 old_txed = 0;
    uint32 start = millis();

    uint32 sent = 0;

    while (txed < len && (millis() - start < USB_TIMEOUT)) {
        sent = usb_midi_tx((const uint32*)buf + txed, len - txed);
        txed += sent;
        if (old_txed != txed) {
            start = millis();
        }
        old_txed = txed;
    }


    if (sent == usb_midi_txEPSize) {
        while (usb_midi_is_transmitting() != 0) {
        }
        /* flush out to avoid having the pc wait for more data */
        usb_midi_tx(NULL, 0);
    }
}

uint32 USBMIDI::available(void) {
    return usb_midi_data_available();
}

uint32 USBMIDI::readPackets(void *buf, uint32 len) {
    if (!buf) {
        return 0;
    }

    uint32 rxed = 0;
    while (rxed < len) {
        rxed += usb_midi_rx((uint32*)buf + rxed, len - rxed);
    }

    return rxed;
}

/* Blocks forever until 1 byte is received */
uint32 USBMIDI::readPacket(void) {
    uint32 p;
    this->readPackets(&p, 1);
    return p;
}

uint8 USBMIDI::pending(void) {
    return usb_midi_get_pending();
}

uint8 USBMIDI::isConnected(void) {
    return usb_is_connected(USBLIB) && usb_is_configured(USBLIB);
}




// These are midi status message types are defined in MidiSpec.h

union EVENT_t {
    uint32 i;
    uint8 b[4];
    MIDI_EVENT_PACKET_t p;
};

// Handle decoding incoming MIDI traffic a word at a time -- remembers
//  what it needs to from one call to the next.
//
//  This is a private function & not meant to be called from outside this class.
//  It's used whenever data is available from the USB port.
//
void USBMIDI::dispatchPacket(uint32 p)
{
    union EVENT_t e;

    e.i=p;
    
    switch (e.p.cin) {
        case CIN_3BYTE_SYS_COMMON:
            if (e.p.midi0 == MIDIv1_SONG_POSITION_PTR) {
                handleSongPosition(((uint16)e.p.midi2)<<7|((uint16)e.p.midi1));
            }
            break;

        case CIN_2BYTE_SYS_COMMON:
             switch (e.p.midi0) {
                 case MIDIv1_SONG_SELECT:
                     handleSongSelect(e.p.midi1);
                     break;
                 case MIDIv1_MTC_QUARTER_FRAME:
                     // reference library doesnt handle quarter frame.
                     break;
             }
            break;
        case CIN_NOTE_OFF:
            handleNoteOff(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1, e.p.midi2);
            break;
        case CIN_NOTE_ON:
            handleNoteOn(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1, e.p.midi2);
            break;
        case CIN_AFTER_TOUCH:
            handleVelocityChange(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1, e.p.midi2);
            break;
        case CIN_CONTROL_CHANGE:
            handleControlChange(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1, e.p.midi2);
            break;
        case CIN_PROGRAM_CHANGE:
            handleProgramChange(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1);
            break;
        case CIN_CHANNEL_PRESSURE:
            handleAfterTouch(MIDIv1_VOICE_CHANNEL(e.p.midi0), e.p.midi1);
            break;
                     
        case CIN_PITCH_WHEEL:
            handlePitchChange(((uint16)e.p.midi2)<<7|((uint16)e.p.midi1));
            break;
        case CIN_1BYTE:
            switch (e.p.midi0) {
                case MIDIv1_CLOCK:
                    handleSync();
                    break;
                case MIDIv1_TICK:
                    break;
                case MIDIv1_START:
                    handleStart();
                    break;
                case MIDIv1_CONTINUE:
                    handleContinue();
                    break;
                case MIDIv1_STOP:
                    handleStop();
                    break;
                case MIDIv1_ACTIVE_SENSE:
                    handleActiveSense();
                    break;
                case MIDIv1_RESET:
                    handleReset();
                    break;
                case MIDIv1_TUNE_REQUEST:
                    handleTuneRequest();
                    break;

                default:
                    break;
            }
            break;
    }
}


// Try to read data from USB port & pass anything read to processing function
void USBMIDI::poll(void)
{   while(available()) {
        dispatchPacket(readPacket());
    }
}

static union EVENT_t outPacket; // since we only use one at a time no point in reallocating it

// Send Midi NOTE OFF message to a given channel, with note 0-127 and velocity 0-127
void USBMIDI::sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_NOTE_OFF;
    outPacket.p.midi0=MIDIv1_NOTE_OFF|(channel & 0x0f);
    outPacket.p.midi1=note;
    outPacket.p.midi2=velocity;
    writePacket(outPacket.i);
    
}


// Send Midi NOTE ON message to a given channel, with note 0-127 and velocity 0-127
void USBMIDI::sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_NOTE_ON;
    outPacket.p.midi0=MIDIv1_NOTE_ON|(channel & 0x0f);
    outPacket.p.midi1=note;
    outPacket.p.midi2=velocity;
    writePacket(outPacket.i);
    
}

// Send a Midi VELOCITY CHANGE message to a given channel, with given note 0-127,
// and new velocity 0-127
// Note velocity change == polyphonic aftertouch.
// Note aftertouch == channel pressure.
void USBMIDI::sendVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_AFTER_TOUCH;
    outPacket.p.midi0=MIDIv1_AFTER_TOUCH |(channel & 0x0f);
    outPacket.p.midi1=note;
    outPacket.p.midi2=velocity;
    writePacket(outPacket.i);
    
}


// Send a Midi CC message to a given channel, as a given controller 0-127, with given
//  value 0-127
void USBMIDI::sendControlChange(unsigned int channel, unsigned int controller, unsigned int value)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_CONTROL_CHANGE;
    outPacket.p.midi0=MIDIv1_CONTROL_CHANGE |(channel & 0x0f);
    outPacket.p.midi1=controller;
    outPacket.p.midi2=value;
    writePacket(outPacket.i);
    
}

// Send a Midi PROGRAM CHANGE message to given channel, with program ID 0-127
void USBMIDI::sendProgramChange(unsigned int channel, unsigned int program)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_PROGRAM_CHANGE;
    outPacket.p.midi0=MIDIv1_PROGRAM_CHANGE |(channel & 0x0f);
    outPacket.p.midi1=program;
    writePacket(outPacket.i);
    
}

// Send a Midi AFTER TOUCH message to given channel, with velocity 0-127
void USBMIDI::sendAfterTouch(unsigned int channel, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_CHANNEL_PRESSURE;
    outPacket.p.midi0=MIDIv1_CHANNEL_PRESSURE |(channel & 0x0f);
    outPacket.p.midi1=velocity;
    writePacket(outPacket.i);
    
}

// Send a Midi PITCH CHANGE message, with a 14-bit pitch (always for all channels)
void USBMIDI::sendPitchChange(unsigned int pitch)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_PITCH_WHEEL;
    outPacket.p.midi0=MIDIv1_PITCH_WHEEL;
    outPacket.p.midi1= (uint8) pitch & 0x07F;
    outPacket.p.midi2= (uint8)  (pitch>>7) & 0x7f;
    writePacket(outPacket.i);
    
}

// Send a Midi SONG POSITION message, with a 14-bit position (always for all channels)
void USBMIDI::sendSongPosition(unsigned int position)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_3BYTE_SYS_COMMON;
    outPacket.p.midi0=MIDIv1_SONG_POSITION_PTR;
    outPacket.p.midi1= (uint8) position & 0x07F;
    outPacket.p.midi2= (uint8)  (position>>7) & 0x7f;
    writePacket(outPacket.i);
    
}

// Send a Midi SONG SELECT message, with a song ID of 0-127 (always for all channels)
void USBMIDI::sendSongSelect(unsigned int song)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_2BYTE_SYS_COMMON;
    outPacket.p.midi0=MIDIv1_SONG_SELECT;
    outPacket.p.midi1= (uint8) song & 0x07F;
    writePacket(outPacket.i);
    
}

// Send a Midi TUNE REQUEST message (TUNE REQUEST is always for all channels)
void USBMIDI::sendTuneRequest(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_TUNE_REQUEST;
    writePacket(outPacket.i);
}


// Send a Midi SYNC message (SYNC is always for all channels)
void USBMIDI::sendSync(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_CLOCK;
    writePacket(outPacket.i);
}

// Send a Midi START message (START is always for all channels)
void USBMIDI::sendStart(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_START ;
    writePacket(outPacket.i);
}


// Send a Midi CONTINUE message (CONTINUE is always for all channels)
void USBMIDI::sendContinue(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_CONTINUE ;
    writePacket(outPacket.i);
}


// Send a Midi STOP message (STOP is always for all channels)
void USBMIDI::sendStop(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_STOP ;
    writePacket(outPacket.i);
}

// Send a Midi ACTIVE SENSE message (ACTIVE SENSE is always for all channels)
void USBMIDI::sendActiveSense(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_ACTIVE_SENSE ;
    writePacket(outPacket.i);
}

// Send a Midi RESET message (RESET is always for all channels)
void USBMIDI::sendReset(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_RESET ;
    writePacket(outPacket.i);
}

const uint32 midiNoteFrequency_10ths[128] = {
	 82, 87, 92, 97, 103, 109, 116, 122, 130, 138, 146, 154, 164, 173, 184, 194, 
	 206, 218, 231, 245, 260, 275, 291, 309, 327, 346, 367, 389, 412, 437, 462, 490, 
	 519, 550, 583, 617, 654, 693, 734, 778, 824, 873, 925, 980, 1038, 1100, 1165, 1235, 
	 1308, 1386, 1468, 1556, 1648, 1746, 1850, 1960, 2077, 2200, 2331, 2469, 2616, 2772, 2937, 3111, 
	 3296, 3492, 3700, 3920, 4153, 4400, 4662, 4939, 5233, 5544, 5873, 6223, 6593, 6985, 7400, 7840, 
	 8306, 8800, 9323, 9878, 10465, 11087, 11747, 12445, 13185, 13969, 14800, 15680, 16612, 17600, 18647, 19755, 
	 20930, 22175, 23493, 24890, 26370, 27938, 29600, 31360, 33224, 35200, 37293, 39511, 41860, 44349, 46986, 49780, 
	 52740, 55877, 59199, 62719, 66449, 70400, 74586, 79021, 83720, 88698, 93973, 99561, 105481, 111753, 118398, 125439 };

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// Placeholders.  You should subclass the Midi base class and define these to have your
//  version called.
void USBMIDI::handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {}
void USBMIDI::handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {}
void USBMIDI::handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity) {}
void USBMIDI::handleControlChange(unsigned int channel, unsigned int controller, unsigned int value) {}
void USBMIDI::handleProgramChange(unsigned int channel, unsigned int program) {}
void USBMIDI::handleAfterTouch(unsigned int channel, unsigned int velocity) {}
void USBMIDI::handlePitchChange(unsigned int pitch) {}
void USBMIDI::handleSongPosition(unsigned int position) {}
void USBMIDI::handleSongSelect(unsigned int song) {}
void USBMIDI::handleTuneRequest(void) {}
void USBMIDI::handleSync(void) {}
void USBMIDI::handleStart(void) {}
void USBMIDI::handleContinue(void) {}
void USBMIDI::handleStop(void) {}
void USBMIDI::handleActiveSense(void) {}
void USBMIDI::handleReset(void) {}
#pragma GCC diagnostic pop
