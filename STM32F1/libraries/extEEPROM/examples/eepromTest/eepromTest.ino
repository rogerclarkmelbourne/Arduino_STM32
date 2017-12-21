//Test extEEPROM library.
//Writes the EEPROM full of 32-bit integers and reads them back to verify.
//Wire a button from digital pin 6 to ground, this is used as a start button
//so the sketch doesn't do unnecessary EEPROM writes every time it's reset.
//Jack Christensen 09Jul2014

#include <extEEPROM.h>    //http://github.com/JChristensen/extEEPROM/tree/dev
#include <Streaming.h>    //http://arduiniana.org/libraries/streaming/
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire

//Two 24LC256 EEPROMs on the bus
const uint32_t totalKBytes = 64;         //for read and write test functions
extEEPROM eep(kbits_256, 2, 64);         //device size, number of devices, page size

const uint8_t btnStart = 6;              //start button

void setup(void)
{
    pinMode(btnStart, INPUT_PULLUP);
    Serial.begin(115200);
    uint8_t eepStatus = eep.begin(extEEPROM::twiClock400kHz);   //go fast!
    if (eepStatus) {
        Serial << endl << F("extEEPROM.begin() failed, status = ") << eepStatus << endl;
        while (1);
    }
    
    Serial << endl << F("Press button to start...") << endl;
    while (digitalRead(btnStart) == HIGH) delay(10);    //wait for button push

    uint8_t chunkSize = 64;    //this can be changed, but must be a multiple of 4 since we're writing 32-bit integers
//    eeErase(chunkSize, 0, totalKBytes * 1024 - 1);
    eeWrite(chunkSize);
    eeRead(chunkSize);

    dump(0, 16);            //the first 16 bytes
    dump(32752, 32);        //across the device boundary
    dump(65520, 16);        //the last 16 bytes
}

void loop(void)
{
}

//write test data (32-bit integers) to eeprom, "chunk" bytes at a time
void eeWrite(uint8_t chunk)
{
    chunk &= 0xFC;                //force chunk to be a multiple of 4
    uint8_t data[chunk];
    uint32_t val = 0;
    Serial << F("Writing...") << endl;
    uint32_t msStart = millis();
    
    for (uint32_t addr = 0; addr < totalKBytes * 1024; addr += chunk) {
        if ( (addr &0xFFF) == 0 ) Serial << addr << endl;
        for (uint8_t c = 0; c < chunk; c += 4) {
            data[c+0] = val >> 24;
            data[c+1] = val >> 16;
            data[c+2] = val >> 8;
            data[c+3] = val;
            ++val;
        }
        eep.write(addr, data, chunk);
    }
    uint32_t msLapse = millis() - msStart;
    Serial << "Write lapse: " << msLapse << " ms" << endl;
}

//read test data (32-bit integers) from eeprom, "chunk" bytes at a time
void eeRead(uint8_t chunk)
{
    chunk &= 0xFC;                //force chunk to be a multiple of 4
    uint8_t data[chunk];
    uint32_t val = 0, testVal;
    Serial << F("Reading...") << endl;
    uint32_t msStart = millis();
    
    for (uint32_t addr = 0; addr < totalKBytes * 1024; addr += chunk) {
        if ( (addr &0xFFF) == 0 ) Serial << addr << endl;
        eep.read(addr, data, chunk);
        for (uint8_t c = 0; c < chunk; c += 4) {
            testVal =  ((uint32_t)data[c+0] << 24) + ((uint32_t)data[c+1] << 16) + ((uint32_t)data[c+2] << 8) + (uint32_t)data[c+3];
            if (testVal != val) Serial << F("Error @ addr ") << addr+c << F(" Expected ") << val << F(" Read ") << testVal << F(" 0x") << _HEX(testVal) << endl;
            ++val;
        }
    }
    uint32_t msLapse = millis() - msStart;
    Serial << "Last value: " << --val << " Read lapse: " << msLapse << " ms" << endl;
}

//write 0xFF to eeprom, "chunk" bytes at a time
void eeErase(uint8_t chunk, uint32_t startAddr, uint32_t endAddr)
{
    chunk &= 0xFC;                //force chunk to be a multiple of 4
    uint8_t data[chunk];
    Serial << F("Erasing...") << endl;
    for (int i = 0; i < chunk; i++) data[i] = 0xFF;
    uint32_t msStart = millis();
    
    for (uint32_t a = startAddr; a <= endAddr; a += chunk) {
        if ( (a &0xFFF) == 0 ) Serial << a << endl;
        eep.write(a, data, chunk);
    }
    uint32_t msLapse = millis() - msStart;
    Serial << "Erase lapse: " << msLapse << " ms" << endl;
}

//dump eeprom contents, 16 bytes at a time.
//always dumps a multiple of 16 bytes.
void dump(uint32_t startAddr, uint32_t nBytes)
{
    Serial << endl << F("EEPROM DUMP 0x") << _HEX(startAddr) << F(" 0x") << _HEX(nBytes) << ' ' << startAddr << ' ' << nBytes << endl;
    uint32_t nRows = (nBytes + 15) >> 4;

    uint8_t d[16];
    for (uint32_t r = 0; r < nRows; r++) {
        uint32_t a = startAddr + 16 * r;
        eep.read(a, d, 16);
        Serial << "0x";
        if ( a < 16 * 16 * 16 ) Serial << '0';
        if ( a < 16 * 16 ) Serial << '0';
        if ( a < 16 ) Serial << '0';
        Serial << _HEX(a) << ' ';
        for ( int c = 0; c < 16; c++ ) {
            if ( d[c] < 16 ) Serial << '0';
            Serial << _HEX( d[c] ) << ( c == 7 ? "  " : " " );
        }
        Serial << endl;
    }
}

