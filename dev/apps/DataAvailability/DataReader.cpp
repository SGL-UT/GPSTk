#pragma ident "$Id$"


//lgpl-license START
//lgpl-license END

#include <iostream>
#include <iomanip>
#include <map>

#include "SP3EphemerisStore.hpp"
#include "RinexEphemerisStore.hpp"
#include "GPSGeoid.hpp"

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

#include "DataReader.hpp"

#include "FormatConversionFunctions.hpp"

namespace gpstk
{
   using namespace std;

   DataReader::DataReader()
      : verbosity(1), 
        timeFormat("%4y %03j %02H:%02M:%5.1f"), 
        startTime(DayTime::BEGINNING_OF_TIME),
        stopTime(DayTime::END_OF_TIME),
        msid(0), eph(NULL), 
        haveEphData(false), haveObsData(false), havePosData(false),
        firstHeader(true)
   {}


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read(CommandOption& files)
   {
      string fn;
      try
      {
         for (int i=0; i<files.getCount(); i++)
         {
            fn = (files.getValue())[i];
            while(true)
            {
               try {
                  read_rinex_obs_data(fn); 
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3) 
                     cout << e << endl;
               }

               try {
                  read_smo_data(fn);
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3)
                     cout << e << endl;
               }

               try {
                  read_mdp_data(fn);
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3)
                     cout << e << endl;
               }

               try {
                  read_rinex_nav_data(fn);
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3)
                     cout << e << endl;
               }

