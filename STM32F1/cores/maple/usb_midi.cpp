/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2013 Magnus Lundin.
 * Copyright (c) 2013 Donald Delmar Davis, Suspect Devices.
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
 *  This is a munging of Tymm's arduino Midi Library into the Libusb class.
 *  I am not sure this is the right way to go especially since its licensed differently.
 *
 *  Midi.cpp: Code for MIDI processing library, Arduino version
 *
 *             (c) 2003-2008 Tymm Twillman <tymm@booyaka.com>
 *
 *  This file is part of Tymm's Arduino Midi Library.
 *
 *  Tymm's Arduino Midi Library is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  Tymm's Arduino Midi Library is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Tymm's Arduino Midi Library.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 *
 *****************************************************************************/

/**
 * @brief USB MIDI device with a class compatible with maplemidi
 */

#ifdef USB_MIDI

#include <usb_midi.h>

#include <string.h>
#include <stdint.h>
#include <libmaple/nvic.h>
#include <libmaple/usb_midi_device.h>
#include <libmaple/usb.h>

#include <wirish.h>

/*
 * USBMidi interface
 */

#define USB_TIMEOUT 50

USBMidi::USBMidi(void) {

}

// Constructor -- set up defaults for variables, get ready for use (but don't
//  take over serial port yet)

void USBMidi::begin(unsigned int channel) {

    usb_midi_enable(BOARD_USB_DISC_DEV, BOARD_USB_DISC_BIT);
    /* Not in proprietary stream */
    recvMode_ = 0;
    /* No bytes recevied */
    recvByteCount_ = 0;
    /* Not processing an event */
    recvEvent_ = 0;
    /* No arguments to the event we haven't received */
    recvArg0_ = 0;
    /* Not waiting for bytes to complete a message */
    recvBytesNeeded_ = 0;
    // There was no last event.
    lastStatusSent_ = false;
    // Don't send the extra bytes; just send deltas
    sendFullCommands_ = false;

}

void USBMidi::end(void) {

    usb_midi_disable(BOARD_USB_DISC_DEV, BOARD_USB_DISC_BIT);

}

void USBMidi::writePacket(uint32 p) {
    this->writePackets(&p, 1);
}

void USBMidi::writePackets(const void *buf, uint32 len) {
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


    if (sent == USB_MIDI_TX_EPSIZE) {
        while (usb_midi_is_transmitting() != 0) {
        }
        /* flush out to avoid having the pc wait for more data */
        usb_midi_tx(NULL, 0);
    }
}

uint32 USBMidi::available(void) {
    return usb_midi_data_available();
}

uint32 USBMidi::readPackets(void *buf, uint32 len) {
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
uint32 USBMidi::readPacket(void) {
    uint32 p;
    this->readPackets(&p, 1);
    return p;
}

uint8 USBMidi::pending(void) {
    return usb_midi_get_pending();
}

uint8 USBMidi::isConnected(void) {
    return usb_is_connected(USBLIB) && usb_is_configured(USBLIB);
}


USBMidi MidiUSB;



// This is used for tracking when we're processing a proprietary stream of data
//  The assigned value is arbitrary; just for internal use.
static const int MODE_PROPRIETARY = 0xff;


// These are midi status message types are defined in MidiSpec.h

union EVENT_t {
    uint32 i;
    uint8 b[4];
    MIDI_EVENT_PACKET_t p;
};

// Handle decoding incoming MIDI traffic a byte at a time -- remembers
//  what it needs to from one call to the next.
//
//  This is a private function & not meant to be called from outside this class.
//  It's used whenever data is available from the serial port.
//
void USBMidi::dispatchPacket(uint32 p)
{
    union EVENT_t e;
    
    e.i=p;
    // !!!!!!!!!!!!!!!!  Add a sysex handler  FIX THIS VERY VERY SHORTLY !!!!!!!!!!!!!!
    if (recvMode_ & MODE_PROPRIETARY
        && CIN_IS_SYSEX(e.p.cin))
    {
        /* If sysex handling compiled in, just pass all data received
         * to the sysex handler
         */
        
#ifdef CONFIG_MIDI_PROPRIETARY
//        handleSysex(p);
#endif
        
        return;
    }
    
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


// Try to read data at serial port & pass anything read to processing function
void USBMidi::poll(void)
{   while(available()) {
        dispatchPacket(readPacket());
    }
}

static union EVENT_t outPacket; // since we only use one at a time no point in reallocating it

// Send Midi NOTE OFF message to a given channel, with note 0-127 and velocity 0-127
void USBMidi::sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_NOTE_OFF;
    outPacket.p.midi0=MIDIv1_NOTE_OFF|(channel & 0x0f);
    outPacket.p.midi1=note;
    outPacket.p.midi2=velocity;
    writePacket(outPacket.i);
    
}


// Send Midi NOTE ON message to a given channel, with note 0-127 and velocity 0-127
void USBMidi::sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity)
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
void USBMidi::sendVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity)
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
void USBMidi::sendControlChange(unsigned int channel, unsigned int controller, unsigned int value)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_CONTROL_CHANGE;
    outPacket.p.midi0=MIDIv1_CONTROL_CHANGE |(channel & 0x0f);
    outPacket.p.midi1=controller;
    outPacket.p.midi2=value;
    writePacket(outPacket.i);
    
}

