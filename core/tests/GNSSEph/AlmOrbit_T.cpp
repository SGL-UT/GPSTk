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

#include "AlmOrbit.hpp"
#include "Xvt.hpp"
#include "GPSWeekSecond.hpp"
#include "CommonTime.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sstream>

using namespace std;
using namespace gpstk;

/** Threshold for how much different our velocities can be between
 * being computed directly via svXvt and computed via differencing
 * svXvt positions over time. */
double velDiffThresh = 0.0008;

class AlmOrbit_T: public AlmOrbit
{
public:
   AlmOrbit_T()
   {
      eps = 1E-12;  // Default Constructor, set the precision value
   }
   ~AlmOrbit_T() {} // Default Desructor

//=============================================================================
// Test will check the initialization of AlmOrbit objects
//=============================================================================
   int initializationTest(void)
   {
      TUDEF("AlmOrbit", "Default Constructor");
      std::string testMesg;

      gpstk::AlmOrbit empty;

         //--------------------------------------------------------------------
         //Does the default constructor function correctly?
         //--------------------------------------------------------------------
      testMesg = "PRN not initialized to 0";
      TUASSERTE(short, 0, empty.getPRN());
      testMesg = "Ecc not initialized to 0";
      TUASSERTFEPS(0, empty.getecc(), eps);
      testMesg = "Offset not initialized to 0";
      TUASSERTFEPS(0, empty.geti_offset(), eps);
      testMesg = "OMEGAdot not initialized to 0";
      TUASSERTFEPS(0, empty.getOMEGAdot(), eps);
      testMesg = "Ahalf not initialized to 0";
      TUASSERTFEPS(0, empty.getAhalf(), eps);
      testMesg = "OMEGA0 not initialized to 0";
      TUASSERTFEPS(0, empty.getOMEGA0(), eps);
      testMesg = "W not initialized to 0";
      TUASSERTFEPS(0, empty.getw(), eps);
      testMesg = "M0 not initialized to 0";
      TUASSERTFEPS(0, empty.getM0(), eps);
      testMesg = "AF0 not initialized to 0";
      TUASSERTFEPS(0, empty.getAF0(), eps);
      testMesg = "AF1 not initialized to 0";
      TUASSERTFEPS(0, empty.getAF1(), eps);
      testMesg = "Toa not initialized to 0";
      TUASSERTE(long, 0, empty.getToaSOW());
      testMesg = "Xmit_time not initialized to 0";
      TUASSERTE(long, 0, empty.getxmit_time());
      testMesg = "Week not initialized to 0";
      TUASSERTE(short, 0, empty.getToaWeek());
// No SV health get method
//  testMesg = "SV_health not initialized to 0";
//  TUASSERTE(short, 0, empty.getSV_health());

//============================================================================

//  Should be tested by testing the inhereted members as comparison points
//  instead of using the get methods, but compare.PRN doesn't work due to
//  inheritance issues

//============================================================================

      gpstk::AlmOrbit compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
                              -0.296182, -1.31888, 2.79387, 0.000148773,
                              7.63976E-11, 466944, 250560, 797, 0);

      TUCSM("Explicit Constructor");
         //----------------------------------------------------------------
         //Does the explicit constructor function correctly?
         //----------------------------------------------------------------
      testMesg = "PRN value was not initialized correctly";
      TUASSERTE(short, 1, compare.getPRN());
      testMesg = "Ecc value was not initialized correctly";
      TUASSERTFEPS(0.00346661, compare.getecc(), eps);
      testMesg = "Offset value was not initialized correctly";
      TUASSERTFEPS(0.00388718, compare.geti_offset(), eps);
      testMesg = "OMEGAdot value was not initialized correctly";
      TUASSERTFEPS(-8.01176E-09, compare.getOMEGAdot(), eps);
      testMesg = "Ahalf value was not initialized correctly";
      TUASSERTFEPS(5153.58, compare.getAhalf(), eps);
      testMesg = "OMEGA0 value was not initialized correctly";
      TUASSERTFEPS(-0.296182, compare.getOMEGA0(), eps);
      testMesg = "W value was not initialized correctly";
      TUASSERTFEPS(-1.31888, compare.getw(), eps);
      testMesg = "M0 value was not initialized correctly";
      TUASSERTFEPS(2.79387, compare.getM0(), eps);
      testMesg = "AF0 value was not initialized correctly";
      TUASSERTFEPS(0.000148773, compare.getAF0(), eps);
      testMesg = "AF1 value was not initialized correctly";
      TUASSERTFEPS(7.63976E-11, compare.getAF1(), eps);
      testMesg = "Toa value was not initialized correctly";
      TUASSERTE(long, 466944, compare.getToaSOW());
      testMesg = "Xmit_time value was not initialized correctly";
      TUASSERTE(long, 250560, compare.getxmit_time());
      testMesg = "Week value was not initialized correctly";
      TUASSERTE(short, 797, compare.getToaWeek());
// No SV health get method
//  testMesg = "SV_health value was not initialized correctly";
//  TUASSERTE(short, 0, AlmOrbit::SV_health);

