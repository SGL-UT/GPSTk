#pragma ident "$Id: SatOrbitStore.cpp 3140 2012-06-18 15:03:02Z susancummins $"

/**
* @file SatOrbitStore.hpp
* 
*/

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010, 2011
//
//============================================================================


#include "SatOrbitStore.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "ReferenceFrames.hpp"
#include "CivilTime.hpp"
#include <sstream>
#include <iomanip>

namespace gpstk
{

   using namespace std;


      // Load the given SP3 file
   void SatOrbitStore::loadSP3File(const std::string& filename)
      throw(FileMissingException)
   {
      try
      {

         SP3Stream strm(filename.c_str());
         if (!strm)
         {
            FileMissingException e("File "+filename+" could not be opened.");
            GPSTK_THROW(e);
         }

         SP3Header header;
         strm >> header;

            // If any file doesn't have the velocity data, clear the
            // the flag indicating that there is any velocity data
         if( !header.containsVelocity )
         {
               // There are no velocity data in the file
               // It's not supported
            Exception e("There are no velocity data in the file : " + filename);
            GPSTK_THROW(e);
         }

         SP3Data rec;
         while(strm >> rec)
         {
            bool rejectBadPosFlag = false;
            bool rejectBadClockFlag = false;

            // If there is a bad or absent clock value, and
            // corresponding flag is set, then continue
            if( (rec.clk == 999999.999999) &&
               ( rejectBadClockFlag ) )
            {
               continue;
            }

            // If there are bad or absent positional values, and
            // corresponding flag is set, then continue
            if( ( (rec.x[0] == 0.0)    ||
               (rec.x[1] == 0.0)    ||
               (rec.x[2] == 0.0) )  &&
               ( rejectBadPosFlag ) )
            {
               continue;
            }

            if( pe.find(rec.sat) == pe.end())
            {
               ostringstream ss;
               ss << rec.sat;
               pe.insert(pair<SatID,PvtStore>(rec.sat,PvtStore(ss.str(),PvtStore::ITRF)));
            }

            PvtStore& svEph = pe[rec.sat];
            
            PvtStore::Pvt eph;

            // if the epoch have been added, we should read it first
            if(svEph.isEpochExist(rec.time))
            {
               eph = svEph.getPvt(rec.time);
            }
            
            if(tolower(rec.RecType)=='p')
            {
               eph.position[0] = rec.x[0] * 1000.0;
               eph.position[1] = rec.x[1] * 1000.0;
               eph.position[2] = rec.x[2] * 1000.0;
               eph.dtime = rec.clk * 1e-6;

            }
            else if(tolower(rec.RecType)=='v')
            {
               eph.velocity[0] = rec.x[0] / 10.0;
               eph.velocity[1] = rec.x[1] / 10.0;
               eph.velocity[2] = rec.x[2] / 10.0;
               eph.ddtime = rec.clk * 1e-10;
            }
            else
            {
               // never go here
               Exception e("It should never go here");
               GPSTK_THROW(e);
            }
            
            svEph.addPvt(rec.time, eph);         

         }  // end of 'while(strm >> rec)'

      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // End of method 'SatOrbitStore::loadSP3File()'


   void SatOrbitStore::writeSP3File(const std::string& filename,bool sp3c)
   {
      try
      {

         SP3Stream strm(filename.c_str(),std::ios::out);
         if (!strm)
         {
            FileMissingException e("File " +filename+ " could not be opened.");
            GPSTK_THROW(e);
         }
         
        
            // write header
         SP3Header header;
         
         if( sp3c ) header.version = SP3Header::SP3c; else SP3Header::SP3a;
         header.containsVelocity = true;

         header.timeSystem = TimeSystem::GPS;
         header.coordSystem = "ITRF";
         
         header.time = CivilTime(2010,1,10,23,59,0.0);
         header.epochInterval = 60.0;
         header.numberOfEpochs = 1443;
         
         header.dataUsed = "ORBIT";
         header.orbitType = "FIT";
         header.agency = "IGG";

         header.basePV = 0.0;
         header.baseClk = 0.0;

         header.comments.clear();
         header.comments.push_back("POD Solutions of "+ orbit + " by PhDSoft");
         header.comments.push_back("EMail: yanweigps@hotmail.com");
         header.comments.push_back("QQ   : 269358547");
         header.comments.push_back("NO PAIN, NO GAIN.");

         for(SvEphMap::iterator it = pe.begin();
            it != pe.end();
            ++it)
         {
            header.satList[it->first] = 1;
         }
         
         strm << header;

         
         // write body
         
         SatID sv(05,SatID::systemLEO);
         SvEphMap::iterator it0 = pe.begin();
         if(it0 != pe.end())
         {
            sv = it0->first;
         }
         else        // no data and return
         {
            return;
         }
        
         
         PvtStore::EpochList epochList = it0->second.epochList();
         for(PvtStore::EpochList::iterator it = epochList.begin();
            it != epochList.end();
            ++it)
         {
            ostringstream ss;
            ss<<fixed;
            ss <<"*  "
               <<setw(4)<<static_cast<CivilTime>(*it).year<<" "
               <<setw(2)<<static_cast<CivilTime>(*it).month<<" "
               <<setw(2)<<static_cast<CivilTime>(*it).day<<" "
               <<setw(2)<<static_cast<CivilTime>(*it).hour<<" "
               <<setw(2)<<static_cast<CivilTime>(*it).minute<<" "
               <<setw(11)<<setprecision(8)<<static_cast<CivilTime>(*it).second<<endl;

            strm<<ss.str();

            for(std::map<SP3SatID,short>::iterator its = header.satList.begin();
                its != header.satList.end();
                ++its)
            {
               PvtStore& svEph = pe[its->first];
               PvtStore::Pvt eph = svEph.getPvt(*it);

               SP3Data rec;
               rec.sat = its->first;
               rec.time = *it;
               rec.sig[0]=0; rec.sig[1]=0; rec.sig[2]=0; rec.sig[3]=0;

               SP3Data p(rec); 
               p.RecType =  'P';
               p.x[0] = eph.position[0]/1000.0;
               p.x[1] = eph.position[1]/1000.0;
               p.x[2] = eph.position[2]/1000.0;
               p.clk = eph.dtime * 1e6;

               SP3Data v(rec);
               v.RecType =  'V';
               v.x[0] = eph.velocity[0]*10.0;
               v.x[1] = eph.velocity[1]*10.0;
               v.x[2] = eph.velocity[2]*10.0;
               v.clk = eph.ddtime * 1e10;

               strm << p;
               strm << v;
            }
            
         }  // End of 'for(PvtStore::EpochList::iterator it = epochList.begin();...'

         strm<<"EOF"<<endl;
         
         
      
         // close the sp3 file
         strm.close();
      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }

   }  // End of method 'SatOrbitStore::writeSP3File()'


   void SatOrbitStore::loadGNV1BFile(const std::string& filename)
   {
      try
      {
         // open the data file
         ifstream fin(filename.c_str());
         if(fin.bad()) 
         {
            FileMissingException e("Failed to open file: "+filename);
         }

         string buf;

         // read header section

         CommonTime refEpoch;
         double firstObsSec(0.0);
         double lastObsSec(0.0);
         string satelliteName = "GRACE ?";

         int hdrLineCounter(0); 
         while(getline(fin,buf))
         { 
            string flag = StringUtils::strip(buf.substr(0,30));
            string data = StringUtils::strip(buf.substr(32));

            if(flag == "PRODUCER AGENCY")
            {
               string agency = data;
            }
            else if(flag == "PRODUCER INSTITUTION")
            {
               string institution = data;
            }
            else if(flag == "FILE TYPE ipGNV1BF")
            {
               int fileType = StringUtils::asInt(data);  // 5
               if(fileType != 5)
               {
                  Exception e("It's NOT a GNV1B file: " + filename);
                  GPSTK_THROW(e);
               }
            }
            else if(flag == "FILE FORMAT 0=BINARY 1=ASCII")
            {
               int fileFormat = StringUtils::asInt(data);  // 1
            }
            else if(flag == "NUMBER OF HEADER RECORDS")
            {
               int numberHeaderRecord = StringUtils::asInt(data);
            }
            else if(flag == "SATELLITE NAME")
            {
               satelliteName = data;
            }
            else if(flag == "SENSOR NAME")
            {
               string sensorName = data;
            }
            else if(flag == "TIME EPOCH (GPS TIME)")
            {
               //data = "2001-12-11 12:33:22";
               short yr = StringUtils::asInt(data.substr(0,4));
               short mo = StringUtils::asInt(data.substr(5,2));
               short day = StringUtils::asInt(data.substr(8,2));
               short hr = StringUtils::asInt(data.substr(11,2));
               short min = StringUtils::asInt(data.substr(14,2));
               double sec = StringUtils::asDouble(data.substr(17,2));

               refEpoch=CivilTime(yr,mo,day,hr,min,sec);
            }
            else if(flag == "TIME FIRST OBS(SEC PAST EPOCH)")
            {
               firstObsSec = StringUtils::asDouble(data.substr(0,16));
            }
            else if(flag == "TIME LAST OBS(SEC PAST EPOCH)")
            {
               lastObsSec = StringUtils::asDouble(data.substr(0,16));
            }
            else if(flag == "NUMBER OF DATA RECORDS")
            {
               int numberDataRecord = StringUtils::asInt(data);
            }
            else
            {
               // skip this line
            }


            if(StringUtils::strip(buf)=="END OF HEADER")
            {
               CommonTime firstEpoch = refEpoch;
               firstEpoch += firstObsSec;

               CommonTime lastEpoch = refEpoch;
               lastEpoch += lastObsSec;

               break;
            }

            hdrLineCounter++;
         }
         
         // TODO: check if we read the header correctly


         // read data section
         int recLineCounter(0);
         while(getline(fin,buf))
         { 
            if(buf.length() <= 200) continue;

            istringstream ss(buf);

            long gps_time;
            char grace_id, coord_ref;
            double xpos,ypos,zpos;
            double xvel,yvel,zvel;

            double temp;
            
            ss >> gps_time >> grace_id >> coord_ref
               >> xpos >> ypos >> zpos >> temp >> temp >> temp
               >> xvel >> yvel >> zvel >> temp >> temp >> temp;

            CommonTime epoch = refEpoch;
            epoch += gps_time;

            
            PvtStore::Pvt eph( Triple(xpos,ypos,zpos), 
               Triple(xvel,yvel,zvel), 
               0.0, 
               0.0);
            
            int prn = (StringUtils::lowerCase(satelliteName)=="grace a") ? 5 : 6;
            SatID sv(prn,SatID::systemLEO);
            
            if( pe.find(sv) == pe.end())
            {
               pe.insert(pair<SatID,PvtStore>(sv,PvtStore(satelliteName,PvtStore::ITRF)));
            }

            PvtStore& svEph = pe[sv];
            svEph.addPvt(epoch,eph);

            recLineCounter++;
         }

         // TODO: check if we read all data correctly

         // close the file
         fin.close();
      }
      catch (gpstk::Exception& e)
      {
         GPSTK_RETHROW(e);
      }
     
   }  // End of method 'SatOrbitStore::writeGNV1BFile()'


      // Get satellite state in specific reference frame
   PvtStore::Pvt SatOrbitStore::getPvt(const SatID sat,
                                       const CommonTime& t, 
                                       bool j2k )
      throw(InvalidRequest)
   {
      SvEphMap::iterator svit = pe.find(sat);
      if(svit == pe.end())
      {
         InvalidRequest e("Data for satellite  " + StringUtils::asString(sat)
            + " not found.");
         GPSTK_THROW(e);
      }
      
      PvtStore& pvs = pe[svit->first];
      PvtStore::Pvt eph = pvs.getPvt(t);
      
      if(j2k)     // if J2000 was wanted
      {
         CommonTime gpst(t);
	UTCTime utc;
	 GPST2UTC(t,utc);

         Vector<double> ecefPosVel(6,0.0), j2kPosVel(6,0.0);

         for(int i=0;i<3;i++)
         {
            ecefPosVel[i] = eph.position[i];
            ecefPosVel[i+3] = eph.velocity[i];
         }
         
         j2kPosVel = ReferenceFrames::ECEFPosVelToJ2k(UTCTime(utc),ecefPosVel);

         for(int i=0;i<3;i++)
         {
            eph.position[i] = j2kPosVel[i];
            eph.velocity[i] = j2kPosVel[i+3];
         }

      }  // end if

   
      return eph;

   }  // End of method 'SatOrbitStore::getPvt()'


      // Get epoch list for specific satellite
   PvtStore::EpochList SatOrbitStore::epochList(const SatID sat)
   {
      SvEphMap::iterator it = pe.find(sat);
      if(it != pe.end())
      {
         return it->second.epochList();
      }
      else
      {
         return PvtStore::EpochList();
      }

   }  // End of method 'SatOrbitStore::epochList()'
   
      // delete all but specific sat
   void SatOrbitStore::keepOnly(const SatID sat)
   {
      for(SvEphMap::iterator it = pe.begin();
         it != pe.end();
         ++it)
      {
         if(it->first != sat) pe.erase(it);
      }
   }

      // delete specific sat
   void SatOrbitStore::deleteOnly(const SatID sat)
   {
      SvEphMap::iterator it = pe.find(sat);
      if(it != pe.end()) pe.erase(it);
   }

   void SatOrbitStore::test()
   {
      //loadGNV1BFile("GNV1B_2010-01-11_A_01.dat.asc");
      //loadGNV1BFile("GNV1B_2010-01-11_B_01.dat.asc");
      //writeSP3File("GNV1B_2010-01-11_A_01.dat.sp3",true);

      loadSP3File("graceab.sp3");
      writeSP3File("graceab2.sp3",true);

   }

}  // End of namespace 'gpstk'
