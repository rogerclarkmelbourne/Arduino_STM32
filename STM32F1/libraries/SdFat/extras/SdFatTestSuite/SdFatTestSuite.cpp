/* Arduino SdFat Library
 * Copyright (C) 2011 by William Greiman
 *
 * This file is part of the Arduino SdFat Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Arduino SdFat Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <SdFatTestSuite.h>
static uint16_t failCount;
static uint16_t testCount;
static Print* testOut = &Serial;
//------------------------------------------------------------------------------
static size_t strlenPGM(PGM_P str) {
  PGM_P end = str;
  while (pgm_read_byte(end++)) {}
  return end - str;
}
//------------------------------------------------------------------------------
void testBegin() {
  Serial.begin(9600);
  while (!Serial) {}  // wait for leonardo
  testOut = &Serial;
  Serial.println(F("Type any character to begin."));
  while (Serial.read() <= 0) {}
  delay(200); // Catch Due reset problem

  testOut->print(F("FreeRam: "));
  testOut->println(FreeRam());
  testOut->println();
  failCount = 0;
  testCount = 0;
}
//------------------------------------------------------------------------------
void testEnd() {
  testOut->println();
  testOut->println(F("Compiled: " __DATE__ " " __TIME__));
  testOut->print(F("FreeRam: "));
  testOut->println(FreeRam());
  testOut->print(F("Test count: "));
  testOut->println(testCount);
  testOut->print(F("Fail count: "));
  testOut->println(failCount);
}
//------------------------------------------------------------------------------
static void testResult(bool b, uint8_t n) {
 while (n++ < 60) testOut->write(' ');
  if (b) {
    testOut->println(F("..ok"));
  } else {
    testOut->println(F("FAIL"));
    failCount++;
  }
  testCount++;
}
//------------------------------------------------------------------------------
void testVerify_P(char* result, PGM_P expect) {
  testOut->write('"');
  testOut->print(result);
  testOut->print("\",\"");
  testOut->print((const __FlashStringHelper*)expect);
  testOut->write('"');
  uint8_t n = strlen(result) + strlenPGM(expect) + 5;
  testResult(!strcmp_P(result, expect), n);
}
//------------------------------------------------------------------------------
void testVerify_P(bool b, PGM_P msg) {
  testOut->print((const __FlashStringHelper*)msg);
  uint8_t n = strlenPGM(msg);
  testResult(b, n);
}
