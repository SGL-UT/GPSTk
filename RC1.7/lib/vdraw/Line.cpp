#pragma ident "$Id$"

/// @file Line.cpp Defines a simple line. Class definitions.

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

#include <iomanip>

#include "Line.hpp"

namespace vdraw
{
   Line::Line(double ix1, double iy1, double ix2, double iy2)
         : Path(ix1, iy1), Markable()
   {
      addPointAbsolute(ix1,iy1);
      addPointAbsolute(ix2,iy2);
   }

   Line::Line(double ix1, double iy1, double ix2, double iy2,
              const StrokeStyle& istyle)
         : Path(ix1, iy1), Markable(istyle)
   {
      addPointAbsolute(ix1,iy1);
      addPointAbsolute(ix2,iy2);
   }


} // namespace vdraw

