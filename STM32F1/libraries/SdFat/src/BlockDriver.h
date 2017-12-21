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
 /**
 * \file
 * \brief Define block driver.
 */
#ifndef BlockDriver_h
#define BlockDriver_h
#include "FatLib/BaseBlockDriver.h"
#include "SdCard/SdSpiCard.h"
//-----------------------------------------------------------------------------
/** typedef for BlockDriver */
#if ENABLE_EXTENDED_TRANSFER_CLASS || ENABLE_SDIO_CLASS
typedef BaseBlockDriver BlockDriver;
#else  // ENABLE_EXTENDED_TRANSFER_CLASS || ENABLE_SDIO_CLASS
typedef SdSpiCard BlockDriver;
#endif  // ENABLE_EXTENDED_TRANSFER_CLASS || ENABLE_SDIO_CLASS
#endif  // BlockDriver_h
