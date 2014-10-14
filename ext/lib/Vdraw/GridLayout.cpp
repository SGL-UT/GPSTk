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

/// @file GridLayout.cpp Class to lay out graphic elements vertially (definitions).

#include <iostream>
#include "GridLayout.hpp"

namespace vdraw
{
   GridLayout::GridLayout(const Frame& frame, int rows, int cols) : 
         Layout(frame), targetList(rows*cols), nrows(rows), ncols(cols)
   {
      Frame parent=getParentFrame();

      double ylow=parent.ly();
      double yhigh=parent.uy();
      double height = parent.getHeight()/nrows;

      double xlow=parent.lx();
      double xhigh=parent.ux();
      double width = parent.getWidth()/ncols;

      for (int r=0;r<nrows;r++)
         for (int c=0; c<ncols; c++)
         {
            double yloc = ylow + r*(yhigh-ylow)/nrows;
            double xloc = xlow + c*(xhigh-xlow)/ncols;

               // The list is linear; It's row major
            int i=c+r*ncols; 

            targetList[i].setWidth(width);
            targetList[i].setHeight(height);

	    targetList[i].nest(parent, xloc, yloc);
         }

   }
}
