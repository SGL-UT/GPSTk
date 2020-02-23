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
#include <iomanip>
#include <limits>

#include "StringUtils.hpp"
#include "DayTime.hpp"
#include "icd_gps_constants.hpp"

using namespace std;
using namespace gpstk;

template<class T>
void testmeth(unsigned long incCount, T incValue)
{
   gpstk::DayTime dtorig(2000,12,1,0,0,0.0), dtcopy;
   double tdiff;
   long totalIncrements=0;
   long incCountUse;

   dtcopy = dtorig;
   incCountUse = incCount/2;
   for (long j=0; j<incCountUse; ++j)
   {
      dtcopy.addSeconds(incValue);
      totalIncrements++;
   }

   for (long j=0; j<incCountUse; ++j)
   {
      dtcopy.addSeconds(-incValue);
      totalIncrements++;
   }

   tdiff = dtcopy-dtorig;
   cout << setw(8) << std::numeric_limits<T>::digits10
        << setw(10) << incValue
        << setw(15) << totalIncrements
        << setw(20) << setprecision(10) << tdiff
        << endl;
}

/// returns 0 if all tests pass
int main()
{
   try
   {
      cout << "Testing DayTime addSeconds accuracy & speed." << endl;


      cout << setw(8)  << "digits"
           << setw(10) << "incr"
           << setw(15) << "count"
           << setw(20) << "dt (sec)"
           << endl;

      DayTime startTime;
      testmeth(60,       (long)1);
      testmeth(3600,     (long)1);
      testmeth(86400,    (long)1);
      testmeth(7*86400,  (long)1);
      cout << endl;

      testmeth(1000,         1e-3);
      testmeth(60*1000,      1e-3);
      testmeth(3600*1000,    1e-3);
      testmeth(86400*1000,   1e-3);
      testmeth(86400*2*1000, 1e-3);
      testmeth(86400*7*1000, 1e-3);
      cout << endl;

      testmeth(1000*1000,      1e-6);
      testmeth(60*1000*1000,   1e-6);
      testmeth(5*60*1000*1000, 1e-6);
      cout << endl;

      testmeth(1000*1000,     1e-9);
      testmeth(100*1000*1000, 1e-9);
      cout << endl;

      testmeth(1000*1000,     1e-11);
      testmeth(100*1000*1000, 1e-11);
      cout << endl;

      DayTime endTime;

      cout << endl
           << "Processing time "
           << setprecision(4) << endTime-startTime
           << " seconds."
           << endl;
   }
   catch(gpstk::Exception& e)
   {
      cout << e << endl;
   }
   catch(...)
   {
      cout << "Some other exception thrown..." << endl;
   }
}
