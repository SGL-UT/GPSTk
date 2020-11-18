//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "BrcKeplerOrbit.hpp"
#include "TestUtil.hpp"
#include "GPSWeekZcount.hpp"
#include "TimeString.hpp"

using namespace std;
using namespace gpstk;

/** Threshold for how much different our velocities can be between
 * being computed directly via svXvt and computed via differencing
 * svXvt positions over time. */
double velDiffThresh = 0.0008;

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
   unsigned svXvtTest();
   unsigned relativityTest();
   unsigned fitIntTest();
};


void BrcKeplerOrbit_T ::
fill(BrcKeplerOrbit& orbit)
{
   ObsID oi(ObservationType::NavMsg, CarrierBand::L5, TrackingCode::Y);
   orbit.loadData("GPS", oi, 31, GPSWeekZcount(1886, 398400),
                  GPSWeekZcount(1887, 0), GPSWeekZcount(1887, 0), 1, true,
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
   orbitCopy.obsID = ObsID(ObservationType::NavMsg, CarrierBand::L1, TrackingCode::P);
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

unsigned BrcKeplerOrbit_T ::
svXvtTest()
{
   TUDEF("BrcKeplerOrbit", "svXvt");
   BrcKeplerOrbit orbit;
   ObsID oi(ObservationType::NavMsg, CarrierBand::L1, TrackingCode::Y);
   gpstk::CommonTime toc = gpstk::CivilTime(2015,7,19,1,59,28.0,
                                            gpstk::TimeSystem::GPS);
   orbit.loadData("GPS", oi, 2, toc, toc+7200,
                  gpstk::GPSWeekSecond(1854,.716800000000e+04), 0, true,
                  -.324845314026e-05, .101532787085e-04, .168968750000e+03,
                  -.646250000000e+02, .320374965668e-06, .117346644402e-06,
                  -.136404614938e+01, .489591822036e-08, 0,
                  .146582192974e-01, .515359719276e+04 * .515359719276e+04,
                  .515359719276e+04, 0, -.296605403382e+01,
                  .941587707856e+00, -.224753761329e+01, -.804390648956e-08,
                  .789318592573e-10);
                  
   bool testFailed = false;
   try
   {
         // first compute Xvt
      static const unsigned SECONDS = 7200;
      gpstk::Xvt zeroth_array[SECONDS];
      for (unsigned ii = 0; ii < SECONDS; ii++)
      {
         zeroth_array[ii] = orbit.svXvt(orbit.getOrbitEpoch() + ii);
      }
         // then compute first derivative of position, i.e. velocity
      gpstk::Triple deriv[SECONDS];
      double h = 1; // time step size in seconds
      for (unsigned ii = 0; ii < SECONDS; ii++)
      {
         if (ii == 0)
         {
            deriv[ii] = (1/h)*(-1.5*zeroth_array[ii].getPos() +
                               2.0*zeroth_array[ii+1].getPos() -
                               0.5*zeroth_array[ii+2].getPos());
         }
         else if ((ii == 1) || (ii == (SECONDS-2)))
         {
            deriv[ii] = (1/h)*(-0.5*zeroth_array[ii-1].getPos() +
                               0.5*zeroth_array[ii+1].getPos());
         }
         else if (ii == (SECONDS-1))
         {
            deriv[ii] = (1/h)*(0.5*zeroth_array[ii-2].getPos() -
                               2.0*zeroth_array[ii-1].getPos() +
                               1.5*zeroth_array[ii].getPos());
         }
         else
         {
            deriv[ii] = (1/h)*((1.0/12.0)*zeroth_array[ii-2].getPos() -
                               (2.0/3.0)*zeroth_array[ii-1].getPos() +
                               (2.0/3.0)*zeroth_array[ii+1].getPos() -
                               (1.0/12.0)*zeroth_array[ii+2].getPos());
         }
      }
         // then check the difference between derived and computed velocity
      for (unsigned ii = 0; ii < SECONDS; ii++)
      {
         double derivedMag = deriv[ii].mag();
         double computedMag = zeroth_array[ii].getVel().mag();
            // If you want to print the data e.g. to plot, uncomment
            // this stream output statement and comment out tbe break
            // statement below.
            // Just don't check it in that way, please.
            // cerr << ii << " " << (computedMag - derivedMag) << endl;
         if (fabs(computedMag - derivedMag) > velDiffThresh)
         {
               // no sense in printing 7200 success/fail messages.
            testFailed = true;
            break;
         }
      }
      if (testFailed)
      {
         TUFAIL("computed velocity is significantly different from derived"
                " velocity");
      }
      else
      {
         TUPASS("velocity check");
      }
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
      TUFAIL("Exception");
   }
   catch (...)
   {
      TUFAIL("Exception");
   }
   TURETURN();
}


unsigned BrcKeplerOrbit_T ::
relativityTest()
{
   TUDEF("BrcKeplerOrbit", "svRelativity");
   BrcKeplerOrbit orbit;
   fill(orbit);
   TUASSERTFE(-1.7274634252517538304e-08,
              orbit.svRelativity(GPSWeekZcount(1886, 398400)));
   TURETURN();
}


unsigned BrcKeplerOrbit_T ::
fitIntTest()
{
   TUDEF("BrcKeplerOrbit", "getBeginningOfFitInterval");
   BrcKeplerOrbit orbit;
   fill(orbit);
   CommonTime beg(GPSWeekZcount(1886, 398400));
   CommonTime end(GPSWeekZcount(1887, 0));
   CommonTime before(beg-1), after(end+1);
   TUASSERTE(CommonTime, beg, orbit.getBeginningOfFitInterval());
   TUCSM("getEndOfFitInterval");
   TUASSERTE(CommonTime, end, orbit.getEndOfFitInterval());
   TUCSM("withinFitInterval");
   TUASSERT(!orbit.withinFitInterval(before));
   TUASSERT(orbit.withinFitInterval(beg));
   TUASSERT(orbit.withinFitInterval(end));
   TUASSERT(!orbit.withinFitInterval(after));
   TURETURN();
}


int main() //Main function to initialize and run all tests above
{
   using namespace std;
   BrcKeplerOrbit_T testClass;
   unsigned errorTotal = 0;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.equalityTest();
   errorTotal += testClass.svXvtTest();
   errorTotal += testClass.relativityTest();
   errorTotal += testClass.fitIntTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
