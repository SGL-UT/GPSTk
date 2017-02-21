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

#include "BrcClockCorrection.hpp"
#include "TestUtil.hpp"
#include "GPSWeekZcount.hpp"

using namespace gpstk;

#ifdef _MSC_VER
#define LDEXP(x,y) ldexp(x,y)
#else
#define LDEXP(x,y) std::ldexp(x,y)
#endif

class BrcClockCorrection_T
{
public:
      /// set the fields to some non-default values
   void fill(BrcClockCorrection& orbit);
   unsigned initializationTest();
   unsigned equalityTest();
};


void BrcClockCorrection_T ::
fill(BrcClockCorrection& orbit)
{
   ObsID oi(ObsID::otNavMsg, ObsID::cbL5, ObsID::tcY);
   orbit.loadData("GPS", oi, 31, GPSWeekZcount(1886, 398400),
                  GPSWeekZcount(1887, 0), 5, 7, 9, true,
                  LDEXP(double( int32_t(0xfffff91d)), -31), // af0
                  LDEXP(double( int16_t(0xffed)),     -43), // af1
                  1.0e-15 // af2
                  );
}


unsigned BrcClockCorrection_T ::
initializationTest()
{
   BrcClockCorrection empty;
   CommonTime emptyTime;
   ObsID emptyObsID;
   TUDEF("BrcClockCorrection", "Default Constructor");
   TUASSERTE(bool, false, empty.dataLoaded);
   TUASSERTE(std::string, "", empty.satSys);
   TUASSERTE(ObsID, emptyObsID, empty.obsID);
   TUASSERTE(short, 0, empty.PRNID);
   TUASSERTE(CommonTime, emptyTime, empty.Toc);
   TUASSERTE(CommonTime, emptyTime, empty.Top);
   TUASSERTE(short, 0, empty.URAoc);
   TUASSERTE(short, 0, empty.URAoc1);
   TUASSERTE(short, 0, empty.URAoc2);
   TUASSERTE(bool, false, empty.healthy);
   TUASSERTE(double, 0, empty.af0);
   TUASSERTE(double, 0, empty.af1);
   TUASSERTE(double, 0, empty.af2);
   TURETURN();
}


unsigned BrcClockCorrection_T ::
equalityTest()
{
   TUDEF("BrcClockCorrection", "operator== / !=");
   BrcClockCorrection clock;
   fill(clock);
   BrcClockCorrection clockCopy(clock);
      // make sure our copy reports as being the same
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
      // Tweak each data member one by one and compare.
      // Yes the comments are fairly redundant but it helps to
      // visually separate each batch of statements per data
      // member.
   clockCopy.dataLoaded = false;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // satSys
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.satSys = "twaffle";
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // obsID
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.obsID = ObsID(ObsID::otNavMsg, ObsID::cbL1, ObsID::tcP);
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // PRNID
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.PRNID = 93;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // Toc
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.Toc = GPSWeekZcount(1234,56789);
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // Top
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.Top = GPSWeekZcount(1234,56789);
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // URAoc
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.URAoc = 943;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // URAoc1
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.URAoc1 = 943;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // URAoc2
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.URAoc2 = 943;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // healthy
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.healthy = false;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // af0
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.af0 = 1.5e-12;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // af1
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.af1 = 1.5e-12;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
      // af2
   TUCATCH(clockCopy = clock);
   TUASSERTE(BrcClockCorrection, clock, clockCopy);
   clockCopy.af2 = 1.5e-12;
   TUASSERT(clockCopy != clock);
   TUASSERT(!(clockCopy == clock));
   TURETURN();
}


int main() //Main function to initialize and run all tests above
{
   using namespace std;
   BrcClockCorrection_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.equalityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
