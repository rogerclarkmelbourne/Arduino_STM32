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
#ifndef SdFatTestSuite_h
#define SdFatTestSuite_h
#include <SdFat.h>
#include <SdFatUtil.h>

#if defined(__arm__) && !defined(strcmp_P)
#define strcmp_P(a, b) strcmp((a), (b))
#endif  // strcmp_P

#if defined(__arm__) && !defined(strncpy_P)
#define strncpy_P(s, t, n) strncpy(s, t, n)
#endif  // strncpy_P

#if defined(__arm__) && !defined(strlen_P)
#define strlen_P(str) strlen(str)
#endif  // strlen_P

#define testVerifyBool(result) testVerify_P(result, PSTR(#result))
#define testVerifyMsg(result, msg) testVerify_P(result, PSTR(msg))
#define testVerifyStr(result, expect) testVerify_P(result, PSTR(expect))

void testBegin();
void testEnd();
void testVerify_P(bool b, PGM_P msg);
void testVerify_P(char* result, PGM_P expect);
#endif  // SdFatTestSuite_h
