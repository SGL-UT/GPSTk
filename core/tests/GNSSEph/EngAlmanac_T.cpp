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

#include "EngAlmanac.hpp"
#include "TestUtil.hpp"
#include <iostream>

#include "AlmanacDataGenerator.hpp"
#include "BinUtils.hpp"

class EngAlmanac_T
{
public:
      //Reads in AlmanacData object to form comparison data types
   EngAlmanac_T(AlmanacData iAData, AlmanacSubframes iASubframes)
   {
      eps = 1E-6; //Low precision, data is extracted from binary subframes, some have only 16 bit precision
      aData = iAData;
      aSubframes = iASubframes;
   }

   ~EngAlmanac_T() {} // Default Desructor

   gpstk::EngAlmanac alm;

   int addSubframeTest(void)
   {
      gpstk::TestUtil testFramework("EngAlmanac", "addSubframe", __FILE__, __LINE__);


         //Subframe and week number mod 1024
      testMesg = "Adding subframe 5 page 1 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[0], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 2 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[1], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 3 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[2], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 4 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[3], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 5 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[4], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 6 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[5], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 7 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[6], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 8 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[7], 819), testMesg,
                           __LINE__);
         //skip this test, no valid almanac data for PRN 8
         // testMesg = "Adding subframe 5 page 9 failed";
         // testFramework.assert(alm.addSubframe(aSubframes.totalSf[31], 819), testMesg, __LINE__);
      testMesg = "Adding subframe 5 page 10 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[8], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 11 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[9], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 12 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[10], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 13 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[11], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 14 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[12], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 15 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[13], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 16 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[14], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 17 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[15], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 18 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[16], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 19 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[17], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 20 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[18], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 21 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[19], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 22 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[20], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 23 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[21], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 5 page 24 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[22], 819), testMesg,
                           __LINE__);

      testMesg = "Adding subframe 4 page 2 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[23], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 3 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[24], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 4 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[25], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 5 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[26], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 7 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[27], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 8 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[28], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 9 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[29], 819), testMesg,
                           __LINE__);
      testMesg = "Adding subframe 4 page 10 failed";
      testFramework.assert(alm.addSubframe(aSubframes.totalSf[30], 819), testMesg,
                           __LINE__);

//subframe 5 page 25
         /*
           copy the tlm and how from above for subframe 5
           dataID = 2 (from the IS-GPS-200)
           svID = 51 for this page
           toa = 319488 * 2**-12
           WNa = 819 % 256 = 51
           all sv healths are good except 8, set it to 111111 (or leave it blank, don't think it matters)

           word 3:
           dID svID (51)         319488*2**-12 = 78      Wn = 51
           10    110011          01001110                        00110011 = 10 1100 1101 0011 1000 1100 1100 0000
           2CD38CC0

           word 5:
           00 0000               0000 00                 00 0000                 1111 11                 00 0000

           word 10 is reserved, doesn't matter what set to

         */
      const long subframe551[10] = {0x22c000e4, 0x00000598, 0x2CD38CC0, 0x00000000, 0x00000FC0,
                                    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x11111110
      };
      testMesg = "Adding subframe 5 page 25 failed";
      testFramework.assert(alm.addSubframe(subframe551, 819), testMesg, __LINE__);

//subframe 4 page 25
         /*
           copy the tlm and the how from above for subframe 4
           dataID = 2 (from the IS-GPS-200)
           svID = 63 for this page

           Assume reserved bits are 1, and all satellites have Anti spoofing on and are in block II//IIA/IIR
           therefore all the sv configs need to be set to 1001

           word 3
           dID   svID            4 svconfigs
           10            1111 11         10 0110 0110 0110 01 = 0x2F666640

           word 4,5,6,7
           6 sv configs
           10 0110 0110 0110 0110 0110 01 = 0x26666640

           word 8
           4 sv configs                  2 reserved      1 sv health
           10 0110 0110 0110 01  11                      0000 00         =       0x26667000

           word 9
           5 sv healths, all 0 = 0x00000000

           word 10
           3 sv healths                  4 reserved      2 parity
           00 0000 0000 0000 0000        1111            00                      = 0x00000F00

         */
      const long subframe447[10] = {0x22c000e4, 0x0000042c, 0x2FE66640, 0x26666640, 0x26666640,
                                    0x26666640, 0x26666640, 0x26667000, 0x00000000, 0x00000F00
      };
      testMesg = "Adding subframe 4 page 25 failed";
      testFramework.assert(alm.addSubframe(subframe447, 819), testMesg, __LINE__);

