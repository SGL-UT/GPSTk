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
#include "Rinex3ObsHeader.hpp"
#include <iostream>

using namespace std;

namespace gpstk
{
   ostream& operator<<(ostream& s, CarrierBand cb)
   {
      s << gpstk::StringUtils::asString(cb);
      return s;
   }
   ostream& operator<<(ostream& s, TrackingCode tc)
   {
      s << gpstk::StringUtils::asString(tc);
      return s;
   }
   ostream& operator<<(ostream& s, ObservationType ot)
   {
      s << gpstk::StringUtils::asString(ot);
      return s;
   }
}


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
   gpstk::RinexObsID roid("GX1 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Channel, roid.type);
      // RINEX requires that the "band" be "1" at all times, but I'm
      // not sure it strictly makes sense to actually translate it to
      // L1.
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined, roid.code);

      // test that iono delay pseudo obs is decoded properly
   TUCSM("RinexObsID(\"GI1 \")");
   gpstk::RinexObsID roidI1("GI1 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI1.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roidI1.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI1.code);
   TUCSM("RinexObsID(\"GI2 \")");
   gpstk::RinexObsID roidI2("GI2 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI2.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L2, roidI2.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI2.code);
   TUCSM("RinexObsID(\"RI3 \")");
   gpstk::RinexObsID roidI3("RI3 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI3.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::G3, roidI3.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI3.code);
   TUCSM("RinexObsID(\"RI4 \")");
   gpstk::RinexObsID roidI4("RI4 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI4.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::G1a, roidI4.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI4.code);
   TUCSM("RinexObsID(\"GI5 \")");
   gpstk::RinexObsID roidI5("GI5 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI5.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L5, roidI5.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI5.code);
   TUCSM("RinexObsID(\"EI6 \")");
   gpstk::RinexObsID roidI6("EI6 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI6.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::E6, roidI6.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI6.code);
   TUCSM("RinexObsID(\"CI7 \")");
   gpstk::RinexObsID roidI7("CI7 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI7.type);
      /** @bug The constructor rather unintelligently returns the same
       * band regardless of the GNSS being decoded.  Fixing this will
       * require rewriting the ObsID constructor which I'm putting off
       * for now. */
      //TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B2, roidI7.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI7.code);
   TUCSM("RinexObsID(\"EI8 \")");
   gpstk::RinexObsID roidI8("EI8 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI8.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::E5ab, roidI8.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI8.code);
   TUCSM("RinexObsID(\"II9 \")");
   gpstk::RinexObsID roidI9("II9 ", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Iono, roidI9.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::I9, roidI9.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Undefined,roidI9.code);

      // test RINEX 3.02 crapola
   TUCSM("RinexObsID() 3.02");
   gpstk::RinexObsID roid302("CC1I", 3.02);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid302.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid302.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CI1, roid302.code);
   TUASSERTE(std::string, "C1I", roid302.asString());
   TUASSERTFE(3.02, roid302.rinexVersion);
   roid302.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2I", roid302.asString());

   gpstk::RinexObsID roid302q("CC1Q", 3.02);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid302q.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid302q.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CQ1, roid302q.code);
   TUASSERTE(std::string, "C1Q", roid302q.asString());
   TUASSERTFE(3.02, roid302q.rinexVersion);
   roid302q.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2Q", roid302q.asString());

   gpstk::RinexObsID roid302iq("CC1X", 3.02);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Range,
             roid302iq.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid302iq.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CIQ1, roid302iq.code);
   TUASSERTE(std::string, "C1X", roid302iq.asString());
   TUASSERTFE(3.02, roid302iq.rinexVersion);
   roid302iq.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2X", roid302iq.asString());

   TUCSM("RinexObsID() 3.03");
   gpstk::RinexObsID roid303("CC2I", 3.03);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid303.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid303.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CI1, roid303.code);
   TUASSERTFE(3.03, roid303.rinexVersion);

   TUCSM("RinexObsID() 3.04");
   gpstk::RinexObsID roid304("CC2I", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid304.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CI1, roid304.code);
   TUASSERTFE(3.04, roid304.rinexVersion);

   gpstk::RinexObsID roid304d("CC1D", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304d.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304d.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CCD1, roid304d.code);
   TUASSERTFE(3.04, roid304d.rinexVersion);

   gpstk::RinexObsID roid304p("CC1P", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304p.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304p.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CCP1, roid304p.code);
   TUASSERTFE(3.04, roid304p.rinexVersion);

   gpstk::RinexObsID roid304dp("CC1X", 3.04);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Range,
             roid304dp.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304dp.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CCDP1, roid304dp.code);
   TUASSERTFE(3.04, roid304dp.rinexVersion);

   gpstk::RinexObsID roid304a("CC1A", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304a.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304a.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CA1, roid304a.code);
   TUASSERTFE(3.04, roid304a.rinexVersion);

   gpstk::RinexObsID roid304n("CL1N", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Phase,roid304n.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304n.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::CodelessC,
             roid304n.code);
   TUASSERTFE(3.04, roid304n.rinexVersion);

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
