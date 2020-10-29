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

#include "Xvt.hpp"
#include "Triple.hpp"
#include "GPSEllipsoid.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace gpstk;

class Xvt_T
{
public: 
      /// Default Constructor, set the precision value
   Xvt_T()
         : eps(1E-12)
   {}
      /// Do-nothing Destructor
   ~Xvt_T()
   {}

      /*Tests the get methods and constructor of Xvt */
   unsigned getTest()
   {
      TUDEF("Xvt","Get");

         //Add in blank initialization check here

      Triple pos(1,2,3);
      Triple vel(4,5,6);
      double clockBias = 7, clockDrift = 8, relativityCorrection = 9;
      Xvt compare;

      compare.x = pos;
      compare.v = vel;
      compare.clkbias = clockBias;
      compare.clkdrift = clockDrift;
      compare.relcorr = relativityCorrection;

      TUCSM("getPos");
      TUASSERTE(Triple, pos, compare.getPos());
      TUCSM("getVel");
      TUASSERTE(Triple, vel, compare.getVel());
      TUCSM("getClockBias");
      TUASSERTFE(clockBias, compare.getClockBias());
      TUCSM("getClockDrift");
      TUASSERTFE(clockDrift, compare.getClockDrift());
      TUCSM("getRelativityCorr");
      TUASSERTFE(relativityCorrection, compare.getRelativityCorr());
      TUCSM("Xvt()");
      TUASSERTE(gpstk::Xvt::HealthStatus, gpstk::Xvt::Uninitialized,
                compare.health);

      TURETURN();
   }

      /* Ensures the computeRelativityCorrection method is accurate */
   unsigned computeRelativityCorrectionTest()
   {
      TUDEF("Xvt","computeRelativityCorrection");

      Triple pos( 1234567000, 887654, 321 );
      Triple vel(4321, 1234, 1342); 
      Xvt compare;
      double relcorr = -0.00011873444357376972;
      compare.x = pos;
      compare.v = vel;

      TUASSERTFEPS(relcorr, compare.computeRelativityCorrection(), eps);

      pos[0] = -1234567000;
      pos[1] = 887654;
      pos[2] = -100000;
      vel[0] = 3000;
      vel[1] = -500;
      vel[2] = -20;
      relcorr =8.242829448184317e-05;
      compare.x = pos;
      compare.v = vel;

      TUASSERTFEPS(relcorr, compare.computeRelativityCorrection(), eps);

      pos[0] = 0;
      pos[1] = 0;
      pos[2] = 0;
      vel[0] = 0;
      vel[1] = 0;
      vel[2] = 0;
      relcorr =0;
      compare.x = pos;
      compare.v = vel;

      TUASSERTFEPS(relcorr, compare.computeRelativityCorrection(), eps);

      TURETURN();
   }

#if 0
      /* Ensures the preciseRho method is accurate */
   unsigned preciseRhoTest()
   {
      TUDEF("Xvt","preciseRho Method Unverified");
      std::string failMesg;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//      Unable to verify if operations done in Xvt.hpp are correct
//      Creating placeholder for testing, and setting it to fail as a signifier
      failMesg = "UNVERIFIED preciseRhoTest. Set to FAIL until verified";
      testFramework.assert(false, failMesg, __LINE__);

      return testFramework.countFails();
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
   }
#endif

      /* Tests to see if the stream output operator << is functioning properly*/
   unsigned operatorTest()
   {
      TUDEF("Xvt","operator<<");

      Triple pos(1,2,3);
      Triple vel(4,5,6);
      double clockBias = 7, clockDrift = 8, relativityCorrection = 9;
      Xvt output;               
      output.x = pos;
      output.v = vel;
      output.clkbias = clockBias;
      output.clkdrift = clockDrift;
      output.relcorr = relativityCorrection;

      std::stringstream streamOutput;
      std::stringstream streamCompare;
      std::string stringOutput;
      std::string stringCompare; 

      try
      { 
            // Creates a string with the contents of output via the
            // stream output operator
         streamOutput << output;
         stringOutput = streamOutput.str();

            // Creates a string of what the output stream should be
         streamCompare << "x:" << output.x
                       << ", v:" << output.v
                       << ", clk bias:" << output.clkbias
                       << ", clk drift:" << output.clkdrift
                       << ", relcorr:" << output.relcorr
                       << ", health:" << output.health;
         stringCompare = streamCompare.str();
         TUASSERTE(std::string, stringCompare, stringOutput);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }

   unsigned healthStatusStreamTest()
   {
      TUDEF("Xvt::HealthStatus", "operator<<");
      for (unsigned i = static_cast<unsigned>(Xvt::HealthStatus::MinValue);
           i <= static_cast<unsigned>(Xvt::HealthStatus::MaxValue);
           i++)
      {
         ostringstream s;
         s << static_cast<Xvt::HealthStatus>(i);
         string str(s.str());
         TUASSERT(!str.empty());
         TUASSERT(str != "???");
      }
      unsigned i = static_cast<unsigned>(Xvt::HealthStatus::MaxValue) + 1;
      ostringstream s;
      s << static_cast<Xvt::HealthStatus>(i);
      string str(s.str());
      TUASSERT(!str.empty());
      TUASSERTE(string, "???", str);
      TURETURN();
   }

private:
   double eps;
};


int main() //Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   Xvt_T testClass;

   errorTotal += testClass.getTest();
      //errorTotal += testClass.preciseRhoTest();
   errorTotal += testClass.computeRelativityCorrectionTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.healthStatusStreamTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; //Return the total number of errors
}

