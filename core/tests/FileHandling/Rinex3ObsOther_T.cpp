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

#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <string>

using namespace std;

/** Perform tests that aren't in Rinex3Obs_FromScratch_T or
 * Rinex3Obs_T.  These will probably get merged into one of the other
 * two at some point but are being added in a new file to facilitate
 * review. */
class Rinex3ObsOther_T
{
public:
      /** Make sure that non-phase measurements don't get output in
       * the header phase shift field. */
   unsigned phaseShiftTest();
      /** Make sure that header fields use the correct observation
       * codes for the version of the RINEX header being written. */
   unsigned obsIDVersionTest();
      /** Make sure that channel number pseudo-observables are written
       * to the file correctly. */
   unsigned channelNumTest();
      /** Make sure that ionospheric delay pseudo-observables are
       * written to the file correctly. */
   unsigned ionoDelayTest();
      /// generic filling of generic data.
   void setObs(gpstk::TestUtil& testFramework, const std::string& system,
               gpstk::Rinex3ObsHeader& hdr, gpstk::Rinex3ObsData& rod);
      /// Fill a v3.02 header with data for the obsIDVersionTest method.
   void fillHeader302(gpstk::Rinex3ObsHeader& hdr);
      /// Fill a v3.04 header with data for the obsIDVersionTest method.
   void fillHeader304(gpstk::Rinex3ObsHeader& hdr);
};


unsigned Rinex3ObsOther_T ::
phaseShiftTest()
{
   TUDEF("Rinex3ObsHeader", "writeHeaderRecords (PHASE SHIFT)");

   std::string outfn = gpstk::getPathTestTemp() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_PHASE_SHIFT.out";
   std::string expfn = gpstk::getPathData() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_PHASE_SHIFT.exp";

   gpstk::Rinex3ObsStream strm(outfn, std::ios::out | std::ios::trunc);
   gpstk::Rinex3ObsHeader hdr;
   gpstk::RinexObsID roidInvalid("GC1C",gpstk::Rinex3ObsBase::currentVersion);
   gpstk::RinexObsID roidValid("GL1C", gpstk::Rinex3ObsBase::currentVersion);
   gpstk::SatID sid(7, gpstk::SatelliteSystem::GPS);
   strm.exceptions(std::fstream::failbit);
      // Not setting most of the header fields because they're not being tested.
      // Basically, the phase shift record for "GL1C" should be
      // present in the header, but the phase shift record for "GC1C"
      // should not.
   hdr.sysPhaseShift["G"][roidInvalid][sid] = 23.456;
   hdr.sysPhaseShift["G"][roidValid][sid] = 54.321;
   hdr.date = "20200512 181734 UTC";
   hdr.preserveDate = true;
   hdr.version = 3.04;
   hdr.valid |= gpstk::Rinex3ObsHeader::validVersion;
   hdr.valid |= gpstk::Rinex3ObsHeader::validRunBy;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerName;
   hdr.valid |= gpstk::Rinex3ObsHeader::validObserver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validReceiver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaPosition;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaDeltaHEN;
   hdr.valid |= gpstk::Rinex3ObsHeader::validFirstTime;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerNumber;
   hdr.valid |= gpstk::Rinex3ObsHeader::validInterval;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemNumObs;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemPhaseShift;
   hdr.validEoH = true;
   TUCATCH(strm << hdr);
   TUCMPFILE(expfn, outfn, 0);
   TURETURN();
}


