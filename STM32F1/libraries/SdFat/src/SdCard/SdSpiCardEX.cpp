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
#include "SdSpiCard.h"
bool SdSpiCardEX::readBlock(uint32_t block, uint8_t* dst) {
  if (m_curState != READ_STATE || block != m_curBlock) {
    if (!syncBlocks()) {
      return false;
    }
    if (!SdSpiCard::readStart(block)) {
      return false;
    }
    m_curBlock = block;
    m_curState = READ_STATE;
  }
  if (!SdSpiCard::readData(dst)) {
    return false;
  }
  m_curBlock++;
  return true;
}
//-----------------------------------------------------------------------------
bool SdSpiCardEX::readBlocks(uint32_t block, uint8_t* dst, size_t nb) {
  for (size_t i = 0; i < nb; i++) {
    if (!readBlock(block + i, dst + i*512UL)) {
      return false;
    }
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdSpiCardEX::syncBlocks() {
  if (m_curState == READ_STATE) {
    m_curState = IDLE_STATE;
    if (!SdSpiCard::readStop()) {
      return false;
    }
  } else if (m_curState == WRITE_STATE) {
    m_curState = IDLE_STATE;
    if (!SdSpiCard::writeStop()) {
      return false;
    }
  }
  return true;
}
//-----------------------------------------------------------------------------
bool SdSpiCardEX::writeBlock(uint32_t block, const uint8_t* src) {
  if (m_curState != WRITE_STATE || m_curBlock != block) {
    if (!syncBlocks()) {
      return false;
    }
    if (!SdSpiCard::writeStart(block)) {
      return false;
    }
    m_curBlock = block;
    m_curState = WRITE_STATE;
  }
  if (!SdSpiCard::writeData(src)) {
    return false;
  }
  m_curBlock++;
  return true;
}
//-----------------------------------------------------------------------------
bool SdSpiCardEX::writeBlocks(uint32_t block,
                                  const uint8_t* src, size_t nb) {
  for (size_t i = 0; i < nb; i++) {
    if (!writeBlock(block + i, src + i*512UL)) {
      return false;
    }
  }
  return true;
}
