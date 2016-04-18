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
//=============================================================================

#include "EngEphemeris.hpp"
#include "TestUtil.hpp"
#include "GPSWeekZcount.hpp"
#include <iostream>
#include <sstream>
using namespace gpstk;

#ifdef _MSC_VER
#define LDEXP(x,y) ldexp(x,y)
#else
#define LDEXP(x,y) std::ldexp(x,y)
#endif


/** Ephemeris subframe words at the end of a week.  Useful for a
 * week-rollover test of toe and toc as well as other things.
 * Sorry about the decimal, it came that way out of HDF5. 
 * @note this data has been modified so that toe != toc, to facilitate
 * verifying that the appropriate quantity is used where
 * appropriate. */
const uint32_t ephEOW[] = 
{  583228942, 824945128,  904134685,  184026330,  459310087,
    16899638, 845363969, 0x0f647980,    4193148, 1073290676,
   583228942, 824953464,  260012308,  225364840,  787693093,
  1065730353, 298759921,   46377054,   57870868,       8172,
   583228942, 824962032, 1072401983,  485782594,      84477,
   301605863, 145566781,  506082625, 1072230894,  259901040 };
/* original data as broadcast
{  583228942, 824945128,  904134685,  184026330,  459310087,
    16899638, 845363969,  255852580,    4193148, 1073290676,
   583228942, 824953464,  260012308,  225364840,  787693093,
  1065730353, 298759921,   46377054,   57870868,       8172,
   583228942, 824962032, 1072401983,  485782594,      84477,
   301605863, 145566781,  506082625, 1072230894,  259901040 };
*/
const unsigned ephEOWwk  = 1886;
const unsigned ephEOWToeWk = 1887;
const unsigned ephEOWprn = 14;
// the rest of these values were broken out by hand
const CommonTime ephEOWhowTime1 = GPSWeekZcount(ephEOWwk, 402804);
const CommonTime ephEOWhowTime2 = GPSWeekZcount(ephEOWwk, 402808);
const CommonTime ephEOWhowTime3 = GPSWeekZcount(ephEOWwk, 402812);
const long ephEOWhowSec1 = 604206;
const long ephEOWhowSec2 = 604212; 
const long ephEOWhowSec3 = 604218;
const CommonTime ephEOWxmitTime1 = ephEOWhowTime1 - 6;
const CommonTime ephEOWxmitTime2 = ephEOWhowTime2 - 6;
const CommonTime ephEOWxmitTime3 = ephEOWhowTime3 - 6;
const double ephEOWxmitTimeSec1 = GPSWeekSecond(ephEOWxmitTime1).sow;
const double ephEOWtocSec = 597600;
const long   ephEOWtocZ   = 398400;
const CommonTime ephEOWtoc = GPSWeekZcount(ephEOWwk, ephEOWtocZ);
// as-broadcast
//const CommonTime ephEOWtoc = GPSWeekZcount(ephEOWwk+1, 0);
const double ephEOWaf0      = LDEXP(double( int32_t(0xfffff91d)), -31);
const double ephEOWaf1      = LDEXP(double( int16_t(0xffed)),     -43);
const double ephEOWaf2      = 0.;
const double ephEOWiode     = 61.;
const double ephEOWCrs      = LDEXP(double( int16_t(0xfde4)),      -5);
const double ephEOWdn       = LDEXP(double( int16_t(0x35bb)),     -43) * PI;
const double ephEOWM0       = LDEXP(double( int32_t(0x2dbbccf8)), -31) * PI;
const double ephEOWCuc      = LDEXP(double( int16_t(0xfe17)),     -29);
const double ephEOWecc      = LDEXP(double(uint32_t(0x04473adb)), -33);
const double ephEOWCus      = LDEXP(double( int16_t(0x0b0e)),     -29);
const double ephEOWAhalf    = LDEXP(double(uint32_t(0xa10dcc28)), -19);
const double ephEOWToe      = 0.; //LDEXP(double(uint16_t()),4);
const CommonTime ephEOWtoe = GPSWeekSecond(ephEOWToeWk, ephEOWToe);
const double ephEOWCic      = LDEXP(double( int16_t(0xffae)),     -29);
const double ephEOWOMEGA0   = LDEXP(double( int32_t(0x3873d1d1)), -31) * PI;
const double ephEOWCis      = LDEXP(double( int16_t(0x0005)),     -29);
const double ephEOWi0       = LDEXP(double( int32_t(0x2747e88f)), -31) * PI;
const double ephEOWCrc      = LDEXP(double( int16_t(0x22b4)),      -5);
const double ephEOWw        = LDEXP(double( int32_t(0xb078a8d5)), -31) * PI;
const double ephEOWOMEGAdot = LDEXP(double( int32_t(0xffffa3c7)), -43) * PI;
const double ephEOWidot     = LDEXP(double( int16_t(0xfdc6)),     -43) * PI;
const double ephEOWTgd      = LDEXP(double(  int8_t(0xec)),       -31);
const short  ephEOWcodeflgs = 1;
const short  ephEOWl2pData  = 0;
const short  ephEOWhealth   = 0;
const double ephEOWiodc     = 0x03d;
// URA index = 0, worst case 2.4m 20.3.3.3.1.3
const double ephEOWacc      = 2.4;
// fit interval *flag*
const double ephEOWfitint   = 0;


