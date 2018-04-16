//
//  MinSysex.c
//  LibMaple4Midi
//
//  Created by Donald D Davis on 4/11/13.
//  Copyright (c) 2013 Suspect Devices. All rights reserved.
//  Modified BSD Liscense 
/*
 0xF0  SysEx
 0x7E  Non-Realtime
 0x7F  The SysEx channel. Could be from 0x00 to 0x7F.
 Here we set it to "disregard channel".
 0x06  Sub-ID -- General Information
 0x01  Sub-ID2 -- Identity Request
 0xF7  End of SysEx
---- response
 0xF0  SysEx
 0x7E  Non-Realtime
 0x7F  The SysEx channel. Could be from 0x00 to 0x7F.
 Here we set it to "disregard channel".
 0x06  Sub-ID -- General Information
 0x02  Sub-ID2 -- Identity Reply
 0xID  Manufacturer's ID
 0xf1  The f1 and f2 bytes make up the family code. Each
 0xf2  manufacturer assigns different family codes to his products.
 0xp1  The p1 and p2 bytes make up the model number. Each
 0xp2  manufacturer assigns different model numbers to his products.
 0xv1  The v1, v2, v3 and v4 bytes make up the version number.
 0xv2
 0xv3
 0xv4
 0xF7  End of SysEx
*/

#define USB_MIDI
#ifdef USB_MIDI

// change this to packets
#define STANDARD_ID_RESPONSE_LENGTH 7

#include "usb_midi_device.h"
#include <libmaple/nvic.h>
#include <libmaple/delay.h>
#include <MinSysex.h>
//#include <wirish/wirish.h>


#define MAX_SYSEX_SIZE 256

/********************************* ACHTUNG! ignores usbmidi cable ********************************/
/*const MIDI_EVENT_PACKET_t standardIDResponse[]={
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX,
        MIDIv1_SYSEX_START,
        USYSEX_NON_REAL_TIME,
        USYSEX_ALL_CHANNELS},
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX,
        USYSEX_GENERAL_INFO,
        USYSEX_GI_ID_RESPONSE,
        LEAFLABS_MMA_VENDOR_1},
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX,
        LEAFLABS_MMA_VENDOR_2, // extended ID
        LEAFLABS_MMA_VENDOR_3, // extended ID
        1}, // family #1
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX,
        2, // family #2
        1, // part   #1
        2}, // part   #2
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX,
        0, // version 1
        0, // version 2
        1}, // version 3
    { DEFAULT_MIDI_CABLE,
        CIN_SYSEX_ENDS_IN_2,
        '!', // lgl compatible
        MIDIv1_SYSEX_END,
        0}
};
*/
const uint8 standardIDResponse[]={
    CIN_SYSEX,
    MIDIv1_SYSEX_START,
    USYSEX_NON_REAL_TIME,
    USYSEX_ALL_CHANNELS,
    CIN_SYSEX,
    USYSEX_GENERAL_INFO,
    USYSEX_GI_ID_RESPONSE,
    LEAFLABS_MMA_VENDOR_1,
    CIN_SYSEX,
    LEAFLABS_MMA_VENDOR_2, // extended ID
    LEAFLABS_MMA_VENDOR_3, // extended ID
    1, // family #1
    CIN_SYSEX,
    2, // family #2
    1, // part   #1
    2, // part   #2
    CIN_SYSEX,
    0, // version 1
    0, // version 2
    1, // version 3
    CIN_SYSEX_ENDS_IN_2,
    '!', // lgl compatible
    MIDIv1_SYSEX_END,
    0
};
//#define STANDARD_ID_RESPONSE_LENGTH (sizeof(standardIDResponse))

typedef enum  {NOT_IN_SYSEX=0,COULD_BE_MY_SYSEX,YUP_ITS_MY_SYSEX,ITS_NOT_MY_SYSEX} sysexStates;
volatile uint8 sysexBuffer[MAX_SYSEX_SIZE];
volatile sysexStates sysexState;
volatile int sysexFinger=0;

/* 
 0xF0  SysEx
 0x??  LEAFLABS_MMA_VENDOR_1
 0x??  LEAFLABS_MMA_VENDOR_2

 0x??  LEAFLABS_MMA_VENDOR_3
 0x10  LGL_DEVICE_NUMBER
 0xLE  CMD: REBOOT

 0xf7  EOSysEx
*/
#define STACK_TOP 0x20000800
#define EXC_RETURN 0xFFFFFFF9
#define DEFAULT_CPSR 0x61000000
#define RESET_DELAY 100000
#if 0
static void wait_reset(void) {
    delay_us(RESET_DELAY);
    nvic_sys_reset();
}
#endif

/* -----------------------------------------------------------------------------dealWithItQuickly()
 * Note: at this point we have established that the sysex belongs to us.
 * So we need to respond to any generic requests like information requests.
 * We also need to handle requests which are meant for us. At the moment this is just the 
 * reset request. 
 * 
 */
