#pragma ident "$Id$"

/// @file BorderLayout.cpp Class to create frame within a frame, with a border or margin. (definition)

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include "BorderLayout.hpp"

namespace vdraw
{
   BorderLayout::BorderLayout(const Frame& frame, double iMarginSize) : 
     Layout(frame), targetList(1)
   {
      Frame parent=getParentFrame();
      targetList[0].setHeight(parent.getHeight()-2*iMarginSize);
      targetList[0].setWidth(parent.getWidth()-2*iMarginSize);
      targetList[0].nest(parent, iMarginSize, iMarginSize);
   }

   BorderLayout::BorderLayout(const Frame& frame, 
                              double xMarginSize, double yMarginSize ) : 
     Layout(frame), targetList(1)
   {
      Frame parent=getParentFrame();
      targetList[0].setHeight(parent.getHeight()-2*yMarginSize);
      targetList[0].setWidth(parent.getWidth()-2*xMarginSize);
      targetList[0].nest(parent, xMarginSize, yMarginSize);
   }

   BorderLayout::BorderLayout(const Frame& frame, 
                              double leftMargin, double topMargin,
                              double rightMargin, double bottomMargin) : 
     Layout(frame), targetList(1)
   {
      Frame parent=getParentFrame();
      targetList[0].setHeight(parent.getHeight()-topMargin-bottomMargin);
      targetList[0].setWidth(parent.getWidth()-leftMargin-rightMargin);
      targetList[0].nest(parent, leftMargin, bottomMargin);
   }
}
