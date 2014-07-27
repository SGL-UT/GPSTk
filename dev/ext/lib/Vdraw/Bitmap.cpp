//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file Bitmap.cpp Defines a simple bitmap. Class definitions.

#include "Bitmap.hpp"

namespace vdraw
{
  Bitmap::Bitmap(double ix1, double iy1, double ix2, double iy2, 
      const ColorMap &c)
  { 
    x1=ix1; 
    y1=iy1; 
    x2=ix2; 
    y2=iy2; 
    osr = 1;
    osc = 1;
    boxes = false;
    fixPoints(); 
    cm=c; 
    which=CM; 
  }

  Bitmap::Bitmap(double ix1, double iy1, double ix2, double iy2, 
      const InterpolatedColorMap &ic)
  { 
    x1=ix1; 
    y1=iy1; 
    x2=ix2; 
    y2=iy2; 
    osr = 1;
    osc = 1;
    boxes = false;
    fixPoints(); 
    icm=ic; 
    which=ICM; 
  }

  Bitmap::Bitmap(const Bitmap& o)
  {
    x1 = o.x1; 
    y1 = o.y1;
    x2 = o.x2;
    y2 = o.y2;
    osr = o.osr;
    osc = o.osc;
    boxes = o.boxes;
    which = o.which;
    if(which==ICM)
      icm = o.icm;
    else if(which==CM)
      cm = o.cm;
  }

  void Bitmap::fixPoints()
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
