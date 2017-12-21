/* FatLib Library
 * Copyright (C) 2016 by William Greiman
 *
 * This file is part of the FatLib Library
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
 * along with the FatLib Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef BaseBlockDriver_h
#define BaseBlockDriver_h
#include "FatLibConfig.h"
/**
 * \class BaseBlockDriver
 * \brief Base block driver.
 */
class BaseBlockDriver {
 public:
    /**
   * Read a 512 byte block from an SD card.
   *
   * \param[in] block Logical block to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */  
  virtual bool readBlock(uint32_t block, uint8_t* dst) = 0;
  /** End multi-block transfer and go to idle state.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */     
  virtual bool syncBlocks() = 0;
  /**
   * Writes a 512 byte block to an SD card.
   *
   * \param[in] block Logical block to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */    
  virtual bool writeBlock(uint32_t block, const uint8_t* src) = 0;
#if USE_MULTI_BLOCK_IO
  /**
   * Read multiple 512 byte blocks from an SD card.
   *
   * \param[in] block Logical block to be read.
   * \param[in] nb Number of blocks to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */  
  virtual bool readBlocks(uint32_t block, uint8_t* dst, size_t nb) = 0;
  /**
   * Write multiple 512 byte blocks to an SD card.
   *
   * \param[in] block Logical block to be written.
   * \param[in] nb Number of blocks to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return The value true is returned for success and
   * the value false is returned for failure.
   */    
  virtual bool writeBlocks(uint32_t block, const uint8_t* src, size_t nb) = 0;
#endif  // USE_MULTI_BLOCK_IO
};
#endif  // BaseBlockDriver_h
