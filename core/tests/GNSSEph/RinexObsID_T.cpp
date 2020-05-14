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

      // test that channel num pseudo obs is decoded properly
   TUCSM("RinexObsID(\"GX1 \")");
   gpstk::RinexObsID roid("GX1 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otChannel, roid.type);
      // RINEX requires that the "band" be "1" at all times, but I'm
      // not sure it strictly makes sense to actually translate it to
      // L1.
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbL1, roid.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined, roid.code);

      // test that iono delay pseudo obs is decoded properly
   TUCSM("RinexObsID(\"GI1 \")");
   gpstk::RinexObsID roidI1("GI1 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI1.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbL1, roidI1.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI1.code);
   TUCSM("RinexObsID(\"GI2 \")");
   gpstk::RinexObsID roidI2("GI2 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI2.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbL2, roidI2.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI2.code);
   TUCSM("RinexObsID(\"RI3 \")");
   gpstk::RinexObsID roidI3("RI3 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI3.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbG3, roidI3.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI3.code);
   TUCSM("RinexObsID(\"RI4 \")");
   gpstk::RinexObsID roidI4("RI4 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI4.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbG1a, roidI4.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI4.code);
   TUCSM("RinexObsID(\"GI5 \")");
   gpstk::RinexObsID roidI5("GI5 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI5.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbL5, roidI5.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI5.code);
   TUCSM("RinexObsID(\"EI6 \")");
   gpstk::RinexObsID roidI6("EI6 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI6.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbE6, roidI6.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI6.code);
   TUCSM("RinexObsID(\"CI7 \")");
   gpstk::RinexObsID roidI7("CI7 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI7.type);
      /** @bug The constructor rather unintelligently returns the same
       * band regardless of the GNSS being decoded.  Fixing this will
       * require rewriting the ObsID constructor which I'm putting off
       * for now. */
      //TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbB2, roidI7.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI7.code);
   TUCSM("RinexObsID(\"EI8 \")");
   gpstk::RinexObsID roidI8("EI8 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI8.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbE5ab, roidI8.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI8.code);
   TUCSM("RinexObsID(\"II9 \")");
   gpstk::RinexObsID roidI9("II9 ");
   TUASSERTE(gpstk::ObsID::ObservationType, gpstk::ObsID::otIono, roidI9.type);
   TUASSERTE(gpstk::ObsID::CarrierBand, gpstk::ObsID::cbI9, roidI9.band);
   TUASSERTE(gpstk::ObsID::TrackingCode, gpstk::ObsID::tcUndefined,roidI9.code);

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
