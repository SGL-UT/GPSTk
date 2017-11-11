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
*  Test program for gpstk/ext/lib/GNSSEph/OrbSysGpsC_??
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

#include "OrbSysGpsC_30.hpp"
#include "OrbSysGpsC_32.hpp"
#include "OrbSysGpsC_33.hpp"

#include "build_config.h"
#include "TestUtil.hpp"

using namespace std;
using namespace gpstk;

class OrbSysGpsC_T
{
public:
   OrbSysGpsC_T();

   void init();

   unsigned createStore();
   unsigned test_MT30(); 
   unsigned test_MT32(); 
   unsigned test_MT33(); 

   CommonTime exercise_getUT1(CommonTime& ct) throw(InvalidRequest);


   void setUpCNAV();
   gpstk::PackedNavBits getPnbCNav(const gpstk::ObsID& oidr,
                                   const std::string& str)
             throw(gpstk::InvalidParameter);

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

OrbSysGpsC_T::
OrbSysGpsC_T(): nid(NavID::ntGPSCNAVL2)
{
   debugLevel = 0; 
   init();
}

unsigned OrbSysGpsC_T::
createStore()
{
   bool loadFail = false;
   unsigned retVal = 0; 
   string currMethod = typeDesc + " create/store OrbSysGpsC objects";
   TUDEF("OrbSysGpsC",currMethod);

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

void OrbSysGpsC_T::
init()
{
   dataList.clear();
} 

void OrbSysGpsC_T::
setUpCNAV()
{
   init();

      // Define state variables for writing an CNAV data
   gpstk::ObsID currObsID(gpstk::ObsID::otNavMsg, 
                    gpstk::ObsID::cbL2, 
                    gpstk::ObsID::tcC2LM);
   typeDesc = "GPS_CNAV";
   initialCT = CivilTime(2017,1,1,00,00,24,TimeSystem::GPS);
   finalCT   = CivilTime(2017,1,1,00,54,12,TimeSystem::GPS);

      // Literals for CNAV test data 
      // Test data set corresponds to Dec 30, 2016 - Jan 1, 2017 in order to test Dec 31, 2016-Jan 1, 2017 Leap Second Event
   const unsigned short CNavExCount = 42;
   const std::string CNavEx[] =
   {
   "365,12/30/2016,03:16:12,1929,443772,15,55,11, 0x8B3CB907, 0x62E4AE0B, 0x04A48977, 0x9B3AFF75, 0x7ED94014, 0x0007DFFC, 0xF5001EBF, 0x60193CFF, 0xD8100A34, 0x98700000",
   "365,12/30/2016,03:16:48,1929,443808,15,55,10, 0x8B3CA907, 0x90F126E2, 0x76E48108, 0x37800147, 0x43CB47FF, 0xBBE0A633, 0xB25024D2, 0x75B85A8C, 0x02CE01FC, 0x21300000",
   "365,12/30/2016,03:17:00,1929,443820,15,55,11, 0x8B3CB907, 0xA2E4AE0B, 0x04A48977, 0x9B3AFF75, 0x7ED94014, 0x0007DFFC, 0xF5001EBF, 0x60193CFF, 0xD8100831, 0x6AB00000",
   "365,12/30/2016,03:17:12,1929,443832,15,55,30, 0x8B3DE907, 0xB255757B, 0x95A72431, 0x7FF62001, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x8900091B, 0x3A400000",
   "365,12/30/2016,03:18:00,1929,443880,15,55,30, 0x8B3DE907, 0xF2E270FB, 0x93A72421, 0x7FF61801, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x89000DFD, 0xDC300000",
   "365,12/30/2016,03:18:12,1929,443892,15,55,33, 0x8B3E1908, 0x02E270FB, 0x93A72421, 0x7FF61800, 0x005C0110, 0x022D8367, 0x893C4B89, 0x00000000, 0x000004F3, 0x17600000",
   "365,12/30/2016,03:19:00,1929,443940,15,55,32, 0x8B3E0908, 0x42E270FB, 0x93A72421, 0x7FF61800, 0xD8361530, 0xAE1E043D, 0x400BE3FE, 0x5F57FBBB, 0xA6800F51, 0xB5D00000",
   "366,12/31/2016,02:28:48,1929,527328,15,55,10, 0x8B3CAABA, 0x90F1276D, 0xF7748106, 0x330000F4, 0x43D98FFF, 0xCD50BCD8, 0x242024D3, 0xBB585AAF, 0xD95200E0, 0x5DB00000",
   "366,12/31/2016,02:29:00,1929,527340,15,55,11, 0x8B3CBABA, 0xA374AE07, 0x2E094977, 0x848EDF5B, 0xAF3E8014, 0xC0057FFD, 0xEFE020A8, 0x6015E0FF, 0xE5C80BB7, 0x5FF00000",
   "366,12/31/2016,02:29:12,1929,527352,15,55,30, 0x8B3DEABA, 0xB36DECFD, 0xD3A71D95, 0xFFF61801, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x89000AE1, 0x35500000",
   "366,12/31/2016,02:30:12,1929,527412,15,55,33, 0x8B3E1ABB, 0x036DECFD, 0xD3A71D95, 0xFFF61801, 0xFFC1FEE0, 0x02301147, 0x893C4B89, 0x00000000, 0x000003E9, 0x99B00000",
   "366,12/31/2016,02:31:00,1929,527460,15,55,32, 0x8B3E0ABB, 0x436DECFD, 0xD3A71D95, 0xFFF61801, 0x011414D8, 0xEE11643A, 0x810C1FFE, 0x5E91ABB0, 0xF8800677, 0x25E00000",
   "001,01/01/2017,00:00:12,1930,000012,15,55,11, 0x8B3CB000, 0x20092B8F, 0xA27D4977, 0x772E3F44, 0xFFD0400B, 0x400F9FFF, 0x428021A4, 0xC013D9FF, 0xF5E00889, 0x70E00000",
   "001,01/01/2017,00:00:24,1930,000024,15,55,30, 0x8B3DE000, 0x336DF570, 0x25A71796, 0xFFF62001, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x89000853, 0xDFA00000",
   "001,01/01/2017,00:00:36,1930,000036,15,55,32, 0x8B3E0000, 0x436DF570, 0x25A71796, 0xFFF62001, 0x011414D8, 0xEE11643A, 0x810C1FFE, 0x5E91ABB0, 0xF88000CE, 0x2A900000",
   "001,01/01/2017,00:00:48,1930,000048,15,55,10, 0x8B3CA000, 0x50F1476D, 0x84090107, 0x14DFFF23, 0x73E68000, 0x150E4396, 0xE1A024D2, 0xC3285AC8, 0x85920020, 0xC2B00000",
   "001,01/01/2017,00:01:00,1930,000060,15,55,11, 0x8B3CB000, 0x60092B8F, 0xA27D4977, 0x772E3F44, 0xFFD0400B, 0x400F9FFF, 0x428021A4, 0xC013D9FF, 0xF5E00975, 0xDEA00000",
   "001,01/01/2017,00:01:12,1930,000072,15,55,30, 0x8B3DE000, 0x736DF570, 0x25A71796, 0xFFF62001, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x890009AF, 0x71E00000",
   "001,01/01/2017,00:01:24,1930,000084,15,55,33, 0x8B3E1000, 0x836DF570, 0x25A71796, 0xFFF62001, 0xFFC1FEE0, 0x02301147, 0x893C4B89, 0x00000000, 0x000006A9, 0xCA400000",
   "001,01/01/2017,00:38:24,1930,002304,15,55,10, 0x8B3CA00C, 0x10F14401, 0xF4088107, 0x36FFFF23, 0x73E6D000, 0x151E2710, 0x6E3024D2, 0xC3545AC8, 0x883E022F, 0x6C300000",
   "001,01/01/2017,00:38:36,1930,002316,15,55,11, 0x8B3CB00C, 0x2008AB8F, 0xA5F58977, 0x7731FF44, 0xFFD0000B, 0x400F9FFF, 0x428021A4, 0xC013D9FF, 0xF5E00966, 0x66D00000",
   "001,01/01/2017,00:38:48,1930,002328,15,55,30, 0x8B3DE00C, 0x336DF570, 0x25A71796, 0xFFF62001, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2DF9FE0B, 0x890000A1, 0xA3700000",
   "001,01/01/2017,00:39:36,1930,002376,15,55,30, 0x8B3DE00C, 0x7001F170, 0x23A71796, 0x7FF61801, 0xE90FF903, 0x36001000, 0x08FEFF02, 0x2CFCFE07, 0x8A0007BB, 0x9AC00000",
   "001,01/01/2017,00:40:36,1930,002436,15,55,33, 0x8B3E100C, 0xC001F170, 0x23A71796, 0x7FF61800, 0x000C0050, 0x02400707, 0x8A3C4B89, 0x00000000, 0x00000885, 0x8B900000",
   "001,01/01/2017,00:41:24,1930,002484,15,55,32, 0x8B3E000D, 0x0001F170, 0x23A71796, 0x7FF61800, 0x00701483, 0x2E03E437, 0xF40C6E02, 0x5DAE3FA8, 0xB2000D31, 0x11D00000",
   "365,12/30/2016,03:55:36,1929,446136,32,70,30, 0x8B81E913, 0xB2E6F17B, 0x93BB8D2A, 0x7F792800, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2DF9FE0B, 0x8900089D, 0x4DF00000",
   "365,12/30/2016,03:55:48,1929,446148,32,70,33, 0x8B821913, 0xC2E6F17B, 0x93BB8D2A, 0x7F792800, 0x005E0110, 0x022D9887, 0x893C4B89, 0x00000000, 0x000001AB, 0x36700000",
   "365,12/30/2016,03:56:00,1929,446160,32,70,10, 0x8B80A913, 0xD0F126E6, 0xF6E4809C, 0x999FFE7D, 0x134AF800, 0x37B4CFD2, 0xD3B8028C, 0x70B24240, 0x5DF60835, 0x4E700000",
   "365,12/30/2016,03:56:12,1929,446172,32,70,11, 0x8B80B913, 0xE2E4AF45, 0xD55B49C3, 0x0AF68011, 0xFEE34001, 0x3FFC3FFC, 0xC6601F3E, 0xE01B9CFF, 0xD40808FC, 0x5F200000",
   "365,12/30/2016,03:56:24,1929,446184,32,70,30, 0x8B81E913, 0xF2E6F17B, 0x93BB8D2A, 0x7F792800, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2DF9FE0B, 0x89000961, 0xE3B00000",
   "365,12/30/2016,03:56:36,1929,446196,32,70,32, 0x8B820914, 0x02E6F17B, 0x93BB8D2A, 0x7F792800, 0xD988152C, 0xDE1E043D, 0x580BE3FE, 0x5F4F6FBB, 0xA68005A1, 0x16200000",
   "366,12/31/2016,03:01:36,1929,529296,32,70,10, 0x8B80AAC4, 0xD0F12770, 0xFB74809A, 0x0D9FFE38, 0xC359A800, 0x46B4ED1B, 0x37E8028F, 0x9D1240C5, 0xEF7E0864, 0xB9D00000",
   "366,12/31/2016,03:01:48,1929,529308,32,70,11, 0x8B80BAC4, 0xE374AF42, 0x2BBB89C2, 0xF17CFFF7, 0x8F480003, 0x1FFB9FFD, 0xA2A0213B, 0x20181EFF, 0xDFD80DC2, 0x84100000",
   "366,12/31/2016,03:02:00,1929,529320,32,70,30, 0x8B81EAC4, 0xF370F0FD, 0xD3BB343C, 0x7F795000, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2DF9FE0B, 0x890003FA, 0xF2900000",
   "366,12/31/2016,03:02:12,1929,529332,32,70,32, 0x8B820AC5, 0x0370F0FD, 0xD3BB343C, 0x7F795001, 0x01F614D6, 0x4E11643A, 0x910C1FFE, 0x5E8B17B0, 0xF880019B, 0xBCF00000",
   "366,12/31/2016,03:03:00,1929,529380,32,70,33, 0x8B821AC5, 0x4370F0FD, 0xD3BB343C, 0x7F795001, 0xFFBFFEE0, 0x02301F67, 0x893C4B89, 0x00000000, 0x0000043C, 0xA0600000",
   "001,01/01/2017,00:00:12,1930,000012,32,70,11, 0x8B80B000, 0x20092CCA, 0xC93D89C2, 0xE12BFFD7, 0xEF827FFD, 0x60041FFE, 0xC0202272, 0xA0161DFF, 0xEEA8061D, 0x4F400000",
   "001,01/01/2017,00:00:24,1930,000024,32,70,30, 0x8B81E000, 0x3370F4F0, 0x25BAE29D, 0x7F799000, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2DF9FE0B, 0x89000209, 0xF7200000",
   "001,01/01/2017,00:00:36,1930,000036,32,70,32, 0x8B820000, 0x4370F4F0, 0x25BAE29D, 0x7F799001, 0x01F614D6, 0x4E11643A, 0x910C1FFE, 0x5E8B17B0, 0xF8800463, 0x5CC00000",
   "001,01/01/2017,00:00:48,1930,000048,32,70,10, 0x8B80A000, 0x50F14770, 0x8409009E, 0xCA1FFFE4, 0x536CDFFF, 0xEBA278A5, 0x59C80293, 0x1AC23FB6, 0x08CE0456, 0x5DA00000",
   "001,01/01/2017,00:01:00,1930,000060,32,70,11, 0x8B80B000, 0x60092CCA, 0xC93D89C2, 0xE12BFFD7, 0xEF827FFD, 0x60041FFE, 0xC0202272, 0xA0161DFF, 0xEEA807E1, 0xE1000000",
   "001,01/01/2017,00:01:12,1930,000072,32,70,30, 0x8B81E000, 0x7370F4F0, 0x25BAE29D, 0x7F799000, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2DF9FE0B, 0x890003F5, 0x59600000",
   "001,01/01/2017,00:01:24,1930,000084,32,70,33, 0x8B821000, 0x8370F4F0, 0x25BAE29D, 0x7F799001, 0xFFBFFEE0, 0x02301F67, 0x893C4B89, 0x00000000, 0x0000023D, 0x1CD00000",
   "001,01/01/2017,00:53:24,1930,003204,32,70,32, 0x8B820010, 0xC00370F0, 0x23BAE2DD, 0x7F798000, 0x00E21481, 0xDE03E437, 0xFC0C6E02, 0x5DAA9BA8, 0xB2000809, 0xCDB00000",
   "001,01/01/2017,00:53:36,1930,003216,32,70,10, 0x8B80A010, 0xD0F14403, 0x7808809E, 0xC89FFFE4, 0x536D4FFF, 0xEBB25C1F, 0x35300293, 0x1C8E3FB5, 0xEE7E0CA6, 0x6A000000",
   "001,01/01/2017,00:53:48,1930,003228,32,70,11, 0x8B80B010, 0xE008ACCA, 0xCC8BC9C2, 0xE13A3FD7, 0xEF825FFD, 0x60041FFE, 0xC0202272, 0xA0161DFF, 0xEEA8032E, 0x56500000",
   "001,01/01/2017,00:54:00,1930,003240,32,70,30, 0x8B81E010, 0xF00370F0, 0x23BAE2DD, 0x7F798000, 0x0100047F, 0x50262140, 0x08FEFF02, 0x2CFCFE07, 0x8A000C90, 0x28A00000",
   "001,01/01/2017,00:54:12,1930,003252,32,70,33, 0x8B821011, 0x000370F0, 0x23BAE2DD, 0x7F798000, 0x000C0050, 0x02400E27, 0x8A3C4B89, 0x00000000, 0x00000210, 0x45D00000"
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
 
   //-------------------------------------------------
   gpstk::PackedNavBits 
   OrbSysGpsC_T::
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

// Stub for future methods that may be written
unsigned OrbSysGpsC_T::
test_MT30()
{
   string currMethod = typeDesc + " MT30 get methods";
   TUDEF("OrbSysGpsC",currMethod);
   unsigned retVal = 0;
   return retVal; 
}

unsigned OrbSysGpsC_T::
test_MT32()
{
   string currMethod = "getUT1()";
   TUDEF("OrbSysGpsC_32",currMethod);
   unsigned retVal = 0;

   double epsilon = 1.0e-9; 

   // We want to verify UT1 is appropriate in the following cases:
   //  1.)  At the beginning of the day of the leap second (IS-GPS-200 20.3.3.5.2.4.a)
   //  2.)  During the leap second transition wherre UTC is discontinuous but UT1 is 
   //       continuous (23:59:59, 23:59:60, and 00:00:00 UTC).
   //       In this case, 
   //  3.)  At the end of the day after the leap second (once a new upload has occurred).
   static const unsigned int NVALUES = 5;
   CommonTime test[NVALUES];
   test[0] = CivilTime(2016,12,31, 0,00,00, TimeSystem::GPS); 
   test[1] = CivilTime(2017, 1, 1, 0,00,16, TimeSystem::GPS);
   test[2] = CivilTime(2017, 1, 1, 0,00,17, TimeSystem::GPS);
   test[3] = CivilTime(2017, 1, 1, 0,00,18, TimeSystem::GPS);
   test[4] = CivilTime(2017, 1, 1,23,00,00, TimeSystem::GPS);

   CommonTime truth[NVALUES];
   truth[0] = CivilTime(2016,12,30,23,59,42.5921964802, TimeSystem::UTC);
   truth[1] = CivilTime(2016,12,31,23,59,58.5912595337, TimeSystem::UTC);
   truth[2] = CivilTime(2016,12,31,23,59,59.5912595198, TimeSystem::UTC);
   truth[3] = CivilTime(2017, 1, 1,00,00,00.5912595058, TimeSystem::UTC);
   truth[4] = CivilTime(2017, 1, 1,22,59,42.5902221216, TimeSystem::UTC);

   for (int i=0;i<NVALUES;i++)
   {
      try
      {
         CommonTime ut1 = exercise_getUT1(test[i]); 
         //cout << "ct, ut1: " << printTime(test[i],"%02H:%02M:%02S") 
         //     << ", " << printTime(ut1,"%02H:%02M:%015.12f") << endl;
         double diff = ut1 - truth[i];
         TUASSERTFEPS(diff,0.0,epsilon);
      }
      catch (InvalidRequest ir)
      {
         stringstream ss;
         ss << ir << " for time " << printTime(test[i],"%02H:%02M:%02S");
         TUFAIL(ss.str());
      }
   }

   TURETURN(); 
}

CommonTime OrbSysGpsC_T::
exercise_getUT1(CommonTime& currT)
   throw(InvalidRequest)
{
   const OrbDataSys* odsp;
   const OrbSysGpsC_32* mt32;
   const OrbSysGpsC_33* mt33;
   try
   {
      odsp = oss.find(nid,32,currT);
      mt32 = dynamic_cast<const OrbSysGpsC_32*>(odsp);
   }
   catch(InvalidRequest ir)
   {
      GPSTK_RETHROW(ir);
   }

   try
   {
      odsp = oss.find(nid,33,currT);
      mt33 = dynamic_cast<const OrbSysGpsC_33*>(odsp);
   }
   catch(InvalidRequest ir)
   {
      GPSTK_RETHROW(ir);
   }

   CommonTime utc = currT - mt33->getUtcOffset(currT); 
   utc.setTimeSystem(TimeSystem::UTC);
   CommonTime ut1 = mt32->getUT1(currT,mt33);
   return ut1;
}

unsigned OrbSysGpsC_T::
test_MT33()
{
   const OrbDataSys* odsp;
   const OrbSysGpsC_33* mt33;

   string currMethod = "isUTCValid()";
   TUDEF("OrbSysGpsC_33",currMethod);
   unsigned retVal = 0;

   // Test a time in the middle of the period of interest
   CommonTime testValid = CivilTime(2017, 1, 1, 0, 0, 0, TimeSystem::GPS);
   try
   {
      odsp = oss.find(nid,33,testValid);
      mt33 = dynamic_cast<const OrbSysGpsC_33*>(odsp);
      TUASSERTE(bool,true,mt33->isUtcValid(testValid));
   }
   catch (InvalidRequest ir)
   {
      stringstream ss;
      ss << ir << " for time " << printTime(testValid,"%02m/%02d/%04Y %02H:%02M:%02S");
      TUFAIL(ss.str());
   }

   //Test a time clearly too early
   testValid = CivilTime(2016,10,10, 0, 0, 0, TimeSystem::GPS);
   TUASSERTE(bool,false,mt33->isUtcValid(testValid));

   // Test a time more than 74 hours in the future
   testValid = CivilTime(2017, 1, 4, 0, 0, 0, TimeSystem::GPS);
   TUASSERTE(bool,false,mt33->isUtcValid(testValid));

   currMethod = "getUtcOffset()";
   TUCSM(currMethod);
   double epsilon = 1.0e-9; 

   // We want to verify UTC is appropriate in the following cases:
   //  1.)  At the beginning of the day of the leap second (IS-GPS-200 20.3.3.5.2.4.a)
   //  2.)  During the leap second transition wherre UTC is discontinuous but UT1 is 
   //       continuous (23:59:59, 23:59:60, and 00:00:00 UTC).
   //       In this case, 
   //  3.)  At the end of the day after the leap second (once a new upload has occurred).
   static const unsigned int NVALUES = 5;
   CommonTime test[NVALUES];
   test[0] = CivilTime(2016,12,31, 0,00,00, TimeSystem::GPS); 
   test[1] = CivilTime(2017, 1, 1, 0,00,16, TimeSystem::GPS);
   test[2] = CivilTime(2017, 1, 1, 0,00,17, TimeSystem::GPS);
   test[3] = CivilTime(2017, 1, 1, 0,00,18, TimeSystem::GPS);
   test[4] = CivilTime(2017, 1, 1,23,00,00, TimeSystem::GPS);

   double truth[NVALUES];
   truth[0] = 17.0000000019;
   truth[1] = 16.9999999984;
   truth[2] = 16.9999999984;
   truth[3] = 17.9999999984;
   truth[4] = 18.0000000004;

   for (int i=0;i<NVALUES;i++)
   {
      try
      {
         odsp = oss.find(nid,33,test[i]);
         mt33 = dynamic_cast<const OrbSysGpsC_33*>(odsp);
         double uoff = mt33->getUtcOffset(test[i]); 

         char tchar[40];
         sprintf(tchar,"%15.12f",uoff); 
         string tstr(tchar);
         /*
         cout << "ct, utc offset: " << printTime(test[i],"%02m/%02d/%04Y %02H:%02M:%02S") 
              << ", " << tstr 
              << ", dtLS, dtLSF, A0, A1, A2 : " << mt33->dtLS << ", " << mt33->dtLSF
              << ", " << mt33->A0 << ", " << mt33->A1 << ", " << mt33->A2
              << ", tot " << printTime(mt33->ctEpoch,"%02m/%02d/%04Y %02H:%02M:%02S")
              << ", ctLSF " << printTime(mt33->ctLSF,"%02m/%02d/%04Y %02H:%02M:%02S")
              << endl;
         */
         TUASSERTFEPS(truth[i],uoff,epsilon);
      }
      catch (InvalidRequest ir)
      {
         stringstream ss;
         ss << ir << " for time " << printTime(test[i],"%02H:%02M:%02S");
         TUFAIL(ss.str());
      }
   }

   TURETURN(); 
}

int main(int argc, char *argv[])
{
  unsigned errorTotal = 0;
  
  OrbSysGpsC_T testClass;

  testClass.setUpCNAV();
  errorTotal += testClass.createStore();

     // Since MT32 operation is dependent on MT33, we should test MT33 first. 
  errorTotal += testClass.test_MT33();

  errorTotal += testClass.test_MT32();

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}