void dealWithItQuickly(){
    switch (sysexBuffer[1]) {
        case USYSEX_NON_REAL_TIME:
            switch (sysexBuffer[3]) {
                case USYSEX_GENERAL_INFO:
                    if (sysexBuffer[4]==USYSEX_GI_ID_REQUEST) {
                        usb_midi_tx((uint32 *) standardIDResponse, STANDARD_ID_RESPONSE_LENGTH);
                    }
            }
        case USYSEX_REAL_TIME:
            break;
#if 0			
        case LEAFLABS_MMA_VENDOR_1:
            if (sysexBuffer[5]==LGL_RESET_CMD) {
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
#endif    
        default:
            break;
    }
    ;//turn the led on?
}

/* -----------------------------------------------------------------------------LglSysexHandler()
 * The idea here is to identify which Sysex's belong to us and deal with them.
 */
void LglSysexHandler(uint32 *midiBufferRx, uint32 *rx_offset, uint32 *n_unread_packets) {
    MIDI_EVENT_PACKET_t * midiPackets = (MIDI_EVENT_PACKET_t *) (midiBufferRx+(*rx_offset));
    uint8 nPackets=((*n_unread_packets)-(*rx_offset));
    int cPacket;
    uint8 soPackets=0;
    /********************************* ACHTUNG! ignores usbmidi cable ********************************/
    MIDI_EVENT_PACKET_t *packet;
    for (cPacket=0;cPacket<nPackets;cPacket++){
        packet=midiPackets+cPacket;
        if (!CIN_IS_SYSEX(packet->cin)) {
            continue;
        } // else {
        if (!soPackets) {
            soPackets=cPacket*4;
        }
        if ((sysexState==YUP_ITS_MY_SYSEX) && ((sysexFinger+3)>=MAX_SYSEX_SIZE)){
            sysexState=ITS_NOT_MY_SYSEX;  //eisenhower policy. Even if its mine I cant deal with it. 
        }
        switch (packet->cin) {
            case CIN_SYSEX:
                switch (sysexState) {
                    case NOT_IN_SYSEX : // new sysex.
                        sysexFinger=0;
                        if (packet->midi0 == MIDIv1_SYSEX_START) {
                            if (packet->midi1==USYSEX_REAL_TIME
                                ||packet->midi1==USYSEX_NON_REAL_TIME) {
                                if ((packet->midi2==myMidiChannel)
                                    ||(packet->midi2==USYSEX_ALL_CHANNELS)
                                    ) {
                                    sysexState=YUP_ITS_MY_SYSEX;
                                    sysexBuffer[sysexFinger++]=MIDIv1_SYSEX_START;
                                    sysexBuffer[sysexFinger++]=packet->midi1;
                                    sysexBuffer[sysexFinger++]=packet->midi2;
                                    break;
                                }
                            } else if ((packet->midi1==myMidiID[0])
                                       && (packet->midi2==myMidiID[1])
                                       ){
                                sysexState=COULD_BE_MY_SYSEX;
                                sysexBuffer[sysexFinger++]=MIDIv1_SYSEX_START;
                                sysexBuffer[sysexFinger++]=packet->midi1;
                                sysexBuffer[sysexFinger++]=packet->midi2;
                                break;
                            }
                        }
                        break;
                        
                    case COULD_BE_MY_SYSEX:
                        if (packet->midi0==myMidiID[2]) {
                            sysexState=YUP_ITS_MY_SYSEX;
                            sysexBuffer[sysexFinger++]=packet->midi0;
                            sysexBuffer[sysexFinger++]=packet->midi1;
                            sysexBuffer[sysexFinger++]=packet->midi2;
                        } else {
                            sysexState=ITS_NOT_MY_SYSEX;
                            sysexFinger=0;
                        }
                        break;
                    default:
                        break;
                        
                }
                
                break;
            case CIN_SYSEX_ENDS_IN_1:
            case CIN_SYSEX_ENDS_IN_2:
            case CIN_SYSEX_ENDS_IN_3:
                sysexBuffer[sysexFinger++]=packet->midi0;
                sysexBuffer[sysexFinger++]=packet->midi1;
                sysexBuffer[sysexFinger++]=packet->midi2;
                if (sysexState==YUP_ITS_MY_SYSEX) {
                    if(cPacket>=(int32)(*n_unread_packets)){
                        *n_unread_packets = soPackets;
                        *rx_offset = soPackets;
                    } else {
                        uint8 c = cPacket;
                        uint32 *s;
                        uint32 *d = midiBufferRx + soPackets;
                        for (s = midiBufferRx+c;
                             ((*n_unread_packets) && (s <= midiBufferRx+(USB_MIDI_RX_EPSIZE/4)));
                             d++,s++
                            ) {
                                (*d)=(*s);
                                (*n_unread_packets)--;
                                (*rx_offset)++;
                        
                            }
                        // we need to reset the for loop variables to re process remaining data.
                        nPackets=((*n_unread_packets)-(*rx_offset));
                        cPacket=(*rx_offset);                        
                    }
                    dealWithItQuickly();
                    
                }
                sysexFinger=0;
                sysexState=NOT_IN_SYSEX;

                break;
            default:
                return;
        }
        //}
    

    
    }
    // its our sysex and we will cry if we want to
    return;
}

#endif
