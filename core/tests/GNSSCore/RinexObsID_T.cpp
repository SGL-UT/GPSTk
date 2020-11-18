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


// This makes sure that
// 1) The RINEX obs ID decodes as expected
// 2) The RINEX obs ID encodes as expected
// 3) The wildcard (Any) ObsID enums match
#define CONTEST(RINEXCODE, CARRIERBAND, TRACKINGCODE)                   \
   try                                                                  \
   {                                                                    \
      TUCSM("RinexObsID(\"" RINEXCODE "\")");                           \
      gpstk::RinexObsID obs(RINEXCODE,                                  \
                            gpstk::Rinex3ObsBase::currentVersion);      \
      TUASSERTE(gpstk::ObservationType,                                 \
                gpstk::ObservationType::Phase,                          \
                obs.type);                                              \
      TUASSERTE(gpstk::CarrierBand,                                     \
                CARRIERBAND, obs.band);                                 \
      TUASSERTE(gpstk::TrackingCode,                                    \
                TRACKINGCODE, obs.code);                                \
      gpstk::RinexObsID obs2(gpstk::ObservationType::Phase,             \
                             CARRIERBAND, TRACKINGCODE);                \
      TUASSERTE(std::string, std::string(RINEXCODE).substr(1),          \
                obs2.asString());                                       \
      gpstk::RinexObsID wildcard("****",                                \
                                 gpstk::Rinex3ObsBase::currentVersion); \
      TUASSERTE(gpstk::RinexObsID, wildcard, obs);                      \
   }                                                                    \
   catch (gpstk::Exception& exc)                                        \
   {                                                                    \
      cerr << exc << endl;                                              \
      TUFAIL("Unexpected exception");                                   \
   }                                                                    \
   catch (std::exception& exc)                                          \
   {                                                                    \
      TUFAIL("Unexpected exception " + std::string(exc.what()));        \
   }                                                                    \
   catch (...)                                                          \
   {                                                                    \
      TUFAIL("Unknown exception");                                      \
   }

class RinexObsID_T
{
public:
   RinexObsID_T() {} // Default Constructor, set the precision value
   ~RinexObsID_T() {} // Default Desructor
      /// Make sure RinexObsID can decode all valid observation codes.
   unsigned decodeTest();
      /// This was migrated from ObsID_T when the RINEX-isms were
      /// moved and probably duplicates a fair bit of decodeTest, but
      /// I can't be bothered to unify the testing right now.
   unsigned fromStringConstructorTest();
   unsigned newIDTest();
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
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::G2a, roidI6.band);
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
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1I, roid302.code);
   TUASSERTE(std::string, "C1I", roid302.asString());
   TUASSERTFE(3.02, roid302.rinexVersion);
   roid302.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2I", roid302.asString());

   gpstk::RinexObsID roid302q("CC1Q", 3.02);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid302q.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid302q.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1Q, roid302q.code);
   TUASSERTE(std::string, "C1Q", roid302q.asString());
   TUASSERTFE(3.02, roid302q.rinexVersion);
   roid302q.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2Q", roid302q.asString());

   gpstk::RinexObsID roid302iq("CC1X", 3.02);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Range,
             roid302iq.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid302iq.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1IQ, roid302iq.code);
   TUASSERTE(std::string, "C1X", roid302iq.asString());
   TUASSERTFE(3.02, roid302iq.rinexVersion);
   roid302iq.rinexVersion = 3.04;
   TUASSERTE(std::string, "C2X", roid302iq.asString());

   TUCSM("RinexObsID() 3.03");
   gpstk::RinexObsID roid303("CC2I", 3.03);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid303.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid303.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1I, roid303.code);
   TUASSERTFE(3.03, roid303.rinexVersion);

   TUCSM("RinexObsID() 3.04");
   gpstk::RinexObsID roid304("CC2I", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::B1, roid304.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1I, roid304.code);
   TUASSERTFE(3.04, roid304.rinexVersion);

   gpstk::RinexObsID roid304d("CC1D", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304d.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304d.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1CD, roid304d.code);
   TUASSERTFE(3.04, roid304d.rinexVersion);

   gpstk::RinexObsID roid304p("CC1P", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304p.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304p.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1CP, roid304p.code);
   TUASSERTFE(3.04, roid304p.rinexVersion);

   gpstk::RinexObsID roid304dp("CC1X", 3.04);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Range,
             roid304dp.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304dp.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1CDP, roid304dp.code);
   TUASSERTFE(3.04, roid304dp.rinexVersion);

   gpstk::RinexObsID roid304a("CC1A", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,roid304a.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304a.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::B1A, roid304a.code);
   TUASSERTFE(3.04, roid304a.rinexVersion);

   gpstk::RinexObsID roid304n("CL1N", 3.04);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Phase,roid304n.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::L1, roid304n.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::BCodeless,
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


   //reads in 3-4 length string
