#pragma ident "$Id$"

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
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

/**
 * @file positiontest.cpp  Test the Position module
 *
 */

#include <iostream>
#include <iomanip>
#include "Position.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

int main()
{
   try
   {
      int i;
      Position c,s,d,g;
      cout << "Position() " << c << endl;

      // test transformTo
      cout << "Test set...() and transformTo\n";
      {
         c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
         cout << "Position.set        to Cartesian  " << c << endl;
         Position t(c);
         t.transformTo(Position::Geodetic);
         cout << "Transform Cartesian to Geodetic   " << t;
         cout << "  Error : " << range(t,c) << " m" << endl;
         t = c;
         t.transformTo(Position::Geocentric);
         cout << "Transform Cartesian to Geocentric " << t;
         cout << "  Error : " << range(t,c) << " m" << endl;
         t = c;
         t.transformTo(Position::Spherical);
         cout << "Transform Cartesian to Spherical  " << t;
         cout << "  Error : " << range(t,c) << " m" << endl;
      }
      {
         d.setGeodetic(39.000004186778,251.499999999370,1400.009066903964);
         cout << "Position.set         to Geodetic  " << d << endl;
         Position t(d);
         t.transformTo(Position::Cartesian);
         cout << "Transform Geodetic to Cartesian   " << t;
         cout << "  Error : " << range(t,d) << " m" << endl;
         t = d;
         t.transformTo(Position::Geocentric);
         cout << "Transform Geodetic to Geocentric  " << t;
         cout << "  Error : " << range(t,d) << " m" << endl;
         t = d;
         t.transformTo(Position::Spherical);
         cout << "Transform Geodetic to Spherical   " << t;
         cout << "  Error : " << range(t,d) << " m" << endl;
      }
      {
         g.setGeocentric(38.811958506159,251.499999999370,6371110.627671023800);
         cout << "Position.set        to Geocentric " << g << endl;
         Position t(g);
         t.transformTo(Position::Cartesian);
         cout << "Transform Geocentric to Cartesian " << t;
         cout << "  Error : " << range(t,g) << " m" << endl;
         t = g;
         t.transformTo(Position::Geodetic);
         cout << "Transform Geocentric to Geodetic  " << t;
         cout << "  Error : " << range(t,g) << " m" << endl;
         t = g;
         t.transformTo(Position::Spherical);
         cout << "Transform Geocentric to Spherical " << t;
         cout << "  Error : " << range(t,g) << " m" << endl;
      }
      {
         s.setSpherical(51.188041493841,251.499999999370,6371110.627671023800);
         cout << "Position.set      to Spherical    " << s << endl;
         Position t(s);
         t.transformTo(Position::Cartesian);
         cout << "Transform Spherical to Cartesian  " << t;
         cout << "  Error : " << range(t,s) << " m" << endl;
         t = s;
         t.transformTo(Position::Geocentric);
         cout << "Transform Spherical to Geocentric " << t;
         cout << "  Error : " << range(t,s) << " m" << endl;
         t = s;
         t.transformTo(Position::Geodetic);
         cout << "Transform Spherical to Geodetic   " << t;
         cout << "  Error : " << range(t,s) << " m" << endl;
      }

      // test printf
      cout << "\nTest printf and the indicated 'get()' functions\n";
      cout << c.printf("%13.4x     X() (meters)") << endl;
      cout << c.printf("%13.4y     Y() (meters)") << endl;
      cout << c.printf("%13.4z     Z() (meters)") << endl;
      cout << c.printf("%13.4X     X()/1000 (kilometers)") << endl;
      cout << c.printf("%13.4Y     Y()/1000 (kilometers)") << endl;
      cout << c.printf("%13.4Z     Z()/1000 (kilometers)") << endl;
      cout << c.printf("%15.6A   geodeticLatitude() (degrees North)") << endl;
      cout << c.printf("%15.6a   geocentricLatitude() (degrees North)") << endl;
      cout << c.printf("%15.6L   longitude() (degrees East)") << endl;
      cout << c.printf("%15.6l   longitude() (degrees East)") << endl;
      cout << c.printf("%15.6w   longitude() (degrees West)") << endl;
      cout << c.printf("%15.6W   longitude() (degrees West)") << endl;
      cout << c.printf("%15.6t   theta() (degrees)") << endl;
      cout << c.printf("%15.6T   theta() (radians)") << endl;
      cout << c.printf("%15.6p   phi() (degrees)") << endl;
      cout << c.printf("%15.6P   phi() (radians)") << endl;
      cout << c.printf("%13.4r     radius() meters") << endl;
      cout << c.printf("%13.4R     radius()/1000 kilometers") << endl;
      cout << c.printf("%13.4h     height() meters") << endl;
      cout << c.printf("%13.4H     height()/1000 kilometers") << endl;

      // test setToString
      cout << "\nTest setToString and equality operators\n";
      string fmt[5]={
         "",
         "%A degN %L degE %h m",
         "%a degN %L degE %r m",
         "%x m %y m %z m",
         "%t deg %p deg %r M"};
      for(i=1; i<5; i++) {
         string str;
         Position t(c),tt;
         t.transformTo(static_cast<Position::CoordinateSystem>(i));
         {
            ostringstream o;
            o << t;
            str = o.str();
         }
         tt.setToString(str,fmt[i]);
         cout << "System: " << t.getSystemName() << endl;
         cout << "Original    " << t << endl;
         cout << "string      " << str << endl;
         cout << "setToString " << tt
            << " Error " << range(tt,t) << " m" << endl;
         cout << " equality: " << (t==tt ? "==" : "not ==");
         cout << " but " << (t!=tt ? "!=" : "not !=") << endl;
         cout << endl;
      }

      // test elevation and azimuth
      cout << "Elevation and azimth" << fixed << endl;
      c.setECEF(-1575232.0141,-4707872.2332, 3993198.4383);
      s.setECEF(3*6371110.62767,0,0);
      cout << setw(6) << setprecision(2) << c.elevation(s)
         << " " << setw(6) << setprecision(2) << c.azimuth(s) << endl;
      // compare with functions from Triple::
      cout << setw(6) << setprecision(2) << c.elvAngle(s)
         << " " << setw(6) << setprecision(2) << c.azAngle(s) << endl;
      cout << endl;

      // test IPP
      cout << "The IPP at " << g << " (90,0,350000) is\n"
           << "           " << g.getIonosphericPiercePoint(90,0,350000)
           << " " << g.getSystemName() << endl << endl;

      // test ALL transforms with pole
      // X -> D -> C -> S -> X -> C -> D -> X -> S -> D -> S -> C -> X
      cout << "Try to break it at the pole\n";
      c.setECEF(0,0,6371110.6277);
      //c.setECEF(0,0,0.0001);         // this will break it
      cout << "Set to the pole in cartesian : " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The pole in geodetic   " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The pole in geocentric " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The pole in spherical  " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The pole in cartesian  " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The pole in geocentric " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The pole in geodetic   " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The pole in cartesian  " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The pole in spherical  " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The pole in geodetic   " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The pole in spherical  " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The pole in geocentric " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The pole in cartesian  " << c << endl;

      // test ALL transforms with origin
      // X -> D -> C -> S -> X -> C -> D -> X -> S -> D -> S -> C -> X
      cout << endl << "Try to break it at the origin\n";
      c.setECEF(0,0,0);
      cout << "Set to the origin in cartesian : " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The origin in geodetic   " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The origin in geocentric " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The origin in spherical  " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The origin in cartesian  " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The origin in geocentric " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The origin in geodetic   " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The origin in cartesian  " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The origin in spherical  " << c << endl;
      c.transformTo(Position::Geodetic);
      cout << "The origin in geodetic   " << c << endl;
      c.transformTo(Position::Spherical);
      cout << "The origin in spherical  " << c << endl;
      c.transformTo(Position::Geocentric);
      cout << "The origin in geocentric " << c << endl;
      c.transformTo(Position::Cartesian);
      cout << "The origin in cartesian  " << c << endl;

      cout << "Tests complete." << endl;
      return 0;
   }
   catch(gpstk::Exception& e)
   {
      cout << e << endl;
   }
   catch(...)
   {
      cout << "Some other exception thrown..." << endl;
   }

   cout << "Exiting with exceptions." << endl;
   return -1;
}