// Send a Midi PROGRAM CHANGE message to given channel, with program ID 0-127
void USBMidi::sendProgramChange(unsigned int channel, unsigned int program)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_PROGRAM_CHANGE;
    outPacket.p.midi0=MIDIv1_PROGRAM_CHANGE |(channel & 0x0f);
    outPacket.p.midi1=program;
    writePacket(outPacket.i);
    
}

// Send a Midi AFTER TOUCH message to given channel, with velocity 0-127
void USBMidi::sendAfterTouch(unsigned int channel, unsigned int velocity)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_CHANNEL_PRESSURE;
    outPacket.p.midi0=MIDIv1_CHANNEL_PRESSURE |(channel & 0x0f);
    outPacket.p.midi1=velocity;
    writePacket(outPacket.i);
    
}

// Send a Midi PITCH CHANGE message, with a 14-bit pitch (always for all channels)
void USBMidi::sendPitchChange(unsigned int pitch)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_PITCH_WHEEL;
    outPacket.p.midi0=MIDIv1_PITCH_WHEEL;
    outPacket.p.midi1= (uint8) pitch & 0x07F;
    outPacket.p.midi2= (uint8)  (pitch<<7) & 0x7f;
    writePacket(outPacket.i);
    
}

// Send a Midi SONG POSITION message, with a 14-bit position (always for all channels)
void USBMidi::sendSongPosition(unsigned int position)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_3BYTE_SYS_COMMON;
    outPacket.p.midi0=MIDIv1_SONG_POSITION_PTR;
    outPacket.p.midi1= (uint8) position & 0x07F;
    outPacket.p.midi2= (uint8)  (position<<7) & 0x7f;
    writePacket(outPacket.i);
    
}

// Send a Midi SONG SELECT message, with a song ID of 0-127 (always for all channels)
void USBMidi::sendSongSelect(unsigned int song)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_2BYTE_SYS_COMMON;
    outPacket.p.midi0=MIDIv1_SONG_SELECT;
    outPacket.p.midi1= (uint8) song & 0x07F;
    writePacket(outPacket.i);
    
}

// Send a Midi TUNE REQUEST message (TUNE REQUEST is always for all channels)
void USBMidi::sendTuneRequest(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_TUNE_REQUEST;
    writePacket(outPacket.i);
}


// Send a Midi SYNC message (SYNC is always for all channels)
void USBMidi::sendSync(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_CLOCK;
    writePacket(outPacket.i);
}

// Send a Midi START message (START is always for all channels)
void USBMidi::sendStart(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_START ;
    writePacket(outPacket.i);
}


// Send a Midi CONTINUE message (CONTINUE is always for all channels)
void USBMidi::sendContinue(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_CONTINUE ;
    writePacket(outPacket.i);
}


// Send a Midi STOP message (STOP is always for all channels)
void USBMidi::sendStop(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_STOP ;
    writePacket(outPacket.i);
}

// Send a Midi ACTIVE SENSE message (ACTIVE SENSE is always for all channels)
void USBMidi::sendActiveSense(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_ACTIVE_SENSE ;
    writePacket(outPacket.i);
}

// Send a Midi RESET message (RESET is always for all channels)
void USBMidi::sendReset(void)
{
    outPacket.p.cable=DEFAULT_MIDI_CABLE;
    outPacket.p.cin=CIN_1BYTE;
    outPacket.p.midi0=MIDIv1_RESET ;
    writePacket(outPacket.i);
}


// Set (package-specific) parameters for the Midi instance
void USBMidi::setParam(unsigned int param, unsigned int val)
{
    if (param == PARAM_SEND_FULL_COMMANDS) {
        if (val) {
            sendFullCommands_ = true;
        } else {
            sendFullCommands_ = false;
        }
    } else if (param == PARAM_CHANNEL_IN) {
        channelIn_ = val;
    }
}


// Get (package-specific) parameters for the Midi instance
unsigned int USBMidi::getParam(unsigned int param)
{
    if (param == PARAM_SEND_FULL_COMMANDS) {
        return sendFullCommands_;
    } else if (param == PARAM_CHANNEL_IN) {
        return channelIn_;
    }
    
    return 0;
}

// Placeholders.  You should subclass the Midi base class and define these to have your
//  version called.
/*
void USBMidi::handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
    digitalWrite(33,0);
}
 
void USBMidi::handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {
    digitalWrite(33,1);
}

void USBMidi::handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity) {}
void USBMidi::handleControlChange(unsigned int channel, unsigned int controller, unsigned int value) {}
void USBMidi::handleProgramChange(unsigned int channel, unsigned int program) {}
void USBMidi::handleAfterTouch(unsigned int channel, unsigned int velocity) {}
void USBMidi::handlePitchChange(unsigned int pitch) {}
void USBMidi::handleSongPosition(unsigned int position) {}
void USBMidi::handleSongSelect(unsigned int song) {}
void USBMidi::handleTuneRequest(void) {}
void USBMidi::handleSync(void) {}
void USBMidi::handleStart(void) {}
void USBMidi::handleContinue(void) {}
void USBMidi::handleStop(void) {}
void USBMidi::handleActiveSense(void) {}
void USBMidi::handleReset(void) {}
  */


#endif
