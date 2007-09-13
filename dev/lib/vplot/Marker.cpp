#pragma ident "$Id: //depot/msn/r5.3/wonky/gpstkplot/lib/draw/Marker.cpp#2 $"

/// @file Marker.cpp Defines marker appearance. Class definitions.

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

#include <sstream>
#include <iomanip>

#include "Marker.hpp"

namespace vplot
{
  /**
   * Methods
   */

  std::string Marker::uniqueName(void) const
  {
    using namespace std;

    stringstream ss;
    switch(mark)
    {
      case DOT:
        ss << "dot";
        break;
      case PLUS:
        ss << "plus";
        break;
      case X:
        ss << "X";
        break;
    }
    ss << range << "x";
    ss << hex << setw(6) << setfill('0') << markerColor.getRGB();

    return ss.str();
  }

  /**
   * Comparison operators:
   */

  bool Marker::shapeEquals(const Marker& other)
  {
    return ((mark == other.getMark()) && (range == other.getRange()));
  }

  bool Marker::equals( const Marker& other )
  {
    /*
     * \note these first two are commented as the basic shape ability has been
     * removed from marker at this point.
     */

    // This gets too complicated to check.
    //if ( !usesDefault ) return false;

    // This one default, other not.
    //if ( !other.hasDefaultMark() ) return false;

    return ((this->shapeEquals(other)) && (markerColor == other.getColor()));

    // Check if the marks are different.
    //if ( mark != other.getMark() ) return false;

    // Check if the mark colors are different.
    //if ( markerColor != other.getColor() ) return false;

    // Check if the ranges are different
    //if ( range != other.getRange() ) return false;

    // Bothe use the same default mark and color.
    //return true;
  }

}


