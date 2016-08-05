//============================================================================
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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================
//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//============================================================================
 /*********************************************************************
*
*  Test program for gpstk/ext/lib/GNSSEph/OrbAlm*
*
*********************************************************************/
#include <iostream>
#include <fstream>

#include "CivilTime.hpp"
#include "Exception.hpp"
#include "GPSWeekSecond.hpp"
#include "OrbAlm.hpp"
#include "OrbAlmGen.hpp"
#include "OrbAlmStore.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"
#include "Xvt.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class OrbAlm_T
{
public:

   typedef struct PassFailData
   {
      SatID subjID;
      CommonTime testTime;
      Xvt truthXvt;

      PassFailData(): 
         subjID(SatID()),
         testTime(CommonTime::END_OF_TIME)
         { }

      PassFailData(const SatID& subj, const CommonTime& testT, const Xvt& xvt):
         subjID(subj),
         testTime(testT),
         truthXvt(xvt)
         { }
   } PassFailData;

   OrbAlm_T();

   void init();

   unsigned createAndDump();
   void setUpLNAV();
   void setUpCNAV();
   void setUpBDS();
   void setUpGLO();
   gpstk::PackedNavBits getPnbLNav(const gpstk::ObsID& oidr,
                                   const std::string& str)
             throw(gpstk::InvalidParameter);
   gpstk::PackedNavBits getPnbCNav(const gpstk::ObsID& oidr,
                                   const std::string& str)
             throw(gpstk::InvalidParameter);

      // Methods above exist to set up the following
      // members
   list<PackedNavBits> dataList;
   string typeDesc;

      // List of PassFailData objects that contain definitions for sepcific find( ) tests.
   list<PassFailData> pfList;

   ofstream out; 

      // For testing the test
   int debugLevel;   
};

OrbAlm_T::
OrbAlm_T()
{
   debugLevel = 0; 
   init();
}

unsigned OrbAlm_T::
createAndDump()
{
   string currMethod = typeDesc + " create/store OrbDataSys objects";
   TUDEF("OrbDataSys",currMethod);

      // Open an output stream specific to this navigation message type
   std::string fs = getFileSep(); 
   std::string tf(getPathTestTemp()+fs);
   std::string tempFile = tf + "test_output_OrbAlm_T_" +
                         typeDesc+".out";
   out.open(tempFile.c_str(),std::ios::out);
   if (!out)
   {
      stringstream ss;
      ss << "Could not open file " << tempFile << " for output.";
      TUFAIL(ss.str());
      TURETURN();
   }

      // All the navigation message data will be placed here. 
   OrbAlmStore oas;
   oas.setDebugLevel(debugLevel); 

   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         unsigned short retval = oas.addMessage(pnbr);
      }
      catch(gpstk::InvalidParameter ir)
      {
            // Do nothing except move to next message. 
      }
   }

   out << " Alm-SP3 Comparions for " << typeDesc << endl;
   out << "         Xmit               Differences" << endl;
   out << "       Sat  mm/dd/yy HH:MM:SS  Position RSS (m)  Velocity RSS (m/s)"
       << "  Clock Bias (nsec)  Clock Drift (nsec/sec)" << endl;
   string tform = "%02m/%02d/%02y %02H:%02M:%02S"; 

      // For each SatID on the list, retrieve the appropriate OrbAlm messages and
      // check them against the provided truth data. 
   list<PassFailData>::const_iterator cit2;
   for (cit2=pfList.begin();cit2!=pfList.end();cit2++)
   {
      const PassFailData& pfd = *cit2;
      const Xvt& truth = pfd.truthXvt; 
      const OrbAlmStore::OrbAlmMap& almMap = oas.getOrbAlmMap(pfd.subjID);
      OrbAlmStore::OrbAlmMap::const_iterator calm;
      for (calm=almMap.begin();calm!=almMap.end();calm++)
      {
         OrbAlm* alm = calm->second;
         Xvt test = alm->svXvt(pfd.testTime);
         Triple xDiff = test.x - truth.x;
         Triple vDiff = test.v - truth.v;
         double clockDiff = test.clkbias - truth.clkbias;
         double driftDiff = test.clkdrift - truth.clkdrift; 

         double xMag = xDiff.mag();
         double vMag = vDiff.mag();

         char temp[150];
         sprintf(temp," %15.3lf  %18.3lf  %17.3lf  %22.3lf",
                        xMag, vMag, clockDiff * 1.0e9, driftDiff * 1.0e9);
         string temps(temp);
         int begLen = 8;
         if (pfd.subjID.id>9) begLen = 7;
         out << setw(begLen) << pfd.subjID << "  " << printTime(alm->beginValid,tform) << "  "
             << temps << endl; 
      }
   }

      // Clean up
   oas.clear();
   out.close();

   return 0; 
}

