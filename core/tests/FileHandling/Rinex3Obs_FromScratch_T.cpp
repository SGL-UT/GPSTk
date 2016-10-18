#include "Rinex3ObsStream.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <string>

using namespace gpstk;
using namespace std;

class Rinex3Obs_FromScratch_T {

public:

      // @param satString entered into header.mapObsTypes to test throw for bad satString
      // @param testID a string to identify the test for output files and debugging
      // @param includeConflict include two RinexObsIDs that convert to the same R2 Obs Type
   void Rinex3ObsFromScratch(string satString, string testID = "",
                             bool includeConflict = false, bool completeR = false) {

      Rinex3ObsHeader header;
      Rinex3ObsData data;

      header.fileProgram = "rinex3Test";
      std::ostringstream ostr;
      ostr << gpstk::CivilTime(TimeSystem::GPS);
      header.date = ostr.str();
      header.fileAgency = "From_Scratch_T_" + testID;
      header.valid |= Rinex3ObsHeader::validRunBy;
      header.markerName = "NCCM1";
      header.valid |= Rinex3ObsHeader::validMarkerName;
      header.observer = "Monitor Station";
      header.agency = "NGA";
      header.valid |= Rinex3ObsHeader::validObserver;
      header.recNo = "1";
      header.recType = "ITT MSN SAASM";
      header.recVers = "3.2.13";
      header.valid |= Rinex3ObsHeader::validReceiver;
      header.antNo = "1";
      header.antType = "Topcon CR-G5";
      header.valid |= Rinex3ObsHeader::validAntennaType;
      Triple ntPosition(-18000.27, -4991850.12, 3957008.50);
      header.antennaPosition = ntPosition;
      header.valid |= Rinex3ObsHeader::validAntennaPosition;
      Triple ntDelta(0, 0, 0);
      header.antennaDeltaHEN = ntDelta;
      header.valid |= Rinex3ObsHeader::validAntennaDeltaHEN;
      header.firstObs = CivilTime(2016, 10, 2, 11, 15, 30.0, TimeSystem::GPS);
      header.valid |= Rinex3ObsHeader::validFirstTime;
      header.markerNumber = "85401";
      header.valid |= Rinex3ObsHeader::validMarkerNumber;
      header.interval = 30;
      header.valid |= Rinex3ObsHeader::validInterval;
      header.validEoH = true;

      // As of Sep 2016, the following aren't working correctly so force
      // these to be set.
      // what should happen is that the Rinex3ObsHeader should fill in default values
      // for the SystemPhaseShift
      header.valid |= Rinex3ObsHeader::validSystemPhaseShift;
      if(completeR) {
         header.glonassFreqNo = Rinex3ObsHeader::GLOFreqNumMap();
         header.valid |= Rinex3ObsHeader::validGlonassSlotFreqNo;
         header.glonassCodPhsBias = Rinex3ObsHeader::GLOCodPhsBias();
         header.valid |= Rinex3ObsHeader::validGlonassCodPhsBias;
      }

      data = Rinex3ObsData();
      data.epochFlag = 0;
      data.numSVs = 2;
      data.clockOffset = 0;
      data.time = CommonTime().set(985354, 40530, 0.0, TimeSystem::GPS);

       std::vector<RinexObsID> newObsIds = setupObsIDs(includeConflict);


       RinexDatum datumL1_1;
       datumL1_1.data = 1;
       datumL1_1.lli = 0;
       datumL1_1.ssi = 0;
       RinexDatum datumL1_2;
       datumL1_2.data = 2;
       datumL1_2.lli = 0;
       datumL1_2.ssi = 0;


       RinexDatum datumP1_1;
       datumP1_1.data = 3;
       datumP1_1.lli = 0;
       datumP1_1.ssi = 0;
       RinexDatum datumP1_2;
       datumP1_2.data = 4;
       datumP1_2.lli = 0;
       datumP1_2.ssi = 0;


       RinexDatum datumC1_1;
       datumC1_1.data = 5;
       datumC1_1.lli = 0;
       datumC1_1.ssi = 0;
       RinexDatum datumC1_2;
       datumC1_2.data = 6;
       datumC1_2.lli = 0;
       datumC1_2.ssi = 0;


       RinexDatum datumL2_1;
       datumL2_1.data = 7;
       datumL2_1.lli = 0;
       datumL2_1.ssi = 0;
       RinexDatum datumL2_2;
       datumL2_2.data = 8;
       datumL2_2.lli = 0;
       datumL2_2.ssi = 0;

       RinexDatum datumP2_1;
       datumP2_1.data = 9;
       datumP2_1.lli = 0;
       datumP2_1.ssi = 0;
       RinexDatum datumP2_2;
       datumP2_2.data = 10;
       datumP2_2.lli = 0;
       datumP2_2.ssi = 0;

      header.mapObsTypes.insert(std::pair<std::string, std::vector<RinexObsID> >(satString, newObsIds));
      header.valid |= Rinex3ObsHeader::validNumObs;
      header.valid |= Rinex3ObsHeader::validSystemNumObs;

      std::vector<RinexDatum> datumVec1(5);
      std::vector<RinexDatum> datumVec2(5);
      datumVec1[0] = datumL1_1;
      datumVec2[0] = datumL1_2;
      datumVec1[1] = datumP1_1;
      datumVec2[1] = datumP1_2;
      datumVec1[2] = datumC1_1;
      datumVec2[2] = datumC1_2;
      datumVec1[3] = datumL2_1;
      datumVec2[3] = datumL2_2;
      datumVec1[4] = datumP2_1;
      datumVec2[4] = datumP2_2;
      RinexSatID GPS3 = RinexSatID("03");
      RinexSatID GPS6 = RinexSatID("06");
      data.obs.insert(std::pair<RinexSatID, std::vector<RinexDatum> >(GPS3, datumVec1));
      data.obs.insert(std::pair<RinexSatID, std::vector<RinexDatum> >(GPS6, datumVec2));


      string dataFilePath = gpstk::getPathData();
      string file_sep = getFileSep();

      header.version = 3.02;
      header.valid |= Rinex3ObsHeader::validVersion;
      Rinex3ObsStream *strm = new Rinex3ObsStream(dataFilePath+file_sep+"rinex3Test_v302_"+testID+".16o.exp",
                                    std::ios::out | std::ios::trunc);
//      strm->exceptions(std::basic_fstream::failbit);
      strm->exceptions(ifstream::failbit);
      *strm << header;
      *strm << data;

      header.version = 2.11;
      header.valid |= Rinex3ObsHeader::validVersion;
      Rinex3ObsStream *strm2 = new Rinex3ObsStream(dataFilePath+file_sep+"rinex3Test_v211_"+testID+".16o.exp",
                                     std::ios::out | std::ios::trunc);
      strm2->exceptions(ifstream::failbit);
      *strm2 << header;
      *strm2 << data;
       delete strm, strm2;
   }

