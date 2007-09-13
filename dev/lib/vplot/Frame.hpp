#pragma ident "$Id:$"

/// @file Frame.hpp Class to represent a region to draw in. Class declarations.

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


#ifndef VPLOT_FRAME_H
#define VPLOT_FRAME_H

#include "GraphicsConstants.hpp"
#include "VGImage.hpp"

namespace vplot
{
   /** \addtogroup BasicVectorGraphics */ 
   //@{

   class Frame
   {

    public:

      friend Frame& operator<<(Frame& f, const Text& text);
      friend Frame& operator<<(Frame& f, const Rectangle& rect);

         /// Default constructor. Creates an unusable frame. Needed for containers.
      Frame(void):display(0) 
         {valid=false;};

      Frame(VGImage& target);
      Frame(const Frame& rhs);

         /// Returns x coordinate of center of frame in frame coordinates.
      double cx(void) { return width/2;}
         /// Returns y coordinate of center of frame in frame coordinates.
      double cy(void) { return height/2;}
         /// Returns x coordinate of the left edge of the frame in frame coordinates.
      double lx(void){ return 0;}
         /// Returns x coordinate of the right edge of the frame in frame coordinates.
      double ux(void) { return width;}
         /// Returns y coordinate of the upper edge of the frame in frame coordinates.
      double uy(void) { return height;}
         /// Returns y coordinate of the lower edge of the frame in frame coordinates.
      double ly(void) { return 0;}

   protected:
      bool isTopLevel;
      double originX, originY, width, height;
      bool valid;
      VGImage *display;
   };

  //@}
   
} // namespace vplot

#endif