void OrbAlm_T::
init()
{
   dataList.clear();
} 

//--------------------------------------------------------------------
// IS-GPS-200 makes two statements regarding accuradcy of the almanac
// data.
//  1.20.3.3.5.2.1 - There is a table that indicates the the almanac ephemeris URE (1 simga) 
//                   should be 900 m.   There follows list of exceptions and caveats that
//                   includes Normal Operations, Eclipse season, and SV thrust events.
//  2. 20.3.3.5.2.3 - states that "it is expected that the almanac time parameters will
//                    provide a stistical URE component of less than 135 meters, one signma".
void OrbAlm_T::
setUpLNAV()
{
   init();

      // Define state variables for creating a LNAV store
   typeDesc = "GPS_LNAV";

      // Literals for LNAV test data 
   const unsigned short LNavExCount = 9;
   const std::string LNavEx[] =
   {
      "365,12/31/2015,00:02:18,1877,345738,1,63,425, 0x22C3550A, 0x1C230C58, 0x1FEE6CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C",
      "365,12/31/2015,00:02:24,1877,345744,1,63,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x000000E0",
      "365,12/31/2015,00:02:54,1877,345774,1,63,501, 0x22C3550A, 0x1C23CDB4, 0x104A1B03, 0x1EC3752A, 0x3F52C00A, 0x284334F8, 0x04C97D73, 0x04F1B747, 0x0917642F, 0x004000C4",
      "365,12/31/2015,00:03:18,1877,345798,1,63,402, 0x22C3550A, 0x1C244CC8, 0x164A03ED, 0x1EC5DBEA, 0x3F56803C, 0x28431268, 0x2F65B770, 0x0716C3D8, 0x37EDFFB5, 0x3CFFF7D4",
      "365,12/31/2015,00:03:24,1877,345804,1,63,502, 0x22C3550A, 0x1C246D40, 0x109F1A3A, 0x1EFFFE0A, 0x3F4E0031, 0x2843466E, 0x045B1909, 0x29A9E68A, 0x0E9523D3, 0x13800480",
      "365,12/31/2015,12:19:54,1877,389994,1,63,525, 0x22C3550A, 0x1FBD0DB0, 0x1CE4157D, 0x00000FC8, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000",
      "365,12/31/2015,12:20:24,1877,390024,1,63,501, 0x22C3550A, 0x1FBDAD2C, 0x104A1BA6, 0x24037521, 0x3F52803B, 0x284333DF, 0x04C7ADAD, 0x04F16DE7, 0x08E35CE8, 0x004001F0",
      "365,12/31/2015,12:20:48,1877,390048,1,63,402, 0x22C3550A, 0x1FBE2C68, 0x164A0792, 0x2405DB9B, 0x3F560035, 0x28431195, 0x2F63F00F, 0x07172429, 0x37BA98FC, 0x3CFFF734",
      "365,12/31/2015,12:20:54,1877,390054,1,63,502, 0x22C3550A, 0x1FBE4D18, 0x109F1D58, 0x243FFE3E, 0x3F4DC023, 0x284345AC, 0x04593A83, 0x29AAEDF7, 0x0E5F25B7, 0x13800480"
   };

      // Convert the LNAV strings to PNB
   if (debugLevel) std::cout << "Building PNB from strings" << std::endl;
   gpstk::ObsID currObsID(gpstk::ObsID::otNavMsg,
                          gpstk::ObsID::cbL1,
                          gpstk::ObsID::tcCA);
   gpstk::PackedNavBits msg;
   for (unsigned short i=0; i<LNavExCount; i++)
   {
      msg = getPnbLNav(currObsID,LNavEx[i]);
      dataList.push_back(msg);
   }

   SatID sid(1,SatID::systemGPS); 

      // 12/31/2015 12:00 == Week 1877, DOW 4, SOD 43200
   /*
    *  Excerpt from apc18774
    *  2015 12 31 12  0  0.00000000
    P  1  21660.911135  14060.970775  -6739.268021      7.973244                    
    V  1  -8691.174513   -995.106026 -30138.831453      0.009383                    
    P  2  -1441.679351 -16790.824227  21014.933039    598.668560                    
    V  2  24392.452591   8269.357517   8018.651385      0.004558                    
    P 25 -14870.571034  -8788.484129  20126.874993    -91.725396                    
    V 25  -2637.830466 -25557.191698 -12868.533309     -0.062373                    
    *
    *  In SP3
    *     Position is in km
    *     Clock is in microseconds
    *     Velocity is in dm/s
    *     Clock rate is 10**-4 microseconds/sec
    */
   double usecToSec = 1.0e-6;
   double rateChgToSec = usecToSec * 1.0e-2; 
   CommonTime truthTime = CivilTime(2015,12,31,12,00,00,TimeSystem::GPS);
   Xvt truthXvt; 
   truthXvt.x = Triple(  21660.911135, 14060.970775,  -6739.268021);
   truthXvt.v = Triple(  -8691.174513,  -995.106026, -30138.831453);
   truthXvt.x = 1000.0 * truthXvt.x;    // Convert from km to m
   truthXvt.v =    0.1 * truthXvt.v; 
   truthXvt.clkbias =  7.973244 * usecToSec;
   truthXvt.clkdrift = 0.009383 * rateChgToSec;
   truthXvt.relcorr = 0.0; 
   PassFailData pfd(sid,truthTime,truthXvt);
   pfList.push_back(pfd);

   sid = SatID(2,SatID::systemGPS); 
   truthXvt.x = Triple( -1441.679351, -16790.824227,  21014.933039);
   truthXvt.v = Triple( 24392.452591,   8269.357517,   8018.651385);
   truthXvt.x = 1000.0 * truthXvt.x;    // Convert from km to m
   truthXvt.v =    0.1 * truthXvt.v; 
   truthXvt.clkbias = 598.668560 * usecToSec;
   truthXvt.clkdrift =  0.004558 * rateChgToSec;
   pfd = PassFailData(sid,truthTime,truthXvt);
   pfList.push_back(pfd);

   sid = SatID(25,SatID::systemGPS); 
   truthXvt.x = Triple( -14870.571034,  -8788.484129,  20126.874993);
   truthXvt.v = Triple(  -2637.830466, -25557.191698, -12868.533309);
   truthXvt.x = 1000.0 * truthXvt.x;    // Convert from km to m
   truthXvt.v =    0.1 * truthXvt.v; 
   truthXvt.clkbias =  -91.725396 * usecToSec;
   truthXvt.clkdrift =  -0.062373 * rateChgToSec;
   pfd = PassFailData(sid,truthTime,truthXvt);
   pfList.push_back(pfd);

   return;
}

