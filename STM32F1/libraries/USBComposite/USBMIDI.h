/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 * Copyright (c) 2013 Magnus Lundin.
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
 *****************************************************************************/

/**
 * @brief Wirish USB MIDI port (MidiUSB).
 */

#ifndef _USBMIDI_H_
#define _USBMIDI_H_

//#include <Print.h>
#include <boards.h>
#include <USBComposite.h>
#include "usb_generic.h"

/*
 * This is the Midi class.  If you are just sending Midi data, you only need to make an
 *  instance of the class, passing it your USB port -- in most cases it looks like
 *
 *   USBMidi midi;
 *
 *  then you don't need to do anything else; you can start using it to send Midi messages,
 *  e.g.
 *
 *   midi.sendNoteOn(1, note, velocity);
 *
 * If you are using it to receive Midi data, it's a little more complex & you will need
 *  to subclass the Midi class.
 *
 *  For people not used to C++ this may look confusing, but it's actually pretty easy.
 *   Note that you only need to write the functions for event types you want to handle.
 *   They should match the names & prototypes of the functions in the class; look at
 *   the functions in the Midi class below that have the keyword "virtual" to see which
 *   ones you can use.
 *
 *   Here's an example of one that takes NOTE ON, NOTE OFF, and CONTROL CHANGE:
 *
 *   class MyMidi : public USBMidi {
 *     public:
 *
 *     // Need this to compile; it just hands things off to the Midi class.
 *     MyMidi() : USBMidi(s) {}
 *
 *     void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity)
 *     {
 *         if (note == 40) {digitalWrite(13, HIGH); }
 *     }
 *
 *     void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity)
 *     {
 *       if (note == 40) { digitalWrite(13, LOW); }
 *     }
 *
 *     void handleControlChange(unsigned int channel, unsigned int controller,
 *                              unsigned int value)
 *     {
 *       analogWrite(6, value * 2);
 *     }
 *
 *   Then you need to make an instance of this class:
 *
 *     MyMidi midi();
 *
 *   If receiving Midi data, you also need to call the poll function every time through
 *    loop(); e.g.
 *
 *    void loop() {
 *      midi.poll();
 *    }
 *
 *   This causes the Midi class to read data from the USB port and process it.
 */

class USBMIDI {
private:
    bool enabled = false;
    
    /* Private Receive Parameters */
    
    // The channel this Midi instance receives data for (0 means all channels)
    int channelIn_;
    
    /* Internal functions */
    
    // Called whenever data is read from the USB port
    void dispatchPacket(uint32 packet);
    
    uint32 txPacketSize = 64;
    uint32 rxPacketSize = 64;

public:
	static bool init(USBMIDI* me);
	// This registers this USB composite device component with the USBComposite class instance.
	bool registerComponent();
	void setChannel(unsigned channel=0);
	unsigned getChannel() {
		return channelIn_;
	}
    
    void setRXPacketSize(uint32 size=64) {
        rxPacketSize = size;
    }

    void setTXPacketSize(uint32 size=64) {
        txPacketSize = size;
    }

    // Call to start the USB port, at given baud.  For many applications
    //  the default parameters are just fine (which will cause messages for all
    //  MIDI channels to be delivered)
    void begin(unsigned int channel = 0);
    //void begin();
    void end();
    
    uint32 available(void);
    
    uint32 readPackets(void *buf, uint32 len);
    uint32 readPacket(void);
    
    void writePacket(uint32);
//    void write(const char *str);
    void writePackets(const void*, uint32);
    
    uint8 isConnected();
    uint8 pending();

    // poll() should be called every time through loop() IF dealing with incoming MIDI
    //  (if you're only SENDING MIDI events from the Arduino, you don't need to call
    //  poll); it causes data to be read from the USB port and processed.
    void poll();
    
    // Call these to send MIDI messages of the given types
    void sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendControlChange(unsigned int channel, unsigned int controller, unsigned int value);
    void sendProgramChange(unsigned int channel, unsigned int program);
    void sendAfterTouch(unsigned int channel, unsigned int velocity);
    void sendPitchChange(unsigned int pitch);
    void sendSongPosition(unsigned int position);
    void sendSongSelect(unsigned int song);
    void sendTuneRequest(void);
    void sendSync(void);
    void sendStart(void);
    void sendContinue(void);
    void sendStop(void);
    void sendActiveSense(void);
    void sendReset(void);
    
    // Overload these in a subclass to get MIDI messages when they come in
    virtual void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    virtual void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
    virtual void handleVelocityChange(unsigned int channel, unsigned int note, unsigned int velocity);
    virtual void handleControlChange(unsigned int channel, unsigned int controller, unsigned int value);
    virtual void handleProgramChange(unsigned int channel, unsigned int program);
    virtual void handleAfterTouch(unsigned int channel, unsigned int velocity);
    virtual void handlePitchChange(unsigned int pitch);
    virtual void handleSongPosition(unsigned int position);
    virtual void handleSongSelect(unsigned int song);
    virtual void handleTuneRequest(void);
    virtual void handleSync(void);
    virtual void handleStart(void);
    virtual void handleContinue(void);
    virtual void handleStop(void);
    virtual void handleActiveSense(void);
    virtual void handleReset(void);
    
};

extern const uint32 midiNoteFrequency_10ths[128];

#endif