class EngEphemeris_T
{
public:
      /// Default Constructor, set the precision value
   EngEphemeris_T()
   {
      eps = 1E-12;
      b10 = 10;
   }

   ~EngEphemeris_T() {}

      /** Used to initalize a object before rewriting with the valid
       * subframes.  Necessary for addSubframe and
       * addSubframeNoParity.  Makes it seem like it has 3 valid
       * subframes. */
   gpstk::EngEphemeris fakeEphemerisInit(void)
   {
      gpstk::EngEphemeris fakeEphemeris;

         // array 30 bit words all set to one, an invalid
         // subframe. Word 2 is different, contains SF id
      const uint32_t data1[10] =
         { 0x22FFFFFF, 0x3FFFF930, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
           0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF };
      const uint32_t data2[10] =
         { 0x22FFFFFF, 0x3FFFFA88, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
           0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF };
      const uint32_t data3[10] =
         { 0x22FFFFFF, 0x3FFFFBD0, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF,
           0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF, 0x3FFFFFFF };

      fakeEphemeris.haveSubframe[0] = true;
      fakeEphemeris.haveSubframe[1] = true;
      fakeEphemeris.haveSubframe[2] = true;
      for (int i=0; i<10; ++i) fakeEphemeris.subframeStore[0][i] = data1[i];
      for (int i=0; i<10; ++i) fakeEphemeris.subframeStore[1][i] = data2[i];
      for (int i=0; i<10; ++i) fakeEphemeris.subframeStore[2][i] = data3[i];

      return fakeEphemeris;
   }

//=======================================================================
// The following 3 methods are used to see if the data specificly set for
// each subframe was set correctly
//
// Data can be stored in EngEphemeris in multiple ways. This ensures
// the method of storing data was successful
//
// Doesn't test any of the data stored in the orbit or clock objects
//=======================================================================

   void subframe1Check(gpstk::EngEphemeris dataStore,
                       gpstk::TestUtil& testFramework,
                       bool skipASAlert = false)
   {
      TUASSERT(dataStore.haveSubframe[0]);
      TUASSERTE(unsigned short, 0, dataStore.tlm_message[0]);
      TUASSERTE(short, 6, dataStore.PRNID);
      TUASSERTE(short, 1, dataStore.tracker);

         // ASAlert is set to 1 by that addIncompleteSF1Thru3, so
         // bypassing the ASAlerts test only for that function HOWTime
         // is not set by this function either
      if (!skipASAlert)
      {
         TUASSERTE(short, 0, dataStore.ASalert[0]);
         TUASSERTE(long, 409902, dataStore.HOWtime[0]);
      }

      TUASSERTE(short, 1025, dataStore.weeknum);
      TUASSERTE(short, 2, dataStore.codeflags);
      TUASSERTE(short, 0, dataStore.health);
      TUASSERTE(short, 0, dataStore.L2Pdata);
      TUASSERTE(short, 91, dataStore.IODC);
   }


   void subframe2Check(gpstk::EngEphemeris dataStore,
                       gpstk::TestUtil& testFramework,
                       bool skipASAlert = false)
   {
      TUASSERT(dataStore.haveSubframe[1]);
      TUASSERTE(unsigned short, 0, dataStore.tlm_message[1]);

         // ASAlert is set to 1 by that addIncompleteSF1Thru3, so
         // bypassing the ASAlerts test only for that function HOWTime
         // is not set by this function either
      if (!skipASAlert)
      {
         TUASSERTE(short, 0, dataStore.ASalert[1]);
         TUASSERTE(long, 409908, dataStore.HOWtime[1]);
      }

      TUASSERTE(short, 91, dataStore.IODE);
      TUASSERTE(short, 0, dataStore.fitint);
   }


