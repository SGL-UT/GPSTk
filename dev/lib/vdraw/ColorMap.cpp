#pragma ident "$Id$"

/// @file ColorMap.cpp Defines a color map. Class defintions.

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
