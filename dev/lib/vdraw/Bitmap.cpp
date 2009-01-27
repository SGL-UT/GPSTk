#pragma ident "$Id$"

/// @file Bitmap.cpp Defines a simple bitmap. Class definitions.

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