      TURETURN();
   }
//=============================================================================
// Test will check the dump method for various verbosities
//=============================================================================
   int dumpTest(void)
   {
      TUDEF("AlmOrbit", "Dump");
      std::string testMesg;

      gpstk::AlmOrbit compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
                              -0.296182, -1.31888, 2.79387, 0.000148773,
                              7.63976E-11, 466944, 250560, 797, 0);


      std::stringstream outputStream1, outputStream2, outputStream3;
      std::string outputString1, referenceString1, outputString2,
          referenceString2, outputString3, referenceString3;

      referenceString1 = "1, 466944, 797, 0, 1.4877e-04, 7.6398e-11,"
                         " 3.4666e-03, -1.3189e+00, 5.1536e+03, 2.7939e+00, -2.9618e-01, "
                         "-8.0118e-09, 3.8872e-03\n";
      compare.dump(outputStream1, 0);
      outputString1 = outputStream1.str();

         //-----------------------------------------------------------------
         //Did the least verbose dump method function correctly?
         //-----------------------------------------------------------------
      testMesg = "Least-verbose dump method did not function correctly";
      TUASSERTE(std::string, referenceString1, outputString1);

      referenceString2 = "PRN:1 Toa:466944 H:0 AFO:1.4877e-04 AF1:7.6398e-11 "
                         "Ecc:3.4666e-03\n   w:-1.3189e+00 Ahalf:5.1536e+03 "
                         "M0:2.7939e+00\n   OMEGA0:-2.9618e-01 "
                         "OMEGAdot:-8.0118e-09 Ioff:3.8872e-03\n";
      compare.dump(outputStream2, 1);
      outputString2 = outputStream2.str();

         //-----------------------------------------------------------------
         //Did the mid-level verbose dump method function correctly?
         //-----------------------------------------------------------------
      testMesg = "Medium-verbose dump method did not function correctly";
      TUASSERTE(std::string, referenceString2, outputString2);


      referenceString3 = "PRN:                   1\n"
                         "Toa:                   466944\n"
                         "xmit_time:             250560\n"
                         "week:                  797\n"
                         "SV_health:             0\n"
                         "AFO:                     1.4877e-04 sec\n"
                         "AF1:                     7.6398e-11 sec/sec\n"
                         "Sqrt A:                  5.1536e+03 sqrt meters\n"
                         "Eccentricity:            3.4666e-03\n"
                         "Arg of perigee:         -1.3189e+00 rad\n"
                         "Mean anomaly at epoch:   2.7939e+00 rad\n"
                         "Right ascension:        -2.9618e-01 rad"
                         "         -8.0118e-09 rad/sec\n"
                         "Inclination offset:      3.8872e-03 rad    \n";
      compare.dump(outputStream3, 2);
      outputString3 = outputStream3.str();

         //-----------------------------------------------------------------
         //Did the most verbose dump method function correctly?
         //-----------------------------------------------------------------
      testMesg = "High-verbose dump method did not function correctly";
      TUASSERTE(std::string, referenceString3, outputString3);

      TURETURN();

   }
