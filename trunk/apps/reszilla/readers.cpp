#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/readers.cpp#10 $"

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


#include "readers.hpp"


using namespace std;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_msc_data(std::string& fn, 
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
void read_obs_data(gpstk::CommandOption& files, //string& fn, 
                   unsigned long msid, 
                   RODEpochMap& rem, 
                   gpstk::RinexObsHeader& roh)
{
   string fn;
   try
   {
      for (int i=0; i<files.getCount(); i++)
      {
         fn = (files.getValue())[i];
         try {
            read_rinex_data(fn, rem, roh);
         }
         catch (gpstk::FFStreamError& e) {
            if (verbosity > 3)
               cout << e << endl;
            try {
               read_smo_data(fn, msid, rem, roh);
            } 
            catch (gpstk::FFStreamError& e) {
               if (verbosity > 3) 
                  cout << e << endl;
               cout << "Could not determine the type of obs data files" << endl;
            }
         }
      }
   }
   catch (std::exception e)
   {
      cout << "Error opening obs data file " << fn << endl;
      exit(-1);
   }

} // end of read_obs_data()



void read_rinex_data(string& fn, RODEpochMap& rem, gpstk::RinexObsHeader& roh)
{
   gpstk::RinexObsStream ros(fn.c_str(), ios::in);
   ros.exceptions(std::fstream::failbit);
   ros >> roh;

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
      if (t<t0) t0=t;
      if (t>t1) t1=t;
      if (t<=t2 && verbosity>1)
         cout << "Out of order obs data in rinex input ("
              << t2 << " -> " << t << ")" << endl;
      t2=t;
      rem[t] = rod;
   }

   roh.firstObs = t0;
   roh.valid |= gpstk::RinexObsHeader::firstTimeValid;
   roh.lastObs = t1;
   roh.valid |= gpstk::RinexObsHeader::lastTimeValid;
} // end of read_rinex_data()



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
short snr2ssi(float x)
{
   // These values were obtained from the comments in a RINEX obs file that was
   // generated from a TurboBinary file recorded on an AOA Benchmark  receiver
   if (x>316) return 9;
   if (x>100) return 8;
   if (x>31.6) return 7;
   if (x>10) return 6;
   if (x>3.2) return 5;
   if (x>0) return 4;
   return 0;
}



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_smo_data(string& fn,
                   unsigned long msid,
                   RODEpochMap& rem,
                   gpstk::RinexObsHeader& roh)
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

   gpstk::DayTime startTime(gpstk::DayTime::END_OF_TIME);
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
      startTime = min(thisTime, startTime);
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

      gpstk::RinexPrn p(smodata.PRNID, gpstk::systemGPS);
      rod.obs[p] = rotm;
      }

   roh.firstObs = startTime;
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
            cout << "Attempting to read " << fn << "as FIC." << endl;
         
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
            cout << "Have ephemeris data from " << eph->getInitialTime()
                 << " to " << eph->getFinalTime() << endl;
         else
            cout << "Do not have any ephemeris data" << endl;
   }

   return *eph;
} // end of read_eph_data()


std::string myGetLine(std::ifstream& s, unsigned long& lineNumber, unsigned long target=0)
{
   std::string line;

   while (true)
   {
      std::getline(s, line);
      lineNumber++;
      if (lineNumber>=target) 
         break;
   }

   return line;
}


// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
void read_pec_data(string& fn, unsigned long msid, RODEpochMap& rem)
{
   std::ifstream input(fn.c_str(), ios::in);
   input.exceptions(std::fstream::failbit);

   unsigned long lineNumber=0;

   try
   {
      std::istringstream p2(myGetLine(input, lineNumber, 6));
      unsigned year, doy;
      string master;
      p2 >> master >> year >> doy;
      cout << master << ", " << year << ", " << doy;

      std::istringstream parser(myGetLine(input, lineNumber, 7));
      int n;
      parser >> n;
      unsigned long msIndex;
      for (int j=1; j<=n; j++)
      {
         unsigned long i;
         parser >> i;
         if (i==msid) 
         { 
            msIndex=i;
            if (verbosity)
               cout << "msIndex: " << msIndex << endl;
               continue;
         }
      }
      if (msIndex>n)
         cout << "uh, we are borked.";
      
      gpstk::DayTime startTime(gpstk::DayTime::END_OF_TIME);
      gpstk::DayTime endTime(gpstk::DayTime::BEGINNING_OF_TIME);
      gpstk::DayTime thisTime=endTime;
      gpstk::DayTime lastTime=endTime;
      gpstk::RinexObsData rod;
      gpstk::RinexObsData::RinexObsTypeMap rotm;
      unsigned long epochNumber=0;
      myGetLine(input, lineNumber, 10);

      while (input)
      {
         std::istringstream parser(myGetLine(input, lineNumber));
         string w[10];
         parser >> w[0] >> w[1] >> w[2];
         if (w[0] == "DATA" && w[1]=="RECORD")
         {
            std::istringstream parser(myGetLine(input, lineNumber));
            long double epochOffset;
            parser >> epochOffset;
            thisTime.setYDoySod(year, doy, 0);
            thisTime+= epochOffset;
            cout << "epoch : " << thisTime;
         }
         if (w[0]=="STATION" && w[1]=="TIME" && w[2]=="OFFSET")
         {
            cout << "Rx offset: " << endl;
         }
      }
      if (verbosity>1)
         cout << "Have clock data from " << startTime << " to " << endTime << "." << endl;
   }
   catch (...)
   {
      cout << "caught one!" << endl;
   }

} // end of read_pec_data()
