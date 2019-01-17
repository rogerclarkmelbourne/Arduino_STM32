#include <USBComposite.h>

#define SPEAKER_PIN PA0

class myMidi : public USBMIDI {
 virtual void handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
  noTone(SPEAKER_PIN);
 }
 virtual void handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {
   tone(SPEAKER_PIN, (midiNoteFrequency_10ths[note]+5)/10);
  }
  
};

myMidi midi;
USBCompositeSerial CompositeSerial;

void setup() {
    USBComposite.setProductId(0x0030);
    pinMode(SPEAKER_PIN, OUTPUT);
    midi.registerComponent();
    CompositeSerial.registerComponent();
    USBComposite.begin();
}

void loop() {
  midi.poll();
}

