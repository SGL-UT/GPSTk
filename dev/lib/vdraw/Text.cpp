#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Text.cpp#1 $"

/// @file Text.cpp Class to represent text. Class definitions.

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

#include "Text.hpp"

namespace vdraw
{
   /**
    * Constructor
    */
   Text::Text(const char* str, double ix, double iy, ALIGNMENT align, int angle)
             :hasOwnStyle(false)
   { 
      textString = std::string(str);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(const char* str, double ix, double iy, const TextStyle& its, ALIGNMENT align, int angle)
             :hasOwnStyle(true), textStyle(its)
   { 
      textString = std::string(str);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(int num, double ix, double iy, ALIGNMENT align, int angle)
             :hasOwnStyle(false)
   { 
      char buffer [33];
      sprintf(buffer,"%d",num);
      textString = std::string(buffer);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }

   Text::Text(int num, double ix, double iy, const TextStyle& its, ALIGNMENT align, int angle)
             :hasOwnStyle(true), textStyle(its)
   { 
      char buffer [33];
      sprintf(buffer,"%d",num);
      textString = std::string(buffer);
      x = ix;
      y = iy;
      textAlign = align;
      textAngle = angle;
   }
   
} // namespace vdraw