    std::vector<RinexObsID> setupObsIDs(bool includeConflict){
        std::vector<RinexObsID> newObsIds;


        RinexObsID obsID1; //L1
        obsID1.band = ObsID::cbL1;
        obsID1.code = ObsID::tcP;
        obsID1.type = ObsID::otPhase;
        newObsIds.push_back(obsID1);

        RinexObsID obsID2; //P1
        obsID2.band = ObsID::cbL1;
        obsID2.code = ObsID::tcP;
        obsID2.type = ObsID::otRange;
        newObsIds.push_back(obsID2);

        RinexObsID obsID3; //C1
        obsID3.band = ObsID::cbL1;
        obsID3.code = ObsID::tcCA;
        obsID3.type = ObsID::otRange;
        newObsIds.push_back(obsID3);

        RinexObsID obsID4; //L2
        obsID4.band = ObsID::cbL2;
        obsID4.code = ObsID::tcP;
        obsID4.type = ObsID::otPhase;
        newObsIds.push_back(obsID4);

        RinexObsID obsID5;
        if(includeConflict){ //Y1
            obsID5.band = ObsID::cbL1;
            obsID5.code = ObsID::tcY;;
            obsID5.type = ObsID::otRange;
        }
        else { //P2
            obsID5.band = ObsID::cbL2;
            obsID5.code = ObsID::tcP;
            obsID5.type = ObsID::otRange;
        }
        newObsIds.push_back(obsID5);

        return newObsIds;
    }

   int runFromScratch(void){
      TUDEF("Rinex3Obs", "Rinex3ObsFromScratch");
      try{
          Rinex3ObsFromScratch("G","ValidTest");
          testFramework.assert(true,"valid input completed successfully",206);
      }
      catch(...){
          testFramework.assert(false, "valid input threw exception", 206);
      }
      try{
         Rinex3ObsFromScratch("R","Incomplete R");
         testFramework.assert(false,"Glonass file should need GlonassSlotFreqNo and GlonassCodPhsBias",206);
      }
      catch(...){
         testFramework.assert(true, "Glonass file failed for lacking necessary fields", 206);
      }
      try{
         Rinex3ObsFromScratch("R","Complete R", false, true);
         testFramework.assert(true,"Glonass file has GlonassSlotFreqNo and GlonassCodPhsBias",206);
      }
      catch(...){
         testFramework.assert(false, "Glonass file failed despite having all necessary fields", 206);
      }
      try{
          Rinex3ObsFromScratch("G","Conflict", true);
          testFramework.assert(false,"Conflicting R3->R2 conversions threw no error",213);
      }
      catch(...){
          testFramework.assert(true, "Conflicting conversion threw", 213);
      }
      try{
          Rinex3ObsFromScratch("GPS", "BadSys");
          testFramework.assert(false, "no exception for invalid sys char string", 220);
      }
      catch(...){
          testFramework.assert(true,"exception thrown for invalid input",220);
      }
      return testFramework.countFails();
   }
};

int main()
{
    int errorTotal = 0;
    Rinex3Obs_FromScratch_T testClass;

    errorTotal += testClass.runFromScratch();

    cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

    return( errorTotal );
}
