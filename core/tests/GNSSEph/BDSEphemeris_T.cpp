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
#include "BDSEphemeris.hpp"
#include "CivilTime.hpp"
#include "BDSWeekSecond.hpp"
#include "TimeString.hpp"

using namespace std;

/** Threshold for how much different our velocities can be between
 * being computed directly via svXvt and computed via differencing
 * svXvt positions over time. */
double velDiffThresh = 0.0008;


class BDSEphemeris_T
{
public:
   unsigned testSvXvtMEO();
   unsigned testSvXvtGEO();
      // call writeVel for a variety of ephemerides
      // unsigned wut();
      /** Write to a file the difference between the magnitude of the
       * velocity vector as computed by svXvt and the magnitude of the
       * position derivative as computed by this function.  For
       * plotting. */
   // void writeVel(const gpstk::BDSEphemeris& oe);
};


unsigned BDSEphemeris_T ::
testSvXvtMEO()
{
   TUDEF("BDSEphemeris", "svXvt");
      // Hard code orbital parameters mostly so we can copy and paste
      // the data into other similar tests with minimal changes.
   gpstk::BDSEphemeris oe;
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
   oe.ctToc    = gpstk::CivilTime(2015,7,19,1,59,28.0,gpstk::TimeSystem::BDT);
   oe.af0      =  .579084269702e-03;
   oe.af1      =  .227373675443e-11;
   oe.af2      =  .000000000000e+00;
   oe.dataLoadedFlag = true;
   oe.satID = gpstk::SatID(2, gpstk::SatelliteSystem::BeiDou);
   oe.ctToe    = gpstk::BDSWeekSecond(498,.716800000000e+04);
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


unsigned BDSEphemeris_T ::
testSvXvtGEO()
{
   TUDEF("BDSEphemeris", "svXvt");
      // Hard code orbital parameters mostly so we can copy and paste
      // the data into other similar tests with minimal changes.
   gpstk::BDSEphemeris oe;
   oe.Cuc      = -1.08121894E-05;
   oe.Cus      = -1.25728548E-06;
   oe.Crc      =  3.97031250E+01;
   oe.Crs      = -3.23656250E+02;
   oe.Cic      = -2.02562660E-07;
   oe.Cis      = -2.00234354E-08;
   oe.M0       =  2.81324357E+00;
   oe.dn       = -1.00075597E-09;
   oe.dndot    =  0.00000000E+00;
   oe.ecc      =  2.62024812E-04;
   oe.A        =  4.21651139E+07;
   oe.Adot     =  0.00000000E+00;
   oe.OMEGA0   = -2.99944238E+00;
   oe.i0       =  1.06909427E-01;
   oe.w        =  2.63078773E+00;
   oe.OMEGAdot =  2.13687472E-09;
   oe.idot     =  1.45363198E-10;
   oe.ctToc    = gpstk::CivilTime(2019,3,1,0,0,0,gpstk::TimeSystem::BDT);
   oe.af0      =  2.59640510E-04;
   oe.af1      =  4.48929782E-11;
   oe.af2      =  0.00000000E+00;
   oe.dataLoadedFlag = true;
   oe.satID = gpstk::SatID(1, gpstk::SatelliteSystem::BeiDou);
   oe.ctToe    = oe.ctToc;
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


// I'm not saving all the code generated for wut().
// Look at dump2code.pl if you need to do this again.
// For now it's all being left in the file but #if'd out.
#if 0
unsigned BDSEphemeris_T ::
wut()
{
   try
   {
      gpstk::BDSEphemeris oe;
      oe.Cuc      = -1.08121894E-05;
      oe.Cus      = -1.25728548E-06;
      oe.Crc      = 3.97031250E+01;
      oe.Crs      = -3.23656250E+02;
      oe.Cic      = -2.02562660E-07;
      oe.Cis      = -2.00234354E-08;
      oe.M0       = 2.81324357E+00;
      oe.dn       = -1.00075597E-09;
      oe.dndot    = 0.00000000E+00;
      oe.ecc      = 2.62024812E-04;
      oe.A        = 4.21651139E+07;
      oe.Adot     = 0.00000000E+00;
      oe.OMEGA0   = -2.99944238E+00;
      oe.i0       = 1.06909427E-01;
      oe.w        = 2.63078773E+00;
      oe.OMEGAdot = 2.13687472E-09;
      oe.idot     = 1.45363198E-10;
      oe.ctToc    = gpstk::CivilTime(2020,3,1,0,0,0,gpstk::TimeSystem::BDT);
      oe.af0      = 2.59640510E-04;
      oe.af1      = 4.48929782E-11;
      oe.af2      = 0.00000000E+00;
      oe.dataLoadedFlag = true;
      oe.satID = gpstk::SatID(1, gpstk::SatelliteSystem::BeiDou);
      oe.ctToe    = gpstk::CivilTime(2020,3,1,0,0,0,gpstk::TimeSystem::BDT);
      writeVel(oe);
   }
   catch(...)
   {
      cerr << "exception" << endl;
   }
}


void BDSEphemeris_T ::
writeVel(const gpstk::BDSEphemeris& oe)
{
   ostringstream ss;
   ss << setw(2) << setfill('0') << oe.satID.id << "_"
      << gpstk::printTime(oe.ctToc, "%04Y%02m%02d_%02H%02M%02S.dat");
   ofstream s(ss.str().c_str());
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
         // then write the difference between derived and computed velocity
      for (unsigned ii = 0; ii < SECONDS; ii++)
      {
         double derivedMag = deriv[ii].mag();
         double computedMag = zeroth_array[ii].getVel().mag();
         s << ii << " " << (computedMag - derivedMag) << endl;
      }
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
   }
   catch (...)
   {
      cerr << "exception" << endl;
   }
}
#endif
 

int main()
{
   unsigned total = 0;
   BDSEphemeris_T testClass;
   total += testClass.testSvXvtMEO();
   total += testClass.testSvXvtGEO();
      //testClass.wut();

   cout << "Total Failures for " << __FILE__ << ": " << total << endl;
   return total;
}