               try {
                  read_fic_data(fn);
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3)
                     cout << e << endl;
               }

               try {
                  read_sp3_data(fn);
                  break;
               } catch (FFStreamError& e) {
                  if (verbosity > 3)
                     cout << e << endl;
               }

               if (verbosity > 1) 
                  cout << "Could not determine the format of " << fn << endl;
            }
            filesRead.push_back(fn);
         }
      }
      catch (exception e)
      {
         cout << "Error reading data " << endl;
      }
   } // end of read()


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_msc_file(const string& fn)
   {
      if (msid == 0)
      {
         cout << "Monitor station id is not set." << endl;
         return;
      }

      MSCStream msc(fn.c_str(), ios::in);
      MSCData mscd;
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
   void DataReader::merge_rinex_obs_header(const RinexObsHeader& newHeader)
   {
      if (firstHeader)
      {
         roh = newHeader;
         firstHeader=false;
      }

      if (newHeader.valid & RinexObsHeader::firstTimeValid)
      {
         if (!(roh.valid & RinexObsHeader::firstTimeValid &&
               newHeader.firstObs>roh.firstObs ))
         {
            roh.firstObs = newHeader.firstObs;
            roh.valid |= RinexObsHeader::firstTimeValid;
         }
      }

      if (newHeader.valid & RinexObsHeader::lastTimeValid)
      {
         if (!(roh.valid & RinexObsHeader::lastTimeValid &&
               newHeader.lastObs>roh.lastObs ))
         {
            roh.lastObs = newHeader.lastObs;
            roh.valid |= RinexObsHeader::lastTimeValid;
         }
      }
   }


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_rinex_obs_data(const string& fn)
   {
      RinexObsStream ros(fn.c_str(), ios::in);
      ros.exceptions(fstream::failbit);
      if (verbosity>2)
         cout << "Trying " << fn << " as RINEX obs."<< endl;

      RinexObsHeader temp_roh;
      ros >> temp_roh;

      DayTime t0(DayTime::END_OF_TIME);
      DayTime t1(DayTime::BEGINNING_OF_TIME);
      DayTime t2(DayTime::BEGINNING_OF_TIME);

      RinexObsData rod;
      while (ros >> rod)
      {
         const DayTime& t = rod.time;
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

      haveObsData = true;

      temp_roh.valid |= RinexObsHeader::firstTimeValid;
      temp_roh.firstObs = t0;

      temp_roh.valid |= RinexObsHeader::lastTimeValid;
      temp_roh.lastObs = t1;

      merge_rinex_obs_header(temp_roh);

      if (verbosity > 2)
         roh.dump(cout);

      if (verbosity>1)
         cout << "Read " << fn << " as RINEX obs."<< endl;
   } // end of read_rinex_data()



   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_mdp_data(const string& fn)
   {
      MDPStream mdps(fn.c_str(), ios::in);
      mdps.exceptions(fstream::failbit);

      if (verbosity>2)
         cout << "Trying " << fn << " as MDP."<< endl;

      DayTime t0(DayTime::END_OF_TIME);
      DayTime t1(DayTime::BEGINNING_OF_TIME);
      DayTime t2(DayTime::BEGINNING_OF_TIME);
      
      MDPObsEpoch obs;
      MDPHeader header;

      if (verbosity>1)
         header.debugLevel = 1;

      while (mdps >> header)
      {
         switch (header.id)
         {
            case MDPObsEpoch::myId:
               mdps >> obs;

               const DayTime& t = obs.time;
               if (t<startTime || t>stopTime)
                  continue;
               if (t<t0) t0=t;
               if (t>t1) t1=t;
               t2=t;
               RinexPrn prn(obs.prn, systemGPS);
               RinexObsData& rod = rem[t];
               rod.obs[prn] = makeRinexObsTypeMap(obs);
               break;
         }
      }

      haveObsData = true;
      roh.firstObs = t0;
      roh.valid |= RinexObsHeader::firstTimeValid;
      roh.lastObs = t1;
      roh.valid |= RinexObsHeader::lastTimeValid;
      if (verbosity>1)
         cout << "Read " << fn << " as MDP."<< endl;
   } // end of read_rinex_data()


   // ---------------------------------------------------------------------
   // ---------------------------------------------------------------------
   void DataReader::read_smo_data(const string& fn)
   {
      SMODFStream smo(fn.c_str(), ios::in);
      smo.exceptions(fstream::failbit);
      if (verbosity>2)
         cout << "Trying " << fn << " as SMODF."<< endl;

      roh.valid |= RinexObsHeader::allValid21;
      roh.fileType = "unk";
      roh.markerName = StringUtils::asString(msid);
      roh.observer = "unk";
      roh.agency = "unk";
      roh.antennaOffset = Triple(0,0,0);
      roh.wavelengthFactor[0] = 1;
      roh.wavelengthFactor[1] = 1;
      roh.recType = "unk";
      roh.recVers = "unk";
      roh.recNo = "1";
      roh.antType = "unk";
      roh.antNo = "1";
      roh.obsTypeList.push_back(RinexObsHeader::P1);
      roh.obsTypeList.push_back(RinexObsHeader::L1);

      DayTime beginTime(DayTime::END_OF_TIME);
      DayTime endTime(DayTime::BEGINNING_OF_TIME);
      DayTime thisTime=endTime;
      DayTime lastTime=endTime;
   
      SMODFData smodata;
      RinexObsData rod;
      RinexObsData::RinexObsTypeMap rotm;

      while (smo >> smodata)
      {
         if (smodata.station != msid)
            continue;

         rod.numSvs++;
         const DayTime& thisTime = smodata.time;
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
            rotm[RinexObsHeader::P1].data = smodata.obs*1000;
            rotm[RinexObsHeader::P1].lli = 0;
            rotm[RinexObsHeader::P1].ssi = 9;
         }
         else if (smodata.type==9)
         {
            rotm[RinexObsHeader::L1].data = smodata.obs;
            rotm[RinexObsHeader::L1].lli = 0;
            rotm[RinexObsHeader::L1].ssi = 9;
         }

         RinexPrn p(smodata.PRNID, systemGPS);
         rod.obs[p] = rotm;
      }

      haveObsData = true;
      roh.firstObs = beginTime;
      roh.valid |= RinexObsHeader::firstTimeValid;
      roh.lastObs = endTime;
      roh.valid |= RinexObsHeader::lastTimeValid;
      if (verbosity>1)
         cout << "Read " << fn << " as SMODF obs."<< endl;
   } // end of read_smo_data()


   // ---------------------------------------------------------------------
   // Read in ephemeris data in rinex format
   // ---------------------------------------------------------------------
   void DataReader::read_rinex_nav_data(const string& fn)
   {
      BCEphemerisStore* bce;
      if (eph == NULL)
      {
         bce = new(BCEphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(BCEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<BCEphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Trying " << fn << " as RINEX nav."<< endl;
         
      RinexNavStream rns(fn.c_str(), ios::in);
      rns.exceptions(ifstream::failbit);
      RinexNavData rnd;
      while (rns >> rnd)
         bce->addEphemeris(rnd);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(bce);
      if (verbosity>1)
         cout << "Read " << fn << " as RINEX nav."<< endl;
   } // end of read_rinex_nav_data()


   void DataReader::read_fic_data(const string& fn)
   {
      BCEphemerisStore* bce;

      if (eph == NULL)
      {
         bce = new(BCEphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(BCEphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         bce = dynamic_cast<BCEphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Trying " << fn << " as FIC nav."<< endl;
      
      FICStream fs(fn.c_str(), ios::in);
      FICHeader header;
      fs >> header;
      
      FICData data;
      while(fs >> data)
         if (data.blockNum==9) // Only look at the eng ephemeris
            bce->addEphemeris(data);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(bce);
      if (verbosity>1)
         cout << "Read " << fn << " as FIC nav."<< endl;
   } // end of read_fic_data()

   void DataReader::read_sp3_data(const string& fn)
   {
      SP3EphemerisStore* pe;

      if (eph == NULL)
      {
         pe = new(SP3EphemerisStore);
      }
      else
      {
         if (typeid(*eph) != typeid(SP3EphemerisStore))
            throw(FFStreamError("Don't mix nav data types..."));
         pe = dynamic_cast<SP3EphemerisStore*>(eph);
      }
      if (verbosity>2)
         cout << "Trying " << fn << " as SP3 ephemeris."<< endl;

      SP3Stream pefile(fn.c_str(),ios::in);
      pefile.exceptions(ifstream::failbit);
      
      SP3Header header;
      pefile >> header;

      SP3Data data;
      while(pefile >> data)
         pe->addEphemeris(data);

      haveEphData = true;
      if (eph==NULL)
         eph = dynamic_cast<EphemerisStore*>(pe);
      if (verbosity>1)
         cout << "Read " << fn << " as SP3 ephemeris."<< endl;
   } // end of read_sp3_data()
}