void OrbAlm_T::
setUpCNAV()
{
   init();

      // Define state variables for writing an CNAV data
   gpstk::ObsID currObsID(gpstk::ObsID::otNavMsg, 
                    gpstk::ObsID::cbL2, 
                    gpstk::ObsID::tcC2LM);
   typeDesc = "GPS_CNAV";

      // Literals for CNAV test data 
   const unsigned short CNavExCount = 8;
   const std::string CNavEx[] =
   {
     "067,03/07/2016,00:00:12,1887,086412,1,63,11, 0x8B04B1C2, 0x2099701F, 0xCD37C9D1, 0xBACE000D, 0xFED7C008, 0x8003BFF3, 0x2D2018DA, 0x4027AAFF, 0x4D180E4F, 0x7C300000",
     "067,03/07/2016,00:00:24,1887,086424,1,63,30, 0x8B05E1C2, 0x304CEA72, 0x640393DD, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000B16, 0x6FF00000",
     "067,03/07/2016,00:00:36,1887,086436,1,63,33, 0x8B0611C2, 0x404CEA72, 0x640393DD, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x00000502, 0xCA100000",
     "067,03/07/2016,00:00:48,1887,086448,1,63,10, 0x8B04A1C2, 0x50EBE44C, 0xE899007E, 0xF4400186, 0xD2F057FF, 0xC94FB679, 0x2D0014F5, 0x70B04AC8, 0x5FD807FD, 0xA7700000",
     "067,03/07/2016,02:00:00,1887,093600,1,63,10, 0x8B04A1E7, 0x90EBE44C, 0xE8A50073, 0x88BFFE1F, 0x93079000, 0x80226376, 0xB44814F4, 0x518C4AB0, 0x475C0E58, 0x3A700000",
     "067,03/07/2016,02:00:12,1887,093612,1,63,11, 0x8B04B1E7, 0xA0A5701F, 0x7F6BC9D1, 0xB8C2E015, 0xCEBEC007, 0xDFFCBFF1, 0x8F201A17, 0x202586FF, 0x38100791, 0x85000000",
     "067,03/07/2016,02:00:24,1887,093624,1,63,30, 0x8B05E1E7, 0xB04CEA72, 0x9403945F, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000C5A, 0xF4A00000",
     "067,03/07/2016,02:00:36,1887,093636,1,63,33, 0x8B0611E7, 0xC04CEA72, 0x9403945F, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x0000024E, 0x51400000"
   };

      // Convert the CNAV strings to PNB
   gpstk::PackedNavBits msg;
   for (unsigned short i=0; i<CNavExCount; i++)
   {
      msg = getPnbCNav(currObsID, CNavEx[i]);
      dataList.push_back(msg);
   }
   return;
}

