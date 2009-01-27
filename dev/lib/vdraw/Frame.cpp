#pragma ident "$Id$"

/// @file Frame.cpp Class to represent a region to draw in. Class definitions.

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

#include "Frame.hpp"

namespace vdraw
{
  Frame::Frame(VGImage& target) 
    : Canvas(), 
    originX(0), originY(0),
    width(target.getWidth()), height(target.getHeight()),
    valid(true),
    display(&target)         
  {}

  void Frame::nest(const Frame& parentFrame, double xoffset, double yoffset)
  {
    valid=true;
    isTopLevel=false;
    // Compute global coordinates of the nested frame
    originX = parentFrame.originX + xoffset;
    originY = parentFrame.originY + yoffset;
    display = parentFrame.display;
  }  

  void Frame::line (const Line& line)
  {
    Line lcopy(line);
    lcopy.translate(originX,originY);
    fixMarker(&lcopy);
    fixStrokeStyle(&lcopy);    
    display->line(lcopy);
  }

  void Frame::rectangle (const Rectangle& rect)  
  {
    Rectangle rcopy(rect);
    rcopy.x1 += originX;
    rcopy.x2 += originX;
    rcopy.y1 += originY;
    rcopy.y2 += originY;
    fixFillColor(&rcopy);
    fixStrokeStyle(&rcopy);
    display->rectangle(rcopy);
  }

  void Frame::circle (const Circle& circle)
  {
    Circle ccopy(circle);
    ccopy.xc += originX;
    ccopy.yc += originY;
    fixFillColor(&ccopy);
    fixStrokeStyle(&ccopy);
    display->circle(ccopy);
  }

  void Frame::text(const Text& text)  
  {
    //TODO: add check validity
    Text tcopy(text);
    tcopy.setPosition(text.x+originX, text.y+originY);
    fixTextStyle(&tcopy);
    display->text(tcopy);
  }

  void Frame::polygon(const Polygon& polygon)
  {
    Polygon pcopy(polygon);
    pcopy.translate(originX,originY);
    fixFillColor(&pcopy);
    fixStrokeStyle(&pcopy);
    display->polygon(pcopy);
  }

  void Frame::bitmap(const Bitmap& bitmap)
  {
    Bitmap bcopy(bitmap);
    bcopy.x1 += originX;
    bcopy.y1 += originY;
    bcopy.x2 += originX;
    bcopy.y2 += originY;
    display->bitmap(bcopy);
  }

  void Frame::comment(const Comment& comment)
  {
    display->comment(comment);
  }

  void Frame::fixMarker(Markable* m)
  {
    if(defaults->useM && !m->hasOwnMarker())
    {
      m->setMarker(defaults->marker);
    }
  }

  void Frame::fixStrokeStyle(BasicShape* s)
  {
    if(defaults->useSS && !s->hasOwnStrokeStyle())
    {
      s->setStrokeStyle(defaults->strokeStyle);
    }
  }

  void Frame::fixFillColor(Fillable* f)
  {
    if(defaults->useFC && !f->hasOwnFillColor())
    {
      f->setFillColor(defaults->fillColor);
    }
  }

  void Frame::fixTextStyle(Text* t)
  {
    if(defaults->useTS && !t->hasOwnTextStyle())
    {
      t->setStyle(defaults->textStyle);
    }
  }

}
