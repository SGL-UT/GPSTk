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

#include "ObsID.hpp"

#include "TestUtil.hpp"
#include "Rinex3ObsHeader.hpp"
#include <iostream>
#include <sstream>

namespace gpstk
{
      // define some stream operators so that test failures involving
      // enums are a bit more readable.

   std::ostream& operator<<(std::ostream& s, gpstk::ObservationType e)
   {
      s << (long)e << " (" << StringUtils::asString(e) << ")";
      return s;
   }
   std::ostream& operator<<(std::ostream& s, CarrierBand e)
   {
      s << (long)e << " (" << StringUtils::asString(e) << ")";
      return s;
   }
   std::ostream& operator<<(std::ostream& s, gpstk::TrackingCode e)
   {
      s << (long)e << " (" << StringUtils::asString(e) << ")";
      return s;
   }
}

// This makes sure that
// 1) The RINEX obs ID decodes as expected
// 2) The RINEX obs ID encodes as expected
// 3) The wildcard (Any) ObsID enums match
#define CONTEST(RINEXCODE, CARRIERBAND, TRACKINGCODE) {               \
      TUCSM("ObsID(\"" RINEXCODE "\")");                              \
      gpstk::ObsID obs(RINEXCODE, gpstk::Rinex3ObsBase::currentVersion); \
      TUASSERTE(gpstk::ObservationType,                        \
                gpstk::ObservationType::Phase,                                \
                obs.type);                                            \
      TUASSERTE(gpstk::CarrierBand,                            \
                CARRIERBAND, obs.band);                               \
      TUASSERTE(gpstk::TrackingCode,                           \
                TRACKINGCODE, obs.code);                              \
      gpstk::RinexObsID obs2(gpstk::ObservationType::Phase, CARRIERBAND,      \
                             TRACKINGCODE);                           \
      TUASSERTE(std::string, std::string(RINEXCODE).substr(1),        \
                obs2.asString());                                     \
      gpstk::ObsID wildcard("****", gpstk::Rinex3ObsBase::currentVersion); \
      TUASSERTE(gpstk::ObsID, wildcard, obs);                         \
   }

#define CBDESCTEST(EXP, CARRIERBAND) TUASSERTE(std::string, EXP, gpstk::ObsID::cbDesc[CARRIERBAND])
#define TCDESCTEST(EXP, TRACKINGCODE) TUASSERTE(std::string, EXP, gpstk::ObsID::tcDesc[TRACKINGCODE])

class ObsID_T
{
public:

   unsigned initializationTest()
   {
      TUDEF("ObsID", "ObsID()");
      std::string failMesg;

      gpstk::ObsID empty;
      TUASSERTE(gpstk::ObservationType,
                gpstk::ObservationType::Unknown, empty.type);
      TUASSERTE(gpstk::CarrierBand,
                gpstk::CarrierBand::Unknown, empty.band);
      TUASSERTE(gpstk::TrackingCode,
                gpstk::TrackingCode::Unknown, empty.code);

      TUCSM("ObsID(ObservationType,CarrierBand,TrackingCode)");
      gpstk::ObsID compare(gpstk::ObservationType::Range, gpstk::CarrierBand::L1,
                           gpstk::TrackingCode::CA);
      TUASSERTE(gpstk::ObservationType,
                gpstk::ObservationType::Range, compare.type);
      TUASSERTE(gpstk::CarrierBand,
                gpstk::CarrierBand::L1, compare.band);
      TUASSERTE(gpstk::TrackingCode,
                gpstk::TrackingCode::CA, compare.code);

      TURETURN();
   }


   unsigned dumpTest()
   {
      TUDEF("ObsID", "dump");
      std::string failMesg;
      gpstk::ObsID compare(gpstk::ObservationType::Doppler, gpstk::CarrierBand::L2,
                           gpstk::TrackingCode::Y);

      std::string outputString, referenceString;
      std::stringstream outputStream;

      compare.dump(outputStream);
      outputString = outputStream.str();

      referenceString = "L2 GPSY doppler";

      TUASSERTE(std::string, referenceString, outputString);

      TURETURN();
   }


