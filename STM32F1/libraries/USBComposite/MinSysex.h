//
//  lgl_min_sysex.h
//  LibMaple4Midi
//
//  Created by Donald D Davis on 4/11/13.
//  Copyright (c) 2013 Suspect Devices. All rights reserved.
//

#ifndef __LGL_MIN_SYSEX_H__
#define __LGL_MIN_SYSEX_H__ 1


#include "MidiSpecs.h"
//#include "LGL.h"

#define LEAFLABS_MMA_VENDOR_1   0x7D
#define LEAFLABS_MMA_VENDOR_2   0x1E
#define LEAFLABS_MMA_VENDOR_3   0x4F

// move to LGL.h
#define LGL_RESET_CMD           0x1e

#define DEFAULT_MIDI_CHANNEL    0x0A
#define DEFAULT_MIDI_DEVICE     0x0A
#define DEFAULT_MIDI_CABLE      0x00

// eventually all of this should be in a place for settings which can be written to flash.
extern volatile uint8 myMidiChannel;
extern volatile uint8 myMidiDevice;
extern volatile uint8 myMidiCable;
extern volatile uint8 myMidiID[];


void LglSysexHandler(uint32 *midiBufferRx,uint32 *rx_offset,uint32 *n_unread_bytes);

#endif

