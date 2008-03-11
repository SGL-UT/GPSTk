
/// @file TextStyle.cpp Defines how text appears. Class definitions

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


#include "TextStyle.hpp"

namespace vplot
{
  /*
   * Constructors/Destructors
   */

  TextStyle::TextStyle(void)
    :pointSize(12), color(Color::BLACK), font(MONOSPACE)
  {
    setStyle(0);
  }


  TextStyle::TextStyle(double ips, int istyle)
    :pointSize(ips), color(Color::BLACK), font(MONOSPACE)
  {
    setStyle(istyle);
  }

  TextStyle::TextStyle(double ips, const char* istyle)
    :pointSize(ips), color(Color::BLACK), font(MONOSPACE)
  {
    setStyle(istyle);
  }

  TextStyle::TextStyle(double ips, int istyle, const Color& icolor, Font ifont)
    :pointSize(ips), color(icolor), font(ifont)
  {
    setStyle(istyle);
  }

  TextStyle::TextStyle(double ips, const char* istyle, Color& icolor, Font ifont)
    :pointSize(ips), color(icolor), font(ifont)
  {
    setStyle(istyle);
  }



  /*
   * Methods
   */

  void TextStyle::setStyle(const char* str)
  {
    using namespace std;

    int i;
    char tmp;
    int set = 0;

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
          cout << "Parse Error: " + tmp; 
      }
    }
    setStyle(set);
  }

  void TextStyle::setStyle(int istyle)
  {
    bold = istyle & BOLD;
    italic = istyle & ITALIC;
    underline = istyle & UNDERLINE;
    strike = istyle & STRIKE;
  }

}