   unsigned asStringTest()
   {
      TUDEF("ObsID", "asString");
      std::string failMesg;
      gpstk::ObsID compare(gpstk::ObservationType::Phase, gpstk::CarrierBand::E5b,
                           gpstk::TrackingCode::IE5);

      std::string outputString, referenceString;

      outputString = gpstk::StringUtils::asString(compare);
      referenceString = "E5b GALI5 phase";

      TUASSERTE(std::string, referenceString, outputString);

      TURETURN();
   }


   unsigned asStringEnumTest()
   {
      TUDEF("ObsID", "asString");
         // These tests verify that every enum has a string
         // representation and every string has a corresponding enum.
         // It also implicitly verifies that the string
         // representations aren't duplicated, since if two enums
         // translated to string "XXX", the attempt to reverse the
         // translation would fail.
      for (gpstk::ObservationType ot : gpstk::ObservationTypeIterator())
      {
         std::string s;
         TUCATCH(s = gpstk::StringUtils::asString(ot));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::ObservationType ot2;
         TUCATCH(ot2 = gpstk::StringUtils::asObservationType(s));
         TUASSERTE(gpstk::ObservationType, ot, ot2);
      }
      for (gpstk::CarrierBand cb : gpstk::CarrierBandIterator())
      {
         std::string s;
         TUCATCH(s = gpstk::StringUtils::asString(cb));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::CarrierBand cb2;
         TUCATCH(cb2 = gpstk::StringUtils::asCarrierBand(s));
         TUASSERTE(gpstk::CarrierBand, cb, cb2);
      }
      for (gpstk::TrackingCode tc : gpstk::TrackingCodeIterator())
      {
         std::string s;
         TUCATCH(s = gpstk::StringUtils::asString(tc));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::TrackingCode tc2;
         TUCATCH(tc2 = gpstk::StringUtils::asTrackingCode(s));
         TUASSERTE(gpstk::TrackingCode, tc, tc2);
      }
      TURETURN();
   }