unsigned Rinex3ObsOther_T ::
channelNumTest()
{
      // Note that Rinex3ObsData is also being tested
   TUDEF("Rinex3ObsHeader", "writeHeaderRecords (SYS / # / OBS TYPES)");

   std::string outfn = gpstk::getPathTestTemp() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_SYS_NUM_OBS_TYPES.out";
   std::string expfn = gpstk::getPathData() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_SYS_NUM_OBS_TYPES.exp";

      // abbreviate.
   double cv = gpstk::Rinex3ObsBase::currentVersion;
   gpstk::Rinex3ObsStream strm(outfn, std::ios::out | std::ios::trunc);
   gpstk::Rinex3ObsHeader hdr;
   gpstk::RinexObsID roidValid("GL1C", cv);
   gpstk::SatID sid(7, gpstk::SatelliteSystem::GPS);
   strm.exceptions(std::fstream::failbit);
      // Not setting most of the header fields because they're not being tested.
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1C", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1C", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1C", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1C", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1S", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1S", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1S", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1S", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1L", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1L", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1L", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1L", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1X", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1X", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1X", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1X", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1P", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1P", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1P", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1P", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1W", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1W", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1W", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1W", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1Y", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1Y", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1Y", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1Y", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1M", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1M", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1M", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1M", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1N", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1N", cv));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1N", cv));
      // now have fun with channels
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GX1 ", cv)));
   TUTHROW(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GX2Y", cv)));
      // This should not appear in the header as it is redundant, but
      // it is still valid as an ObsID.
   gpstk::RinexObsID forced(gpstk::ObservationType::Channel, gpstk::CarrierBand::L1,
                            gpstk::TrackingCode::CA);
   TUCATCH(hdr.mapObsTypes["G"].push_back(forced));
   hdr.sysPhaseShift["G"][roidValid][sid] = 54.321;
   hdr.date = "20200512 181734 UTC";
   hdr.preserveDate = true;
   hdr.version = cv;
   hdr.valid |= gpstk::Rinex3ObsHeader::validVersion;
   hdr.valid |= gpstk::Rinex3ObsHeader::validRunBy;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerName;
   hdr.valid |= gpstk::Rinex3ObsHeader::validObserver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validReceiver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaPosition;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaDeltaHEN;
   hdr.valid |= gpstk::Rinex3ObsHeader::validFirstTime;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerNumber;
   hdr.valid |= gpstk::Rinex3ObsHeader::validInterval;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemNumObs;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemPhaseShift;
   hdr.validEoH = true;
   TUCATCH(strm << hdr);
   gpstk::Rinex3ObsData rod;
   gpstk::RinexDatum data;
   data.dataBlank = false;
   data.lliBlank = true;
   data.ssiBlank = true;
   rod.time = gpstk::CivilTime(2020,3,11,12,0,0,gpstk::TimeSystem::GPS);
   rod.epochFlag = 0;
   rod.numSVs = 1;
   rod.clockOffset = 0;
   const gpstk::Rinex3ObsHeader::RinexObsVec &rov(hdr.mapObsTypes["G"]);
      // set a data value for each observable including the redundant
      // channel number, which should get printed out as concatenated
      // two-digit numbers.
   for (unsigned i = 0; i < rov.size(); i++)
   {
      data.data++;
      rod.setObs(data, sid, rov[i], hdr);
   }
   TUCATCH(strm << rod);
   TUCMPFILE(expfn, outfn, 0);
   TURETURN();
}


unsigned Rinex3ObsOther_T ::
ionoDelayTest()
{
      // Note that Rinex3ObsData is also being tested
   TUDEF("Rinex3ObsHeader", "writeHeaderRecords (SYS / # / OBS TYPES)");

   std::string outfn = gpstk::getPathTestTemp() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_IonoDelay.out";
   std::string expfn = gpstk::getPathData() + gpstk::getFileSep() +
      "rinex3ObsTest_v304_IonoDelay.exp";

      // abbreviate.
   double cv = gpstk::Rinex3ObsBase::currentVersion;
   gpstk::Rinex3ObsStream strm(outfn, std::ios::out | std::ios::trunc);
   gpstk::Rinex3ObsHeader hdr;
   gpstk::RinexObsID roidValid("GL1C", cv);
   gpstk::SatID sid(7, gpstk::SatelliteSystem::GPS);
   strm.exceptions(std::fstream::failbit);
      // Not setting most of the header fields because they're not being tested.
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI1 ", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC2C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL2C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD2C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS2C", cv)));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI2 ", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RC3I", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RL3I", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RD3I", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RS3I", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RI3 ", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RC4A", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RL4A", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RD4A", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RS4A", cv)));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RI4 ", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC5I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL5I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED5I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES5I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI5 ", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC6A", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL6A", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED6A", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES6A", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI6 ", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC7I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL7I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED7I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES7I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI7 ", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC8I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL8I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED8I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES8I", cv)));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI8 ", cv)));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IC9A", cv)));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IL9A", cv)));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("ID9A", cv)));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IS9A", cv)));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("II9 ", cv)));
      // now have fun with decoding
   TUTHROW(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI2Y", cv)));
      // This should not appear in the header as it is redundant, but
      // it is still valid as an ObsID.
   gpstk::RinexObsID forced(gpstk::ObservationType::Iono, gpstk::CarrierBand::L1,
                            gpstk::TrackingCode::CA);
   TUCATCH(hdr.mapObsTypes["G"].push_back(forced));
   hdr.sysPhaseShift["G"][roidValid][sid] = 54.321;
   hdr.date = "20200512 181734 UTC";
   hdr.preserveDate = true;
   hdr.version = 3.04;
   hdr.valid |= gpstk::Rinex3ObsHeader::validVersion;
   hdr.valid |= gpstk::Rinex3ObsHeader::validRunBy;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerName;
   hdr.valid |= gpstk::Rinex3ObsHeader::validObserver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validReceiver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaPosition;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaDeltaHEN;
   hdr.valid |= gpstk::Rinex3ObsHeader::validFirstTime;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerNumber;
   hdr.valid |= gpstk::Rinex3ObsHeader::validInterval;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemNumObs;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemPhaseShift;
   hdr.valid |= gpstk::Rinex3ObsHeader::validGlonassSlotFreqNo;
   hdr.valid |= gpstk::Rinex3ObsHeader::validGlonassCodPhsBias;
   hdr.validEoH = true;
   TUCATCH(strm << hdr);
   gpstk::Rinex3ObsData rod;
   rod.time = gpstk::CivilTime(2020,3,11,12,0,0,gpstk::TimeSystem::GPS);
   rod.epochFlag = 0;
   rod.numSVs = 1;
   rod.clockOffset = 0;
   for (const auto& motIter : hdr.mapObsTypes)
   {
      setObs(testFramework, motIter.first, hdr, rod);
   }
   TUCSM("operator<<");
   TUCATCH(strm << rod);
   TUCMPFILE(expfn, outfn, 0);
   TURETURN();
}


