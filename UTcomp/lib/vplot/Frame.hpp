
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

  /**
   * This class defines a frame that can be drawn in.  A frame is positioned
   * somewhere on a VGImage, the frame automatically does translation of
   * things drawn in it to page space.  (Currently does not scale)
   */
  class Frame
  {

    public:

      friend const Frame& operator<<(const Frame& f, const Text& text);
      friend const Frame& operator<<(const Frame& f, const Rectangle& rect);

      /// Default constructor. Creates an unusable frame. Needed for containers.
      Frame(void):display(0) 
      {valid=false;};

      /// Constructor.  Create a top level frame from a VGImage.
      Frame(VGImage& target);
      //Frame(const Frame& rhs);

      /**
       * @param newWidth new width for the frame.
       */
      void setWidth(double newWidth) {width=newWidth;}

      /**
       * @param newHeight new height for the frame.
       */
      void setHeight(double newHeight) {height=newHeight;}

      /**
       * @return Width of the frame.
       */
      double getWidth(void) const {return width;}

      /**
       * @return Height of the frame.
       */
      double getHeight(void) const {return height;}

      /**
       * Nests this frame within the parent by the given amounts.
       * @param parent Frame in which to nest this frame.
       * @param xoffset X offset to decrease on either side of the frame.
       * @param yoffset Y offset to decrease on top and bottom of the frame.
       */
      void nest(const Frame& parent, double xoffset, double yoffset);

      /// Returns x coordinate of center of frame in frame coordinates.
      double cx(void) const { return width/2;}
      /// Returns y coordinate of center of frame in frame coordinates.
      double cy(void) const { return height/2;}
      /// Returns x coordinate of the left edge of the frame in frame coordinates.
      double lx(void) const { return 0;}
      /// Returns x coordinate of the right edge of the frame in frame coordinates.
      double ux(void) const { return width;}
      /// Returns y coordinate of the upper edge of the frame in frame coordinates.
      double uy(void) const { return height;}
      /// Returns y coordinate of the lower edge of the frame in frame coordinates.
      double ly(void) const { return 0;}

    protected:
      /// If this frame's parent is the VGImage, not another frame or a layout.
      bool isTopLevel;
      /// X value of the origin of this frame.
      double originX;
      /// Y value of the origin of this frame.      
      double originY;
      /// Width of this frame
      double width; 
      /// Height of this frame
      double height;
      /// If this frame is valid and can be drawn in.
      bool valid;
      /// The VGImage in which to draw.
      VGImage *display;
  };

  //@}

} // namespace vplot

#endif
