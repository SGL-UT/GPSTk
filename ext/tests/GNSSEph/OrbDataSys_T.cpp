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
*  Test program for gpstk/ext/lib/GNSSEph/OrbDataSys* and /OrbSysStore*
*
*********************************************************************/
#include <iostream>
#include <fstream>

#include "CivilTime.hpp"
#include "Exception.hpp"
#include "GPSWeekSecond.hpp"
#include "OrbDataSys.hpp"
#include "OrbSysStore.hpp"
#include "OrbSysStoreGpsL.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class OrbDataSys_T
{
public:
   OrbDataSys_T();

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
   CommonTime initialCT;
   CommonTime finalCT;

   ofstream out; 

      // For testing the test
   int debugLevel;   
};

OrbDataSys_T::
OrbDataSys_T()
{
   debugLevel = 0; 
   init();
}

unsigned OrbDataSys_T::
createAndDump()
{
   string currMethod = typeDesc + " create/store OrbDataSys objects";
   TUDEF("OrbDataSys",currMethod);

      // Open an output stream specific to this navigation message type
   std::string fs = getFileSep(); 
   std::string tf(getPathTestTemp()+fs);
   std::string tempFile = tf + "test_output_OrbDatSys_T_" +
                         typeDesc+".txt";
   out.open(tempFile.c_str(),std::ios::out);
   if (!out)
   {
      stringstream ss;
      ss << "Could not open file " << tempFile << " for output.";
      TUFAIL(ss.str());
      TURETURN();
   }

      // All the navigation message data will be placed here. 
   OrbSysStoreGpsL oss;
   oss.setDebugLevel(debugLevel); 

   bool passed = true;
   unsigned long addSuccess = 0; 
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         bool retval = oss.addMessage(pnbr);
         if (retval) addSuccess++;
      }
      catch(gpstk::InvalidRequest ir)
      {
         passed = false;
         std::stringstream ss;
         ss << "Load of OrbSysStore failed." << std::endl;
         ss << ir; 
         TUFAIL(ss.str());
      }
   }
   unsigned long count = oss.size();
   if (count!=addSuccess)
   {
      stringstream ss;
      ss << "Size of ObsSysStore incorrect after loading.  Expected " 
         << addSuccess << " actual size " << count;
      TUFAIL(ss.str());
      passed = false; 
   }
   if (passed) TUPASS("Successfully loaded data to store.");

//--- Test the isPresent( ) method --------------------------------
   currMethod = typeDesc + " OrbSysStore.isPresent() "; 
   TUCSM(currMethod);
   SatID sidT1(1,SatID::systemGPS);
   if (oss.isPresent(sidT1)) 
      TUPASS("");
   else
      TUFAIL("Failed to find PRN 1 in store"); 
   
   SatID sidT2(33,SatID::systemGPS);
   if (oss.isPresent(sidT2))
      TUFAIL("Reported PRN 33 as present (which is not true)");
   else 
      TUPASS("");
      
//--- Test the getXXXTime( ) methods -------------------------
   currMethod = typeDesc + " OrbSysStore.getXxxxTime() "; 
   TUCSM(currMethod);
   try
   {
      const CommonTime& initialTestT = oss.getInitialTime();
      if (initialTestT==initialCT) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Incorrect initial time.  Expected "
            << printTime(initialCT,"%02m/%02d/%4Y %02H:%02M:%02S %P")
            << " found "
            << printTime(initialTestT,"%02m/%02d/%4Y %02H:%02M:%02S %P");
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
     stringstream ss;
     ss << "Unexpected exception." << ir;
     TUFAIL(ss.str());
   }

   try
   {
      const CommonTime& finalTestT = oss.getFinalTime();
      if (finalTestT==finalCT) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Incorrect final time.  Expected "
            << printTime(finalCT,"%02m/%02d/%4Y %02H:%02M:%02S %P")
            << " found "
            << printTime(finalTestT,"%02m/%02d/%4Y %02H:%02M:%02S %P");
         InvalidRequest ir(ss.str());
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
     stringstream ss;
     ss << "Unexpected exception." << ir;
     TUFAIL(ss.str());
   }

//--- Test the find( ) method --------------------------------
   currMethod = typeDesc + " OrbSysStore.find() "; 
   TUCSM(currMethod);
   SatID sidTest(1,SatID::systemGPS);
   NavID nidTest(NavID::ntGPSLNAV);
   unsigned long UID = 56;

      // TOO EARLY
   CommonTime testTime = CivilTime(2015,12,31,00,00,00,TimeSystem::GPS); 
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime); 
      stringstream ss;
      ss << "Failed to throw exception for time prior to all data";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }

      // Right on time (which is still too early)
   testTime = CivilTime(2015,12,31,00,11,18,TimeSystem::GPS); 
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime); 
      stringstream ss;
      ss << "Failed to throw exception for time prior to all data";
      TUFAIL(ss.str());
   }
   catch (InvalidRequest)
   {
      TUPASS("");
   }

      // Should return object with xMit of 00:11:31
   CommonTime expTime = testTime;
   testTime = CivilTime(2015,12,31,02,00,00,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime); 
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time " 
            << printTime(p->beginValid,"%02H:%02M:%02S"); 
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }
   
      // Should return object with xMit of 00:11:31
   testTime = CivilTime(2015,12,31,12,28,48,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime); 
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time " 
            << printTime(p->beginValid,"%02H:%02M:%02S"); 
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }

      // Should return object with xMit of 12:28:48
   expTime =  CivilTime(2015,12,31,12,28,48,TimeSystem::GPS);
   testTime = CivilTime(2015,12,31,14,00,00,TimeSystem::GPS);
   try
   {
      const OrbDataSys* p = oss.find(sidTest,nidTest,UID,testTime); 
      if (p->beginValid==expTime) TUPASS("");
      else
      {
         stringstream ss;
         ss << "Wrong object found.  Expected xmit time "
            << printTime(expTime,"%02H:%02M:%02S")
            << " found time " 
            << printTime(p->beginValid,"%02H:%02M:%02S"); 
         TUFAIL(ss.str());
      }
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << "Unexpected exception" << endl;
      ss << ir << endl;
      TUFAIL(ss.str());
   }

      // Dump the store
   currMethod = typeDesc + " OrbSysStore.dump()";
   TUCSM(currMethod);
   oss.dump(out);

      // Dump terse (one-line) summaries
   oss.dump(out,1);

      // Dump all contents
   oss.dump(out,2);

      // Dump terese in time order
   oss.dump(out,3);

      // Clear the store
   currMethod = typeDesc + " OrbSysStore.clear()";
   TUCSM(currMethod);

   oss.clear();
   if (oss.size()!=0)
   {
      TUFAIL("Failed to entirely clear OrbSysStore.");
   }
   else TUPASS("");

   out.close();

   TURETURN();
}

