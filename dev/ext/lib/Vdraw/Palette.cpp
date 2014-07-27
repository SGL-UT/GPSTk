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

/// @file Palette.cpp Defines a color palette. Class defintions.

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
