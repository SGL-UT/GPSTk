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

 /*********************************************************************
*
*  Test program for 
*     gpstk/ext/lib/GNSSEph/CNavReducedAlm
*     gpstk/ext/lib/GNSSEph/DiffCorrBase
*     gpstk/ext/lib/GNSSEph/DiffCorrClk
*     gpstk/ext/lib/GNSSEph/DiffCorrEph
*
*
*********************************************************************/

#include <iostream>
#include <fstream>

#include "CivilTime.hpp"
#include "Exception.hpp"
#include "GPSWeekSecond.hpp"
#include "OrbSysGpsC.hpp"
#include "OrbSysStore.hpp"
#include "NavID.hpp"
#include "SystemTime.hpp"
#include "TimeString.hpp"
#include "TimeSystem.hpp"

#include "CNavReducedAlm.hpp"
#include "DiffCorrClk.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class CNavPackets_T
{
public:
   CNavPackets_T();

   void init();

   unsigned createStore();
   unsigned test_ReducedAlm(); 
   unsigned test_Corrections(); 

//-------------------------------------------------------------------
   void setUpCNAV();
   gpstk::PackedNavBits getPnbCNav(const gpstk::ObsID& oidr,
                                   const std::string& str);

      // Methods above exist to set up the following
      // members
   list<PackedNavBits> dataList;
   string typeDesc;
   CommonTime initialCT;
   CommonTime finalCT;
   OrbSysStore oss;
   NavID nid;

   ofstream out; 

      // For testing the test
   int debugLevel;   
};

//-------------------------------------------------------------------
CNavPackets_T::
CNavPackets_T(): nid(NavType::GPSCNAV2)
{
   debugLevel = 0; 
   init();
}

//-------------------------------------------------------------------
unsigned CNavPackets_T::
createStore()
{
   bool loadFail = false;
   unsigned retVal = 0; 
   string currMethod = typeDesc + " create/store CNAV-2 objects";
   TUDEF("CNavPackets_T",currMethod);

      // Open an output stream specific to this navigation message type
      // All the navigation message data will be placed here. 
   oss.setDebugLevel(debugLevel); 

   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin();cit!=dataList.end();cit++)
   {
      try
      {
         const PackedNavBits& pnbr = *cit;
         bool retval = oss.addMessage(pnbr);
         /*
         unsigned long sf = pnbr.asUnsignedLong(49,3,1);
         unsigned long svid = 0;
         if (sf>3) svid = pnbr.asUnsignedLong(62,6,1);
         if (debugLevel) cout << "    succeeded...."
                              << pnbr.getsatSys() << ", "
                              << " sf, SV ID: " << sf << ", " << svid << endl;
         */
      }
      catch(gpstk::InvalidRequest ir)
      {
         loadFail = true;
         cout << ir << endl;
      }
   }

   if (loadFail) retVal = 1;
   return retVal; 
}

//-------------------------------------------------------------------
void CNavPackets_T::
init()
{
   dataList.clear();
} 

