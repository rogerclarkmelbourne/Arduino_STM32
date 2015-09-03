// Test the offloaded RAM stash mechanism.
// 2011-07-10 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <EtherCard_STM.h>
#include <SPI.h>

// ethernet interface mac address, must be unique on the LAN
byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[700];

void dumpBlock (const char* msg, byte idx) {
  Serial.print(msg);
  Serial.print(" [");
  Serial.print(idx, DEC);
  Serial.print("] @ ");
  Serial.print(Stash::bufs[idx].bnum, DEC);
  Serial.print(" free ");
  Serial.print(Stash::freeCount(), DEC);
  for (byte i = 0; i < 64; ++i) {
    if (i % 16 == 0)
      Serial.println();
    Serial.print(' ');
    Serial.print(Stash::bufs[idx].bytes[i], DEC);
  }
  Serial.println();
}

void dumpStash (const char* msg, void* ptr) {
  Stash& buf = *(Stash*) ptr;
  Serial.print(msg);
  Serial.print(" c");
  Serial.print(buf.count, DEC);
  Serial.print(" f");
  Serial.print(buf.first, DEC);
  Serial.print(" l");
  Serial.print(buf.last, DEC);
  Serial.print(" u");
  Serial.print(buf.curr, DEC);
  Serial.print(" o");
  Serial.print(buf.offs, DEC);
  Serial.print(" #");
  Serial.println(buf.size());
}

void setup () {
  Serial.begin(57600);
  delay(1);
  Serial.println("\n[stashTest]");
  ether.begin(sizeof Ethernet::buffer, mymac);
  
#if 1  
  Stash buf;
  dumpStash("> AAA", &buf);
  byte fd = buf.create();
  Serial.print("fd ");
  Serial.println(fd, DEC);
  dumpStash("> BBB", &buf);
  dumpBlock("EMPTY", 0);
  for (char c = 'a'; c <= 'z'; ++c)
    buf.put(c);
  for (char c = 'A'; c <= 'Z'; ++c)
    buf.put(c);
  dumpBlock("LETTERS", 0);
  dumpStash("> CCC", &buf);
  for (char c = '0'; c <= '9'; ++c)
    buf.put(c);
  dumpBlock("DIGITS", 0);
  dumpStash("> DDD", &buf);
  buf.print(" x = ");
  buf.println(123.45);
  dumpBlock("PRINT", 0);
  dumpStash("> EEE", &buf);
  buf.load(1, 1);
  dumpBlock("FIRST", 1);
  buf.save();
  buf.load(1, 1);
  dumpBlock("FLUSHED", 1);
  dumpStash("> FFF", &buf);
  for (;;) {
    char c = buf.get();
    if (c == 0)
      break;
    Serial.print(c);
  }
  Serial.println();
  dumpStash("> GGG", &buf);
#endif
  
  Stash buf2;
  byte fd2 = buf2.create();
  buf2.print("<XYZ>");
  buf2.save();
  buf2.load(1, fd2);
  dumpBlock("SECOND", 1);
  dumpStash("> HHH", &buf2);
  
  Stash::prepare(PSTR("a $S b $F c $D d $H e"),
                  "123", PSTR("4567"), -12345, fd2);
  dumpBlock("BUFFER", 0);
  Serial.println(Stash::length());
  for (word i = 0, n = Stash::length(); i < n; ++i) {
    char c;
    Stash::extract(i, 1, &c);
    Serial.print(c);
  }
  Serial.println();
  Stash::cleanup();
#if 1
  buf.release();
#endif
  Serial.print("free ");
  Serial.println(Stash::freeCount(), DEC);
  Serial.println("DONE");
}

void loop () {
  // ether.packetLoop(ether.packetReceive());
}