   void subframe3Check(gpstk::EngEphemeris dataStore,
                       gpstk::TestUtil& testFramework,
                       bool skipASAlert = false)
   {
      TUASSERT(dataStore.haveSubframe[2]);
      TUASSERTE(unsigned short, 0, dataStore.tlm_message[2]);

         // ASAlert is set to 1 by that addIncompleteSF1Thru3, so
         // bypassing the ASAlerts test only for that function HOWTime
         // is not set by this function either
      if (!skipASAlert)
      {
         TUASSERTE(short, 0, dataStore.ASalert[2]);
         TUASSERTE(long, 409914, dataStore.HOWtime[2]);
      }
   }


   unsigned initializationTest(void)
   {
      TUDEF("EngEphemeris", "Default Constructor");
      unsigned badCount = 0;

      gpstk::EngEphemeris empty;

      TUASSERTE(short, 0, empty.PRNID);
      TUASSERTE(short, 0, empty.tracker);
      TUASSERTE(short, 0, empty.IODC);
      TUASSERTE(short, 0, empty.IODE);
      TUASSERTE(short, 0, empty.weeknum);
      TUASSERTE(short, 0, empty.codeflags);
      TUASSERTE(short, 0, empty.health);
      TUASSERTE(short, 0, empty.L2Pdata);
      TUASSERTE(std::string, "", empty.satSys);
      TUASSERTFE(0, empty.Tgd);
      TUASSERT(empty.isFIC);
      TUASSERTE(short, 0, empty.ASalert[0]);
      TUASSERTE(short, 0, empty.ASalert[1]);
      TUASSERTE(short, 0, empty.ASalert[2]);
      TUASSERTE(long, 0, empty.HOWtime[0]);
      TUASSERTE(long, 0, empty.HOWtime[1]);
      TUASSERTE(long, 0, empty.HOWtime[2]);
      TUASSERT(!empty.haveSubframe[0]);
      TUASSERT(!empty.haveSubframe[1]);
      TUASSERT(!empty.haveSubframe[2]);

      for (unsigned i=0; i<3; i++)
      {
         for (unsigned j=0; j<10; j++)
         {
            if (empty.subframeStore[i][j] > eps)
               badCount++;
         }
      }
      TUASSERTE(unsigned, 0, badCount);
      badCount = 0;

      for (unsigned i=0; i<3; i++)
      {
         if (empty.haveSubframe[i] != false)
            badCount++;
      }
      TUASSERTE(unsigned, 0, badCount);
      badCount = 0;

      TUASSERT(!empty.isValid());
      TUASSERT(!empty.isDataSet());

      TURETURN();
   }


   unsigned addSubframeTest(void)
   {
      TUDEF("EngEphemeris", "addSubframe");

      gpstk::EngEphemeris dataStore;

         // Same values as for addSubframeNoParityTest below, just
         // added correct parity.  Parity was calculated using the
         // verified functions from EngNav, bad use of time to do at
         // least 30 binary calculations with take >5 minutes each

      const uint32_t subframe1P[10] =
         { 0x22c000e4, 0x215ba160, 0x00180012, 0x1fffffc0, 0x3fffffc3,
           0x3ffffffc, 0x3fffc009, 0x16d904f0, 0x003fdbac, 0x1b83ed54 };
      const uint32_t subframe2P[10] =
         { 0x22c000e4, 0x215bc2f0, 0x16c2eb4d, 0x09f7c524, 0x2fdc3384,
           0x0289c0dd, 0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4 };
      const uint32_t subframe3P[10] =
         { 0x22c000e4, 0x215be378, 0x3ffcc344, 0x1a8441d8, 0x3ff80b74,
           0x1c8deb5e, 0x0a34d525, 0x14a5012e, 0x3fee8c06, 0x16c35c80 };
      dataStore = fakeEphemerisInit();

         // Week: 1025, PRN: 6, tracker:1
      TUASSERT(dataStore.addSubframe(subframe1P, 1025, 6, 1)); 
      subframe1Check(dataStore, testFramework);

      TUASSERT(dataStore.addSubframe(subframe2P, 1025, 6, 1));
      subframe2Check(dataStore, testFramework);

      TUASSERT(dataStore.addSubframe(subframe3P, 1025, 6, 1));
      subframe3Check(dataStore, testFramework);

      TURETURN();
   }


