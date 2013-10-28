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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include "SystemTime.hpp"
#include "CommonTime.hpp"
#include "CivilTime.hpp"
#include "YDSTime.hpp"
#include "GPSWeekSecond.hpp"
#include "MJD.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{

   try
   {

         // In the GPSTk there are multiple classes to manage time, depending
         // on the specific operation that we want to carry out. This modular
         // approach eases handling the many different time systems used in the
         // modern Global Navigation Satellite Systems.

         // Note, however, that in the GPSTk the unifying class to do time
         // computations is the 'CommonTime' class.

         // Read current time from system clock
      SystemTime systime;

         // Convert to 'CommonTime', the standard way to handle time at GPSTk
         // Please note that declaration, initialization and conversion are
         // done in the same line.
      CommonTime comtime( systime );

         // This is the typical way to handle civil time
      CivilTime civtime( comtime );

         // The YDSTime class is very useful for common GNSS tasks
      YDSTime ydstime( comtime );

         // This is a typical class to handle time in GPS system
      GPSWeekSecond gpstime( comtime );

         // Class to handle Modified Julian Date
      MJD mjd( comtime );

      cout << "Hello world!" << endl;
      cout << "   The current civil time is " << civtime << endl;
      cout << "   The current year is " << ydstime.year << endl;
      cout << "   The current day of year is " << ydstime.doy << endl;
      cout << "   The current second of day is " << ydstime.sod << endl;
      cout << "   The current full GPS week is " << gpstime.week << endl;
      cout << "   The current short GPS week is " << gpstime.getModWeek()
                                                  << endl;
      cout << "   The current day of GPS week is " << gpstime.getDayOfWeek()
                                                   << endl;
      cout << "   The current second of GPS week is " << gpstime.sow << endl;
      cout << "   The current Modified Julian Date is " << mjd << endl;

   }
   catch( Exception error)
   {
      cout << error << endl;
      exit(-1);
   }

   exit(0);
}
