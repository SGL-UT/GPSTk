#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/Frame.hpp#1 $"

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


#ifndef VDRAW_FRAME_H
#define VDRAW_FRAME_H

#include "GraphicsConstants.hpp"
#include "VGImage.hpp"
#include "Canvas.hpp"

namespace vdraw
{
  /** \addtogroup BasicVectorGraphics */ 
  //@{

  /**
   * This class defines a frame that can be drawn in.  A frame is positioned
   * somewhere on a VGImage, the frame automatically does translation of
   * things drawn in it to page space.  (Currently does not scale)
   */
  class Frame : public Canvas
  {
    public:

      /// Default constructor. Creates an unusable frame. Needed for containers.
      Frame(void)
        :Canvas(),display(0) 
      {valid=false;};

      /// Constructor.  Create a top level frame from a VGImage.
      Frame(VGImage& target);
      //Frame(const Frame& rhs);
      
      ~Frame() {}

      /**
       * @param newWidth new width for the frame.
       */
      inline void setWidth(double newWidth) {width=newWidth;}

      /**
       * @param newHeight new height for the frame.
       */
      inline void setHeight(double newHeight) {height=newHeight;}

      /**
       * @return Width of the frame.
       */
      inline double getWidth(void) const {return width;}

      /**
       * @return Height of the frame.
       */
      inline double getHeight(void) const {return height;}

      /*
       * From canvas
       */
      void comment(const Comment& comment);

      using Canvas::comment;
      
      void line (const Line& line);

      using Canvas::line;

      void rectangle (const Rectangle& rect);

      void circle (const Circle& circle);

      void text (const Text& text);

      void polygon(const Polygon& polygon);

      void bitmap(const Bitmap& bitmap);      

      /**
       * Nests this frame within the parent by the given amounts.
       * @param parent Frame in which to nest this frame.
       * @param xoffset X offset to decrease on either side of the frame.
       * @param yoffset Y offset to decrease on top and bottom of the frame.
       */
      void nest(const Frame& parent, double xoffset, double yoffset);

      /// Returns x coordinate of center of frame in frame coordinates.
      inline double cx(void) const { return width/2;}
      /// Returns y coordinate of center of frame in frame coordinates.
      inline double cy(void) const { return height/2;}
      /// Returns x coordinate of the left edge of the frame in frame coordinates.
      inline double lx(void) const { return 0;}
      /// Returns x coordinate of the right edge of the frame in frame coordinates.
      inline double ux(void) const { return width;}
      /// Returns y coordinate of the upper edge of the frame in frame coordinates.
      inline double uy(void) const { return height;}
      /// Returns y coordinate of the lower edge of the frame in frame coordinates.
      inline double ly(void) const { return 0;}

      inline bool isLL() { return display->isLL(); }

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

      /// If m has no Marker and the default is set, set m's Marker to default.
      void fixMarker(Markable* m);
      /// If s has no StrokeStyle and the default is set, set s's StrokeStyle to default.
      void fixStrokeStyle(BasicShape* s);
      /// If f has no fill Color and the default is set, set f's fill Color to default.
      void fixFillColor(Fillable* f);
      /// If t has no TextStyle and the default is set, set t's TextStyle to default.
      void fixTextStyle(Text* t);

  };

  //@}

} // namespace vdraw

#endif
