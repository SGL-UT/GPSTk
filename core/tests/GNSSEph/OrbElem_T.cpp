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

#include <iostream>
#include "TestUtil.hpp"
#include "OrbElem.hpp"

using namespace std;

/** Threshold for how much different our velocities can be between
 * being computed directly via svXvt and computed via differencing
 * svXvt positions over time. */
double velDiffThresh = 0.0008;

// we have to make a class that isn't abstract to test with.
class OrbElemNonAbstract : public gpstk::OrbElem
{
public:
   OrbElemNonAbstract() = default;
   gpstk::OrbElem* clone() const override
   { return nullptr; }
   std::string getName() const override
   { return "foo"; }
   std::string getNameLong() const override
   { return "bar"; }
   void adjustBeginningValidity() override
   { GPSTK_ASSERT(false); } // don't call this.
   void dumpTerse(std::ostream& s = std::cout) const
      override
   { s << "terse" << endl; }
};


class OrbElem_T
{
public:
   unsigned testSvXvt();
};


unsigned OrbElem_T ::
testSvXvt()
{
   TUDEF("OrbElem", "svXvt");
      // Hard code orbital parameters mostly so we can copy and paste
      // the data into other similar tests with minimal changes.
   OrbElemNonAbstract oe;
   oe.Cuc    = -.324845314026e-05;
   oe.Cus    =  .101532787085e-04;
   oe.Crc    =  .168968750000e+03;
   oe.Crs    = -.646250000000e+02;
   oe.Cic    =  .320374965668e-06;
   oe.Cis    =  .117346644402e-06;
   oe.M0     = -.136404614938e+01;
   oe.dn     =  .489591822036e-08;
   oe.dndot  = 0;
   oe.ecc    =  .146582192974e-01;
   oe.A      =  .515359719276e+04 * .515359719276e+04;
   oe.Adot   = 0;
   oe.OMEGA0 = -.296605403382e+01;
   oe.i0     =  .941587707856e+00;
   oe.w      = -.224753761329e+01;
   oe.OMEGAdot = -.804390648956e-08;
   oe.idot     =  .789318592573e-10;
   oe.ctToc    = gpstk::CivilTime(2015,7,19,1,59,28.0,gpstk::TimeSystem::GPS);
   oe.af0      =  .579084269702e-03;
   oe.af1      =  .227373675443e-11;
   oe.af2      =  .000000000000e+00;
   oe.dataLoadedFlag = true;
   oe.satID = gpstk::SatID(2, gpstk::SatelliteSystem::GPS);
   oe.ctToe    = gpstk::GPSWeekSecond(1854,.716800000000e+04);
   oe.setHealthy(true);
      // iode .700000000000e+01
      // codes on L2 .100000000000e+01
      // L2 P data .000000000000e+00
      // sv accuracy .240000000000e+01
      // sv health .000000000000e+00
      // tgd -.204890966415e-07
      // iodc .700000000000e+01
      // xmit time .360000000000e+04
      // fit int .400000000000e+01
   bool testFailed = false;
   try
   {
         // first compute Xvt
      static const unsigned SECONDS = 7200;
      gpstk::Xvt zeroth_array[SECONDS];
      for (unsigned ii = 0; ii < SECONDS; ii++)
      {
         zeroth_array[ii] = oe.svXvt(oe.ctToc + ii);
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


int main()
{
   unsigned total = 0;
   OrbElem_T testClass;
   total += testClass.testSvXvt();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