//-------------------------------------------------------------------
void CNavPackets_T::
setUpCNAV()
{
   init();

      // Define state variables for writing an CNAV data
   gpstk::ObsID currObsID(gpstk::ObservationType::NavMsg, 
                    gpstk::CarrierBand::L1, 
                    gpstk::TrackingCode::L1CD);
   typeDesc = "GPS_CNAV2";
   initialCT = CivilTime(2017,1,1,00,00,24,TimeSystem::GPS);
   finalCT   = CivilTime(2017,1,1,00,54,12,TimeSystem::GPS);

      // Literals for CNAV-2 test data 
      // Test data set corresponds to GPS III L1C test data collected in Spring 2018.   At the time of this writing, these
      // are the only test data we have with examples of reduced almanac packets and corrction packets.
   const unsigned short CNavExCount = 5;
   const std::string CNavEx[] =
   {
"217,08/05/2011,06:11:00,1647,454260,11,46,305, 0x0B16F85F, 0x00B7FFC0, 0xAC2DF3CF, 0xE58DEA82, 0x9C44FFF2, 0x40000000, 0x00000000, 0x0000002C, 0xD76F0000",
"217,08/05/2011,06:11:18,1647,454278,11,46,305, 0x0B16F85F, 0x0090017F, 0x74242687, 0xF7E94140, 0x9854C002, 0x40000000, 0x00000000, 0x00000025, 0xE0A98000",
"217,08/05/2011,06:15:48,1647,454548,11,46,303, 0x0B0CCDF2, 0x016013C6, 0x811FE81F, 0x886002AE, 0x2438000F, 0xF021F8AE, 0x0915FFAA, 0xD080002D, 0x803B4000",
"217,08/05/2011,06:16:06,1647,454566,11,46,303, 0x0B0CCDF2, 0x02601051, 0x902FEA96, 0x48BFF80B, 0x407FFCAE, 0x400BFFAA, 0x111DFF81, 0x4480001B, 0xA2048000",
// Following is a hand-edited copy the data from 6:15:48 in which the PRN value of the first packed has been changed to zero. 
"217,08/05/2011,06:16:24,1647,454548,11,46,303, 0x0B0CCDF2, 0x000013C6, 0x811FE81F, 0x886002AE, 0x2438000F, 0xF021F8AE, 0x0915FFAA, 0xD080002D, 0x803B4000",
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
 
//-------------------------------------------------------------------
   gpstk::PackedNavBits 
   CNavPackets_T::
   getPnbCNav(const gpstk::ObsID& oidr, const std::string& str)
   {
      try
      {
            // Split the line into words separated by commas.
            // There should be 18 words
         vector<string> words = StringUtils::split(str,',');
         unsigned short numWords = words.size();
         if (numWords!=17) 
         {
            stringstream ss;
            ss << "Line format problem. ";
            ss << "  Should be at least 17 items.";
            InvalidParameter ip(ss.str());
            GPSTK_THROW(ip);
         }

            // Convert the time information into a CommonTime
         int week = gpstk::StringUtils::asInt(words[3]);
         double sow = gpstk::StringUtils::asDouble(words[4]);
         CommonTime ct = GPSWeekSecond(week,sow,TimeSystem::GPS);
         
            // Convert the PRN to a SatID
         int prn = StringUtils::asInt(words[5]);
         SatID sid(prn,SatelliteSystem::GPS);

            // Get the message ID
         int msgID = StringUtils::asInt(words[7]); 

         PackedNavBits pnb(sid,oidr,ct); 

            // Load the raw data
            // Words 0-7 have 32 bits.
            // Word 8 has 18 bits and they'll be "left-justified"
            // in the sense that the string will look like 0xXXX00000;
            // that is to say, 18 bits plus 14 bits of zero padding.
         int offset = 8;
         for (int i=0; i<9; i++)
         {
            int ndx = i + offset;
            string hexStr = StringUtils::strip(words[ndx]);
            string::size_type n = hexStr.find("x"); 
            hexStr = hexStr.substr(n+1);
            unsigned long bits = StringUtils::x2uint(hexStr);
            if (i<8) pnb.addUnsignedLong(bits,32,1);
            else
            {
               bits >>= 14;
               pnb.addUnsignedLong(bits,18,1);
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

//-------------------------------------------------------------------
unsigned CNavPackets_T::
test_ReducedAlm()
{
   string currMethod = "loadData()";
   TUDEF("CNavReducedAlm",currMethod);
   unsigned retVal = 0;

   double epsilon = 1.0e-9;

   int prnVals[2][6] =
   {
      { 11,17,12,14,16,21 },
      { 19,02,23,31,05,29 }
   };
   double deltaAVals[2][6] =
   {
      { 0.0000000000E+00, -1.0240000000E+03,  0.0000000000E+00,  0.0000000000E+00, -2.0480000000E+03, -5.1200000000E+02},
      { 0.0000000000E+00, -1.0240000000E+03, -5.1200000000E+02, -5.1200000000E+02, -5.1200000000E+02, -5.1200000000E+02}
   };
   double Omega0Vals[2][6] = 
   {
      { -2.4052818754E+00, -3.1415926536E+00,  2.0616701789E+00,  4.9087385212E-02,  2.1107575641E+00, -2.1107575641E+00},
      { -3.0925052684E+00, -2.1598449493E+00,  0.0000000000E+00,  1.0308350895E+00, -1.0799224747E+00, -3.1415926536E+00}
   };
   double Psi0Vals[2][6] =
   {
      {  6.3813600776E-01, -9.8174770425E-02, -7.3631077819E-01, -4.9087385212E-02, -3.0925052684E+00,  2.5525440310E+00},
      {  1.7180584824E+00, -1.9144080233E+00, -1.8653206381E+00, -1.3744467859E+00, -3.0434178832E+00, -2.3071071050E+00}
   };
   int L5HVals[2][6] =
   {
      { 0, 1, 1, 0, 1, 1 },
      { 1, 1, 0, 0, 1, 1 }
   };


         // Construct a fake CommonTime for test purposes
   CommonTime ctNow = CivilTime(2011,8,11,0,0,0.0);
   ctNow.setTimeSystem(TimeSystem::GPS); 

   unsigned countMajor = 0; 
   unsigned totalCount = 0; 
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin(); cit!=dataList.end(); cit++)
   {
      const PackedNavBits& pnb = *cit;
      try
      {
         unsigned startBit = 35;
         unsigned countMinor = 0;
         for (int i=0;i<6;i++)
         {
            CNavReducedAlm cra = CNavReducedAlm(CNavReducedAlm::atCNAV2,ctNow,pnb,startBit);
            TUASSERTE(CommonTime,ctNow,cra.ctAlmEpoch); 
            TUASSERTE(int,prnVals[countMajor][countMinor],cra.subjSv.id);
            TUASSERTFEPS(cra.deltaA,deltaAVals[countMajor][countMinor],epsilon); 
            TUASSERTFEPS(cra.OMEGA0,Omega0Vals[countMajor][countMinor],epsilon); 
            TUASSERTFEPS(cra.Psi0,Psi0Vals[countMajor][countMinor],epsilon); 
            TUASSERTE(unsigned,0,cra.L1HEALTH);
            TUASSERTE(unsigned,0,cra.L2HEALTH);
            TUASSERTE(unsigned,L5HVals[countMajor][countMinor],cra.L5HEALTH); 
            startBit += 33;
            countMinor++; 
         }
         countMajor++;
      }
      catch (InvalidParameter ip)
      {
         if (totalCount==0 || totalCount==1)
         {
            stringstream ss;
            ss << "Successfully caught InvalidRequest on wrong message type.";
            TUPASS(ss.str());
         }
         else if (totalCount==4)
         {
            stringstream ss;
            ss << "Successfully detected zero PRN packet." << endl;
            ss << "Exception text: " << ip; 
            TUPASS(ss.str());           
         }
         else
         {
            stringstream ss;
            ss << "Threw InvalidParameter exception on valid data set.";
            ss << endl << "Exception text: " << ip; 
            TUFAIL(ss.str());
         }
      }
      totalCount++; 
   }

      // Puprosely request a packet that would be outside the length of the message
   cit = dataList.begin();
   cit++;
   cit++;   // move to the third entry
   const PackedNavBits& pnbTest = *cit;
   try 
   {
      CNavReducedAlm craTest = CNavReducedAlm(CNavReducedAlm::atCNAV2,ctNow,pnbTest,275);
      TUFAIL("Requested a packet beyond the end of the message and did NOT throw an exception.");
   }
   catch (InvalidParameter ip)
   {
      TUPASS("Correctly threw an exception when requesting a packet outside the message."); 
   }
     
   TURETURN(); 
}

//-------------------------------------------------------------------
unsigned CNavPackets_T::
test_Corrections()
{
   string currMethod = "loadData()";
   TUDEF("CNavDiffCorrection",currMethod);

      // Truth values.  Empirically obtained by cracking the test messages
   CommonTime ctExpected = GPSWeekSecond(1647, 456000.0);
   ctExpected.setTimeSystem(TimeSystem::GPS);
   double epsilon = 1.0e-18;
   unsigned xmitPrnId = 11;
   double daf0[2] = {  -2.9103830457E-11,   1.4551915228E-10 };
   double daf1[2] = {   8.8817841970E-16,  -1.3322676296E-15 };
   unsigned subjPrnId[2] = { 22, 18 };
  
   unsigned retVal = 0;
   unsigned countMajor = 0; 
   unsigned totalCount = 0; 
   list<PackedNavBits>::const_iterator cit;
   for (cit=dataList.begin(); cit!=dataList.end(); cit++)
   {
      const PackedNavBits& pnb = *cit;
      try
      {
         unsigned startBit = 37;
         DiffCorrClk cdc = DiffCorrClk(pnb,startBit);
         TUASSERTE(CommonTime,ctExpected,cdc.topD); 
         TUASSERTE(CommonTime,ctExpected,cdc.tOD);
         TUASSERTFEPS(daf0[countMajor],cdc.daf0,epsilon); 
         TUASSERTFEPS(daf1[countMajor],cdc.daf1,epsilon); 
         TUASSERTE(unsigned,subjPrnId[countMajor],cdc.subjSv.id);
         TUASSERTE(unsigned,xmitPrnId,cdc.xmitSv.id);

         //startBit = startBit + 34; 
         //DiffCorrEph edc = DiffCorrEph(pnb,startBit);
         countMajor++;
      }
      catch (InvalidParameter ip)
      {
         if (totalCount>1)
         {
            stringstream ss;
            ss << "Successfully caught InvalidRequest on wrong message type.";
            TUPASS(ss.str());
         }
         /*
         else if (totalCount==4)
         {
            stringstream ss;
            ss << "Successfully detected zero PRN packet." << endl;
            ss << "Exception text: " << ip; 
            TUPASS(ss.str());           
         }
         */
         else
         {
            stringstream ss;
            ss << "Threw InvalidParameter exception on valid data set.";
            ss << endl << "Exception text: " << ip; 
            TUFAIL(ss.str());
         }
      }
      totalCount++; 
   }

   TURETURN(); 
}

//-------------------------------------------------------------------
int main(int argc, char *argv[])
{
  unsigned errorTotal = 0;
  
  CNavPackets_T testClass;

  testClass.setUpCNAV();
  errorTotal += testClass.createStore();

  errorTotal += testClass.test_ReducedAlm();
  errorTotal += testClass.test_Corrections();

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}


