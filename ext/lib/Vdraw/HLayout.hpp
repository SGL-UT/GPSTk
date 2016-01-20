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

/// @file HLayout.hpp Class to lay out graphic elements horizontally (declarations).

#ifndef VDRAW_HLAYOUT_H
#define VDRAW_HLAYOUT_H

#include <vector>

#include "GraphicsConstants.hpp"
#include "Frame.hpp"
#include "Layout.hpp"

namespace vdraw
{
      /// @ingroup BasicVectorGraphics
      //@{

      /**
       * Assists in arranging frames in a horizontal stack.
       */
   class HLayout : public Layout
   {

   public:

         /**
          * Constructor. 
          * @param frame Frame to create layout within
          * @param nframes Number of evenly stacked horizontal frames to make
          */   
      HLayout(const Frame& frame, int nframes);

         /**
          * Constructor. 
          * @param frame Frame to split into two
          * @param fraction Fraction (>0, <1) of the horizontal space
          *   for the first frame.
          */
      HLayout(const Frame& frame, double fraction);

      virtual int getFrameCount(void)
      {return targetList.size();}

      virtual Frame getFrame(int fnum)
      {return targetList[fnum];}

   protected:

         /// List containing the frames created in this layout.      
      std::vector<Frame> targetList;
   };

      //@}

} // namespace vdraw

#endif
