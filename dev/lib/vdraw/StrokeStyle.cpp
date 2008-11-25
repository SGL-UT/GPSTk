#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/StrokeStyle.cpp#1 $"

/// @file StrokeStyle.cpp Defines line appearance: width, etc. Class definitions.

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

#include "StrokeStyle.hpp"

namespace vdraw
{

   StrokeStyle::StrokeStyle(void):
     color(Color::BLACK), width(1), solid(true)
   {
   }

   StrokeStyle::StrokeStyle( const Color& icolor,  
			     double iwidth) :
     color(icolor), width(iwidth), solid(true)
   {
   }

   StrokeStyle::StrokeStyle( const Color& icolor,
			     double iwidth,
			     const dashLengthList& dll) :
     color(icolor), width(iwidth)
   {
     solid = true;
     if(dll.size() != 0)
     {
       solid = false;
       dashList = dll;
     }
   }

} // namespace vdraw

