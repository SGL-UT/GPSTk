#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Layout.hpp#1 $"

/// @file Layout.hpp Base class to lay out graphic elements (declarations).

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


#ifndef VDRAW_LAYOUT_H
#define VDRAW_LAYOUT_H

#include "GraphicsConstants.hpp"
#include "Frame.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{


  /**
   * This class is used to create new frames from an existing frame.  
   * It is overloaded to partition the space in a frame in several 
   * different ways.
   */
  class Layout
  {

    public:

      /**
       * Constructor. 
       * @param frame Frame to create layout within
       */   
      Layout(const Frame& frame) : target(frame)
      {}

      /**
       * @return Number of frames contained in this layout.
       */
      virtual int getFrameCount(void)
      {return 1;}

      /**
       * @param fnum Frame index number
       * @return The fnum-th frame
       */
      virtual Frame getFrame(int fnum)
      {return target;}


      /**
       * @return The frame from which this layout was created.
       */
      virtual Frame getParentFrame(void)
      {return target;}

    protected:

      /// The frame from which nre frames are created in this layout.
      Frame target;
  };

  //@}

} // namespace vdraw

#endif
