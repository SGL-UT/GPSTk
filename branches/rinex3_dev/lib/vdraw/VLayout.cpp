#pragma ident "$Id$"

/// @file VLayout.cpp Class to lay out graphic elements vertically (definitions).

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
#include "VLayout.hpp"

namespace vdraw
{
   VLayout::VLayout(const Frame& frame, int nframes) : 
     Layout(frame), targetList(nframes)
   {
      Frame parent=getParentFrame();
      double ylow=parent.ly();
      double yhigh=parent.uy();
      double height = parent.getHeight()/nframes;
      for (int i=0;i<nframes;i++)
      {
         double yloc = ylow + i*(yhigh-ylow)/nframes;
         targetList[i].setWidth(parent.getWidth());
	 targetList[i].setHeight(height);
	 targetList[i].nest(parent, 0, yloc);
      }
   }

   VLayout::VLayout(const Frame& frame, double fraction)
         : Layout(frame), targetList(2)
   {
      Frame parent=getParentFrame();

      double ylow=parent.ly();

      targetList[0].setHeight(parent.getHeight()*fraction);      
      targetList[0].setWidth(parent.getWidth());
      targetList[0].nest(parent, 0, ylow);
      targetList[1].setHeight(parent.getHeight()*(1.-fraction));      
      targetList[1].setWidth(parent.getWidth());
      targetList[1].nest(parent, 0, ylow+parent.getHeight()*fraction);

   }
   
}
