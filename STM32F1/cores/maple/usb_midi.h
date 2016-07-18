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

#ifdef USB_MIDI

#ifndef _WIRISH_USB_MIDI_H_
#define _WIRISH_USB_MIDI_H_

#include <Print.h>
#include <boards.h>

/*
 * This is the Midi class.  If you are just sending Midi data, you only need to make an
 *  instance of the class, passing it your serial port -- in most cases it looks like
 *
 *   USBMidi midi(channel);
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
 *   This causes the Midi class to read data from the serial port and process it.
 */

class USBMidi {
private:
    // The serial port used by this Midi instance (it takes complete control over the port)
    
    /* Private Receive Parameters */
    
    // The channel this Midi instance receives data for (0 means all channels)
    int channelIn_;
    
    // These are for keeping track of partial Midi messages as bytes come in
    int recvMode_;
    int recvByteCount_;
    int recvEvent_;
    int recvArg0_;
    int recvBytesNeeded_;
    int lastStatusSent_;
    
    /* Private Send Parameters */
    
    // This controls whether every Midi message gets a command byte sent with it
    //  (Midi can just send a single command byte and then stream events without
    //  sending the command each time)
    bool sendFullCommands_;
    
    /* Internal functions */
    
    // Called whenever data is read from the serial port
    void dispatchPacket(uint32 packet);
    
    // This doesn't work -- by making it private, we ensure nobody ever calls it
    //Midi();
    
public:
    // Set this parameter to anything other than 0 to cause every MIDI update to
    //  include a copy of the current event (e.g. for every note off to include
    //  the NOTE OFF event) -- by default, if an event is the same type of event as
    //  the last one sent, the event byte isn't sent; just the new note/velocity
    static const unsigned int PARAM_SEND_FULL_COMMANDS = 0x1000;
    
    // Use this parameter to update the channel the MIDI code is looking for messages
    //  to.  0 means "all channels".
    static const unsigned int PARAM_CHANNEL_IN         = 0x1001;
    
    
    // Constructor -- generally just use e.g. "Midi midi(Serial);"
    USBMidi();
    
    // Call to start the serial port, at given baud.  For many applications
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

    
    // Changes the updateable parameters (params are Midi::PARAM_SEND_FULL_COMMANDS or
    //  Midi::PARAM_CHANNEL_IN -- see above for what they mean)
    void setParam(unsigned int param, unsigned int val);
    
    // Get current values for the user updateable parameters (params, etc same as above)
    unsigned int getParam(unsigned int param);
    
    
    // poll() should be called every time through loop() IF dealing with incoming MIDI
    //  (if you're only SENDING MIDI events from the Arduino, you don't need to call
    //  poll); it causes data to be read from the serial port and processed.
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


extern USBMidi MidiUSB;


#endif

#endif

