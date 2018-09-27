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

#include "BrcKeplerOrbit.hpp"
#include "TestUtil.hpp"
#include "GPSWeekZcount.hpp"

using namespace gpstk;

#ifdef _MSC_VER
#define LDEXP(x,y) ldexp(x,y)
#else
#define LDEXP(x,y) std::ldexp(x,y)
#endif

class BrcKeplerOrbit_T
{
public:
      /// set the fields to some non-default values
   void fill(BrcKeplerOrbit& orbit);
   unsigned initializationTest();
   unsigned equalityTest();
};


void BrcKeplerOrbit_T ::
fill(BrcKeplerOrbit& orbit)
{
   ObsID oi(ObsID::otNavMsg, ObsID::cbL5, ObsID::tcY);
   orbit.loadData("GPS", oi, 31, GPSWeekZcount(1886, 398400),
                  GPSWeekZcount(1887, 0), GPSWeekZcount(1887, 0), 1, 7, 
                     // these are the same as in EngEphemeris_T.cpp
                  LDEXP(double( int16_t(0xfe17)),     -29), // Cuc
                  LDEXP(double( int16_t(0x0b0e)),     -29), // Cus
                  LDEXP(double( int16_t(0x22b4)),      -5), // Crc
                  LDEXP(double( int16_t(0xfde4)),      -5), // Crs
                  LDEXP(double( int16_t(0xffae)),     -29), // Cic
                  LDEXP(double( int16_t(0x0005)),     -29), // Cis
                  LDEXP(double( int32_t(0x2dbbccf8)), -31) * PI, // M0
                  LDEXP(double( int16_t(0x35bb)),     -43) * PI, //dn
                  123e-12, // dndot, arbitrary, absent from GPS nav id 2
                  LDEXP(double(uint32_t(0x04473adb)), -33), // ecc
                  7.89e9, // A, should this be Ahalf * Ahalf?
                  LDEXP(double(uint32_t(0xa10dcc28)), -19), // Ahalf
                  4.56e7, // Adot, arbitrary, absent from GPS nav id 2
                  LDEXP(double( int32_t(0x3873d1d1)), -31) * PI, // OMEGA0
                  LDEXP(double( int32_t(0x2747e88f)), -31) * PI, // i0
                  LDEXP(double( int32_t(0xb078a8d5)), -31) * PI, // w
                  LDEXP(double( int32_t(0xffffa3c7)), -43) * PI, // OMEGAdot
                  LDEXP(double( int16_t(0xfdc6)),     -43) * PI  // idot
                  );
}


unsigned BrcKeplerOrbit_T ::
initializationTest()
{
   BrcKeplerOrbit empty;
   CommonTime emptyTime;
   ObsID emptyObsID;
   TUDEF("BrcKeplerOrbit", "Default Constructor");
   TUASSERTE(bool, false, empty.dataLoaded);
   TUASSERTE(std::string, "", empty.satSys);
   TUASSERTE(ObsID, emptyObsID, empty.obsID);
   TUASSERTE(short, 0, empty.PRNID);
   TUASSERTE(CommonTime, emptyTime, empty.Toe);
   TUASSERTE(short, 0, empty.URAoe);
   TUASSERTE(bool, false, empty.healthy);
   TUASSERTE(double, 0, empty.Cuc);
   TUASSERTE(double, 0, empty.Cus);
   TUASSERTE(double, 0, empty.Crc);
   TUASSERTE(double, 0, empty.Crs);
   TUASSERTE(double, 0, empty.Cic);
   TUASSERTE(double, 0, empty.Cis);
   TUASSERTE(double, 0, empty.M0);
   TUASSERTE(double, 0, empty.dn);
   TUASSERTE(double, 0, empty.dndot);
   TUASSERTE(double, 0, empty.ecc);
   TUASSERTE(double, 0, empty.A);
   TUASSERTE(double, 0, empty.Ahalf);
   TUASSERTE(double, 0, empty.Adot);
   TUASSERTE(double, 0, empty.OMEGA0);
   TUASSERTE(double, 0, empty.i0);
   TUASSERTE(double, 0, empty.w);
   TUASSERTE(double, 0, empty.OMEGAdot);
   TUASSERTE(double, 0, empty.idot);
   TUASSERTE(CommonTime, emptyTime, empty.beginFit);
   TUASSERTE(CommonTime, emptyTime, empty.endFit);
   TURETURN();
}


unsigned BrcKeplerOrbit_T ::
equalityTest()
{
   TUDEF("BrcKeplerOrbit", "operator== / !=");
   BrcKeplerOrbit orbit;
   fill(orbit);
   BrcKeplerOrbit orbitCopy(orbit);
      // make sure our copy reports as being the same
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
      // Tweak each data member one by one and compare.
      // Yes the comments are fairly redundant but it helps to
      // visually separate each batch of statements per data
      // member.
   orbitCopy.dataLoaded = false;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // satSys
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.satSys = "twaffle";
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // obsID
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.obsID = ObsID(ObsID::otNavMsg, ObsID::cbL1, ObsID::tcP);
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // PRNID
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.PRNID = 93;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Toe
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Toe = GPSWeekZcount(1234,56789);
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // URAoe
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.URAoe = 943;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // healthy
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.healthy = false;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Cuc
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Cuc = 1.5e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Cus
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Cus = 1.7e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Crc
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Crc = 1.9e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Crs
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Crs = 2.1e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Cic
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Cic = 2.4e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Cis
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Cis = 2.5e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // M0
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.M0 = 2.6e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // dn
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.dn = 2.7e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // dndot
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.dndot = 2.8e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // ecc
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.ecc = 2.9e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // A
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.A = 3.0e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Ahalf
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Ahalf = 3.1e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // Adot
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.Adot = 3.2e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // OMEGA0
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.OMEGA0 = 3.3e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // i0
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.i0 = 3.4e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // w
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.w = 3.5e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // OMEGAdot
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.OMEGAdot = 3.6e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // idot
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.idot = 3.7e-12;
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // beginFit
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.beginFit = GPSWeekZcount(1234,98765);
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));
      // endFit
   TUCATCH(orbitCopy = orbit);
   TUASSERTE(BrcKeplerOrbit, orbit, orbitCopy);
   orbitCopy.endFit = GPSWeekZcount(1267, 56533);
   TUASSERT(orbitCopy != orbit);
   TUASSERT(!(orbitCopy == orbit));   
   TURETURN();
}


int main() //Main function to initialize and run all tests above
{
   using namespace std;
   BrcKeplerOrbit_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.equalityTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
