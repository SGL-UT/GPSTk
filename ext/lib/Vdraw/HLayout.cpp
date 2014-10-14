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

/// @file HLayout.cpp Class to lay out graphic elements horizontally (definitions).

#include <iostream>
#include "HLayout.hpp"

namespace vdraw
{
   HLayout::HLayout(const Frame& frame, int nframes) : 
     Layout(frame), targetList(nframes)
   {
      Frame parent=getParentFrame();
      double xlow=parent.lx();
      double xhigh=parent.ux();
      double width = parent.getWidth()/nframes;
      for (int i=0;i<nframes;i++)
      {
         double xloc = xlow + i*(xhigh-xlow)/nframes;
         targetList[i].setHeight(parent.getHeight());
	 targetList[i].setWidth(width);
	 targetList[i].nest(parent, xloc, 0);
      }
   }

   HLayout::HLayout(const Frame& frame, double fraction)
         : Layout(frame), targetList(2)
   {
      Frame parent=getParentFrame();

      double xlow=parent.lx();

      targetList[0].setHeight(parent.getHeight());
      targetList[0].setWidth(parent.getWidth()*fraction);
      targetList[0].nest(parent, xlow, 0);
      targetList[1].setHeight(parent.getHeight());
      targetList[1].setWidth(parent.getWidth()*(1.-fraction));
      targetList[1].nest(parent, xlow+parent.getWidth()*fraction,0);
   }

}
