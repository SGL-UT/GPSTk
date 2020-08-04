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

#define CBDESCTEST(EXP, CARRIERBAND) TUASSERTE(std::string, EXP, gpstk::ObsID::cbDesc[CARRIERBAND])
#define TCDESCTEST(EXP, TRACKINGCODE) TUASSERTE(std::string, EXP, gpstk::ObsID::tcDesc[TRACKINGCODE])
#define OTDESCTEST(EXP, OBSTYPE) TUASSERTE(std::string, EXP, gpstk::ObsID::otDesc[OBSTYPE])

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


   unsigned otDescTest()
   {
      TUDEF("ObsID", "otDesc");
      OTDESCTEST("UnknownType", gpstk::ObservationType::Unknown);
      OTDESCTEST("AnyType", gpstk::ObservationType::Any);
      OTDESCTEST("pseudorange", gpstk::ObservationType::Range);
      OTDESCTEST("phase", gpstk::ObservationType::Phase);
      OTDESCTEST("doppler", gpstk::ObservationType::Doppler);
      OTDESCTEST("snr", gpstk::ObservationType::SNR);
      OTDESCTEST("channel", gpstk::ObservationType::Channel);
      OTDESCTEST("demodStatus", gpstk::ObservationType::DemodStat);
      OTDESCTEST("iono", gpstk::ObservationType::Iono);
      OTDESCTEST("ssi", gpstk::ObservationType::SSI);
      OTDESCTEST("lli", gpstk::ObservationType::LLI);
      OTDESCTEST("tlen", gpstk::ObservationType::TrackLen);
      OTDESCTEST("navmsg", gpstk::ObservationType::NavMsg);
      OTDESCTEST("rngSigma", gpstk::ObservationType::RngStdDev);
      OTDESCTEST("phsSigma", gpstk::ObservationType::PhsStdDev);
      OTDESCTEST("freqIndx", gpstk::ObservationType::FreqIndx);
      OTDESCTEST("undefined", gpstk::ObservationType::Undefined);
      TURETURN();
   }
};


int main()
{
   unsigned errorTotal = 0;
   ObsID_T testClass;

   errorTotal += testClass.initializationTest();
   errorTotal += testClass.dumpTest();
   errorTotal += testClass.asStringTest();
   errorTotal += testClass.asStringEnumTest();
   errorTotal += testClass.operatorTest();
   errorTotal += testClass.cbDescTest();
   errorTotal += testClass.tcDescTest();
   errorTotal += testClass.otDescTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}
