#pragma ident "$Id$"

/// @file BorderLayout.hpp Class to create frame within a frame, with a border or margin. (declarations).

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


#ifndef VDRAW_BORDERLAYOUT_H
#define VDRAW_BORDERLAYOUT_H

#include <vector>

#include "GraphicsConstants.hpp"
#include "Frame.hpp"
#include "Layout.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * Assists in arranging a new frame within a given one, with a margin.
   */
  class BorderLayout : public Layout
  {

    public:

      /**
       * Constructor. 
       * @param frame Outer frame.
       * @param marginSize Length in points between inner and outer frames
       */   
      BorderLayout(const Frame& frame, double marginSize );

      /**
       * Constructor. 
       * @param frame Outer frame.
       * @param xMarginSize Border size between left and right parts of inner and outer frames
       * @param yMarginSize Border size between upper and lower parts of inner and outer frames
       */   
      BorderLayout(const Frame& frame, 
          double xMarginSize, double yMarginSize );

      /**
       * Constructor. 
       * @param frame Outer frame.
       * @param leftMargin Border size between left  parts of inner and outer frames
       * @param topMargin Border size between upper parts of inner and outer frames
       * @param rightMargin Border size between right parts of inner and outer frames
       * @param bottomMargin Border size between bottom parts of inner and outer frames
       */   
      BorderLayout(const Frame& frame, 
          double leftMargin, double topMargin,
          double rightMargin, double bottomMargin);

      virtual int getFrameCount(void)
      {return (1);}

      virtual Frame getFrame(int fnum)
      {return targetList[0];}

    protected:

      /// List containing the frames created in this layout.
      std::vector<Frame> targetList;
  };

  //@}

} // namespace vdraw

#endif
