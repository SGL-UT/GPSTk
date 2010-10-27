#pragma ident "$Id$"

///@file Canvas.cpp Common interface to all canvases that can be draw in. 
///  Class definitions.

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

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include "Canvas.hpp"

namespace vdraw
{

  /**
   * Constructors/Destructors
   */
  Canvas::Canvas()
  {
    defaults = new VGState();
  }

  Canvas::~Canvas()
  {
    while(stateStack.size() > 0)
    {
      delete defaults;
      defaults = stateStack.back();
      stateStack.pop_back();
    }
  }

} // namespace vdraw

