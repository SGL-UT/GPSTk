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
//  Copyright 2004-2019, The University of Texas at Austin
//
//==============================================================================

//==============================================================================
//
//  This software developed by Applied Research Laboratories at the University of
//  Texas at Austin, under contract to an agency or agencies within the U.S. 
//  Department of Defense. The U.S. Government retains all rights to use,
//  duplicate, distribute, disclose, or release this software. 
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

#include "TestUtil.hpp"
#include "RinexObsID.hpp"
#include <iostream>

using namespace std;

class RinexObsID_T
{
public:
   RinexObsID_T() {} // Default Constructor, set the precision value
   ~RinexObsID_T() {} // Default Desructor
      /// Make sure RinexObsID can decode all valid observation codes.
   unsigned decodeTest();
      /** Iterate through codes for testing
       * @param[in,out] testFramework The TestUtil object for the
       *   currently executing test.
       * @param[in] system The single character RINEX 3 system code
       *   to be tested ("G", "R", etc).
       * @param[in] bandCode The two character RINEX 3 code for band
       *   and tracking code, e.g. 1C.
       * @param[in] prValid If false, a pseudorange measurement is
       *   invalid for this code. */
   void testCodes(gpstk::TestUtil& testFramework, const std::string& system,
                  const std::string& bandCode, bool prValid = true);
};


unsigned RinexObsID_T ::
decodeTest()
{
   TUDEF("RinexObsID", "RinexObsID(string)");
      // GPS L1
   std::string codes("CSLXPWYM");
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "G", std::string("1") + codes[i]);
   }
   testCodes(testFramework, "G", "1N", false);
      // GPS L2
   codes = "CDSLXPWYM";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "G", std::string("2") + codes[i]);
   }
   testCodes(testFramework, "G", "2N", false);
      // GPS L5
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "G", std::string("5") + codes[i]);
   }
      // GLONASS G1
   codes = "CP";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "R", std::string("1") + codes[i]);
   }
      // GLONASS G1a
   codes = "ABX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "R", std::string("4") + codes[i]);
   }
      // GLONASS G2
   codes = "CP";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "R", std::string("2") + codes[i]);
   }
      // GLONASS G2a
   codes = "ABX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "R", std::string("6") + codes[i]);
   }
      // GLONASS G3
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "R", std::string("3") + codes[i]);
   }
      // Galileo E1
   codes = "ABCXZ";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "E", std::string("1") + codes[i]);
   }
      // Galileo E5a
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "E", std::string("5") + codes[i]);
   }
      // Galileo E5b
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "E", std::string("7") + codes[i]);
   }
      // Galileo E5
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "E", std::string("8") + codes[i]);
   }
      // Galileo E6
   codes = "ABCXZ";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "E", std::string("6") + codes[i]);
   }
      // SBAS L1
   testCodes(testFramework, "S", "1C");
      // SBAS L5
   codes = "IQX";
   for (unsigned i = 0; i < codes.length(); i++)
   {
      testCodes(testFramework, "S", std::string("5") + codes[i]);
   }
   TURETURN();
}


void RinexObsID_T ::
testCodes(gpstk::TestUtil& testFramework,
          const std::string& system,
          const std::string& bandCode,
          bool prValid)
{
   std::string invalidCodes("ABEFGHIJKMNOPQRTUVWXYZabcdefghijklmnopqrstuvwxyz"),
      validCodes("LDS");
   if (prValid)
      validCodes += "C";
   else
      invalidCodes += "C";
   for (unsigned i = 0; i < invalidCodes.length(); i++)
   {
      std::string rin3code = system + invalidCodes[i] + bandCode;
         // RinexObsID doesn't throw an exception for invalid codes?
         // TUCSM("RinexObsID("+rin3code+")");
         // TUTHROW(gpstk::RinexObsID(rin3code));
      TUCSM("::isValidRinexObsID("+rin3code+")");
      TUASSERT(!gpstk::isValidRinexObsID(rin3code));
   }
   for (unsigned i = 0; i < validCodes.length(); i++)
   {
      std::string rin3code = system + validCodes[i] + bandCode;
      TUCSM("RinexObsID("+rin3code+")");
      TUCATCH(gpstk::RinexObsID(rin3code));
         // this is declared a global at the moment :-/
      TUCSM("::isValidRinexObsID("+rin3code+")");
      TUASSERT(gpstk::isValidRinexObsID(rin3code));
   }
}


int main() //Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   RinexObsID_T testClass;

   errorTotal += testClass.decodeTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
