// ACHTUNG:
// für Midi Eingang die voids von usb_midi.cpp übernehmen -> siehe unten (sind auskommentiert im cpp file)
//#include <usb_midi.h>
#define ledpiny   33
boolean flip=0;
long sendtimer=millis();
void setup() {
    pinMode(ledpiny, OUTPUT);
    //MidiUSB.begin();
}

void loop() {
  //  digitalWrite(ledpiny, MidiUSB.isConnected() );
  /*
    void sendNoteOff(unsigned int channel, unsigned int note, unsigned int velocity);
    void sendNoteOn(unsigned int channel, unsigned int note, unsigned int velocity);
  */
  if (millis()-sendtimer>=500)
  {
    if (flip==0)
   MidiUSB.sendNoteOn(1, 64, 127);
else
    MidiUSB.sendNoteOff(1, 64, 0);
    flip=!flip;
    sendtimer=millis();
   
  }
 MidiUSB.poll();
  }
  void USBMidi::handleNoteOff(unsigned int channel, unsigned int note, unsigned int velocity) {
    digitalWrite(ledpiny,0);
}
  

void USBMidi::handleNoteOn(unsigned int channel, unsigned int note, unsigned int velocity) {
  if (velocity>=0)
    digitalWrite(ledpiny,1);
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
