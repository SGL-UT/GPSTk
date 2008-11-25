#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/GridLayout.cpp#1 $"

/// @file GridLayout.cpp Class to lay out graphic elements vertially (definitions).

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