      //reads in 3-4 length string
   unsigned fromStringConstructorTest()
   {
      TUDEF("ObsID", "ObsID(string)");
      std::string failMesg;

         //set with invalid length
      failMesg = "[testing] ObsID constructor from invalid string, [expected]"
         " exception gpstk::Exception, [actual] threw no exception";
      try
      {
         gpstk::ObsID invalidID("G 10 ",gpstk::Rinex3ObsBase::currentVersion);
         TUFAIL(failMesg);
      }
      catch (gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

      try {
         gpstk::ObsID invalidID("G1",gpstk::Rinex3ObsBase::currentVersion);
         TUFAIL(failMesg);
      }
      catch (gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

         //testing base assign w/out using any of the reused codes
         // GPS L1 C/A PseudoRange
      gpstk::ObsID obs1("GC1C",gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Range,obs1.type);
      TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::L1,obs1.band);
      TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::CA,obs1.code);

         //testing only case of reassinged codes for GPS
         // GPS L5 IQ Doppler
      gpstk::ObsID obs2("GD5X",gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObservationType,
                gpstk::ObservationType::Doppler, obs2.type);
      TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::L5,obs2.band);
      TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::IQ5,obs2.code);

         //testing completely random case
         // QZSS E6 L Carrier Phase
      gpstk::ObsID obs3("JL6L",gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObservationType,gpstk::ObservationType::Phase,obs3.type);
      TUASSERTE(gpstk::CarrierBand,gpstk::CarrierBand::E6,obs3.band);
      TUASSERTE(gpstk::TrackingCode,gpstk::TrackingCode::JQ6,obs3.code);

      CONTEST("CL2I", gpstk::CarrierBand::B1, gpstk::TrackingCode::CI1);
      CONTEST("CL2Q", gpstk::CarrierBand::B1, gpstk::TrackingCode::CQ1);
      CONTEST("CL2X", gpstk::CarrierBand::B1, gpstk::TrackingCode::CIQ1);
      CONTEST("CL1D", gpstk::CarrierBand::L1, gpstk::TrackingCode::CCD1);
      CONTEST("CL1P", gpstk::CarrierBand::L1, gpstk::TrackingCode::CCP1);
      CONTEST("CL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::CCDP1);
      CONTEST("CL1A", gpstk::CarrierBand::L1, gpstk::TrackingCode::CA1);
      CONTEST("CL1N", gpstk::CarrierBand::L1, gpstk::TrackingCode::CodelessC);
         // these are only valid in rinex 3.02 and the macro defaults
         // to the current version, which is not 3.02.
      // CONTEST("CL1I", gpstk::CarrierBand::B1, gpstk::TrackingCode::CI1);
      // CONTEST("CL1Q", gpstk::CarrierBand::B1, gpstk::TrackingCode::CQ1);
      CONTEST("CL5D", gpstk::CarrierBand::L5, gpstk::TrackingCode::CI2a);
      CONTEST("CL5P", gpstk::CarrierBand::L5, gpstk::TrackingCode::CQ2a);
      CONTEST("CL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::CIQ2a);
      CONTEST("CL7I", gpstk::CarrierBand::B2, gpstk::TrackingCode::CI7);
      CONTEST("CL7Q", gpstk::CarrierBand::B2, gpstk::TrackingCode::CQ7);
      CONTEST("CL7X", gpstk::CarrierBand::B2, gpstk::TrackingCode::CIQ7);
      CONTEST("CL7D", gpstk::CarrierBand::B2, gpstk::TrackingCode::CI2b);
      CONTEST("CL7P", gpstk::CarrierBand::B2, gpstk::TrackingCode::CQ2b);
      CONTEST("CL7Z", gpstk::CarrierBand::B2, gpstk::TrackingCode::CIQ2b);
      CONTEST("CL8D", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::CI2ab);
      CONTEST("CL8P", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::CQ2ab);
      CONTEST("CL8X", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::CIQ2ab);
      CONTEST("CL6I", gpstk::CarrierBand::B3, gpstk::TrackingCode::CI6);
      CONTEST("CL6Q", gpstk::CarrierBand::B3, gpstk::TrackingCode::CQ6);
      CONTEST("CL6X", gpstk::CarrierBand::B3, gpstk::TrackingCode::CIQ6);
      CONTEST("CL6A", gpstk::CarrierBand::B3, gpstk::TrackingCode::CIQ3A);
      CONTEST("EL1A", gpstk::CarrierBand::L1, gpstk::TrackingCode::A);
      CONTEST("EL1B", gpstk::CarrierBand::L1, gpstk::TrackingCode::B);
      CONTEST("EL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::C);
      CONTEST("EL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::BC);
      CONTEST("EL1Z", gpstk::CarrierBand::L1, gpstk::TrackingCode::ABC);
      CONTEST("EL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::IE5a);
      CONTEST("EL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::QE5a);
      CONTEST("EL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::IQE5a);
      CONTEST("EL7I", gpstk::CarrierBand::E5b, gpstk::TrackingCode::IE5b);
      CONTEST("EL7Q", gpstk::CarrierBand::E5b, gpstk::TrackingCode::QE5b);
      CONTEST("EL7X", gpstk::CarrierBand::E5b, gpstk::TrackingCode::IQE5b);
      CONTEST("EL8I", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::IE5);
      CONTEST("EL8Q", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::QE5);
      CONTEST("EL8X", gpstk::CarrierBand::E5ab, gpstk::TrackingCode::IQE5);
      CONTEST("EL6A", gpstk::CarrierBand::E6, gpstk::TrackingCode::A6);
      CONTEST("EL6B", gpstk::CarrierBand::E6, gpstk::TrackingCode::B6);
      CONTEST("EL6C", gpstk::CarrierBand::E6, gpstk::TrackingCode::C6);
      CONTEST("EL6X", gpstk::CarrierBand::E6, gpstk::TrackingCode::BC6);
      CONTEST("EL6Z", gpstk::CarrierBand::E6, gpstk::TrackingCode::ABC6);
      CONTEST("RL1C", gpstk::CarrierBand::G1, gpstk::TrackingCode::GCA);
      CONTEST("RL1P", gpstk::CarrierBand::G1, gpstk::TrackingCode::GP);
      CONTEST("RL4A", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OCD);
      CONTEST("RL4B", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OCP);
      CONTEST("RL4X", gpstk::CarrierBand::G1a, gpstk::TrackingCode::L1OC);
      CONTEST("RL2C", gpstk::CarrierBand::G2, gpstk::TrackingCode::GCA);
      CONTEST("RL2P", gpstk::CarrierBand::G2, gpstk::TrackingCode::GP);
      CONTEST("RL6A", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2CSI);
      CONTEST("RL6B", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2OCP);
      CONTEST("RL6X", gpstk::CarrierBand::G2a, gpstk::TrackingCode::L2CSIOCp);
      CONTEST("RL3I", gpstk::CarrierBand::G3, gpstk::TrackingCode::IR3);
      CONTEST("RL3Q", gpstk::CarrierBand::G3, gpstk::TrackingCode::QR3);
      CONTEST("RL3X", gpstk::CarrierBand::G3, gpstk::TrackingCode::IQR3);
      CONTEST("GL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::CA);
      CONTEST("GL1S", gpstk::CarrierBand::L1, gpstk::TrackingCode::G1D);
      CONTEST("GL1L", gpstk::CarrierBand::L1, gpstk::TrackingCode::G1P);
      CONTEST("GL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::G1X);
      CONTEST("GL1P", gpstk::CarrierBand::L1, gpstk::TrackingCode::P);
      CONTEST("GL1W", gpstk::CarrierBand::L1, gpstk::TrackingCode::W);
      CONTEST("GL1Y", gpstk::CarrierBand::L1, gpstk::TrackingCode::Y);
      CONTEST("GL1M", gpstk::CarrierBand::L1, gpstk::TrackingCode::M);
      CONTEST("GL1N", gpstk::CarrierBand::L1, gpstk::TrackingCode::N);
      CONTEST("GL2C", gpstk::CarrierBand::L2, gpstk::TrackingCode::CA);
      CONTEST("GL2D", gpstk::CarrierBand::L2, gpstk::TrackingCode::D);
      CONTEST("GL2S", gpstk::CarrierBand::L2, gpstk::TrackingCode::C2M);
      CONTEST("GL2L", gpstk::CarrierBand::L2, gpstk::TrackingCode::C2L);
      CONTEST("GL2X", gpstk::CarrierBand::L2, gpstk::TrackingCode::C2LM);
      CONTEST("GL2P", gpstk::CarrierBand::L2, gpstk::TrackingCode::P);
      CONTEST("GL2W", gpstk::CarrierBand::L2, gpstk::TrackingCode::W);
      CONTEST("GL2Y", gpstk::CarrierBand::L2, gpstk::TrackingCode::Y);
      CONTEST("GL2M", gpstk::CarrierBand::L2, gpstk::TrackingCode::M);
      CONTEST("GL2N", gpstk::CarrierBand::L2, gpstk::TrackingCode::N);
      CONTEST("GL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::I5);
      CONTEST("GL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::Q5);
      CONTEST("GL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::IQ5);
      CONTEST("IL5A", gpstk::CarrierBand::L5, gpstk::TrackingCode::IA5);
      CONTEST("IL5B", gpstk::CarrierBand::L5, gpstk::TrackingCode::IB5);
      CONTEST("IL5C", gpstk::CarrierBand::L5, gpstk::TrackingCode::IC5);
      CONTEST("IL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::IX5);
      CONTEST("IL9A", gpstk::CarrierBand::I9, gpstk::TrackingCode::IA9);
      CONTEST("IL9B", gpstk::CarrierBand::I9, gpstk::TrackingCode::IB9);
      CONTEST("IL9C", gpstk::CarrierBand::I9, gpstk::TrackingCode::IC9);
      CONTEST("IL9X", gpstk::CarrierBand::I9, gpstk::TrackingCode::IX9);
      CONTEST("JL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::JCA);
      CONTEST("JL1L", gpstk::CarrierBand::L1, gpstk::TrackingCode::JP1);
      CONTEST("JL1S", gpstk::CarrierBand::L1, gpstk::TrackingCode::JD1);
      CONTEST("JL1X", gpstk::CarrierBand::L1, gpstk::TrackingCode::JX1);
      CONTEST("JL1Z", gpstk::CarrierBand::L1, gpstk::TrackingCode::JZ1);
      CONTEST("JL2S", gpstk::CarrierBand::L2, gpstk::TrackingCode::JM2);
      CONTEST("JL2L", gpstk::CarrierBand::L2, gpstk::TrackingCode::JL2);
      CONTEST("JL2X", gpstk::CarrierBand::L2, gpstk::TrackingCode::JX2);
      CONTEST("JL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::JI5);
      CONTEST("JL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::JQ5);
      CONTEST("JL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::JIQ5);
      CONTEST("JL5D", gpstk::CarrierBand::L5, gpstk::TrackingCode::JI5S);
      CONTEST("JL5P", gpstk::CarrierBand::L5, gpstk::TrackingCode::JQ5S);
      CONTEST("JL5Z", gpstk::CarrierBand::L5, gpstk::TrackingCode::JIQ5S);
      CONTEST("JL6S", gpstk::CarrierBand::E6, gpstk::TrackingCode::JI6);
         // This is a duplicate of the previous one only with
         // different expectations so we have to ignore one or the
         // other.  I chose to ignore this one since the previous one
         // is how we've been decoding things in the past.
         //CONTEST("JL6S", gpstk::CarrierBand::E6, gpstk::TrackingCode::JD6);
      CONTEST("JL6L", gpstk::CarrierBand::E6, gpstk::TrackingCode::JQ6);
      CONTEST("JL6X", gpstk::CarrierBand::E6, gpstk::TrackingCode::JIQ6);
      CONTEST("JL6E", gpstk::CarrierBand::E6, gpstk::TrackingCode::JE6);
      CONTEST("JL6Z", gpstk::CarrierBand::E6, gpstk::TrackingCode::JDE6);
      CONTEST("SL1C", gpstk::CarrierBand::L1, gpstk::TrackingCode::SCA);
      CONTEST("SL5I", gpstk::CarrierBand::L5, gpstk::TrackingCode::SI5);
      CONTEST("SL5Q", gpstk::CarrierBand::L5, gpstk::TrackingCode::SQ5);
      CONTEST("SL5X", gpstk::CarrierBand::L5, gpstk::TrackingCode::SIQ5);

      gpstk::ObsID wild("****", gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObservationType, gpstk::ObservationType::Any, wild.type);
      TUASSERTE(gpstk::CarrierBand, gpstk::CarrierBand::Any, wild.band);
      TUASSERTE(gpstk::TrackingCode, gpstk::TrackingCode::Any, wild.code);

      TURETURN();
   }

   unsigned operatorTest()
   {
      TUDEF("ObsID", "operator==");
      std::string failMesg;

      gpstk::ObsID compare1(gpstk::ObservationType::Range, gpstk::CarrierBand::L1,
                            gpstk::TrackingCode::CA);
      gpstk::ObsID compare2(gpstk::ObservationType::Range, gpstk::CarrierBand::L1,
                            gpstk::TrackingCode::CA);
      gpstk::ObsID compare3(gpstk::ObservationType::Doppler, gpstk::CarrierBand::L1,
                            gpstk::TrackingCode::CA);

      TUASSERTE(gpstk::ObsID, compare1, compare2);
      TUASSERT(!(compare1 == compare3));
      TUCSM("operator!=");
      TUASSERT(compare1 != compare3);
      TUASSERT(!(compare1 != compare2));
      TURETURN();
   }


   unsigned newIDTest()
   {
      TUDEF("ObsID", "newID");
      std::string failMesg;

      failMesg = "[testing] ObsID::newID to redefine existing ID, [expected]"
         " exception gpstk::Exception, [actual] threw no exception";
      try
      {
         gpstk::ObsID::newID("C6Z", "L6 Z range");
         TUFAIL(failMesg);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

         //create a fictional ID completely
      gpstk::ObsID fic(gpstk::ObsID::newID("T9W", "L9 W test"));
      TUASSERT(gpstk::ObsID::char2ot.count('T') > 0);
      TUASSERT(gpstk::ObsID::char2cb.count('9') > 0);
      TUASSERT(gpstk::ObsID::char2tc.count('W') > 0);
      TUASSERTE(gpstk::ObservationType,
                fic.type, gpstk::ObsID::char2ot['T']);
      TUASSERTE(gpstk::CarrierBand,
                fic.band, gpstk::ObsID::char2cb['9']);
      TUASSERTE(gpstk::TrackingCode,
                fic.code, gpstk::ObsID::char2tc['W']);

      failMesg = "[testing] ObsID::newID to redefine existing ID, [expected]"
         " exception gpstk::Exception, [actual] threw no exception";
      try
      {
         gpstk::ObsID::newID("T9W", "L9 W test");
         TUFAIL(failMesg);
      }
      catch(gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

      TURETURN();
   }


   unsigned cbDescTest()
   {
      TUDEF("ObsID", "cbDesc");
      CBDESCTEST("B1", gpstk::CarrierBand::B1);
      CBDESCTEST("B2", gpstk::CarrierBand::B2);
      CBDESCTEST("B3", gpstk::CarrierBand::B3);
      CBDESCTEST("comboL1L2", gpstk::CarrierBand::L1L2);
      CBDESCTEST("E5a+b", gpstk::CarrierBand::E5ab);
      CBDESCTEST("E5b", gpstk::CarrierBand::E5b);
      CBDESCTEST("E6", gpstk::CarrierBand::E6);
      CBDESCTEST("G1", gpstk::CarrierBand::G1);
      CBDESCTEST("G2", gpstk::CarrierBand::G2);
      CBDESCTEST("G3", gpstk::CarrierBand::G3);
      CBDESCTEST("L1", gpstk::CarrierBand::L1);
      CBDESCTEST("L2", gpstk::CarrierBand::L2);
      CBDESCTEST("L5", gpstk::CarrierBand::L5);
      TURETURN();
   }


   unsigned tcDescTest()
   {
      TUDEF("ObsID", "tcDesc");
      TCDESCTEST("BDSIB1", gpstk::TrackingCode::CI1);
      TCDESCTEST("BDSIB2", gpstk::TrackingCode::CI7);
      TCDESCTEST("BDSIB3", gpstk::TrackingCode::CI6);
      TCDESCTEST("BDSI+QB2", gpstk::TrackingCode::CIQ7);
      TCDESCTEST("BDSI+QB3", gpstk::TrackingCode::CIQ6);
      TCDESCTEST("BDSQB1", gpstk::TrackingCode::CQ1);
      TCDESCTEST("BDSQB2", gpstk::TrackingCode::CQ7);
      TCDESCTEST("BDSQB3", gpstk::TrackingCode::CQ6);
      TCDESCTEST("GALA+B+C", gpstk::TrackingCode::ABC);
      TCDESCTEST("GALA", gpstk::TrackingCode::A);
      TCDESCTEST("GALB+C", gpstk::TrackingCode::BC);
      TCDESCTEST("GALB", gpstk::TrackingCode::B);
      TCDESCTEST("GALC", gpstk::TrackingCode::C);
      TCDESCTEST("GALI5a", gpstk::TrackingCode::IE5a);
      TCDESCTEST("GALI5b", gpstk::TrackingCode::IE5b);
      TCDESCTEST("GALI5", gpstk::TrackingCode::IE5);
      TCDESCTEST("GALI+Q5a", gpstk::TrackingCode::IQE5a);
      TCDESCTEST("GALI+Q5b", gpstk::TrackingCode::IQE5b);
      TCDESCTEST("GALI+Q5", gpstk::TrackingCode::IQE5);
      TCDESCTEST("GALQ5a", gpstk::TrackingCode::QE5a);
      TCDESCTEST("GALQ5b", gpstk::TrackingCode::QE5b);
      TCDESCTEST("GALQ5", gpstk::TrackingCode::QE5);
      TCDESCTEST("GLOC/A", gpstk::TrackingCode::GCA);
      TCDESCTEST("GLOI+QR5", gpstk::TrackingCode::IQR3);
      TCDESCTEST("GLOIR5", gpstk::TrackingCode::IR3);
      TCDESCTEST("GLOP", gpstk::TrackingCode::GP);
      TCDESCTEST("GLOQR5", gpstk::TrackingCode::QR3);
      TCDESCTEST("GPSC1D", gpstk::TrackingCode::G1D);
      TCDESCTEST("GPSC1(D+P)", gpstk::TrackingCode::G1X);
      TCDESCTEST("GPSC1P", gpstk::TrackingCode::G1P);
      TCDESCTEST("GPSC2L", gpstk::TrackingCode::C2L);
      TCDESCTEST("GPSC2L+M", gpstk::TrackingCode::C2LM);
      TCDESCTEST("GPSC2M", gpstk::TrackingCode::C2M);
      TCDESCTEST("GPSC/A", gpstk::TrackingCode::CA);
      TCDESCTEST("GPScodeless", gpstk::TrackingCode::D);
      TCDESCTEST("GPScodelessZ", gpstk::TrackingCode::W);
      TCDESCTEST("GPSI5", gpstk::TrackingCode::I5);
      TCDESCTEST("GPSI+Q5", gpstk::TrackingCode::IQ5);
      TCDESCTEST("GPSM", gpstk::TrackingCode::M);
      TCDESCTEST("GPSP", gpstk::TrackingCode::P);
      TCDESCTEST("GPSQ5", gpstk::TrackingCode::Q5);
      TCDESCTEST("GPSsquare", gpstk::TrackingCode::N);
      TCDESCTEST("GPSY", gpstk::TrackingCode::Y);
      TCDESCTEST("IRNSSL5A", gpstk::TrackingCode::IA5);
      TCDESCTEST("IRNSSL5B+C", gpstk::TrackingCode::IX5);
      TCDESCTEST("IRNSSL5B", gpstk::TrackingCode::IB5);
      TCDESCTEST("IRNSSL5C", gpstk::TrackingCode::IC5);
      TCDESCTEST("IRNSSL9A", gpstk::TrackingCode::IA9);
      TCDESCTEST("IRNSSL9B+C", gpstk::TrackingCode::IX9);
      TCDESCTEST("IRNSSL9B", gpstk::TrackingCode::IB9);
      TCDESCTEST("IRNSSL9C", gpstk::TrackingCode::IC9);
      TCDESCTEST("QZSSC/A", gpstk::TrackingCode::JCA);
      TCDESCTEST("QZSSL1C(D)", gpstk::TrackingCode::JD1);
      TCDESCTEST("QZSSL1C(D+P)", gpstk::TrackingCode::JX1);
      TCDESCTEST("QZSSL1C(P)", gpstk::TrackingCode::JP1);
      TCDESCTEST("QZSSL1-SAIF", gpstk::TrackingCode::JZ1);
      TCDESCTEST("QZSSL2C(L)", gpstk::TrackingCode::JL2);
      TCDESCTEST("QZSSL2C(M)", gpstk::TrackingCode::JM2);
      TCDESCTEST("QZSSL2C(M+L)", gpstk::TrackingCode::JX2);
      TCDESCTEST("QZSSL5I", gpstk::TrackingCode::JI5);
      TCDESCTEST("QZSSL5I+Q", gpstk::TrackingCode::JIQ5);
      TCDESCTEST("QZSSL5Q", gpstk::TrackingCode::JQ5);
      TCDESCTEST("QZSSL6I", gpstk::TrackingCode::JI6);
      TCDESCTEST("QZSSL6I+Q", gpstk::TrackingCode::JIQ6);
      TCDESCTEST("QZSSL6Q", gpstk::TrackingCode::JQ6);
      TCDESCTEST("SBASC/A", gpstk::TrackingCode::SCA);
      TCDESCTEST("SBASI5", gpstk::TrackingCode::SI5);
      TCDESCTEST("SBASI+Q5", gpstk::TrackingCode::SIQ5);
      TCDESCTEST("SBASQ5", gpstk::TrackingCode::SQ5);
      TURETURN();
   }
};


int main()
{
   unsigned errorTotal = 0;
   ObsID_T testClass;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.fromStringConstructorTest();
   errorTotal += testClass.dumpTest();
   errorTotal += testClass.newIDTest();
   errorTotal += testClass.asStringTest();
   errorTotal += testClass.asStringEnumTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.cbDescTest();
   errorTotal += testClass.tcDescTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}