unsigned RinexObsID_T :: fromStringConstructorTest()
{
   TUDEF("RinexObsID", "RinexObsID(string)");
   std::string failMesg;

      //set with invalid length
   failMesg = "[testing] RinexObsID constructor from invalid string, [expected]"
      " exception gpstk::Exception, [actual] threw no exception";
   try
   {
      gpstk::RinexObsID invalidID("G 10 ",gpstk::Rinex3ObsBase::currentVersion);
      TUFAIL(failMesg);
   }
   catch (gpstk::Exception e)
   {
      TUPASS(failMesg);
   }

   try {
      gpstk::RinexObsID invalidID("G1",gpstk::Rinex3ObsBase::currentVersion);
      TUFAIL(failMesg);
   }
   catch (gpstk::Exception e)
   {
      TUPASS(failMesg);
   }

      //testing base assign w/out using any of the reused codes
      // GPS L1 C/A PseudoRange
   gpstk::RinexObsID obs1("GC1C",gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,obs1.type);
   TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::L1,obs1.band);
   TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::CA,obs1.code);

      //testing only case of reassinged codes for GPS
      // GPS L5 IQ Doppler
   gpstk::RinexObsID obs2("GD5X",gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType,
             gpstk::ObservationType::Doppler, obs2.type);
   TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::L5,obs2.band);
   TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::L5IQ,obs2.code);

      //testing completely random case
      // QZSS E6 L Carrier Phase
   gpstk::RinexObsID obs3("JL6L",gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Phase,obs3.type);
   TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::E6,obs3.band);
   TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::LEXL,obs3.code);

   CONTEST("CL2I", gpstk::CarrierBand::B1, gpstk::TrackingCode::B1I);
   CONTEST("CL2Q", gpstk::CarrierBand::B1, gpstk::TrackingCode::B1Q);
   CONTEST("CL2X", gpstk::CarrierBand::B1, gpstk::TrackingCode::B1IQ);
   CONTEST("CL1D", gpstk::CarrierBand::L1, gpstk::TrackingCode::B1CD);
   CONTEST("CL1P", gpstk::CarrierBand::L1, gpstk::TrackingCode::B1CP);
   CONTEST("CL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::B1CDP);
   CONTEST("CL1A", gpstk::CarrierBand::L1, gpstk::TrackingCode::B1A);
   CONTEST("CL1N", gpstk::CarrierBand::L1, gpstk::TrackingCode::BCodeless);
      // these are only valid in rinex 3.02 and the macro defaults
      // to the current version, which is not 3.02.
      // CONTEST("CL1I", gpstk::CarrierBand::B1, gpstk::TrackingCode::B1I);
      // CONTEST("CL1Q", gpstk::CarrierBand::B1, gpstk::TrackingCode::B1Q);
   CONTEST("CL5D", gpstk::CarrierBand::L5, gpstk::TrackingCode::B2aI);
   CONTEST("CL5P", gpstk::CarrierBand::L5, gpstk::TrackingCode::B2aQ);
   CONTEST("CL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::B2aIQ);
   CONTEST("CL7I", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2I);
   CONTEST("CL7Q", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2Q);
   CONTEST("CL7X", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2IQ);
   CONTEST("CL7D", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2bI);
   CONTEST("CL7P", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2bQ);
   CONTEST("CL7Z", gpstk::CarrierBand::B2, gpstk::TrackingCode::B2bIQ);
   CONTEST("CL8D", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::B2abI);
   CONTEST("CL8P", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::B2abQ);
   CONTEST("CL8X", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::B2abIQ);
   CONTEST("CL6I", gpstk::CarrierBand::B3, gpstk::TrackingCode::B3I);
   CONTEST("CL6Q", gpstk::CarrierBand::B3, gpstk::TrackingCode::B3Q);
   CONTEST("CL6X", gpstk::CarrierBand::B3, gpstk::TrackingCode::B3IQ);
   CONTEST("CL6A", gpstk::CarrierBand::B3, gpstk::TrackingCode::B3AIQ);
   CONTEST("EL1A", gpstk::CarrierBand::L1, gpstk::TrackingCode::E1A);
   CONTEST("EL1B", gpstk::CarrierBand::L1, gpstk::TrackingCode::E1B);
   CONTEST("EL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::E1C);
   CONTEST("EL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::E1BC);
   CONTEST("EL1Z", gpstk::CarrierBand::L1, gpstk::TrackingCode::E1ABC);
   CONTEST("EL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::E5aI);
   CONTEST("EL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::E5aQ);
   CONTEST("EL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::E5aIQ);
   CONTEST("EL7I", gpstk::CarrierBand::E5b, gpstk::TrackingCode::E5bI);
   CONTEST("EL7Q", gpstk::CarrierBand::E5b, gpstk::TrackingCode::E5bQ);
   CONTEST("EL7X", gpstk::CarrierBand::E5b, gpstk::TrackingCode::E5bIQ);
   CONTEST("EL8I", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::E5abI);
   CONTEST("EL8Q", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::E5abQ);
   CONTEST("EL8X", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::E5abIQ);
   CONTEST("EL6A", gpstk::CarrierBand::E6, gpstk::TrackingCode::E6A);
   CONTEST("EL6B", gpstk::CarrierBand::E6, gpstk::TrackingCode::E6B);
   CONTEST("EL6C", gpstk::CarrierBand::E6, gpstk::TrackingCode::E6C);
   CONTEST("EL6X", gpstk::CarrierBand::E6, gpstk::TrackingCode::E6BC);
   CONTEST("EL6Z", gpstk::CarrierBand::E6, gpstk::TrackingCode::E6ABC);
   CONTEST("RL1C", gpstk::CarrierBand::G1, gpstk::TrackingCode::Standard);
   CONTEST("RL1P", gpstk::CarrierBand::G1, gpstk::TrackingCode::Precise);
   CONTEST("RL4A", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OCD);
   CONTEST("RL4B", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OCP);
   CONTEST("RL4X", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OCDP);
   CONTEST("RL2C", gpstk::CarrierBand::G2, gpstk::TrackingCode::Standard);
   CONTEST("RL2P", gpstk::CarrierBand::G2, gpstk::TrackingCode::Precise);
   CONTEST("RL6A", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2CSI);
   CONTEST("RL6B", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2OCP);
   CONTEST("RL6X", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2CSIL2OCp);
   CONTEST("RL3I", gpstk::CarrierBand::G3, gpstk::TrackingCode::L3OCD);
   CONTEST("RL3Q", gpstk::CarrierBand::G3, gpstk::TrackingCode::L3OCP);
   CONTEST("RL3X", gpstk::CarrierBand::G3, gpstk::TrackingCode::L3OCDP);
   CONTEST("GL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::CA);
   CONTEST("GL1S", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CD);
   CONTEST("GL1L", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CP);
   CONTEST("GL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CDP);
   CONTEST("GL1P", gpstk::CarrierBand::L1, gpstk::TrackingCode::P);
   CONTEST("GL1W", gpstk::CarrierBand::L1, gpstk::TrackingCode::Ztracking);
   CONTEST("GL1Y", gpstk::CarrierBand::L1, gpstk::TrackingCode::Y);
   CONTEST("GL1M", gpstk::CarrierBand::L1, gpstk::TrackingCode::MDP);
   CONTEST("GL1N", gpstk::CarrierBand::L1, gpstk::TrackingCode::YCodeless);
   CONTEST("GL2C", gpstk::CarrierBand::L2, gpstk::TrackingCode::CA);
   CONTEST("GL2D", gpstk::CarrierBand::L2, gpstk::TrackingCode::Semicodeless);
   CONTEST("GL2S", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CM);
   CONTEST("GL2L", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CL);
   CONTEST("GL2X", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CML);
   CONTEST("GL2P", gpstk::CarrierBand::L2, gpstk::TrackingCode::P);
   CONTEST("GL2W", gpstk::CarrierBand::L2, gpstk::TrackingCode::Ztracking);
   CONTEST("GL2Y", gpstk::CarrierBand::L2, gpstk::TrackingCode::Y);
   CONTEST("GL2M", gpstk::CarrierBand::L2, gpstk::TrackingCode::MDP);
   CONTEST("GL2N", gpstk::CarrierBand::L2, gpstk::TrackingCode::YCodeless);
   CONTEST("GL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5I);
   CONTEST("GL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5Q);
   CONTEST("GL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5IQ);
   CONTEST("IL5A", gpstk::CarrierBand::L5, gpstk::TrackingCode::SPSL5);
   CONTEST("IL5B", gpstk::CarrierBand::L5, gpstk::TrackingCode::RSL5D);
   CONTEST("IL5C", gpstk::CarrierBand::L5, gpstk::TrackingCode::RSL5P);
   CONTEST("IL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::RSL5DP);
   CONTEST("IL9A", gpstk::CarrierBand::I9, gpstk::TrackingCode::SPSS);
   CONTEST("IL9B", gpstk::CarrierBand::I9, gpstk::TrackingCode::RSSD);
   CONTEST("IL9C", gpstk::CarrierBand::I9, gpstk::TrackingCode::RSSP);
   CONTEST("IL9X", gpstk::CarrierBand::I9, gpstk::TrackingCode::RSSDP);
   CONTEST("JL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::CA);
   CONTEST("JL1L", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CP);
   CONTEST("JL1S", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CD);
   CONTEST("JL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1CDP);
   CONTEST("JL1Z", gpstk::CarrierBand::L1, gpstk::TrackingCode::L1S);
   CONTEST("JL2S", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CM);
   CONTEST("JL2L", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CL);
   CONTEST("JL2X", gpstk::CarrierBand::L2, gpstk::TrackingCode::L2CML);
   CONTEST("JL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5I);
   CONTEST("JL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5Q);
   CONTEST("JL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5IQ);
   CONTEST("JL5D", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5SI);
   CONTEST("JL5P", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5SQ);
   CONTEST("JL5Z", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5SIQ);
   CONTEST("JL6S", gpstk::CarrierBand::E6, gpstk::TrackingCode::LEXS);
      // This is a duplicate of the previous one only with
      // different expectations so we have to ignore one or the
      // other.  I chose to ignore this one since the previous one
      // is how we've been decoding things in the past.
      //CONTEST("JL6S", gpstk::CarrierBand::E6, gpstk::TrackingCode::L6D);
   CONTEST("JL6L", gpstk::CarrierBand::E6, gpstk::TrackingCode::LEXL);
   CONTEST("JL6X", gpstk::CarrierBand::E6, gpstk::TrackingCode::LEXSL);
   CONTEST("JL6E", gpstk::CarrierBand::E6, gpstk::TrackingCode::L6E);
   CONTEST("JL6Z", gpstk::CarrierBand::E6, gpstk::TrackingCode::L6DE);
   CONTEST("SL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::CA);
   CONTEST("SL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5I);
   CONTEST("SL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5Q);
   CONTEST("SL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::L5IQ);

   gpstk::RinexObsID wild("****", gpstk::Rinex3ObsBase::currentVersion);
   TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Any, wild.type);
   TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::Any, wild.band);
   TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Any, wild.code);

   TURETURN();
}


unsigned RinexObsID_T :: newIDTest()
{
   TUDEF("RinexObsID", "newID");
   std::string failMesg;

   failMesg = "[testing] RinexObsID::newID to redefine existing ID, [expected]"
      " exception gpstk::Exception, [actual] threw no exception";
   try
   {
      gpstk::RinexObsID::newID("C6Z", "L6 Z range");
      TUFAIL(failMesg);
   }
   catch(gpstk::Exception e)
   {
      TUPASS(failMesg);
   }

      //create a fictional ID completely
   gpstk::RinexObsID fic(gpstk::RinexObsID::newID("T9W", "L9 W test"));
   TUASSERT(gpstk::RinexObsID::char2ot.count('T') > 0);
   TUASSERT(gpstk::RinexObsID::char2cb.count('9') > 0);
   TUASSERT(gpstk::RinexObsID::char2tc.count('W') > 0);
   TUASSERTE(gpstk::ObservationType,
             fic.type, gpstk::RinexObsID::char2ot['T']);
   TUASSERTE(gpstk::CarrierBand,
             fic.band, gpstk::RinexObsID::char2cb['9']);
   TUASSERTE(gpstk::TrackingCode,
             fic.code, gpstk::RinexObsID::char2tc['W']);

   failMesg = "[testing] RinexObsID::newID to redefine existing ID, [expected]"
      " exception gpstk::Exception, [actual] threw no exception";
   try
   {
      gpstk::RinexObsID::newID("T9W", "L9 W test");
      TUFAIL(failMesg);
   }
   catch(gpstk::Exception e)
   {
      TUPASS(failMesg);
   }

   TURETURN();
}


int main() //Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   RinexObsID_T testClass;

   errorTotal += testClass.decodeTest();
   errorTotal += testClass.fromStringConstructorTest();
   errorTotal += testClass.newIDTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
