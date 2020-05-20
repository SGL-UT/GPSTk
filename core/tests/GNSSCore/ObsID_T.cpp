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

   std::ostream& operator<<(std::ostream& s, ObsID::ObservationType e)
   {
      s << (long)e << " (" << ObsID::asString(e) << ")";
      return s;
   }
   std::ostream& operator<<(std::ostream& s, ObsID::CarrierBand e)
   {
      s << (long)e << " (" << ObsID::asString(e) << ")";
      return s;
   }
   std::ostream& operator<<(std::ostream& s, ObsID::TrackingCode e)
   {
      s << (long)e << " (" << ObsID::asString(e) << ")";
      return s;
   }
}

#define CONTEST(RINEXCODE, CARRIERBAND, TRACKINGCODE) {               \
      TUCSM("ObsID(\"" RINEXCODE "\")");                              \
      gpstk::ObsID obs(RINEXCODE, gpstk::Rinex3ObsBase::currentVersion); \
      TUASSERTE(gpstk::ObsID::ObservationType,                        \
                gpstk::ObsID::otPhase,                                \
                obs.type);                                            \
      TUASSERTE(gpstk::ObsID::CarrierBand,                            \
                CARRIERBAND, obs.band);                               \
      TUASSERTE(gpstk::ObsID::TrackingCode,                           \
                TRACKINGCODE, obs.code);                              \
      gpstk::RinexObsID obs2(gpstk::ObsID::otPhase, CARRIERBAND,      \
                             TRACKINGCODE);                           \
      TUASSERTE(std::string, std::string(RINEXCODE).substr(1),        \
                obs2.asString());                                     \
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
      TUASSERTE(gpstk::ObsID::ObservationType,
                gpstk::ObsID::otUnknown, empty.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,
                gpstk::ObsID::cbUnknown, empty.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,
                gpstk::ObsID::tcUnknown, empty.code);

      TUCSM("ObsID(ObservationType,CarrierBand,TrackingCode)");
      gpstk::ObsID compare(gpstk::ObsID::otRange, gpstk::ObsID::cbL1,
                           gpstk::ObsID::tcCA);
      TUASSERTE(gpstk::ObsID::ObservationType,
                gpstk::ObsID::otRange, compare.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,
                gpstk::ObsID::cbL1, compare.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,
                gpstk::ObsID::tcCA, compare.code);

      TURETURN();
   }


   unsigned dumpTest()
   {
      TUDEF("ObsID", "dump");
      std::string failMesg;
      gpstk::ObsID compare(gpstk::ObsID::otDoppler, gpstk::ObsID::cbL2,
                           gpstk::ObsID::tcY);

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
      gpstk::ObsID compare(gpstk::ObsID::otPhase, gpstk::ObsID::cbE5b,
                           gpstk::ObsID::tcIE5);

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
      for (unsigned i = 0; i < gpstk::ObsID::otLast; i++)
      {
         gpstk::ObsID::ObservationType ot = (gpstk::ObsID::ObservationType)i;
         std::string s;
         TUCATCH(s = gpstk::ObsID::asString(ot));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::ObsID::ObservationType ot2;
         TUCATCH(ot2 = gpstk::ObsID::asObservationType(s));
         TUASSERTE(gpstk::ObsID::ObservationType, ot, ot2);
      }
      for (unsigned i = 0; i < gpstk::ObsID::cbLast; i++)
      {
         gpstk::ObsID::CarrierBand cb = (gpstk::ObsID::CarrierBand)i;
         std::string s;
         TUCATCH(s = gpstk::ObsID::asString(cb));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::ObsID::CarrierBand cb2;
         TUCATCH(cb2 = gpstk::ObsID::asCarrierBand(s));
         TUASSERTE(gpstk::ObsID::CarrierBand, cb, cb2);
      }
      for (unsigned i = 0; i < gpstk::ObsID::tcLast; i++)
      {
         gpstk::ObsID::TrackingCode tc = (gpstk::ObsID::TrackingCode)i;
         std::string s;
         TUCATCH(s = gpstk::ObsID::asString(tc));
         TUASSERT(!s.empty());
         TUASSERT(s != "???");
         gpstk::ObsID::TrackingCode tc2;
         TUCATCH(tc2 = gpstk::ObsID::asTrackingCode(s));
         TUASSERTE(gpstk::ObsID::TrackingCode, tc, tc2);
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
      TUASSERTE(gpstk::ObsID::ObservationType,gpstk::ObsID::otRange,obs1.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbL1,obs1.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcCA,obs1.code);

         //testing only case of reassinged codes for GPS
         // GPS L5 IQ Doppler
      gpstk::ObsID obs2("GD5X",gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObsID::ObservationType,
                gpstk::ObsID::otDoppler, obs2.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbL5,obs2.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcIQ5,obs2.code);

         //testing completely random case
         // QZSS E6 L Carrier Phase
      gpstk::ObsID obs3("JL6L",gpstk::Rinex3ObsBase::currentVersion);
      TUASSERTE(gpstk::ObsID::ObservationType,gpstk::ObsID::otPhase,obs3.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbE6,obs3.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcJQ6,obs3.code);

      CONTEST("CL2I", gpstk::ObsID::cbB1, gpstk::ObsID::tcCI1);
      CONTEST("CL2Q", gpstk::ObsID::cbB1, gpstk::ObsID::tcCQ1);
      CONTEST("CL2X", gpstk::ObsID::cbB1, gpstk::ObsID::tcCIQ1);
      CONTEST("CL1D", gpstk::ObsID::cbL1, gpstk::ObsID::tcCCD1);
      CONTEST("CL1P", gpstk::ObsID::cbL1, gpstk::ObsID::tcCCP1);
      CONTEST("CL1X", gpstk::ObsID::cbL1, gpstk::ObsID::tcCCDP1);
      CONTEST("CL1A", gpstk::ObsID::cbL1, gpstk::ObsID::tcCA1);
      CONTEST("CL1N", gpstk::ObsID::cbL1, gpstk::ObsID::tcCodelessC);
         // these are only valid in rinex 3.02 and the macro defaults
         // to the current version, which is not 3.02.
      // CONTEST("CL1I", gpstk::ObsID::cbB1, gpstk::ObsID::tcCI1);
      // CONTEST("CL1Q", gpstk::ObsID::cbB1, gpstk::ObsID::tcCQ1);
      CONTEST("CL5D", gpstk::ObsID::cbL5, gpstk::ObsID::tcCI2a);
      CONTEST("CL5P", gpstk::ObsID::cbL5, gpstk::ObsID::tcCQ2a);
      CONTEST("CL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcCIQ2a);
      CONTEST("CL7I", gpstk::ObsID::cbB2, gpstk::ObsID::tcCI7);
      CONTEST("CL7Q", gpstk::ObsID::cbB2, gpstk::ObsID::tcCQ7);
      CONTEST("CL7X", gpstk::ObsID::cbB2, gpstk::ObsID::tcCIQ7);
      CONTEST("CL7D", gpstk::ObsID::cbB2, gpstk::ObsID::tcCI2b);
      CONTEST("CL7P", gpstk::ObsID::cbB2, gpstk::ObsID::tcCQ2b);
      CONTEST("CL7Z", gpstk::ObsID::cbB2, gpstk::ObsID::tcCIQ2b);
      CONTEST("CL8D", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcCI2ab);
      CONTEST("CL8P", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcCQ2ab);
      CONTEST("CL8X", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcCIQ2ab);
      CONTEST("CL6I", gpstk::ObsID::cbB3, gpstk::ObsID::tcCI6);
      CONTEST("CL6Q", gpstk::ObsID::cbB3, gpstk::ObsID::tcCQ6);
      CONTEST("CL6X", gpstk::ObsID::cbB3, gpstk::ObsID::tcCIQ6);
      CONTEST("CL6A", gpstk::ObsID::cbB3, gpstk::ObsID::tcCIQ3A);
      CONTEST("EL1A", gpstk::ObsID::cbL1, gpstk::ObsID::tcA);
      CONTEST("EL1B", gpstk::ObsID::cbL1, gpstk::ObsID::tcB);
      CONTEST("EL1C", gpstk::ObsID::cbL1, gpstk::ObsID::tcC);
      CONTEST("EL1X", gpstk::ObsID::cbL1, gpstk::ObsID::tcBC);
      CONTEST("EL1Z", gpstk::ObsID::cbL1, gpstk::ObsID::tcABC);
      CONTEST("EL5I", gpstk::ObsID::cbL5, gpstk::ObsID::tcIE5a);
      CONTEST("EL5Q", gpstk::ObsID::cbL5, gpstk::ObsID::tcQE5a);
      CONTEST("EL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcIQE5a);
      CONTEST("EL7I", gpstk::ObsID::cbE5b, gpstk::ObsID::tcIE5b);
      CONTEST("EL7Q", gpstk::ObsID::cbE5b, gpstk::ObsID::tcQE5b);
      CONTEST("EL7X", gpstk::ObsID::cbE5b, gpstk::ObsID::tcIQE5b);
      CONTEST("EL8I", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcIE5);
      CONTEST("EL8Q", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcQE5);
      CONTEST("EL8X", gpstk::ObsID::cbE5ab, gpstk::ObsID::tcIQE5);
      CONTEST("EL6A", gpstk::ObsID::cbE6, gpstk::ObsID::tcA6);
      CONTEST("EL6B", gpstk::ObsID::cbE6, gpstk::ObsID::tcB6);
      CONTEST("EL6C", gpstk::ObsID::cbE6, gpstk::ObsID::tcC6);
      CONTEST("EL6X", gpstk::ObsID::cbE6, gpstk::ObsID::tcBC6);
      CONTEST("EL6Z", gpstk::ObsID::cbE6, gpstk::ObsID::tcABC6);
      CONTEST("RL1C", gpstk::ObsID::cbG1, gpstk::ObsID::tcGCA);
      CONTEST("RL1P", gpstk::ObsID::cbG1, gpstk::ObsID::tcGP);
      CONTEST("RL4A", gpstk::ObsID::cbG1a, gpstk::ObsID::tcL1OCD);
      CONTEST("RL4B", gpstk::ObsID::cbG1a, gpstk::ObsID::tcL1OCP);
      CONTEST("RL4X", gpstk::ObsID::cbG1a, gpstk::ObsID::tcL1OC);
      CONTEST("RL2C", gpstk::ObsID::cbG2, gpstk::ObsID::tcGCA);
      CONTEST("RL2P", gpstk::ObsID::cbG2, gpstk::ObsID::tcGP);
      CONTEST("RL6A", gpstk::ObsID::cbG2a, gpstk::ObsID::tcL2CSI);
      CONTEST("RL6B", gpstk::ObsID::cbG2a, gpstk::ObsID::tcL2OCP);
      CONTEST("RL6X", gpstk::ObsID::cbG2a, gpstk::ObsID::tcL2CSIOCp);
      CONTEST("RL3I", gpstk::ObsID::cbG3, gpstk::ObsID::tcIR3);
      CONTEST("RL3Q", gpstk::ObsID::cbG3, gpstk::ObsID::tcQR3);
      CONTEST("RL3X", gpstk::ObsID::cbG3, gpstk::ObsID::tcIQR3);
      CONTEST("GL1C", gpstk::ObsID::cbL1, gpstk::ObsID::tcCA);
      CONTEST("GL1S", gpstk::ObsID::cbL1, gpstk::ObsID::tcG1D);
      CONTEST("GL1L", gpstk::ObsID::cbL1, gpstk::ObsID::tcG1P);
      CONTEST("GL1X", gpstk::ObsID::cbL1, gpstk::ObsID::tcG1X);
      CONTEST("GL1P", gpstk::ObsID::cbL1, gpstk::ObsID::tcP);
      CONTEST("GL1W", gpstk::ObsID::cbL1, gpstk::ObsID::tcW);
      CONTEST("GL1Y", gpstk::ObsID::cbL1, gpstk::ObsID::tcY);
      CONTEST("GL1M", gpstk::ObsID::cbL1, gpstk::ObsID::tcM);
      CONTEST("GL1N", gpstk::ObsID::cbL1, gpstk::ObsID::tcN);
      CONTEST("GL2C", gpstk::ObsID::cbL2, gpstk::ObsID::tcCA);
      CONTEST("GL2D", gpstk::ObsID::cbL2, gpstk::ObsID::tcD);
      CONTEST("GL2S", gpstk::ObsID::cbL2, gpstk::ObsID::tcC2M);
      CONTEST("GL2L", gpstk::ObsID::cbL2, gpstk::ObsID::tcC2L);
      CONTEST("GL2X", gpstk::ObsID::cbL2, gpstk::ObsID::tcC2LM);
      CONTEST("GL2P", gpstk::ObsID::cbL2, gpstk::ObsID::tcP);
      CONTEST("GL2W", gpstk::ObsID::cbL2, gpstk::ObsID::tcW);
      CONTEST("GL2Y", gpstk::ObsID::cbL2, gpstk::ObsID::tcY);
      CONTEST("GL2M", gpstk::ObsID::cbL2, gpstk::ObsID::tcM);
      CONTEST("GL2N", gpstk::ObsID::cbL2, gpstk::ObsID::tcN);
      CONTEST("GL5I", gpstk::ObsID::cbL5, gpstk::ObsID::tcI5);
      CONTEST("GL5Q", gpstk::ObsID::cbL5, gpstk::ObsID::tcQ5);
      CONTEST("GL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcIQ5);
      CONTEST("IL5A", gpstk::ObsID::cbL5, gpstk::ObsID::tcIA5);
      CONTEST("IL5B", gpstk::ObsID::cbL5, gpstk::ObsID::tcIB5);
      CONTEST("IL5C", gpstk::ObsID::cbL5, gpstk::ObsID::tcIC5);
      CONTEST("IL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcIX5);
      CONTEST("IL9A", gpstk::ObsID::cbI9, gpstk::ObsID::tcIA9);
      CONTEST("IL9B", gpstk::ObsID::cbI9, gpstk::ObsID::tcIB9);
      CONTEST("IL9C", gpstk::ObsID::cbI9, gpstk::ObsID::tcIC9);
      CONTEST("IL9X", gpstk::ObsID::cbI9, gpstk::ObsID::tcIX9);
      CONTEST("JL1C", gpstk::ObsID::cbL1, gpstk::ObsID::tcJCA);
      CONTEST("JL1L", gpstk::ObsID::cbL1, gpstk::ObsID::tcJP1);
      CONTEST("JL1S", gpstk::ObsID::cbL1, gpstk::ObsID::tcJD1);
      CONTEST("JL1X", gpstk::ObsID::cbL1, gpstk::ObsID::tcJX1);
      CONTEST("JL1Z", gpstk::ObsID::cbL1, gpstk::ObsID::tcJZ1);
      CONTEST("JL2S", gpstk::ObsID::cbL2, gpstk::ObsID::tcJM2);
      CONTEST("JL2L", gpstk::ObsID::cbL2, gpstk::ObsID::tcJL2);
      CONTEST("JL2X", gpstk::ObsID::cbL2, gpstk::ObsID::tcJX2);
      CONTEST("JL5I", gpstk::ObsID::cbL5, gpstk::ObsID::tcJI5);
      CONTEST("JL5Q", gpstk::ObsID::cbL5, gpstk::ObsID::tcJQ5);
      CONTEST("JL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcJIQ5);
      CONTEST("JL5D", gpstk::ObsID::cbL5, gpstk::ObsID::tcJI5S);
      CONTEST("JL5P", gpstk::ObsID::cbL5, gpstk::ObsID::tcJQ5S);
      CONTEST("JL5Z", gpstk::ObsID::cbL5, gpstk::ObsID::tcJIQ5S);
      CONTEST("JL6S", gpstk::ObsID::cbE6, gpstk::ObsID::tcJI6);
         // This is a duplicate of the previous one only with
         // different expectations so we have to ignore one or the
         // other.  I chose to ignore this one since the previous one
         // is how we've been decoding things in the past.
         //CONTEST("JL6S", gpstk::ObsID::cbE6, gpstk::ObsID::tcJD6);
      CONTEST("JL6L", gpstk::ObsID::cbE6, gpstk::ObsID::tcJQ6);
      CONTEST("JL6X", gpstk::ObsID::cbE6, gpstk::ObsID::tcJIQ6);
      CONTEST("JL6E", gpstk::ObsID::cbE6, gpstk::ObsID::tcJE6);
      CONTEST("JL6Z", gpstk::ObsID::cbE6, gpstk::ObsID::tcJDE6);
      CONTEST("SL1C", gpstk::ObsID::cbL1, gpstk::ObsID::tcSCA);
      CONTEST("SL5I", gpstk::ObsID::cbL5, gpstk::ObsID::tcSI5);
      CONTEST("SL5Q", gpstk::ObsID::cbL5, gpstk::ObsID::tcSQ5);
      CONTEST("SL5X", gpstk::ObsID::cbL5, gpstk::ObsID::tcSIQ5);

      TURETURN();
   }

   unsigned operatorTest()
   {
      TUDEF("ObsID", "operator==");
      std::string failMesg;

      gpstk::ObsID compare1(gpstk::ObsID::otRange, gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);
      gpstk::ObsID compare2(gpstk::ObsID::otRange, gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);
      gpstk::ObsID compare3(gpstk::ObsID::otDoppler, gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);

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
      TUASSERTE(gpstk::ObsID::ObservationType,
                fic.type, gpstk::ObsID::char2ot['T']);
      TUASSERTE(gpstk::ObsID::CarrierBand,
                fic.band, gpstk::ObsID::char2cb['9']);
      TUASSERTE(gpstk::ObsID::TrackingCode,
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
      CBDESCTEST("B1", gpstk::ObsID::cbB1);
      CBDESCTEST("B2", gpstk::ObsID::cbB2);
      CBDESCTEST("B3", gpstk::ObsID::cbB3);
      CBDESCTEST("comboL1L2", gpstk::ObsID::cbL1L2);
      CBDESCTEST("E5a+b", gpstk::ObsID::cbE5ab);
      CBDESCTEST("E5b", gpstk::ObsID::cbE5b);
      CBDESCTEST("E6", gpstk::ObsID::cbE6);
      CBDESCTEST("G1", gpstk::ObsID::cbG1);
      CBDESCTEST("G2", gpstk::ObsID::cbG2);
      CBDESCTEST("G3", gpstk::ObsID::cbG3);
      CBDESCTEST("L1", gpstk::ObsID::cbL1);
      CBDESCTEST("L2", gpstk::ObsID::cbL2);
      CBDESCTEST("L5", gpstk::ObsID::cbL5);
      TURETURN();
   }


   unsigned tcDescTest()
   {
      TUDEF("ObsID", "tcDesc");
      TCDESCTEST("BDSIB1", gpstk::ObsID::tcCI1);
      TCDESCTEST("BDSIB2", gpstk::ObsID::tcCI7);
      TCDESCTEST("BDSIB3", gpstk::ObsID::tcCI6);
      TCDESCTEST("BDSI+QB2", gpstk::ObsID::tcCIQ7);
      TCDESCTEST("BDSI+QB3", gpstk::ObsID::tcCIQ6);
      TCDESCTEST("BDSQB1", gpstk::ObsID::tcCQ1);
      TCDESCTEST("BDSQB2", gpstk::ObsID::tcCQ7);
      TCDESCTEST("BDSQB3", gpstk::ObsID::tcCQ6);
      TCDESCTEST("GALA+B+C", gpstk::ObsID::tcABC);
      TCDESCTEST("GALA", gpstk::ObsID::tcA);
      TCDESCTEST("GALB+C", gpstk::ObsID::tcBC);
      TCDESCTEST("GALB", gpstk::ObsID::tcB);
      TCDESCTEST("GALC", gpstk::ObsID::tcC);
      TCDESCTEST("GALI5a", gpstk::ObsID::tcIE5a);
      TCDESCTEST("GALI5b", gpstk::ObsID::tcIE5b);
      TCDESCTEST("GALI5", gpstk::ObsID::tcIE5);
      TCDESCTEST("GALI+Q5a", gpstk::ObsID::tcIQE5a);
      TCDESCTEST("GALI+Q5b", gpstk::ObsID::tcIQE5b);
      TCDESCTEST("GALI+Q5", gpstk::ObsID::tcIQE5);
      TCDESCTEST("GALQ5a", gpstk::ObsID::tcQE5a);
      TCDESCTEST("GALQ5b", gpstk::ObsID::tcQE5b);
      TCDESCTEST("GALQ5", gpstk::ObsID::tcQE5);
      TCDESCTEST("GLOC/A", gpstk::ObsID::tcGCA);
      TCDESCTEST("GLOI+QR5", gpstk::ObsID::tcIQR3);
      TCDESCTEST("GLOIR5", gpstk::ObsID::tcIR3);
      TCDESCTEST("GLOP", gpstk::ObsID::tcGP);
      TCDESCTEST("GLOQR5", gpstk::ObsID::tcQR3);
      TCDESCTEST("GPSC1D", gpstk::ObsID::tcG1D);
      TCDESCTEST("GPSC1(D+P)", gpstk::ObsID::tcG1X);
      TCDESCTEST("GPSC1P", gpstk::ObsID::tcG1P);
      TCDESCTEST("GPSC2L", gpstk::ObsID::tcC2L);
      TCDESCTEST("GPSC2L+M", gpstk::ObsID::tcC2LM);
      TCDESCTEST("GPSC2M", gpstk::ObsID::tcC2M);
      TCDESCTEST("GPSC/A", gpstk::ObsID::tcCA);
      TCDESCTEST("GPScodeless", gpstk::ObsID::tcD);
      TCDESCTEST("GPScodelessZ", gpstk::ObsID::tcW);
      TCDESCTEST("GPSI5", gpstk::ObsID::tcI5);
      TCDESCTEST("GPSI+Q5", gpstk::ObsID::tcIQ5);
      TCDESCTEST("GPSM", gpstk::ObsID::tcM);
      TCDESCTEST("GPSP", gpstk::ObsID::tcP);
      TCDESCTEST("GPSQ5", gpstk::ObsID::tcQ5);
      TCDESCTEST("GPSsquare", gpstk::ObsID::tcN);
      TCDESCTEST("GPSY", gpstk::ObsID::tcY);
      TCDESCTEST("IRNSSL5A", gpstk::ObsID::tcIA5);
      TCDESCTEST("IRNSSL5B+C", gpstk::ObsID::tcIX5);
      TCDESCTEST("IRNSSL5B", gpstk::ObsID::tcIB5);
      TCDESCTEST("IRNSSL5C", gpstk::ObsID::tcIC5);
      TCDESCTEST("IRNSSL9A", gpstk::ObsID::tcIA9);
      TCDESCTEST("IRNSSL9B+C", gpstk::ObsID::tcIX9);
      TCDESCTEST("IRNSSL9B", gpstk::ObsID::tcIB9);
      TCDESCTEST("IRNSSL9C", gpstk::ObsID::tcIC9);
      TCDESCTEST("QZSSC/A", gpstk::ObsID::tcJCA);
      TCDESCTEST("QZSSL1C(D)", gpstk::ObsID::tcJD1);
      TCDESCTEST("QZSSL1C(D+P)", gpstk::ObsID::tcJX1);
      TCDESCTEST("QZSSL1C(P)", gpstk::ObsID::tcJP1);
      TCDESCTEST("QZSSL1-SAIF", gpstk::ObsID::tcJZ1);
      TCDESCTEST("QZSSL2C(L)", gpstk::ObsID::tcJL2);
      TCDESCTEST("QZSSL2C(M)", gpstk::ObsID::tcJM2);
      TCDESCTEST("QZSSL2C(M+L)", gpstk::ObsID::tcJX2);
      TCDESCTEST("QZSSL5I", gpstk::ObsID::tcJI5);
      TCDESCTEST("QZSSL5I+Q", gpstk::ObsID::tcJIQ5);
      TCDESCTEST("QZSSL5Q", gpstk::ObsID::tcJQ5);
      TCDESCTEST("QZSSL6I", gpstk::ObsID::tcJI6);
      TCDESCTEST("QZSSL6I+Q", gpstk::ObsID::tcJIQ6);
      TCDESCTEST("QZSSL6Q", gpstk::ObsID::tcJQ6);
      TCDESCTEST("SBASC/A", gpstk::ObsID::tcSCA);
      TCDESCTEST("SBASI5", gpstk::ObsID::tcSI5);
      TCDESCTEST("SBASI+Q5", gpstk::ObsID::tcSIQ5);
      TCDESCTEST("SBASQ5", gpstk::ObsID::tcSQ5);
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