   unsigned addSubframeNoParityTest(void) //calls add subframe'
   {
      TUDEF("EngEphemeris", "addSubframeNoParity");

      gpstk::EngEphemeris dataStore;
      dataStore = fakeEphemerisInit();

         /*
The following huge comments describe how the broadcast subframe data
is reconstructed from data/test_input_rinex_nav_FilterTest2.99n


Feeding in raw binary values output by satellite (without the parity bits)
Without parity each word in 24 bits instead of 30
All reserved bits taken to be 1

Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
using the data format in fig 20-1 of IS-GPS-200D

Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by
Control Segment and Space segment (?), so left blank)
ref. IS-GPS-200D p 82 fig 20-2
preamble msg         RESERVED
10001011  00000000000000   11          = 0x8B0003

Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
followed by 3b subframe ID (p 81) and 2 parity computation bits
ref. IS-GPS-200D p 82 fig 20-2
TOW was modified to be evenly divisible by 6
17b truncated TOW (409902/6)  Alert    AntiSpoof   SubframeID          Parity
10000101011011101          0     0        001         00             = 0x856E84

Word 3 formed by 10b Week Number (mod 1024), 2b codeflag, 4b URA
index, 6b SV health, and first 2 bits of IODC
ref. IS-GPS-200D 20.3.3.3 (p 82)
Week Num (1025)   CodeFlag (C/A code must exist, and P code flag set to 0) URA   svhealth    IODC 2 MSB
0000000001     10                                              0000  000000      00          = 0x006000

Word 4 is L2 P code flag (0) followed by 23 reserved bits, setting them to 1
0x7FFFFF

Words 5 and 6 are composed on 24 reserved bits, setting them to 1
0xFFFFFF

Word 7 formed by 16 reserved bits, setting them to 1, followed by 8b TGD
TGD is represented by integer scaled by a factor of 2^-31 (IS-GPS-200D
table 20-I)
Reserved          TGD * 2^-31 (0)
1111111111111111  00000000       = 0xFFFF00

Word 8 formed by 8 LSBs of IODC and 16b toc scaled by 2^4
Toc is obtained by converting UTC time from Rinex Nav, scaled to GPS
week second (=25619)
(IS-GPS-200D table 20-I)
8 LSBs of IODC    toc * 2^-4
0101 1011      0110 0100 0001 0011  = 0x5B6413

Word 9 formed by 8b of af2 scaled by 2^-55, and 16b of af1 scaled by
2^-43 (IS-GPS-200D table 20-I)
af2 * 2^52     af1 * 2^43
0000 0000      1111 1111 0110 1110  = 0x00FF6E

Word 10 formed by 22b of af0 scaled by 2^-31 and 2 reserved parity
bits (marked 0)
af * 2^31. # is signed, so found by 2^22 + af0*2^31
-.839701388031E-03 * 2^31 =
1001000111110000010011 + 00 = 1001 0001 1111 0000 0100 1100 = 0x91F04C
         */

         // Is this function designed to just update already stored subframes?
         // Must be, because loop to see if all 3 subframes stored

      const uint32_t subframe1[10] =
         { 0x8B0003, 0x856E84, 0x006000, 0x7FFFFF, 0xFFFFFF,
           0xFFFFFF, 0xFFFF00, 0x5B6413, 0x00FF6E, 0x91F04C };

      TUASSERT(dataStore.addSubframeNoParity(subframe1, 1025, 6, 1));
      subframe1Check(dataStore, testFramework);

         /*

Feeding in raw binary values output by satellite (without the parity bits)
Without parity each word in 24 bits instead of 30
All reserved bits taken to be 1

Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
using the data format in fig 20-1 of IS-GPS-200D

Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by
Control Segment and Space segment (?), so left blank)
ref. IS-GPS-200D p 82 fig 20-2
preamble msg         RESERVED
10001011  00000000000000   11          = 0x8B0003

Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
followed by 3b subframe ID (p 81) and 2 parity computation bits
ref. IS-GPS-200D p 82 fig 20-2
Previous TOW + 6 for transmit time
17b truncated TOW (409908/6)  Alert    AntiSpoof   SubframeID          Parity
10000101011011110          0     0        010         00             = 0x856F08

Word 3 formed by 8b IODE and signed 16b Crs, scaled by 2^-5
IODE (91)   Crs * 2^5 (93.40625 * 2^5)
01011011 0000101110101101     = 0x5B0BAD

Word 4 is signed 16b delta N scaled by 2^-43 with the 8 msbs of M0
scaled by 2^-31
Delta N * 2^43 (.11604054784E-8 * 2^43 / pi = 3249)
0000 1100 1011 0001
M0 * 2^31 (0.162092304801 * 2^31 / pi = 110800671)
0000 0110 (1001 1010 1010 1111 0001 1111) = 0x0CB106

Word 5 is the other 24 bits of M0 listed above
M0
1001 1010 1010 1111 0001 1111 = 0x9AAF1F

Word 6 is signed 16b Cuc scaled by 2^-29 with the signed 8 msbs of e
scaled by 2^-33
Cuc*2^29 (.484101474285E-5*2^29) e*2^33 (.626740418375E-2*2^33)
0000 1010 0010 0111           0000 0011 (0011 0101 0111 1011 0011 0000) = 0x0A2703

Word 7 is the other 24 bits of e listed above
e
0011 0101 0111 1011 0011 0000    = 0x357B30

Word 8 is signed 16b of Cus scaled by 2^-29 and the unsigned 8mbs of
sqrtA scaled by 2^-19
Cus*2^29 (.652112066746E-5*2^29) sqrtA*2^19(.515365489006E4*2^19)
0000 1101 1010 1101           1010 0001 (0000 1101 0011 1101 0011 0111) = 0x0DADA1

Word 9 is the other 24 bits of sqrtA listed above
A
0000 1101 0011 1101 0011 0111 = 0x0D3D37

Word 10 is 16b toe scaled by 2^4 with 1b fit interval flag and 5b AODO
Toe (409902*2^-4)    fitInt   AODO (age of almanac observations, doesn't matter)
0110 0100 0001 0011  0     000 00      00 = 0x641300
         */

      const uint32_t subframe2[10] =
         { 0x8B0003, 0x856F08, 0x5B0BAD, 0x0CB106, 0x9AAF1F,
0x0A2703, 0x357B30, 0x0DADA1, 0x0D3D37, 0x641300 };

      TUASSERT(dataStore.addSubframeNoParity(subframe2, 1025, 6, 1));
      subframe2Check(dataStore, testFramework);

         /*
Feeding in raw binary values output by satellite (without the parity bits)
Without parity each word in 24 bits instead of 30
All reserved bits taken to be 1

Raw SV data is reconstructed from data/test_input_rinex_nav_FilterTest2.99n
using the data format in fig 20-1 of IS-GPS-200D

Word 1 formed by TLM preamble followed by TLM mesg (Msg defined by
Control Segment and Space segment (?), so left blank)
ref. IS-GPS-200D p 82 fig 20-2
preamble msg         RESERVED
10001011  00000000000000   11          = 0x8B0003

Word 2 formed by 17b truncated TOW, 1b Alert Flag and 1b Spoof Flag
followed by 3b subframe ID (p 81) and 2 parity computation bits
ref. IS-GPS-200D p 82 fig 20-2
Previous TOW + 6 for transmit time
17b truncated TOW (409914/6)  Alert    AntiSpoof   SubframeID          Parity
10000101011011111          0     0        011         00             = 0x856F0C

Word 3 is signed 16b Cic scaled by 2^-29 with the signed 8 msbs of
omega0 scaled by 2^-31
Cic*2^29 (-.242143869400E-7*2^29)   omega0*2^31 (.329237003460*2^31/pi)
1111 1111 1111 0010              0000 1101 (0110 1010 0001 0001 0000 0111) = 0xFFF30D

Word 4 is the other 24 bits of omega0 listed above
omega0
0110 1010 0001 0001 0000 0111 = 0x6A1107

Word 5 is signed 16b Cis scaled by 2^-29 with the signed 8 msbs of I0
scaled by 2^-31
Cis*2^29 (-.596046447754E-7*2^29)   I0*2^31 (1.11541663136*2^31/pi)
1111 1111 1110 0000              0010 1101 (0111 0010 0011 0111 1010 1101) = 0xFFE02D

Word 6 is the other 24 bits of I0 listed above
I0
0111 0010 0011 0111 1010 1101 = 0x7237AD

Word 7 is signed 16b of Crc scaled by 2^-5 and the unsigned 8mbs of
omega scaled by 2^-31
Crc*2^5 (326.59375*2^5) omega*2^31(2.06958726335*2^31/pi)
0010 1000 1101 0011     0101 0100 (0101 0010 1001 0100 0000 0100) = 0x28D354

Word 8 is the other 24 bits of omega listed above
omega
0101 0010 1001 0100 0000 0100 = 0x529404

Word 9 is signed 24b of OMEGADOT scaled by 2^-43
OMEGADOT (-.638312302555E-8*2^43/pi)
1111 1111 1011 1010 0011 0000 = 0xFFBA30

Word 10 is 8b IODE with signed 14b IDOT scaled by 2^-43
IODE (91)      IDOT*2^43 (.307155651409E-9*2^43/pi) parity comp
0101 1011      0000 1101 0111 00             00        = 0x5B0D70
         */

      const uint32_t subframe3[10] =
         { 0x8B0003, 0x856F8C, 0xFFF30D, 0x6A1107, 0xFFE02D,
           0x7237AD, 0x28D354, 0x529404, 0xFFBA30, 0x5B0D70 };

      TUASSERT(dataStore.addSubframeNoParity(subframe3, 1025, 6, 1));
      subframe3Check(dataStore, testFramework);

      TURETURN();
   }


