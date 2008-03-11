
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

namespace vplot 
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

  /// Outputs the specified line to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Line& line)
  {
    vgstrm.line(line);
    return vgstrm;
  }

  /// Outputs the specified polygon to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Polygon& polygon)
  {
    vgstrm.polygon(polygon);
    return vgstrm;
  }

  /// Outputs the specified rectangle to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Rectangle& rect)
  {
    vgstrm.rectangle(rect);
    return vgstrm;
  }

  /// Outputs the specified circle to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Circle& circle)
  {
    vgstrm.circle(circle);
    return vgstrm;
  }

  /// Outputs the specified text to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Text& text)
  {
    vgstrm.text(text);
    return vgstrm;
  }

  /// Changes the appearance of lines in the stream   
  VGImage& operator<<(VGImage& vgstrm, 
      const StrokeStyle& style)
  {
    vgstrm.setLineStyle(style);
    return vgstrm;
  }

  /// Changes the appearance of line points in the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Marker& marker)
  {
    vgstrm.setMarker(marker);
    return vgstrm;
  }

  /// Outputs a comment to the stream
  VGImage& operator<<(VGImage& vgstrm, 
      const Comment& comment)
  {
    vgstrm.comment(comment);
    return vgstrm;
  }


  /*
   * Constructors/Destructors
   */

  /*
   * Methods
   */

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
} // namespace vplot