void OrbDataSys_T::
init()
{
   dataList.clear();
} 

void OrbDataSys_T::
setUpLNAV()
{
   init();

      // Define state variables for creating a LNAV store
   typeDesc = "GPS_LNAV";
   initialCT = CivilTime(2015,12,31,00,02,18,TimeSystem::GPS);
   finalCT   = CivilTime(2015,12,31,18,43,48,TimeSystem::GPS);

      // Literals for LNAV test data 
   const unsigned short LNavExCount = 14;
   const std::string LNavEx[] =
   {
      "365,12/31/2015,00:00:00,1877,345600,1,63,100, 0x22C3550A, 0x1C2029AC, 0x35540023, 0x0EA56C31, 0x16E4B88E, 0x37CECD3F, 0x171242FF, 0x09D588A2, 0x0000023F, 0x00429930",
      "365,12/31/2015,00:00:06,1877,345606,1,63,200, 0x22C3550A, 0x1C204A3C, 0x09FDB732, 0x0BC06889, 0x3C5827D1, 0x3E08808B, 0x21A678CF, 0x0472285B, 0x0350F3B4, 0x15889F94",
      "365,12/31/2015,00:00:12,1877,345612,1,63,300, 0x22C3550A, 0x1C206BB4, 0x3FFAC4D5, 0x0CAD96FA, 0x3FFA09D3, 0x10F0C405, 0x06D1C4E4, 0x31C1B694, 0x3FEA6E36, 0x09FFA5F4",
      "365,12/31/2015,00:00:18,1877,345618,1,63,421, 0x22C3550A, 0x1C208C44, 0x1E7181C9, 0x1C2E68A2, 0x0F4507DA, 0x247093F0, 0x26C720E5, 0x07E00109, 0x0196E4A3, 0x1D588110",
      "365,12/31/2015,00:00:24,1877,345624,1,63,521, 0x22C3550A, 0x1C20ADCC, 0x156ED525, 0x1EFEDF83, 0x3F4DC035, 0x2843463D, 0x047D1075, 0x2D2F1B44, 0x3814F871, 0x2FBFF920",
      "365,12/31/2015,00:11:18,1877,346278,1,63,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,00:11:24,1877,346284,1,63,518, 0x22C3550A, 0x1C2E6D4C, 0x14A1B3B8, 0x1EFD15DB, 0x3F4E4029, 0x2843301D, 0x0F1B6C25, 0x2C6E2942, 0x2EFBFAA5, 0x0F400B20",
      "365,12/31/2015,12:28:48,1877,390528,1,63,418, 0x22C3550A, 0x1FC82C44, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x00641562, 0x044EC0EB, 0x044000D8",
      "365,12/31/2015,12:28:54,1877,390534,1,63,518, 0x22C3550A, 0x1FC84D34, 0x14A1B582, 0x243D154A, 0x3F4DC023, 0x28432F8B, 0x0F198ACA, 0x2C6EA741, 0x2EC76168, 0x0F400C54",
      "365,12/31/2015,00:02:18,1877,345738,1,63,425, 0x22C3550A, 0x1C230C58, 0x1FEE6CC4, 0x2AEAEEC0, 0x26A66A75, 0x2A666666, 0x26EEEE53, 0x2AEA4013, 0x0000003F, 0x0000006C",
      "365,12/31/2015,00:02:24,1877,345744,1,63,525, 0x22C3550A, 0x1C232DD0, 0x1CDED544, 0x00000FDE, 0x00000029, 0x00000016, 0x00000029, 0x00000016, 0x00000029, 0x000000E0",
      "365,12/31/2015,00:11:18,1877,346278,2,61,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x005ED55B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,18:43:48,1877,413028,2,61,418, 0x22C3550A, 0x219CECF0, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFE7B, 0x3FFFFFFC, 0x3F641555, 0x044EC0D4, 0x044000B4",
      "365,12/31/2015,00:11:18,1877,346278,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054"
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
   return;
}

void OrbDataSys_T::
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

void OrbDataSys_T::
setUpBDS()
{

}

void OrbDataSys_T::
setUpGLO()
{

}


   //---------------------------------------------------------------
   gpstk::PackedNavBits
   OrbDataSys_T::
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
   OrbDataSys_T::
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
  
  OrbDataSys_T testClass;

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