   unsigned setSF1Test(gpstk::EngEphemeris& dataStore)
   {
      TUDEF("EngEphemeris", "setSF1");

         // The following values were taken from
         // data/test_input_rinex_nav_FilterTest2.99n

         // Rinex documentation found at
         // http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
         // helpful animation at
         // http://emedia.rmit.edu.au/satellite/node/21

         // TLM value just taken to be the TLM msg, which is defined
         // by the CS and SS, so left blank.  HOW value taken to be
         // Time of Week in seconds.

         // More detailed info on each data type can be found in IS-GPS-200D

      dataStore.setSF1(0,                               // tlm
                       409902,                          // how
                       0,                               // ASalert
                       1025,                            // week
                       2,                               // cflags
                       0,                               // acc
                       0,                               // svhealth
                       91,                              // IODC
                       0,                               // l2pdate
                       0.0,                             // tgd
                       25619,                           // Toc
                       0.0,                             // af2
                       -.165982783074*pow(b10,-10),     // af1
                       -.839701388031*pow(b10,-3),      // af0
                       1,                               // tracker
                       6);                              // prn

      subframe1Check(dataStore, testFramework);

      TUASSERT(!dataStore.isValid());
      TUASSERT(!dataStore.isDataSet());

      TURETURN();
   }


   unsigned setSF2Test(gpstk::EngEphemeris& dataStore)
   {
      TUDEF("EngEphemeris", "setSF2");

         // The following values were taken from
         // data/test_input_rinex_nav_FilterTest2.99n

         // Rinex documentation found at
         // http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
         // helpful animation at
         // http://emedia.rmit.edu.au/satellite/node/21

         // TLM value just taken to be the TLM msg, which is defined
         // by the CS and SS, so left blank.  HOW value taken to be
         // Time of Week in seconds.

         // More detailed info on each data type can be found in IS-GPS-200D

      dataStore.setSF2(0,                               // tlm
                       409908.0,                        // how
                       0,                               // ASalert
                       91,                              // IODE
                       93.40625,                        // crs
                       .11604054784 * pow(b10,-8),      // Dn
                       0.162092304801,                  // m0
                       .484101474285*pow(b10,-5),       // cuc
                       .626740418375*pow(b10,-2),       // ecc
                       .652112066746*pow(b10,-5),       // cus
                       .515365489006*pow(b10,4),        // ahalf
                       409902,                          // toe
                       0);                              // fitint

      subframe2Check(dataStore, testFramework);

      TUASSERT(!dataStore.isDataSet());

      TURETURN();
   }


