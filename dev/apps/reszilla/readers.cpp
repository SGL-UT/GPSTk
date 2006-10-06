#pragma ident "$Id$"


#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "SP3EphemerisStore.hpp"
#include "RinexEphemerisStore.hpp"
#include "GPSGeoid.hpp"
#include "TropModel.hpp"
#include "Geodetic.hpp"


#include "DayTime.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsData.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"
#include "RinexMetStream.hpp"
#include "RinexMetData.hpp"

#include "FICStream.hpp"
#include "FICData.hpp"

#include "SMODFStream.hpp"
#include "SMODFData.hpp"

#include "MSCData.hpp"
#include "MSCStream.hpp"

#include "MDPObsEpoch.cpp"
#include "MDPStream.hpp"

#include "readers.hpp"

#include "MDP2Rinex.hpp"

void read_rinex_data(
   std::string& fn, 
   RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime);

void read_smo_data(
   std::string& fn,
   unsigned long msid,
   RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime);

void read_mdp_data(
   std::string& fn,
   RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime);

using namespace std;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_msc_data(
   std::string& fn, 
   unsigned long msid,
   gpstk::RinexObsHeader& roh)
{
   gpstk::MSCStream msc(fn.c_str(), ios::in);
   gpstk::MSCData mscd;
   while (msc >> mscd)
   {
      if (mscd.station == msid)
      {
         roh.antennaPosition = mscd.coordinates;
         if (verbosity>1)
            cout << "Station " << msid
                 << " location: " << mscd.coordinates << endl;
         break;
      }
   }
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_obs_data(
   gpstk::CommandOption& files, //string& fn, 
   unsigned long msid, 
   RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime)
{
   string fn;
   try
   {
      for (int i=0; i<files.getCount(); i++)
      {
         fn = (files.getValue())[i];
         try
         {
            read_rinex_data(fn, rem, roh, startTime, stopTime);
         }
         catch (gpstk::FFStreamError& e)
         {
            if (verbosity > 3)
               cout << e << endl;
            try
            {
               read_smo_data(fn, msid, rem, roh, startTime, stopTime);
            }
            catch (gpstk::FFStreamError& e)
            {
               if (verbosity > 3)
                  cout << e << endl;
               try
               {
                  read_mdp_data(fn, rem, roh, startTime, stopTime);
               }
               catch (gpstk::FFStreamError& e)
               {
                  if (verbosity > 1) 
                     cout << e << endl;
                  cout << "Could not determine the type of obs data file " << fn << endl;
               }
            }
         }
      }
   }
   catch (std::exception e)
   {
      cout << "Error reading obs data file " << fn << endl;
   }
   
   if (verbosity>1)
      cout << "Have obs data from " << roh.firstObs.printf(timeFormat)
           << " to " << roh.lastObs.printf(timeFormat) << endl;
} // end of read_obs_data()



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_rinex_data(
   string& fn, RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime)
{
   gpstk::RinexObsStream ros(fn.c_str(), ios::in);
   ros.exceptions(std::fstream::failbit);
   gpstk::RinexObsHeader temp_roh;
   ros >> temp_roh;

   if (verbosity>1)
      cout << "Reading RINEX obs data from " << fn << "." << endl;

   if (verbosity > 3)
      roh.dump(cout);

   gpstk::DayTime t0(gpstk::DayTime::END_OF_TIME);
   gpstk::DayTime t1(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime t2(gpstk::DayTime::BEGINNING_OF_TIME);

   gpstk::RinexObsData rod;
   while(ros >> rod)
   {
      const gpstk::DayTime& t = rod.time;
      if (t<startTime || t>stopTime)
         continue;

      if (t<t0) t0=t;
      if (t>t1) t1=t;
      if (t<=t2 && verbosity>1)
         cout << "Out of order obs data in rinex input ("
              << t2 << " -> " << t << ")" << endl;
      t2=t;
      rem[t] = rod;
   }

   if (roh.valid & gpstk::RinexObsHeader::firstTimeValid)
   {
      if (t0<roh.firstObs)
         roh.firstObs = t0;
   }
   else
   {
      roh = temp_roh;
      roh.valid |= gpstk::RinexObsHeader::firstTimeValid;
      roh.firstObs = t0;
   }

   if (roh.valid & gpstk::RinexObsHeader::lastTimeValid)
   {
      if (t1>roh.lastObs)
         roh.lastObs = t1;
   }
   else
   {
      roh.valid |= gpstk::RinexObsHeader::lastTimeValid;
      roh.lastObs = t1;
   }
} // end of read_rinex_data()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_mdp_data(
   string& fn, RODEpochMap& rem, 
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime)
{
   gpstk::MDPStream mdps(fn.c_str(), ios::in);
   mdps.exceptions(std::fstream::failbit);

   if (verbosity>1)
      cout << "Reading MDP observations from " << fn << "." << endl;

   gpstk::DayTime t0(gpstk::DayTime::END_OF_TIME);
   gpstk::DayTime t1(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime t2(gpstk::DayTime::BEGINNING_OF_TIME);
      
   gpstk::MDPObsEpoch obs;
   gpstk::MDPHeader header;

   if (verbosity>1)
      header.debugLevel = 1;

   while (mdps >> header)
   {
      switch (header.id)
      {
         case gpstk::MDPObsEpoch::myId:
            mdps >> obs;

            const gpstk::DayTime& t = obs.time;
            if (t<startTime || t>stopTime)
               continue;
            if (t<t0) t0=t;
            if (t>t1) t1=t;
            t2=t;
            gpstk::SatID prn(obs.id, gpstk::SatID::systemGPS);
            gpstk::RinexObsData& rod = rem[t];
            rod.obs[prn] = gpstk::makeRinexObsTypeMap(obs);
            break;
      }
   }

   roh.firstObs = t0;
   roh.valid |= gpstk::RinexObsHeader::firstTimeValid;
   roh.lastObs = t1;
   roh.valid |= gpstk::RinexObsHeader::lastTimeValid;
} // end of read_rinex_data()


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_smo_data(
   string& fn,
   unsigned long msid,
   RODEpochMap& rem,
   gpstk::RinexObsHeader& roh,
   const gpstk::DayTime& startTime,
   const gpstk::DayTime& stopTime)
{
   roh.valid |= gpstk::RinexObsHeader::allValid21;
   roh.fileType = "unk";
   roh.markerName = gpstk::StringUtils::asString(msid);
   roh.observer = "unk";
   roh.agency = "unk";
   roh.antennaOffset = gpstk::Triple(0,0,0);
   roh.wavelengthFactor[0] = 1;
   roh.wavelengthFactor[1] = 1;
   roh.recType = "unk";
   roh.recVers = "unk";
   roh.recNo = "1";
   roh.antType = "unk";
   roh.antNo = "1";
   roh.obsTypeList.push_back(gpstk::RinexObsHeader::P1);
   roh.obsTypeList.push_back(gpstk::RinexObsHeader::L1);

   gpstk::DayTime beginTime(gpstk::DayTime::END_OF_TIME);
   gpstk::DayTime endTime(gpstk::DayTime::BEGINNING_OF_TIME);
   gpstk::DayTime thisTime=endTime;
   gpstk::DayTime lastTime=endTime;
   
   gpstk::SMODFData smodata;
   gpstk::RinexObsData rod;
   gpstk::RinexObsData::RinexObsTypeMap rotm;
   bool first=true;

   gpstk::SMODFStream smo(fn.c_str(), ios::in);

   smo.exceptions(std::fstream::failbit);
   while (smo >> smodata)
   {
      if (first)
      {
         first=false;
         if (verbosity>1)
            cout << "Reading SMODF obs data from " << fn << "." << endl;
      }
         
      if (smodata.station != msid)
         continue;

      rod.numSvs++;
      const gpstk::DayTime& thisTime = smodata.time;
      if (thisTime<startTime || thisTime>stopTime)
         continue;

      beginTime = min(thisTime, beginTime);
      endTime = max(thisTime, endTime);

      if (thisTime != lastTime)
      {
         if (rod.obs.size())
            rem[rod.time]=rod;

         lastTime=thisTime;
         rod.time = thisTime;
         rod.clockOffset = 0;
         rod.obs.clear();
         rod.epochFlag = 0;
         rod.numSvs = 0;
      }

      if (smodata.type==0)
      {
         rotm[gpstk::RinexObsHeader::P1].data = smodata.obs*1000;
         rotm[gpstk::RinexObsHeader::P1].lli = 0;
         rotm[gpstk::RinexObsHeader::P1].ssi = 9;
      }
      else if (smodata.type==9)
      {
         rotm[gpstk::RinexObsHeader::L1].data = smodata.obs;
         rotm[gpstk::RinexObsHeader::L1].lli = 0;
         rotm[gpstk::RinexObsHeader::L1].ssi = 9;
      }

      gpstk::SatID p(smodata.PRNID, gpstk::SatID::systemGPS);
      rod.obs[p] = rotm;
   }

   roh.firstObs = beginTime;
   roh.valid |= gpstk::RinexObsHeader::firstTimeValid;
   roh.lastObs = endTime;
   roh.valid |= gpstk::RinexObsHeader::lastTimeValid;
} // end of read_smo_data()


// ---------------------------------------------------------------------
// Read in weather data into the indicated store
// ---------------------------------------------------------------------
gpstk::WxObsData& read_met_data(gpstk::CommandOption& files)
{
   gpstk::WxObsData *wod = new(gpstk::WxObsData);

   for (int i=0; i<files.getCount(); i++)
   {
      string fn = (files.getValue())[i];
      gpstk::RinexMetStream rms;
      try { rms.open(fn.c_str(), ios::in); }
      catch (...) {
         cout << "Error reading weather data from file " << fn << endl;
         exit(-1);
      }
      if (verbosity>1)
         cout << "Reading met data from " << fn << "." << endl;

      gpstk::RinexMetData rmd;
      while (rms >> rmd)
      {
         gpstk::WxObservation wob(
            rmd.time,
            rmd.data[gpstk::RinexMetHeader::TD],
            rmd.data[gpstk::RinexMetHeader::PR],
            rmd.data[gpstk::RinexMetHeader::HR]);
         wod->insertObservation(wob);
      }

   }

   if (verbosity>1)
      if (wod->firstTime < wod->lastTime)
         cout << "Have met data from " << wod->firstTime 
              << " to " << wod->lastTime << "." << endl;
      else
         cout << "No met data for you!" << endl;

   return *wod;
} // end of read_met_data()


// ---------------------------------------------------------------------
// Read in ephemeris data into the indicated store
// ---------------------------------------------------------------------
gpstk::EphemerisStore& read_eph_data(gpstk::CommandOption& files)
{
   gpstk::EphemerisStore* eph=NULL;
   gpstk::BCEphemerisStore* bce;
   gpstk::SP3EphemerisStore* pe;

   try
   {
      // First try the first file as a RINEX nav file
      bce = new(gpstk::BCEphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attepmting to read " << fn << " as RINEX nav." << endl;
         
         gpstk::RinexNavStream rns(fn.c_str(), ios::in);
         rns.exceptions(ifstream::failbit);
         gpstk::RinexNavData rnd;
         while (rns >> rnd)
            bce->addEphemeris(rnd);
      }
      if (verbosity>1)
         cout << "Read RINEX format broadcast ephemerides." << endl;
      eph = bce;
   }
   catch (gpstk::FFStreamError& e)
   {
      if (verbosity>3)
         cout << e << endl;
      delete bce;
   }

   // If the file failed to read as a RINEX file, try it as a SP3 file
   if (!eph) try
   {
      pe = new(gpstk::SP3EphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attempting to read " << fn << " as SP3." << endl;
         
         gpstk::SP3Stream pefile(fn.c_str(),ios::in);
         pefile.exceptions(ifstream::failbit);

         gpstk::SP3Header header;
         pefile >> header;

         gpstk::SP3Data data;
         while(pefile >> data)
            pe->addEphemeris(data);
      }
      if (verbosity>1)
         cout << "Read SP3 format precise ephemerides." << endl;
      eph = pe;
   }
   catch (gpstk::FFStreamError& e)
   {
      if (verbosity>3)
         cout << e << endl;
      delete pe;
   }

   
   // If the file failed to read as a RINEX or SP3 file, try it as a FIC file
   if (!eph) try
   {
      bce = new(gpstk::BCEphemerisStore);
      for (int i=0; i<files.getCount(); i++)
      {
         string fn = (files.getValue())[i];
         if (verbosity>2 && i==0)
            cout << "Attempting to read " << fn << " as FIC." << endl;
         
         gpstk::FICStream fs(fn.c_str());
         // Note that we don't want to set the stream failbit since the binary
         // read routines take care of throwing all the error conditions.

         gpstk::FICHeader header;
         fs >> header;

         gpstk::FICData data;
         while(fs >> data)
            if (data.blockNum==9) // Only look at the eng ephemeris
               bce->addEphemeris(data);
      }
      if (verbosity>1)
         cout << "Read FIC format broadcast ephemerides." << endl;
      eph = bce;
   }
   catch (gpstk::FFStreamError& e)
   {
      if (verbosity>3)
         cout << e << "Blarf, man." << endl;
      delete bce;
   }


   // If the file failed to read as a RINEX, FIC, or SP3 file give up
   if (!eph)
   {
      cout << "Could not determine type of nav data supplied" << endl;
      eph = new(gpstk::BCEphemerisStore);
   }
   else
   {
      if (verbosity>1)
         if (eph->getInitialTime() < eph->getFinalTime())
            cout << "Have ephemeris data from "
                 << eph->getInitialTime().printf(timeFormat)
                 << " to " << eph->getFinalTime().printf(timeFormat) << endl;
         else
            cout << "Do not have any ephemeris data" << endl;
   }

   return *eph;
} // end of read_eph_data()