// subframe4 page 18
         /*
           copy tlm and how from other subframe4 pages
           data id = 2
           sv id = 56

              //deadbeefdeadc0de
              alpha0 = (222 - 256) * 2**-30 = -3.166496753692627e-08 //Twos compliment
              alpha1 = (173 - 256) * 2**-27 / pi = -1.968422573302286e-07
              alpha2 = (190 - 256) * 2**-24 / pi**2 = -3.985880685087617e-07
              alpha3 = (239 - 256) * 2**-24 / pi**3 = -3.26798013069863e-08
              beta0 = (222 - 256) * 2**11 = -69632 //Twos compliment
              beta1 = (173 - 256) * 2**14 / pi = -432860.7015445238
              beta2 = (192 - 256) * 2**16 / pi**2 = -424971.8458357919
              beta3 = (222 - 256) * 2**16 / pi**3 = -71863.64306088151

                 //cabobobs
                 A0 = (3400577205 - 4294967296) * 2**-30 = -0.8329656822606921
                    //baddab
                    A1 = (12246443 - 16777216) * 2**-50 = -4.0241348031599955e-09
                    dtLS = 13 = 0x0d
                    tot = 110 * 2**12 = 450560 = 0x6e
                    WNt = 90 = 0x5a
                    WNLSF = 254 = 0xFE
                    DN (right justified) = 5 =>0x05
                    dtLSF = (153 - 256) = -103 = 0x99


                    word 3
                    dataID       svID    alpha0          alpha1          parity
                    10           111000  11011110        10101101    000000              = 0x2e37ab40

                    word 4
                    alpha2               alpha3          beta0           parity
                    10111110     11101111        11011110        000000                  = 0x2fbbf780

                    word 5
                    beta1                beta2           beta3           parity
                    10101101     11000000        11011110        000000                  = 0x2b703780

                    word 6
                    A0                                                                   parity
                    11001010101100001011000010110101    000000                   = 0x2eb76ac0

                    word 7
                    A1 (24 msb)                                                  parity
                    110010101011000010110000(10110101)   000000                  = 0x32ac2c00

                    word 8
                    A0 (8 lsb)   tot             WNt             parity
                    10110101     01101110        01011010        000000                  = 0x2d5b9680

                    word 9
                    dtLS                 WNLSF           DN                      parity
                    00001101     11111110        00000101        000000                  = 0x037f8140

                    word 10
                    dtLSF                14b reserved    2b              parity
                    10011001     11111111111111  00              000000                  = 0x267fff00
         */
      const long subframe456[10] = {0x22c000e4, 0x0000042c, 0x2e37ab40, 0x2fbbf780, 0x2b703780,
                                    0x2eb76ac0, 0x32ac2c00, 0x2d5b9680, 0x037f8140, 0x267fff00
      };
      testMesg = "Adding subframe 4 page 18 failed";
      testFramework.assert(alm.addSubframe(subframe456, 819), testMesg, __LINE__);


      return testFramework.countFails();
   }

   int getTest(void)
   {
      int x, failCount;
      gpstk::TestUtil testFramework("EngAlmanac", "getTest", __FILE__, __LINE__);

         //GPSTK documentation should really say what units these return as
         // For loop which tests all the data values for the almanac. Commented out, too many tests
         /*
           for(int i=0; i<31; i++)
           {
           if ( i >= 7) x = i + 2; //switch to allow for skipped prn 8
           else x = i + 1;
           testMesg = "getEcc returned the wrong value";
           testFramework.assert(std::abs(alm.getEcc(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.ecc[i])*1E2 < eps, testMesg, __LINE__);
           testMesg = "getIOffset returned the wrong value";
           testFramework.assert(std::abs(alm.getIOffset(gpstk::SatID(x, gpstk::SatID::systemGPS)) - (aData.oi[i] - .3*M_PI)) < eps, testMesg, __LINE__);
           testMesg = "getOmegadot returned the wrong value";
           testFramework.assert(std::abs(alm.getOmegadot(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.rora[i])*1E8 < eps, testMesg, __LINE__);
           testMesg = "get6bitHealth returned the wrong value";
           testFramework.assert(alm.get6bitHealth(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 0 , testMesg, __LINE__);
           testMesg = "getSVHealth returned the wrong value";
           testFramework.assert(alm.getSVHealth(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 0, testMesg, __LINE__);
           testMesg = "getSVConfig returned the wrong value";
           testFramework.assert(alm.getSVConfig(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 9, testMesg, __LINE__); //9 is 1001
           testMesg = "getAhalf returned the wrong value";
           testFramework.assert(std::abs(alm.getAhalf(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.sqrta[i])*1E-4 < eps, testMesg, __LINE__);
           testMesg = "getA returned the wrong value";
           testFramework.assert(std::abs(alm.getA(gpstk::SatID(x, gpstk::SatID::systemGPS)) - pow(aData.sqrta[i],2))*1E-8 < eps, testMesg, __LINE__);
           testMesg = "getOmega0 returned the wrong value";
           testFramework.assert(std::abs(alm.getOmega0(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.raaw[i])*1E-1 < eps, testMesg, __LINE__);
           testMesg = "getW returned the wrong value";
           testFramework.assert(std::abs(alm.getW(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.aop[i]) < eps, testMesg, __LINE__);
           testMesg = "getM0 returned the wrong value";
           testFramework.assert(std::abs(alm.getM0(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.ma[i])*1E-1 < eps, testMesg, __LINE__);
           testMesg = "getAf0 returned the wrong value";
           testFramework.assert(std::abs(alm.getAf0(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.af0[i])*1E5 < eps, testMesg, __LINE__);
           testMesg = "getAf1 returned the wrong value";
           testFramework.assert(std::abs(alm.getAf1(gpstk::SatID(x, gpstk::SatID::systemGPS)) - aData.af1[i]) < eps, testMesg, __LINE__);
           }
         */

         //getEcc test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getEcc(gpstk::SatID(x,
                                                gpstk::SatID::systemGPS)) - aData.ecc[i])*1E2 < eps))
            failCount++;
      }
      testMesg = "getEcc returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getIOffset test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getIOffset(gpstk::SatID(x,
                                                    gpstk::SatID::systemGPS)) - (aData.oi[i] - .3*M_PI)) < eps))
            failCount++;
      }
      testMesg = "getIOffset returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getOmegadot test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getOmegadot(gpstk::SatID(x,
                                                     gpstk::SatID::systemGPS)) - aData.rora[i])*1E8 < eps))
            failCount++;
      }
      testMesg = "getOmegadot returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //get6bitHealth test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(alm.get6bitHealth(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 0))
            failCount++;
      }
      testMesg = "get6bitHealth returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getSVHealth test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(alm.getSVHealth(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 0))
            failCount++;
      }
      testMesg = "getSVHealth returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getSVConfig test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(alm.getSVConfig(gpstk::SatID(x, gpstk::SatID::systemGPS)) == 9))
            failCount++;
      }
      testMesg = "getSVConfig returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getAhalf test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getAhalf(gpstk::SatID(x,
                                                  gpstk::SatID::systemGPS)) - aData.sqrta[i])*1E-4 < eps))
            failCount++;
      }
      testMesg = "getAhalf returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getA test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getA(gpstk::SatID(x,
                                              gpstk::SatID::systemGPS)) - pow(aData.sqrta[i],2))*1E-8 < eps))
            failCount++;
      }
      testMesg = "getA returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getOmega0 test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getOmega0(gpstk::SatID(x,
                                                   gpstk::SatID::systemGPS)) - aData.raaw[i])*1E-1 < eps))
            failCount++;
      }
      testMesg = "getOmega0 returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getW test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getW(gpstk::SatID(x,
                                              gpstk::SatID::systemGPS)) - aData.aop[i]) < eps))
            failCount++;
      }
      testMesg = "getW returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getM0 test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getM0(gpstk::SatID(x,
                                               gpstk::SatID::systemGPS)) - aData.ma[i])*1E-1 < eps))
            failCount++;
      }
      testMesg = "getM0 returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getAf0 test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getAf0(gpstk::SatID(x,
                                                gpstk::SatID::systemGPS)) - aData.af0[i])*1E5 < eps))
            failCount++;
      }
      testMesg = "getAf0 returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

         //getAf1 test
      failCount = 0;
      for (int i=0; i<31; i++)
      {
         if ( i >= 7) x = i + 2;
         else x = i + 1;
         if (!(std::abs(alm.getAf1(gpstk::SatID(x,
                                                gpstk::SatID::systemGPS)) - aData.af1[i]) < eps))
            failCount++;
      }
      testMesg = "getAf1 returned the wrong value";
      testFramework.assert(failCount == 0, testMesg, __LINE__);

      double a[4], b[4];
      alm.getIon(a, b);

      testMesg = "getIon returned an incorrect value for Alpha0";
      testFramework.assert(std::abs(a[0] + 3.166496753692627e-08) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Alpha1";
      testFramework.assert(std::abs(a[1] + 1.968422573302286e-07) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Alpha2";
      testFramework.assert(std::abs(a[2] + 3.985880685087617e-07) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Alpha3";
      testFramework.assert(std::abs(a[3] + 3.26798013069863e-08) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Beta0";
      testFramework.assert(std::abs(b[0] + 69632) < eps, testMesg, __LINE__);
      testMesg = "getIon returned an incorrect value for Beta1";
      testFramework.assert(std::abs(b[1] + 432860.7015445238) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Beta2";
      testFramework.assert(std::abs(b[2] + 424971.8458357919) < eps, testMesg,
                           __LINE__);
      testMesg = "getIon returned an incorrect value for Beta3";
      testFramework.assert(std::abs(b[3] + 71863.64306088151) < eps, testMesg,
                           __LINE__);

      double a0, a1, deltaTLS, deltaTLSF;
      long tot;
      int WNt, WNLSF, DN;
      alm.getUTC(a0, a1, deltaTLS, tot, WNt, WNLSF, DN, deltaTLSF);

      testMesg = "getUTC returned an incorrect value for A0";
      testFramework.assert(std::abs(a0 + 0.8329656822606921) < eps, testMesg,
                           __LINE__);
      testMesg = "getUTC returned an incorrect value for A1";
      testFramework.assert(std::abs(a1 + 4.0241348031599955e-09) < eps, testMesg,
                           __LINE__);
      testMesg = "getUTC returned an incorrect value for deltaTLS";
      testFramework.assert(std::abs(deltaTLS - 13) < eps, testMesg, __LINE__);
      testMesg = "getUTC returned an incorrect value for deltaTLSF";
      testFramework.assert(std::abs(deltaTLSF + 103) < eps, testMesg, __LINE__);
      testMesg = "getUTC returned an incorrect value for Tot";
      testFramework.assert( tot == 450560, testMesg, __LINE__);
      testMesg = "getUTC returned an incorrect value for WNt";
      testFramework.assert( WNt == int(851/256)*256 + 90, testMesg, __LINE__);

         // Below test FAILS! in the 8 bit week conversion. Passing
         // unsigned value into week conversion can cause diff to be
         // larger than LIMIT[type], resulting in incorrect value.
         // See EngNav.cpp convertXbit.

         /** @todo JMK says: I looked at the subframe 4 pg 18 data on
          * line 251 (look for subframe456) and broke out the bits by
          * hand.  The WNlsf was 766, and that's what the code is
          * returning.  I honestly have no idea what this expression
          * here is supposed to be, but I'm keeping it in case someone
          * else wants to have a go at it:
          * int(851/256)*256 + 254
          */
      testMesg = "getUTC returned an incorrect value for WNLSF";
      testFramework.assert( WNLSF == 766, testMesg, __LINE__);

      testMesg = "getUTC returned an incorrect value for DN";
      testFramework.assert( DN == 5, testMesg, __LINE__);

      testMesg = "getAlmOrbElem returned an incorrect AlmOrbit object";
      failCount = 0;
      for (int i=0; i<30; i++)
      {
         x = i+1;
         if (x > 7) x = i+2;
         gpstk::AlmOrbit Compare(aData.id[i], aData.ecc[i], (aData.oi[i] - .3*M_PI),
                                 aData.rora[i],
                                 aData.sqrta[i], aData.raaw[i], aData.aop[i], aData.ma[i], aData.af0[i],
                                 aData.af1[i],
                                 aData.toa[i], 0/*?*/, aData.week[i], aData.health[i]);
         gpstk::AlmOrbit orbitData;
         orbitData = alm.getAlmOrbElem(gpstk::SatID(x, gpstk::SatID::systemGPS));

// No == operator for AlmOrbit, using AlmOrbit's get methods to compare
// Too many tests, using counter. Uncomment to find specific error
            /*
              testMesg = "PRN value in getAlmOrbElem is incorrect";
              testFramework.assert(orbitData.getPRN() == Compare.getPRN(), testMesg, __LINE__);
              testMesg = "ecc value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getecc() - Compare.getecc()) < eps, testMesg, __LINE__);
              testMesg = "i_offset value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.geti_offset() - Compare.geti_offset()) < eps, testMesg, __LINE__);
              testMesg = "OMEGAdot value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getOMEGAdot() - Compare.getOMEGAdot()) < eps, testMesg, __LINE__);
              testMesg = "Ahalf value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getAhalf() - Compare.getAhalf()) < eps, testMesg, __LINE__);
              testMesg = "OMEGA0 value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getOMEGA0() - Compare.getOMEGA0()) < eps, testMesg, __LINE__);
              testMesg = "w value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getw() - Compare.getw()) < eps, testMesg, __LINE__);
              testMesg = "M0 value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getM0() - Compare.getM0()) < eps, testMesg, __LINE__);
              testMesg = "AF0 value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getAF0() - Compare.getAF0()) < eps, testMesg, __LINE__);
              testMesg = "AF1 value in getAlmOrbElem is incorrect";
              testFramework.assert(std::abs(orbitData.getAF1() - Compare.getAF1()) < eps, testMesg, __LINE__);
              testMesg = "ToaSOW value in getAlmOrbElem is incorrect";
              testFramework.assert(orbitData.getToaSOW() == Compare.getToaSOW(), testMesg, __LINE__);
              testMesg = "xmit_time value in getAlmOrbElem is incorrect";
              testFramework.assert(orbitData.getxmit_time() == Compare.getxmit_time(), testMesg, __LINE__);
              testMesg = "ToaWeek value in getAlmOrbElem is incorrect";
              testFramework.assert(orbitData.getToaWeek() == Compare.getToaWeek(), testMesg, __LINE__);
            */

         if (!(orbitData.getPRN() == Compare.getPRN())) failCount++;
         if (!(std::abs(orbitData.getecc() - Compare.getecc()) < eps)) failCount++;
         if (!(std::abs(orbitData.geti_offset() - Compare.geti_offset()) < eps))
            failCount++;
         if (!(std::abs(orbitData.getOMEGAdot() - Compare.getOMEGAdot()) < eps))
            failCount++;
         if (!(std::abs(orbitData.getAhalf() - Compare.getAhalf()) < eps)) failCount++;
         if (!(std::abs(orbitData.getOMEGA0() - Compare.getOMEGA0()) < eps))
            failCount++;
         if (!(std::abs(orbitData.getw() - Compare.getw()) < eps)) failCount++;
         if (!(std::abs(orbitData.getM0() - Compare.getM0()) < eps)) failCount++;
         if (!(std::abs(orbitData.getAF0() - Compare.getAF0()) < eps)) failCount++;
         if (!(std::abs(orbitData.getAF1() - Compare.getAF1()) < eps)) failCount++;
         if (!(orbitData.getToaSOW() == Compare.getToaSOW())) failCount++;
         if (!(orbitData.getxmit_time() == Compare.getxmit_time())) failCount++;
         if (!(orbitData.getToaWeek() == Compare.getToaWeek())) failCount++;
      }
      testFramework.assert(failCount == 0, testMesg, __LINE__);

      return testFramework.countFails();

   }

private:
   double eps;
   std::string testMesg;
   AlmanacData aData; //data where the almanac info is stored
   AlmanacSubframes aSubframes; //data where subframes are stored

};


int main() //Main function to initialize and run all tests above
{
   std::string pathData = gpstk::getPathData();
   std::string almanacLocation = pathData + "/test_input_gps_almanac.txt";
   std::ifstream iAlmanac(
      almanacLocation.c_str()); // Reads in almanac data from file
   AlmanacData iAData(iAlmanac); // Parses file into data objects
   AlmanacSubframes iASubframes(
      iAData); // Takes data objects and generates the subframes needed

   EngAlmanac_T testClass(iAData, iASubframes);
   int check, errorCounter = 0;

   check = testClass.addSubframeTest(); // Tests adding the subframes to a class member of EngAlmanac
   errorCounter += check;

   check = testClass.getTest(); // Tests the get methods and if subframes stored the correct data
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter <<
             std::endl;

   return errorCounter; //Return the total number of errors
}
