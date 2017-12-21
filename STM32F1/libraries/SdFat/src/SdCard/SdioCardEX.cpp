/* Arduino SdFat Library
 * Copyright (C) 2016 by William Greiman
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
#include "SdioCard.h"

// limit of K66 due to errata KINETIS_K_0N65N.
const uint32_t MAX_SDHC_COUNT = 0XFFFF;

// Max RU is 1024 blocks.
const uint32_t RU_MASK = 0X03FF;

bool SdioCardEX::readBlock(uint32_t lba, uint8_t* dst) {
  if (m_curState != READ_STATE || lba != m_curLba) {
    if (!syncBlocks()) {
      return false;
    }
    m_limitLba = (lba + MAX_SDHC_COUNT) & ~RU_MASK;
    if (!SdioCard::readStart(lba, m_limitLba - lba)) {
      return false;
    }
    m_curLba = lba;
    m_curState = READ_STATE;
  }
  if (!SdioCard::readData(dst)) {
    return false;
  }
  m_curLba++;
  if (m_curLba >= m_limitLba) {
    m_curState = IDLE_STATE;
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCardEX::readBlocks(uint32_t lba, uint8_t* dst, size_t nb) {
  for (size_t i = 0; i < nb; i++) {
    if (!readBlock(lba + i, dst + i*512UL)) {
      return false;
    }
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCardEX::syncBlocks() {
  if (m_curState == READ_STATE) {
    m_curState = IDLE_STATE;
    if (!SdioCard::readStop()) {
      return false;
    }
  } else if (m_curState == WRITE_STATE) {
    m_curState = IDLE_STATE;
    if (!SdioCard::writeStop()) {
      return false;
    }
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCardEX::writeBlock(uint32_t lba, const uint8_t* src) {
  if (m_curState != WRITE_STATE || m_curLba != lba) {
    if (!syncBlocks()) {
      return false;
    }
    m_limitLba = (lba + MAX_SDHC_COUNT) & ~RU_MASK;
    if (!SdioCard::writeStart(lba , m_limitLba - lba)) {
      return false;
    }
    m_curLba = lba;
    m_curState = WRITE_STATE;
  }
  if (!SdioCard::writeData(src)) {
    return false;
  }
  m_curLba++;
  if (m_curLba >= m_limitLba) {
    m_curState = IDLE_STATE;
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdioCardEX::writeBlocks(uint32_t lba, const uint8_t* src, size_t nb) {
  for (size_t i = 0; i < nb; i++) {
    if (!writeBlock(lba + i, src + i*512UL)) {
      return false;
    }
  }
  return true;
}
