#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//============================================================================

#ifndef GPSTK_SCREEN_CONTROL_HPP
#define GPSTK_SCREEN_CONTROL_HPP

/**
 * @file ScreenControl.hpp
 * Clearing, updating and printing to a display.
 */

#include "DayTime.hpp"

namespace gpstk 
{
   void clearScreen(void);
   void printTitle(void);
} // end namespace gpstk

#endif //GPSTK_SCREEN_CONTROL_HPP
