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
   std::string tempFile = tf + "test_output_OrbDataSys_T_" +
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
   OrbSysStore oss;
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
         // do nothing
         // The failure will be reflected in the 
         // fact that the output will be missing expected data
      }
   }

      // Verify number of entries in the store. 
   out << "# of OrbSysStore entries: " << oss.size() << endl; 

      // Dump the store
   oss.dump(out);

      // Dump terse (one-line) summaries
   oss.dump(out,1);

      // Dump all contents
   oss.dump(out,2);

      // Dump terse in time order
   oss.dump(out,3);

      // Clean up
   oss.clear();
   out.close();

   return 0; 
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
   initialCT = CivilTime(2015,12,31,00,00,18,TimeSystem::GPS);
   finalCT   = CivilTime(2015,12,31,18,43,48,TimeSystem::GPS);

      // Literals for LNAV test data 
   const unsigned short LNavExCount = 18;
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
      "365,12/31/2015,00:10:48,1877,346248,1,63,417, 0x22C3550A, 0x1C2DACF0, 0x1DD11675, 0x12959676, 0x1693080D, 0x1689D262, 0x11968C6F, 0x15D0CD3B, 0x15164E8E, 0x0D1640B4",
      "365,12/31/2015,00:11:18,1877,346278,2,61,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x0000014E, 0x00000029, 0x005ED55B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,18:43:48,1877,413028,2,61,418, 0x22C3550A, 0x219CECF0, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFE7B, 0x3FFFFFFC, 0x3F641555, 0x044EC0D4, 0x044000B4",
      "365,12/31/2015,00:11:18,1877,346278,3,69,418, 0x22C3550A, 0x1C2E4CC4, 0x1E037FFB, 0x3FC08E66, 0x3C7FC45D, 0x3FFFFF23, 0x3FFFFFFC, 0x3F9ED57B, 0x044EC0FD, 0x04400054",
      "365,12/31/2015,00:08:48,1877,346128,1,63,413, 0x22C3550A, 0x1C2B2C1C, 0x1D163D8D, 0x0374F72B, 0x0B190095, 0x08F95CEE, 0x0B5F0864, 0x24F97F6B, 0x2B9382F3, 0x2B0D72A8",
      // Following record hand-edited in order to indicate unencrypted in the availability indicator.
      // Also modified to indicate an INVALID ERD (ndx 5), a max + (011111 base 2) (ndx 3), and
      // max negative (100001) (ndx 4).
    //"365,12/31/2015,12:26:18,1877,390378,1,63,413, 0x22C3550A, 0x1FC50CF0, 0x1D1FE70B, 0x31715EBB, 0x1B9122BA, 0x0329194A, 0x18EC680E, 0x074229DF, 0x08E88416, 0x2A2445A4",
      "365,12/31/2015,12:26:18,1877,390378,1,63,413, 0x22C3550A, 0x1FC50CF0, 0x1D0FE70B, 0x31F860BB, 0x1B9122BA, 0x0329194A, 0x18EC680E, 0x074229DF, 0x08E88416, 0x2A2445A4",
      "365,12/31/2015,00:08:48,1877,346128,2,61,413, 0x22C3550A, 0x1C2B2C1C, 0x1D1F18D0, 0x17B5F2ED, 0x3CE0889C, 0x1B176553, 0x129C8100, 0x32321ECF, 0x092F8292, 0x018C79A0"
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
   initialCT = CivilTime(2017,1,1,00,00,24,TimeSystem::GPS);
   finalCT   = CivilTime(2017,1,1,00,54,12,TimeSystem::GPS);

      // Literals for CNAV test data 
      // Test data set corresponds to Jan 1, 2017 in order to test Dec 31, 2016-Jan 1, 2017 Leap Second Event
   const unsigned short CNavExCount = 25;
   const std::string CNavEx[] =
   {
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

    /*
     "067,03/07/2016,00:00:12,1887,086412,1,63,11, 0x8B04B1C2, 0x2099701F, 0xCD37C9D1, 0xBACE000D, 0xFED7C008, 0x8003BFF3, 0x2D2018DA, 0x4027AAFF, 0x4D180E4F, 0x7C300000",
     "067,03/07/2016,00:00:24,1887,086424,1,63,30, 0x8B05E1C2, 0x304CEA72, 0x640393DD, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000B16, 0x6FF00000",
     "067,03/07/2016,00:00:36,1887,086436,1,63,33, 0x8B0611C2, 0x404CEA72, 0x640393DD, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x00000502, 0xCA100000",
     "067,03/07/2016,00:00:48,1887,086448,1,63,10, 0x8B04A1C2, 0x50EBE44C, 0xE899007E, 0xF4400186, 0xD2F057FF, 0xC94FB679, 0x2D0014F5, 0x70B04AC8, 0x5FD807FD, 0xA7700000",
     "067,03/07/2016,02:00:00,1887,093600,1,63,10, 0x8B04A1E7, 0x90EBE44C, 0xE8A50073, 0x88BFFE1F, 0x93079000, 0x80226376, 0xB44814F4, 0x518C4AB0, 0x475C0E58, 0x3A700000",
     "067,03/07/2016,02:00:12,1887,093612,1,63,11, 0x8B04B1E7, 0xA0A5701F, 0x7F6BC9D1, 0xB8C2E015, 0xCEBEC007, 0xDFFCBFF1, 0x8F201A17, 0x202586FF, 0x38100791, 0x85000000",
     "067,03/07/2016,02:00:24,1887,093624,1,63,30, 0x8B05E1E7, 0xB04CEA72, 0x9403945F, 0x00094000, 0x0B0FFB7E, 0x5019E0D4, 0x1100FE00, 0x37FFFC01, 0x5F000C5A, 0xF4A00000",
     "067,03/07/2016,02:00:36,1887,093636,1,63,33, 0x8B0611E7, 0xC04CEA72, 0x9403945F, 0x00094000, 0x00280000, 0x022166A7, 0x5F39D988, 0x80000000, 0x0000024E, 0x51400000"
     */
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

int main(int argc, char *argv[])
{
  if (argc!=2)
  {
     cout << "OrbDataSys_T requires either LNAV or CNAV as arguments" << endl;
     return 1;
  }
  unsigned errorTotal = 0;
  
  OrbDataSys_T testClass;

  string argValue(argv[1]);

  if (argValue==string("LNAV"))
  {
     testClass.setUpLNAV();
     errorTotal += testClass.createAndDump();
  }
  else if (argValue==string("CNAV"))
  {
     testClass.setUpCNAV();
     errorTotal += testClass.createAndDump();
  }
  else
  {
     cout << "Unknown navigation message type provided to OrbDataSys_T" << endl;
     return 1;
  }

  cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
  return errorTotal; // Return the total number of errors
}


