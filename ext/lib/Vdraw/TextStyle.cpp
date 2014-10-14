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

/// @file TextStyle.cpp Defines how text appears. Class definitions

#include <sstream>

#include "TextStyle.hpp"

namespace vdraw
{
  /*
   * Constructors/Destructors
   */

  TextStyle::TextStyle(void)
    :font(MONOSPACE), color(Color::BLACK), pointSize(12)
  {
    setStyle((short)0);
  }


  TextStyle::TextStyle(double ips, short istyle)
    :font(MONOSPACE), color(Color::BLACK), pointSize(ips)
  {
    setStyle(istyle);
  }

  TextStyle::TextStyle(double ips, short istyle, const Color& icolor, Font ifont)
    :font(ifont), color(icolor), pointSize(ips)
  {
    setStyle(istyle);
  }

  TextStyle::TextStyle(double ips, const Color& icolor, Font ifont)
    :font(ifont), color(icolor), pointSize(ips)
  {
  }

  /*
   * Methods
   */

  short TextStyle::style(const char* str)
  {
    using namespace std;

    int i;
    char tmp;
    short set = 0;

    for(i = 0; str[i] != '\0' ; i++)
    {
      tmp = str[i];
      switch(tmp)
      {
        case 'b':
        case 'B':
          set = set | BOLD;
          break;
        case 'i':
        case 'I':
          set = set | ITALIC;
          break;
        case 'u':
        case 'U':
          set = set | UNDERLINE;
          break;
        case 's':
        case 'S':
          set = set | STRIKE;
          break;
        default:
          stringstream os;
		  os << tmp;
		  cout << "Parse Error: " + os.str(); 
      }
    }
    return set;
  }

  void TextStyle::setStyle(short istyle)
  {
    bold = istyle & BOLD;
    italic = istyle & ITALIC;
    underline = istyle & UNDERLINE;
    strike = istyle & STRIKE;
  }

}
