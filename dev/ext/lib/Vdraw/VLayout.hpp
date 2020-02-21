#pragma ident "$Id$"

/// @file VLayout.hpp Class to lay out graphic elements vertically (declarations).

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


#ifndef VDRAW_VLAYOUT_H
#define VDRAW_VLAYOUT_H

#include <vector>

#include "GraphicsConstants.hpp"
#include "Frame.hpp"
#include "Layout.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * Assists in arranging frames in a vertical stack.
   */
  class VLayout : public Layout
  {

    public:

      /**
       * Constructor. 
       * @param frame Frame to create layout within
       * @param nframes Number of evenly stacked vertical frames to make
       */   
      VLayout(const Frame& frame, int nframes);

      /**
       * Constructor. 
       * @param frame Frame to split into two
       * @param fraction Fraction (>0, <1) of the vertical space for the first frame.
       */
      VLayout(const Frame& frame, double fraction);

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
