#pragma ident "$Id: //depot/msn/main/wonky/gpstkplot/lib/draw/VGImage.cpp#1 $"

///@file VGImage.cpp Common interface to all vector plot formats. Class definitions.

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


#include "VGImage.hpp"

namespace vdraw 
{
  /// Units conversion from points to inches
  const double VGImage::PTS_PER_INCH=72;
  /// Units conversion from points to centimeters
  const double VGImage::PTS_PER_CM=72/2.54;

  /// U.S. Customary page size width in points
  const double VGImage::US_LETTER_WIDTH_PTS=612;
  /// U.S. Customary page size height in points;
  const double VGImage::US_LETTER_HEIGHT_PTS=792;

  // For the rest of the world... there's ISO 216-series sizes
  /// A3 page size width in points
  const double VGImage::A3_WIDTH_PTS=841.9;
  /// A3 page size height in points
  const double VGImage::A3_HEIGHT_PTS=1190.6;

  // A4 and US customary letter are of comparable size
  /// A4 page size width in points
  const double VGImage::A4_WIDTH_PTS=595.3;
  /// A4 page size height in points 
  const double VGImage::A4_HEIGHT_PTS=841.9;

  /// A5 page size width in points
  const double VGImage::A5_WIDTH_PTS=419.5;
  /// A5 page size height in points
  const double VGImage::A5_HEIGHT_PTS=595.3;

  /*
   * Global operators
   */

  /*
   * Constructors/Destructors
   */

  /*
   * Methods
   */

  void VGImage::rectangles(const Bitmap &bitmap)
  {
    double bw=bitmap.x2-bitmap.x1;
    double bh=bitmap.y2-bitmap.y1;

    Bitmap::CMType which;
    int w, h;
    InterpolatedColorMap icm;
    ColorMap cm;

    if(bitmap.getICM(&icm))
    {
      which = Bitmap::ICM;
      w = icm.getCols();
      h = icm.getRows();
    }
    else if(bitmap.getCM(&cm))
    {
      which = Bitmap::CM;
      w = cm.getCols();
      h = cm.getRows();
    }

    double dx=bw/w;
    double dy=bh/h;

    StrokeStyle ssb(Color::CLEAR);

    // In the future, this could create bigger boxes if adjacent rectangles
    // would be the same color.

    for(int r=(ll?0:h-1); (ll?r<h:r>=0); r+=(ll?1:-1))
    {
      for(int c=0; c<w; c++)
      {
        double x1=bitmap.x1+c*dx;
        double y1=bitmap.y1+(ll?r:h-r-1)*dy;
        Rectangle rect(x1,y1,x1+dx,y1+dy,ssb); 
        if(which==Bitmap::CM)
          rect.setFillColor(cm.get(r,c)); 
        else
          rect.setFillColor(icm.get(r,c));             
        rectangle(rect); 
      }
    }
  }

  VGImage::StyleType VGImage::getCorrectMarker(Marker *m, const Markable& mark)
  {
    if(mark.hasOwnMarker())
    {
      *m = mark.getMarker();
      if(!m->getColor().isClear())
        return SHAPE;
      else 
        return CLEAR;
    }
    else if(defaults->useM)
    {
      *m = defaults->marker;
      if(!m->getColor().isClear())
        return DEFAULT;
      else
        return CLEAR;
    }
    return NONE;
  }

  VGImage::StyleType VGImage::getCorrectStrokeStyle(StrokeStyle *s, const BasicShape& shape)
  {
    if(shape.hasOwnStrokeStyle())
    {
      *s = shape.getStrokeStyle();
      if(!s->getColor().isClear())
        return SHAPE;
      else
        return CLEAR;
    }
    else if(defaults->useSS)
    {
      *s = defaults->strokeStyle;
      if(!s->getColor().isClear())
        return DEFAULT;
      else
        return CLEAR;
    }
    return NONE;
  }

  VGImage::StyleType VGImage::getCorrectFillColor(Color *c, const Fillable& shape)
  {
    if(shape.hasOwnFillColor())
    {
      *c = shape.getFillColor();
      if(!c->isClear())
        return SHAPE;
      else
        return CLEAR;
    }
    else if(defaults->useFC)
    {
      *c = defaults->fillColor;
      if(!c->isClear())
        return DEFAULT;
      else
        return CLEAR;
    }
    return NONE;
  }

  VGImage::StyleType VGImage::getCorrectTextStyle(TextStyle *s, const Text& text)
  {
    if(text.hasOwnTextStyle())
    {
      *s = text.getStyle();
      if(!s->getColor().isClear())
        return SHAPE;
      else
        return CLEAR;
    }
    else if(defaults->useTS)
    {
      *s = defaults->textStyle;
      if(!s->getColor().isClear())
        return DEFAULT;
      else
        return CLEAR;
    }
    return NONE;
  }
} // namespace vdraw