   unsigned setSF3Test(gpstk::EngEphemeris& dataStore)
   {
      TUDEF("EngEphemeris", "setSF3");

         // The following values were taken from
         // data/test_input_rinex_nav_FilterTest2.99n

         // Rinex documentation found at
         // http://igscb.jpl.nasa.gov/igscb/data/format/rinex211.txt
         // helpful animation at
         // http://emedia.rmit.edu.au/satellite/node/21

         // TLM value just taken to be the TLM msg, which is defined
         // by the CS and SS, so left blank.  HOW value taken to be
         // Time of Week in seconds.

         // More detailed info on each data type can be found in IS-GPS-200D

      dataStore.setSF3(0,                               // tlm
                       409914.0,                        // how
                       0,                               // ASalert
                       -.242143869400*pow(b10,-7),      // cic
                       .10479939309884491,              // Omega0 aka OMEGA
                       -.596046447754*pow(b10,-7),      // cis
                       0.3550481409757088,              // I0
                       326.59375,                       // crc
                       0.6587700862443613,              // W (aka omega)
                       -2.0318111637599545*pow(b10,-9), // OmegaDot
                       .307155651409*pow(b10,-9));      // idot

      subframe3Check(dataStore, testFramework);

      TUASSERT(dataStore.isDataSet());

      TURETURN();
   }


