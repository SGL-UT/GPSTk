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

/// @file ColorMap.cpp Defines a color map. Class defintions.

#include "ColorMap.hpp"

namespace vdraw
{
  ColorMap::ColorMap(const ColorMap &o)
  {
    init(o.getCols(),o.getRows());        

    for(int row=0;row<rows;row++)
      for(int col=0;col<cols;col++)
        c[row][col] = o.get(row,col);
  }

  ColorMap::ColorMap(const InterpolatedColorMap &o)
  {
    init(o.getCols(),o.getRows());        

    for(int row=0;row<rows;row++)
      for(int col=0;col<cols;col++)
        c[row][col] = o.get(row,col);
  }

  //ColorMap::ColorMap(const IndexedColorMap &o)
  //{
  //  init(o.getCols(),o.getRows());        
  //
  //  for(int row=0;row<rows;row++)
  //    for(int col=0;col<cols;col++)
  //      c[row][col] = o.get(row,col);
  //}

  ColorMap::ColorMap(int icols, int irows, const Color &base)
  {
    init(icols,irows);        

    for(int row=0;row<rows;row++)
      for(int col=0;col<cols;col++)
        c[row][col] = base;
  }

  ColorMap& ColorMap::operator=(ColorMap o)
  {
    // o is a copy, swap the variables
    std::swap(rows,o.rows);
    std::swap(cols,o.cols);
    std::swap(c,o.c);
    // o is destructed with the old data from this
    return *this;
  }
  
  void ColorMap::init(int icols, int irows)
  {
    if(icols == 0 || irows == 0)
    {
      cols = rows = 0;
      c = 0;
      return;
    }

    cols = icols;
    rows = irows;

    // Initialize the color array
    c = new Color*[rows];

    for(int row=0;row<rows;row++)
      c[row] = new Color[cols];
  }

  void ColorMap::reset()
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