void OrbAlm_T::
setUpBDS()
{

}

void OrbAlm_T::
setUpGLO()
{

}

   //---------------------------------------------------------------
   gpstk::PackedNavBits
   OrbAlm_T::
   getPnbLNav(const gpstk::ObsID& oidr, const std::string& str)
                          throw(gpstk::InvalidParameter)
   {      
      try
      {
            // Split the line into words separated by commas.
            // There should be 18 words
         vector<string> words = StringUtils::split(str,',');
         unsigned short numWords = words.size();
         /*
         cout << "numWords : " << numWords
              << ", [0]: '" << words[0]
              << "', [numWords-1]: '" << words[numWords-1] << "'." << endl;
         */
         if (numWords!=18) 
         {
            stringstream ss;
            ss << "Line format problem. ";
            ss << "  Should be at least 18 items.";
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }

            // Convert the time information into a CommonTime
         int week = gpstk::StringUtils::asInt(words[3]);
         double sow = gpstk::StringUtils::asDouble(words[4]);
         CommonTime ct = GPSWeekSecond(week,sow,TimeSystem::GPS);
         
            // Convert the PRN to a SatID
         int prn = StringUtils::asInt(words[5]);
         SatID sid(prn,SatID::systemGPS);

            // Get the message ID
         int msgID = StringUtils::asInt(words[7]); 

         PackedNavBits pnb(sid,oidr,ct); 

            // Load the raw data
         int offset = 8;
         for (int i=0; i<10; i++)
         {
            int ndx = i + offset;
            string hexStr = StringUtils::strip(words[ndx]);
            string::size_type n = hexStr.find("x"); 
            hexStr = hexStr.substr(n+1);
            unsigned long bits = StringUtils::x2uint(hexStr);
            pnb.addUnsignedLong(bits,30,1);
         }
         pnb.trimsize();
         return pnb; 
      }
      catch (StringUtils::StringException)
      {
         stringstream ss;
         ss << "String conversion error:'" << str << "'.";
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }  
   }

   
   //-------------------------------------------------
   gpstk::PackedNavBits 
   OrbAlm_T::
   getPnbCNav(const gpstk::ObsID& oidr, const std::string& str)
             throw(gpstk::InvalidParameter)
   {
      try
      {
            // Split the line into words separated by commas.
            // There should be 18 words
         vector<string> words = StringUtils::split(str,',');
         unsigned short numWords = words.size();
         if (numWords!=18) 
         {
            stringstream ss;
            ss << "Line format problem. ";
            ss << "  Should be at least 18 items.";
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }

            // Convert the time information into a CommonTime
         int week = gpstk::StringUtils::asInt(words[3]);
         double sow = gpstk::StringUtils::asDouble(words[4]);
         CommonTime ct = GPSWeekSecond(week,sow,TimeSystem::GPS);
         
            // Convert the PRN to a SatID
         int prn = StringUtils::asInt(words[5]);
         SatID sid(prn,SatID::systemGPS);

            // Get the message ID
         int msgID = StringUtils::asInt(words[7]); 

         PackedNavBits pnb(sid,oidr,ct); 

            // Load the raw data
            // Words 0-8 have 32 bits.
            // Word 9 has 12 bits and they'll be "left-justified"
            // in the sense that the string will look like 0xXXX00000;
            // that is to say, 12 bits plus 20 bits of zero padding.
         int offset = 8;
         for (int i=0; i<10; i++)
         {
            int ndx = i + offset;
            string hexStr = StringUtils::strip(words[ndx]);
            string::size_type n = hexStr.find("x"); 
            hexStr = hexStr.substr(n+1);
            unsigned long bits = StringUtils::x2uint(hexStr);
            if (offset<9) pnb.addUnsignedLong(bits,32,1);
            else
            {
               bits >>= 20;
               pnb.addUnsignedLong(bits,12,1);
            }
         }
         pnb.trimsize();
         return pnb;
      }
      catch (StringUtils::StringException)
      {
         stringstream ss;
         ss << "String conversion error:'" << str << "'.";
         InvalidParameter ip(ss.str());
         GPSTK_THROW(ip);
      }  
   }

int main()
{
  unsigned errorTotal = 0;
  
  OrbAlm_T testClass;

  testClass.setUpLNAV();
  errorTotal += testClass.createAndDump();
  
  testClass.setUpCNAV();
  //errorTotal += testClass.createAndDump();
  
  testClass.setUpBDS();
  //errorTotal += testClass.writeReadTest();
  
  testClass.setUpGLO();
  //errorTotal += testClass.writeReadTest();

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}