   unsigned getTest(gpstk::EngEphemeris& dataStore)
   {
      TUDEF("EngEphemeris", "Get Methods");

      TUASSERTE(short, 6, dataStore.getPRNID());
      TUASSERTE(short, 1, dataStore.getTracker());
      TUASSERTFE(409902, dataStore.getHOWTime(1));
      TUASSERTE(short, 0, dataStore.getASAlert(1));
      TUASSERTE(short, 1025, dataStore.getFullWeek());
      TUASSERTE(short, 2, dataStore.getCodeFlags());
      TUASSERTFE(25619, dataStore.getToc());
      TUASSERTFE(-.839701388031*pow(b10,-3), dataStore.getAf0());
      TUASSERTFE(-.165982783074*pow(b10,-10), dataStore.getAf1());
      TUASSERTFE(0, dataStore.getAf2());
      TUASSERTE(short, 0, dataStore.getHealth());
      TUASSERTE(short, 0, dataStore.getL2Pdata());
      TUASSERTE(short, 91, dataStore.getIODC());
      TUASSERTE(short, 91, dataStore.getIODE());

         //setSF# doesn't set AODO, is only set by loadData which is not tested by getMethods. Skipping
//    testMesg = "The getAODO method didn't function correctly";
//    TUASSERTE(dataStore.getAODO() == 0/*VALUE NOT SET BY SETSF3*/);

      TUASSERTFE(.652112066746*pow(b10,-5),       dataStore.getCus());
      TUASSERTFE(93.40625,                        dataStore.getCrs());
      TUASSERTFE(.484101474285*pow(b10,-5),       dataStore.getCuc());
      TUASSERTFE(409902,                          dataStore.getToe());
      TUASSERTFE(0.162092304801,                  dataStore.getM0());
      TUASSERTFE(.11604054784*pow(b10,-8),        dataStore.getDn());
      TUASSERTFE(.626740418375*pow(b10,-2),       dataStore.getEcc());
      TUASSERTFE(.515365489006*pow(b10,4),        dataStore.getAhalf());
      TUASSERTFE(pow(.515365489006*pow(b10,4),2), dataStore.getA());
      TUASSERTFE(-.596046447754*pow(b10,-7),      dataStore.getCis());
      TUASSERTFE(326.59375,                       dataStore.getCrc());
      TUASSERTFE(-.242143869400*pow(b10,-7),      dataStore.getCic());
      TUASSERTFE(0.10479939309884491,             dataStore.getOmega0());
      TUASSERTFE(0.3550481409757088,              dataStore.getI0());
      TUASSERTFE(0.6587700862443613,              dataStore.getW());
      TUASSERTFE(-2.0318111637599545*pow(b10,-9), dataStore.getOmegaDot());
      TUASSERTFE(.307155651409*pow(b10,-9),       dataStore.getIDot());
      TUASSERTE(long, 409890, dataStore.getTot());
         // earliest of the HOW's (409902) rounded down to nearest
         // multiple of 30

      TURETURN();
   }

   unsigned loadDataTest(void)
   {
      TUDEF("EngEphemeris", "loadData");

      gpstk::EngEphemeris dataStore;

      unsigned short tlm[3] = {0,0,0};
      const long how[3] = {409902, 409908, 409914};
      const short ASalert[3] = {0,0,0};

      dataStore.loadData((std::string) "No Idea",
                         tlm,
                         how,
                         ASalert,
                         1,                             // tracker
                         6,                             // prn
                         1025,                          // week
                         2,                             // cflags
                         0,                             // acc
                         0,                             // svhealth
                         91,                            // iodc
                         0,                             // l2p
                         0.0,                           // aodo
                         0.0,                           // tgd
                         25619,                         // toc
                         0.0,                           // af2
                         -.165982783074*pow(b10,-10),   // af1
                         -.839701388031*pow(b10,-3),    // af0
                         91,                            // iode
                         93.40625,                      // crs
                         .11604054784 * pow(b10,-8),    // dn
                         0.162092304801,                // M0
                         .484101474285*pow(b10,-5),     // cuc
                         .626740418375*pow(b10,-2),     // ecc
                         .652112066746*pow(b10,-5),     // cus
                         .515365489006*pow(b10,4),      // Ahalf
                         409902,                        // toe
                         0,                             // fit int
                         -.242143869400*pow(b10,-7),    // cic
                         .10479939309884491,            // Omega0 aka OMEGA
                         -.596046447754*pow(b10,-7),    // cis
                         0.3550481409757088,            // i0
                         326.59375,                     // crc
                         0.6587700862443613,            // W aka omega
                         -2.0318111637599545*pow(b10,-9), // OmegaDot
                         .307155651409*pow(b10,-9));    // IDot

      subframe1Check(dataStore, testFramework);
      subframe2Check(dataStore, testFramework);
      subframe3Check(dataStore, testFramework);

      TURETURN();
   }


