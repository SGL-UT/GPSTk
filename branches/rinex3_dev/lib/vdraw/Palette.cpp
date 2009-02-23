#pragma ident "$Id$"

/// @file Palette.cpp Defines a color palette. Class defintions.

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

#include "Palette.hpp"

namespace vdraw
{
  Palette::Palette(const Color &base, double imin, double imax)
  {
    setRange(imin,imax);
    setColor(imin,base);
    setColor(imax,base);
  }

  Palette::Palette(const Palette &p)
  {
    min = p.min;
    width = p.width;
    palette = p.palette;
  }

  Palette& Palette::operator=(Palette p)
  {
    // p is a copy, swap the variables
    std::swap(min,p.min);
    std::swap(width,p.width);
    std::swap(palette,p.palette);
    // p is destructed with the old data from this
    return *this;
  }

  void Palette::setColor(double val, const Color &c)
  {
    clamp(val);
    val = (val-min)/width;
    if(palette.size()==0)
      palette.push_back(std::pair<double,Color>(val,c));
    else
    {
      std::list<std::pair<double,Color> >::iterator i=palette.begin();
      while((i!=palette.end())&&(i->first<val)) i++;
      if(i==palette.end())
        palette.push_back(std::pair<double,Color>(val,c));
      else if(i==palette.begin())
        palette.push_front(std::pair<double,Color>(val,c));
      else if(i->first==val)
        i->second=c;
      else
        palette.insert(i,std::pair<double,Color>(val,c));
    }
  }

  Color Palette::getColor(double val) const
  {
    clamp(val);
    val = (val-min)/width;  
    std::list<std::pair<double,Color> >::const_iterator j,i=palette.begin();
    while((i!=palette.end())&&(i->first<val)) i++;
    if(i->first==val || i==palette.begin()) return i->second;
    else if(i==palette.end())return (--i)->second;
    j = i--; // i is before j
    return (i->second).interpolate((val-(i->first))/((j->first)-(i->first)),j->second);
  }
}
