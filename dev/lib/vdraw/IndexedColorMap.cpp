#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/IndexedColorMap.cpp#1 $"

/// @file IndexedColorMap.cpp Defines an indexed color map. Class defintions.

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

#include "IndexedColorMap.hpp"

namespace vdraw
{
  IndexedColorMap::IndexedColorMap(int icols, int irows, const Palette &pp, int base)
  {
    cols = icols;
    rows = irows;
    p = pp;

    // Initialize the color array
    c = new int*[rows];

    for(int row=0;row<rows;row++)
      c[row] = new int[cols];

    for(int row=0;row<rows;row++)
      for(int col=0;col<cols;col++)
        c[row][col] = base;
  }

  IndexedColorMap::reset()
  {
    if(c)
    {
      for(int row=0;row<rows;row++)
        delete[] c[row];
      delete[] c;
    }

    cols=rows=0;
    c=0;
  }
}
