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

/// @file Layout.hpp Base class to lay out graphic elements (declarations).

#ifndef VDRAW_LAYOUT_H
#define VDRAW_LAYOUT_H

#include "GraphicsConstants.hpp"
#include "Frame.hpp"

namespace vdraw
{
  /// @ingroup BasicVectorGraphics  
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