void Rinex3ObsOther_T ::
setObs(gpstk::TestUtil& testFramework, const std::string& system,
       gpstk::Rinex3ObsHeader& hdr, gpstk::Rinex3ObsData& rod)
{
   gpstk::RinexSatID sid;
   gpstk::RinexDatum data;
   data.dataBlank = false;
   data.lliBlank = true;
   data.ssiBlank = true;
   sid.fromString(system + "07");
   const gpstk::Rinex3ObsHeader::RinexObsVec &rov(hdr.mapObsTypes[system]);
      // set a data value for each observable including the redundant
      // channel number, which should get printed out as concatenated
      // two-digit numbers.
   for (unsigned i = 0; i < rov.size(); i++)
   {
      data.data++;
      TUCSM("setObs(" + rov[i].asString() + ")");
      TUCATCH(rod.setObs(data, sid, rov[i], hdr));
   }
}


unsigned Rinex3ObsOther_T ::
obsIDVersionTest()
{
   TUDEF("Rinex3ObsHeader", "writeHeaderRecords");

   std::string outtmpl = gpstk::getPathTestTemp() + gpstk::getFileSep() +
      "rinex3ObsIDHeaderTest_v";
   std::string exptmpl = gpstk::getPathData() + gpstk::getFileSep() +
      "rinex3ObsIDHeaderTest_v";

      // test writing a header containing 3.02 obs to a 3.02 file
   try
   {
      gpstk::Rinex3ObsStream strm(outtmpl + "302.out",
                                  std::ios::out | std::ios::trunc);
      gpstk::Rinex3ObsHeader hdr;
      strm.exceptions(std::fstream::failbit);
      fillHeader302(hdr);
      TUCATCH(strm << hdr);
      TUCMPFILE(exptmpl + "302.exp", outtmpl + "302.out", 0);
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
      TUFAIL("Exception");
   }
   catch (std::exception& exc)
   {
      cerr << "caught " << exc.what() << endl;
      TUFAIL("Exception");
   }
   catch (...)
   {
      TUFAIL("Unknown exception");
   }

      // test writing a header containing 3.02 obs to a 3.04 file
   try
   {
      gpstk::Rinex3ObsStream strm(outtmpl + "304.out",
                                  std::ios::out | std::ios::trunc);
      gpstk::Rinex3ObsHeader hdr;
      strm.exceptions(std::fstream::failbit);
      fillHeader302(hdr);
      hdr.version = 3.04;
      TUCATCH(strm << hdr);
      TUCMPFILE(exptmpl + "304.exp", outtmpl + "304.out", 0);
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
      TUFAIL("Exception");
   }
   catch (std::exception& exc)
   {
      cerr << "caught " << exc.what() << endl;
      TUFAIL("Exception");
   }
   catch (...)
   {
      TUFAIL("Unknown exception");
   }

      // test writing a header containing 3.04 obs to a 3.02 file
   try
   {
      gpstk::Rinex3ObsStream strm(outtmpl + "302b.out",
                                  std::ios::out | std::ios::trunc);
      gpstk::Rinex3ObsHeader hdr;
      strm.exceptions(std::fstream::failbit);
      fillHeader304(hdr);
      hdr.version = 3.02;
      TUCATCH(strm << hdr);
      TUCMPFILE(exptmpl + "302.exp", outtmpl + "302b.out", 0);
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
      TUFAIL("Exception");
   }
   catch (std::exception& exc)
   {
      cerr << "caught " << exc.what() << endl;
      TUFAIL("Exception");
   }
   catch (...)
   {
      TUFAIL("Unknown exception");
   }

      // test writing a header containing 3.04 obs to a 3.04 file
   try
   {
      gpstk::Rinex3ObsStream strm(outtmpl + "304b.out",
                                  std::ios::out | std::ios::trunc);
      gpstk::Rinex3ObsHeader hdr;
      strm.exceptions(std::fstream::failbit);
      fillHeader304(hdr);
      hdr.version = 3.04;
      TUCATCH(strm << hdr);
      TUCMPFILE(exptmpl + "304.exp", outtmpl + "304b.out", 0);
   }
   catch (gpstk::Exception& exc)
   {
      cerr << exc;
      TUFAIL("Exception");
   }
   catch (std::exception& exc)
   {
      cerr << "caught " << exc.what() << endl;
      TUFAIL("Exception");
   }
   catch (...)
   {
      TUFAIL("Unknown exception");
   }
   TURETURN();
}