   unsigned addIncompleteTest(void)
   {
      TUDEF("EngEphemeris", "addIncomplete");

      gpstk::EngEphemeris dataStore;

      const uint32_t subframe1P[8] =
         { 0x00180012, 0x1fffffc0, 0x3fffffc3, 0x3ffffffc,
           0x3fffc009, 0x16d904f0, 0x003fdbac, 0x1b83ed54 };
      const uint32_t subframe2P[8] =
         { 0x16c2eb4d, 0x09f7c524, 0x2fdc3384, 0x0289c0dd,
           0x0d5ecc38, 0x036b6842, 0x034f4df0, 0x1904c0b4 };
      const uint32_t subframe3P[8] =
         { 0x3ffcc344, 0x1a8441d8, 0x3ff80b74, 0x1c8deb5e,
           0x0a34d525, 0x14a5012e, 0x3fee8c06, 0x16c35c80 };

      TUASSERT(dataStore.addIncompleteSF1Thru3(subframe1P, subframe2P,
                                               subframe3P, 444, 1025, 6, 1));

         //ASAlert tests fail, data is not included in the incomplete subframe.
      subframe1Check(dataStore, testFramework, true);
      subframe2Check(dataStore, testFramework, true);
      subframe3Check(dataStore, testFramework, true);

      TURETURN();
   }


   unsigned endOfWeekTest()
   {
      TUDEF("EngEphemeris", "addSubframe");
      EngEphemeris eeph;
      TUASSERT(eeph.addSubframe(&ephEOW[ 0], ephEOWwk, ephEOWprn, 1));
      TUASSERT(eeph.addSubframe(&ephEOW[10], ephEOWwk, ephEOWprn, 1));
      TUASSERT(eeph.addSubframe(&ephEOW[20], ephEOWwk, ephEOWprn, 1));

      TUASSERTE(short, ephEOWprn, eeph.PRNID);
      TUASSERTE(CommonTime, ephEOWtoc, eeph.getEpochTime());
      TUASSERTFE(ephEOWaf0, eeph.bcClock.getAf0());
      TUASSERTFE(ephEOWaf1, eeph.bcClock.getAf1());
      TUASSERTFE(ephEOWaf2, eeph.bcClock.getAf2());
      TUASSERTFE(ephEOWiode, eeph.IODE);
      TUASSERTFE(ephEOWCrs, eeph.orbit.getCrs());
      TUASSERTFE(ephEOWdn, eeph.orbit.getDn());
      TUASSERTFE(ephEOWM0, eeph.orbit.getM0());
      TUASSERTFE(ephEOWCuc, eeph.orbit.getCuc());
      TUASSERTFE(ephEOWecc, eeph.orbit.getEcc());
      TUASSERTFE(ephEOWCus, eeph.orbit.getCus());
      TUASSERTFE(ephEOWAhalf, eeph.orbit.getAhalf());
      TUASSERTFE(ephEOWtoe, eeph.getEphemerisEpoch());
      TUASSERTFE(ephEOWCic, eeph.orbit.getCic());
      TUASSERTFE(ephEOWOMEGA0, eeph.orbit.getOmega0());
      TUASSERTFE(ephEOWCis, eeph.orbit.getCis());
      TUASSERTFE(ephEOWi0, eeph.orbit.getI0());
      TUASSERTFE(ephEOWCrc, eeph.orbit.getCrc());
      TUASSERTFE(ephEOWw, eeph.orbit.getW());
      TUASSERTFE(ephEOWOMEGAdot, eeph.orbit.getOmegaDot());
      TUASSERTFE(ephEOWidot, eeph.orbit.getIDot());
      TUASSERTE(short, ephEOWcodeflgs, eeph.codeflags);
      TUASSERTE(short, ephEOWl2pData, eeph.L2Pdata);
      TUASSERTFE(ephEOWacc, eeph.getAccuracy());
      TUASSERTE(short, ephEOWhealth, eeph.health);
      TUASSERTFE(ephEOWTgd, eeph.Tgd);
      TUASSERTFE(ephEOWiodc, eeph.IODC);
      TUASSERTFE(ephEOWfitint, eeph.fitint);

      TURETURN();
   }

   std::string testMesg;

private:
   double eps;
   double b10;
};

int main() //Main function to initialize and run all tests above
{
   using namespace std;
   EngEphemeris_T testClass;
   unsigned errorTotal = 0;

      // Used to have a running ephemeris for functions that only set
      // part of it
   gpstk::EngEphemeris dataStore;

   errorTotal += testClass.initializationTest();

      // setSF# require previous subframes to be set, and get needs a
      // valid ephemeris object
      // Passing one between the tests
      // If one of the setSF# tests fail, all other setSF# and get
      // tests will fail too
   errorTotal += testClass.setSF1Test(dataStore);
   errorTotal += testClass.setSF2Test(dataStore);
   errorTotal += testClass.setSF3Test(dataStore);
   errorTotal += testClass.getTest(dataStore);
   errorTotal += testClass.addSubframeNoParityTest();
   errorTotal += testClass.addSubframeTest();
   errorTotal += testClass.loadDataTest();
   errorTotal += testClass.addIncompleteTest();
   errorTotal += testClass.endOfWeekTest();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
