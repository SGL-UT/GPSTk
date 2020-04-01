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
         gpstk::ObsID invalidID("G 10 ");
         TUFAIL(failMesg);
      }
      catch (gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

      try {
         gpstk::ObsID invalidID("G1");
         TUFAIL(failMesg);
      }
      catch (gpstk::Exception e)
      {
         TUPASS(failMesg);
      }

         //testing base assign w/out using any of the reused codes
      gpstk::ObsID obs1("GC1C"); // GPS L1 C/A PseudoRange
      TUASSERTE(gpstk::ObsID::ObservationType,gpstk::ObsID::otRange,obs1.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbL1,obs1.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcCA,obs1.code);

         //testing only case of reassinged codes for GPS
      gpstk::ObsID obs2("GD5X"); // GPS L5 IQ Doppler
      TUASSERTE(gpstk::ObsID::ObservationType,
                gpstk::ObsID::otDoppler, obs2.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbL5,obs2.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcIQ5,obs2.code);

         //testing completely random case
      gpstk::ObsID obs3("JL6L"); // QZSS E6 L Carrier Phase
      TUASSERTE(gpstk::ObsID::ObservationType,gpstk::ObsID::otPhase,obs3.type);
      TUASSERTE(gpstk::ObsID::CarrierBand,gpstk::ObsID::cbE6,obs3.band);
      TUASSERTE(gpstk::ObsID::TrackingCode,gpstk::ObsID::tcJQ6,obs3.code);

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

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}