//=============================================================================
// Test will check the various operators
//=============================================================================
   int operatorTest(void)
   {
      TUDEF("AlmOrbit", "operator<<");
      std::string testMesg;

      std::stringstream outputStream;
      std::string outputString, referenceString;

      gpstk::AlmOrbit compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
                              -0.296182, -1.31888, 2.79387, 0.000148773,
                              7.63976E-11, 466944, 250560, 797, 0);

      referenceString = "PRN:1 Toa:466944 H:0 AFO:1.4877e-04 AF1:7.6398e-11 "
                        "Ecc:3.4666e-03\n   w:-1.3189e+00 Ahalf:5.1536e+03 "
                        "M0:2.7939e+00\n   OMEGA0:-2.9618e-01 "
                        "OMEGAdot:-8.0118e-09 Ioff:3.8872e-03\n";

      outputStream << compare;
      outputString = outputStream.str();


         //-----------------------------------------------------------------
         //Did the << operator function correctly?
         //-----------------------------------------------------------------
      testMesg = "The redirection operator << did not function correctly";
      TUASSERTE(std::string, referenceString, outputString);

      TURETURN();

   }
//=============================================================================
// Test will check the various get methods
//=============================================================================
   int getTest(void)
   {
      TUDEF("AlmOrbit", "get Methods");
      std::string testMesg;

      gpstk::AlmOrbit compare(1, 0.00346661, 0.00388718, -8.01176E-09, 5153.58,
                              -0.296182, -1.31888, 2.79387, 0.000148773,
                              7.63976E-11, 466944, 250560, 797, 0);

      gpstk::GPSWeekSecond reference1(797,466944);
      gpstk::CommonTime cRef1 (reference1);

         //------------------------------------------------------------------
         //Did the getToaTime method function correctly?
         //------------------------------------------------------------------
      testMesg = "getToaTime method did not function correctly";
      TUASSERTE(gpstk::CommonTime, cRef1, compare.getToaTime());

      gpstk::GPSWeekSecond reference2(797, 250560);
      gpstk::CommonTime cRef2 (reference2);
         //------------------------------------------------------------------
         //Did the getTransmitTime method function correctly?
         //------------------------------------------------------------------
      testMesg = "getTransmitTime method did not function correctly";
      TUASSERTE(gpstk::CommonTime, cRef2, compare.getTransmitTime());

         //------------------------------------------------------------------
         //Did the getFullWeek method function correctly?
         //------------------------------------------------------------------
      testMesg = "getFullWeek method did not function correctly";
      TUASSERTE(short, 797, compare.getFullWeek());

         //setting Toa to < -302400 & xmit_time to 0
      gpstk::AlmOrbit compare1(1, 0.00346661, 0.00388718, -8.01176E-09,
                               5153.58, -0.296182, -1.31888, 2.79387,
                               0.000148773, 7.63976E-11, -302401, 0, 797, 0);

         //------------------------------------------------------------------
         //Did the getFullWeek method round the week down?
         //------------------------------------------------------------------
      testMesg = "getFullWeek method did not round the week down";
      TUASSERTE(short, 796, compare1.getFullWeek());

         //setting Toa to > 302400 & xmit_time to 0
      gpstk::AlmOrbit compare2(1, 0.00346661, 0.00388718, -8.01176E-09,
                               5153.58, -0.296182, -1.31888, 2.79387,
                               0.000148773, 7.63976E-11, 302401, 0, 797, 0);

         //------------------------------------------------------------------
         //Did the getFullWeek method round the week up?
         //------------------------------------------------------------------
      testMesg = "getFullWeek method did not round the week up";
      TUASSERTE(short, 798, compare2.getFullWeek());

      TURETURN();
   }
//=============================================================================
// Test will check the svXvt method
//=============================================================================
   int svXvtTest(void)
   {
      TUDEF("AlmOrbit", "svXvt");
      gpstk::AlmOrbit oe(2, .146582192974e-01,
                         .941587707856e+00 - (0.3*gpstk::PI),
                         -.804390648956e-08, .515359719276e+04,
                         -.296605403382e+01, -.224753761329e+01,
                         -.136404614938e+01, .579084269702e-03,
                         .227373675443e-11, 7168, 3600, 1854, 0);
      bool testFailed = false;
      try
      {
            // first compute Xvt
         static const unsigned SECONDS = 7200;
         gpstk::Xvt zeroth_array[SECONDS];
         for (unsigned ii = 0; ii < SECONDS; ii++)
         {
            zeroth_array[ii] = oe.svXvt(oe.getToaTime() + ii);
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


private:
   double eps;
};


int main() //Main function to initialize and run all tests above
{
   int errorTotal = 0;
   AlmOrbit_T testClass;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.dumpTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.getTest();
   errorTotal += testClass.svXvtTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal; //Return the total number of errors
}