void Rinex3ObsOther_T ::
fillHeader302(gpstk::Rinex3ObsHeader& hdr)
{
   gpstk::SatID sid(7, gpstk::SatelliteSystem::BeiDou);
   hdr.date = "20200512 181734 UTC";
   hdr.preserveDate = true;
   hdr.version = 3.02;
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC1I", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL1I", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD1I", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS1I", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC1Q", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL1Q", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD1Q", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS1Q", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC7X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL7X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD7X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS7X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC1X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL1X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD1X", 3.02));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS1X", 3.02));
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL1I", 3.02)][sid] = 2.345;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL1Q", 3.02)][sid] = 6.789;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL7X", 3.02)][sid] = 8.765;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL1X", 3.02)][sid] = 0.123;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC1I", 3.02)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL1I", 3.02)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD1I", 3.02)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS1I", 3.02)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC1Q", 3.02)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL1Q", 3.02)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD1Q", 3.02)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS1Q", 3.02)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC7X", 3.02)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL7X", 3.02)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD7X", 3.02)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS7X", 3.02)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC1X", 3.02)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL1X", 3.02)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD1X", 3.02)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS1X", 3.02)] = 1000;
   hdr.valid |= gpstk::Rinex3ObsHeader::validVersion;
   hdr.valid |= gpstk::Rinex3ObsHeader::validRunBy;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerName;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validObserver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validReceiver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaPosition;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaDeltaHEN;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemNumObs;
   hdr.valid |= gpstk::Rinex3ObsHeader::validFirstTime;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemPhaseShift;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemScaleFac;
   hdr.validEoH = true;
}


void Rinex3ObsOther_T ::
fillHeader304(gpstk::Rinex3ObsHeader& hdr)
{
   gpstk::SatID sid(7, gpstk::SatelliteSystem::BeiDou);
   hdr.date = "20200512 181734 UTC";
   hdr.preserveDate = true;
   hdr.version = 3.04;
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC2I", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL2I", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD2I", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS2I", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC2Q", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL2Q", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD2Q", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS2Q", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC7X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL7X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD7X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS7X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CC2X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CL2X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CD2X", 3.04));
   hdr.mapObsTypes["C"].push_back(gpstk::RinexObsID("CS2X", 3.04));
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL2I", 3.04)][sid] = 2.345;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL2Q", 3.04)][sid] = 6.789;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL7X", 3.04)][sid] = 8.765;
   hdr.sysPhaseShift["C"][gpstk::RinexObsID("CL2X", 3.04)][sid] = 0.123;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC2I", 3.04)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL2I", 3.04)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD2I", 3.04)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS2I", 3.04)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC2Q", 3.04)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL2Q", 3.04)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD2Q", 3.04)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS2Q", 3.04)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC7X", 3.04)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL7X", 3.04)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD7X", 3.04)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS7X", 3.04)] = 1000;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CC2X", 3.04)] = 1;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CL2X", 3.04)] = 10;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CD2X", 3.04)] = 100;
   hdr.sysSfacMap["C"][gpstk::RinexObsID("CS2X", 3.04)] = 1000;
   hdr.valid |= gpstk::Rinex3ObsHeader::validVersion;
   hdr.valid |= gpstk::Rinex3ObsHeader::validRunBy;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerName;
   hdr.valid |= gpstk::Rinex3ObsHeader::validMarkerType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validObserver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validReceiver;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaType;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaPosition;
   hdr.valid |= gpstk::Rinex3ObsHeader::validAntennaDeltaHEN;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemNumObs;
   hdr.valid |= gpstk::Rinex3ObsHeader::validFirstTime;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemPhaseShift;
   hdr.valid |= gpstk::Rinex3ObsHeader::validSystemScaleFac;
   hdr.validEoH = true;
}


int main()
{
   unsigned errorTotal = 0;
   Rinex3ObsOther_T testClass;

   errorTotal += testClass.phaseShiftTest();
   errorTotal += testClass.channelNumTest();
   errorTotal += testClass.ionoDelayTest();
   errorTotal += testClass.obsIDVersionTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
