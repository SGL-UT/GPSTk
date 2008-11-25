#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Rectangle.cpp#1 $"

/// @file Rectangle.cpp Defines a simple rectangle. Class definitions.

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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "Rectangle.hpp"

namespace vdraw
{
   void Rectangle::fixPoints()
   {
      double temp;
      if( x1 > x2 )
      {
         temp = x1;
         x1 = x2;
         x2 = temp;
      }
      if( y1 > y2 )
      {
         temp = y1;
         y1 = y2;
         y2 = temp;
      }
   }

} // namespace vdraw

