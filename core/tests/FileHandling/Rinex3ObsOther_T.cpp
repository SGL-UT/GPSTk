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
      /** Make sure that channel number pseudo-observables are written
       * to the file correctly. */
   unsigned channelNumTest();
      /** Make sure that ionospheric delay pseudo-observables are
       * written to the file correctly. */
   unsigned ionoDelayTest();
      /// generic filling of generic data.
   void setObs(gpstk::TestUtil& testFramework, const std::string& system,
               gpstk::Rinex3ObsHeader& hdr, gpstk::Rinex3ObsData& rod);
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
   gpstk::RinexObsID roidInvalid("GC1C");
   gpstk::RinexObsID roidValid("GL1C");
   gpstk::SatID sid(7, gpstk::SatID::systemGPS);
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

   gpstk::Rinex3ObsStream strm(outfn, std::ios::out | std::ios::trunc);
   gpstk::Rinex3ObsHeader hdr;
   gpstk::RinexObsID roidValid("GL1C");
   gpstk::SatID sid(7, gpstk::SatID::systemGPS);
   strm.exceptions(std::fstream::failbit);
      // Not setting most of the header fields because they're not being tested.
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1C"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1C"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1C"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1C"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1S"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1S"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1S"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1S"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1L"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1L"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1L"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1L"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1X"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1X"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1X"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1X"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1P"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1P"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1P"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1P"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1W"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1W"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1W"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1W"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1Y"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1Y"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1Y"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1Y"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1M"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1M"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1M"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1M"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1N"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1N"));
   hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1N"));
      // now have fun with channels
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GX1 ")));
   TUTHROW(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GX2Y")));
      // This should not appear in the header as it is redundant, but
      // it is still valid as an ObsID.
   gpstk::RinexObsID forced(gpstk::ObsID::otChannel, gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);
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

   gpstk::Rinex3ObsStream strm(outfn, std::ios::out | std::ios::trunc);
   gpstk::Rinex3ObsHeader hdr;
   gpstk::RinexObsID roidValid("GL1C");
   gpstk::SatID sid(7, gpstk::SatID::systemGPS);
   strm.exceptions(std::fstream::failbit);
      // Not setting most of the header fields because they're not being tested.
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC1C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL1C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD1C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS1C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI1 ")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GC2C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GL2C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GD2C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GS2C")));
   TUCATCH(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI2 ")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RC3I")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RL3I")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RD3I")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RS3I")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RI3 ")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RC4A")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RL4A")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RD4A")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RS4A")));
   TUCATCH(hdr.mapObsTypes["R"].push_back(gpstk::RinexObsID("RI4 ")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC5I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL5I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED5I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES5I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI5 ")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC6A")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL6A")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED6A")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES6A")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI6 ")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC7I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL7I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED7I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES7I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI7 ")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EC8I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EL8I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ED8I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("ES8I")));
   TUCATCH(hdr.mapObsTypes["E"].push_back(gpstk::RinexObsID("EI8 ")));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IC9A")));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IL9A")));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("ID9A")));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("IS9A")));
   TUCATCH(hdr.mapObsTypes["I"].push_back(gpstk::RinexObsID("II9 ")));
      // now have fun with decoding
   TUTHROW(hdr.mapObsTypes["G"].push_back(gpstk::RinexObsID("GI2Y")));
      // This should not appear in the header as it is redundant, but
      // it is still valid as an ObsID.
   gpstk::RinexObsID forced(gpstk::ObsID::otIono, gpstk::ObsID::cbL1,
                            gpstk::ObsID::tcCA);
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


int main()
{
   unsigned errorTotal = 0;
   Rinex3ObsOther_T testClass;

   errorTotal += testClass.phaseShiftTest();
   errorTotal += testClass.channelNumTest();
   errorTotal += testClass.ionoDelayTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
