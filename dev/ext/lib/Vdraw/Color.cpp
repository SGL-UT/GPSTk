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

/// @file Color.hpp Defines colors. Class declarations.

#include <sstream>
#include <iomanip>
#include <cctype>

#include "Color.hpp"

namespace vdraw
{
  static const double COLORMAX = 255.0;

  void Color::getRGBTriplet( short& red, short& green, short& blue ) const
  {
    using namespace std;

    int temp = rgb;

    blue = temp & 0x0000FF;
    temp = (temp >> 8);

    green = temp & 0x0000FF;
    temp = (temp >> 8);

    red   = temp & 0x0000FF;
  }

  void Color::setRGBTriplet( short red, short green, short blue )
  {
    int lred   = red & 0xFFl, lgreen = green & 0xFFl, lblue = blue & 0xFFl;
    lred = (lred << 16);
    lgreen = (lgreen << 8);
    rgb = lred | lgreen | lblue;
  }

  void Color::getRGBTripletFractional( double& red, double& green,
      double& blue ) const
  {
    short rs, gs, bs;
    getRGBTriplet( rs, gs, bs);

    red   = rs/COLORMAX; 
    green = gs/COLORMAX;
    blue  = bs/COLORMAX;
  }

  void Color::setRGBTripletFractional( double red, double green, double blue )
  {
    short sred   = static_cast<short> (COLORMAX * red);
    short sgreen = static_cast<short> (COLORMAX * green);
    short sblue  = static_cast<short> (COLORMAX * blue);

    setRGBTriplet( sred, sgreen, sblue);
  }

  Color Color::interpolate(double dist, const Color &o) const
  {
    short r1,g1,b1,r2,g2,b2;
    getRGBTriplet(r1,g1,b1);
    o.getRGBTriplet(r2,g2,b2);
    Color c(
        (int)(r1+(r2-r1)*dist),
        (int)(g1+(g2-g1)*dist),
        (int)(b1+(b2-b1)*dist));
    return c;
  }

  void Color::setToString(const std::string& str)
  {
    std::string ls;

    // Eat the white spaces (including those inside)
    std::stringstream os(str);
    std::string temp;
    os >> temp;
    ls = temp;
    while (os >> temp) 
      ls += temp;

    rgb = BLACK;

    // Make sure this runs as expected in the tests..
    int i=0;
    while (ls[i])
    {
      ls[i] = tolower(ls[i]);
      //c[i]=tolower(s[i]);
      i++;
    }

    // Unfortunately switch/case only works on integral types. Blech.
    if ((ls == "r") || (ls=="red"))         rgb=RED;
    else if ((ls == "o") || (ls=="orange")) rgb=ORANGE;
    else if ((ls == "y") || (ls=="yellow")) rgb=YELLOW;
    else if ((ls == "g") || (ls=="green"))  rgb=GREEN;
    else if ((ls == "b") || (ls=="blue"))   rgb=BLUE;
    else if ((ls == "v") || (ls=="violet")) rgb=VIOLET;
    else if ((ls == "w") || (ls=="white"))  rgb=WHITE;
    else if ((ls == "k") || (ls=="brown"))  rgb=BROWN;
    else if ((ls == "n") || (ls=="black"))  rgb=BLACK;

    else if ((ls=="grey") || (ls=="gray"))  rgb=GREY;
    else if ((ls=="forestgreen") || (ls=="forest")) rgb=FOREST_GREEN;      

    else if (ls=="pink")         rgb=PINK;
    else if (ls=="cyan")         rgb=CYAN;
    else if (ls=="olive")        rgb=OLIVE;
    else if (ls=="khaki")        rgb=KHAKI;
    else if (ls=="skyblue")      rgb=SKY_BLUE;
    else if (ls=="turquoise")    rgb=TURQUOISE;
    else if (ls=="magenta")      rgb=MAGENTA;
    else if (ls=="maroon")       rgb=MAROON;
    else if (ls=="burntorange")  rgb=BURNT_ORANGE;
    else if (ls=="cardinal")     rgb=CARDINAL;
    else if (ls=="navy")         rgb=NAVY;
    else if (ls=="darkpurple")   rgb=DARK_PURPLE;
    else if (ls=="clear")        rgb=CLEAR;

    return;
  }
} // namespace vdraw
