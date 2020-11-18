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

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "Xvt.hpp"
#include "RinexEphemerisStore.hpp"
#include "Exception.hpp"
#include "CivilTime.hpp"
#include "CommonTime.hpp"
#include "RinexNavData.hpp"
#include "EngEphemeris.hpp"
#include "GPSEphemerisStore.hpp"
#include "SatID.hpp"
#include "RinexNavStream.hpp"
#include "TestUtil.hpp"



using namespace gpstk;
using namespace std;


class RinexEphemerisStore_T
{
public:
   RinexEphemerisStore_T() {}

//=============================================================================
//      General test for the RinexEphemerisStore (RES) class
//      Test to assure the that RES throws its exceptions in the right
//      place and that it loads the RINEX Nav file correctly
//=============================================================================
   unsigned RESTest()
   {
      TUDEF("RinexEphemerisStore", "Constructor");

         //-----------------------------------------------------------
         // Verify the consturctor builds the RES object
         //-----------------------------------------------------------
      try
      {
         RinexEphemerisStore rinEphStore;
         TUPASS("sedIdentifier");
      }
      catch (...)
      {
         TUFAIL("Exception thrown creating RinexEphemerisStoreObject");
      }
      RinexEphemerisStore rinEphStore;

         //-----------------------------------------------------------
         // Verify the inability to load nonexistent files.
         //-----------------------------------------------------------
      TUCSM("loadFile");
      try
      {
         rinEphStore.loadFile(inputNotaFile.c_str());
         TUFAIL("Loaded a non-existent file.");
      }
      catch (Exception& e)
      {
         TUPASS("sedIdentifier");
      }

         //---------------------------------------------
         // Verify the ability to load existant files.
         //---------------------------------------------
      try
      {
         rinEphStore.loadFile(inputRinexNavData.c_str());
         TUPASS("sedIdentifier");
      }
      catch (...)
      {
         TUFAIL("Could not load existing file");
      }

//-------------------------------------------------------======================
//   It would be nice to verify that the double name exception is
//   indeed thrown. However the InvalidParameter exception thrown will
//   terminate the program even with a catch-all.
         /*
         //--------------RinexEphemerisStore_RESTest_# - Verify that a repeated filename returns an exception
         try
         {
         Store.loadFile(inputRinexNavData.c_str());
         testFramework.failTest();
         }
         catch (Exception& e)
         {
         TUPASS("sedIdentifier");
         cout << "Expected exception received from RinexEphemerisStore" << endl;
         }
         catch (...)
         {
         cout << "Expected exception received from RinexEphemerisStore!!!!!!!!!" << endl;
         } }
         */
//-------------------------------------------------------======================

      TUCSM("clear");
         //--------------------------------------------------------------------
         // Verify that once a clear() has been performed the repeated
         // filename can be opened.
         //--------------------------------------------------------------------
      rinEphStore.gpstk::FileStore<RinexNavHeader>::clear();
      try
      {
         rinEphStore.loadFile(inputRinexNavData.c_str());
         TUPASS("sedIdentifier");
      }
      catch (Exception& e)
      {
         cout << " Exception received from RinexEphemerisStore, e = " << e
              << endl;
         TUFAIL("Could not reopen the cleared file.");
      }

      TURETURN();
   }


//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member findEph()
//      This test makes sure that exceptions are thrown if there is no
//      ephemeris data for the given PRN and also that an exception is
//      thrown if there is no data for the PRN at the given
//      time. Furthermore, this test finds an Ephemeris for a given
//      CivilTime Time and PRN.
//      To see the ephemeris information for the selected Time and PRN
//      please see findEph#.txt
//=============================================================================
   unsigned findEphTest()
   {
      TUDEF("GPSEphemerisStore", "findEphemeris");

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

         // Create a list of GPSEphemerides
      std::list<GPSEphemeris> R3NList;
         // Create a GPSEphemerisStore for testing
      GPSEphemerisStore gpsEphStore;
         // Create an interator for the GPSEphmeris list
      list<GPSEphemeris>::const_iterator it;
         // Add the loaded Rinex Nav Data into the list
      rinEphStore.addToList(R3NList);
         // Loop over the list adding the ephemerides to the GPSEphemerisStore
      for (it = R3NList.begin(); it != R3NList.end(); ++it)
      {
         gpsEphStore.addEphemeris(GPSEphemeris(*it));
      }

         // debug dump of gpsEphStore for reference if needed
      ofstream gDumpData;
      gDumpData.open(gpsEphemerisStoreDumpOutput.c_str());
      gpsEphStore.dump(gDumpData,1);
      gDumpData.close();

      const short PRN0 = 0; // Zero PRN (Border test case)
      const short PRN1 = 1;
      const short PRN15 = 15;
      const short PRN32 = 32;
      const short PRN33 = 33;  // Top PRN (33) (Border test case);

      const SatID sid0(PRN0,SatelliteSystem::GPS);
      const SatID sid1(PRN1,SatelliteSystem::GPS);
      const SatID sid15(PRN15,SatelliteSystem::GPS);
      const SatID sid32(PRN32,SatelliteSystem::GPS);
      const SatID sid33(PRN33,SatelliteSystem::GPS);

      CivilTime Time(2006,1,31,11,45,0,TimeSystem::GPS);
         // Border Time (Time of Border test cases)
      CivilTime bTime(2006,1,31,2,0,0,TimeSystem::GPS);
      const CommonTime ComTime = Time.convertToCommonTime();
      const CommonTime CombTime = bTime.convertToCommonTime();

      try
      {
         CivilTime crazy(1950,1,31,2,0,0,TimeSystem::GPS);
         const CommonTime Comcrazy = (CommonTime)crazy;

            //--------------------------------------------------
            // For proper input, will the method return properly?
            //--------------------------------------------------
         try
         {
            gpsEphStore.findEphemeris(sid1,ComTime);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("findEphemeris threw an exception when it shouldn't.");
         }

            //-----------------------------------------------------------
            // For a wrong SatID (too small), will an exception be thrown?
            //-----------------------------------------------------------
         try
         {
            gpsEphStore.findEphemeris(sid0,CombTime);
            TUFAIL("findEphemeris was successful when it shouldn't be");
         }
         catch (InvalidRequest)
         {
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("findEphemeris threw an unexpected exception");
         }

            //-----------------------------------------------------------
            // For a wrong SatID (too large), will an exception be thrown?
            //-----------------------------------------------------------
         try
         {
            gpsEphStore.findEphemeris(sid33,CombTime);
            TUFAIL("findEphemeris was successful when it shouldn't be");
         }
         catch (InvalidRequest)
         {
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("findEphemeris threw an unexpected exception");
         }

            //-------------------------------------------------------
            // For an improper time, will an exception be thrown?
            //-------------------------------------------------------
         try
         {
            gpsEphStore.findEphemeris(sid32,Comcrazy);
            TUFAIL("findEphemeris was successful when it shouldn't be");
         }
         catch (InvalidRequest)
         {
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("findEphemeris threw an unexpected exception");
         }

            //-------------------------------------------------------
            // Check findEphemeris output with pre-determined standard
            //-------------------------------------------------------
         GPSEphemeris eph(gpsEphStore.findEphemeris(sid1,ComTime));
         GPSWeekSecond xmitTime(1360, 208800.);
         CommonTime xmitTimeCT(xmitTime);
         GPSWeekSecond toe(1360, 215984.);
         CommonTime toeCT(toe);
         GPSWeekSecond toc(1360, 215984.);
         CommonTime tocCT(toc);
         double Ahalf = .515367877960e+04;
         double A = Ahalf * Ahalf;
         TUASSERTE(CommonTime, xmitTimeCT, eph.transmitTime);
         TUASSERTE(long, 208806, eph.HOWtime);
         TUASSERTE(short, 0x174, eph.IODC);
         TUASSERTE(short, 0x74, eph.IODE);
         TUASSERTE(short, 0, eph.health);
         TUASSERTE(short, 2, eph.accuracyFlag);
            // skipping accuracy which is converted from accuracyFlag
            // and is supposed to be a range rather than a single
            // number.
         TUASSERTFE(-3.25962901E-09, eph.Tgd);
         TUASSERTE(short, 1, eph.codeflags);
         TUASSERTE(short, 0, eph.L2Pdata);
         TUASSERTE(short, 4, eph.fitDuration);
            // This really should be zero, but the toolkit is
            // inconsistent in its use of this data.
         TUASSERTE(short, 4, eph.fitint);
            // OrbitEph data
         TUASSERTE(SatID, SatID(1, SatelliteSystem::GPS), eph.satID);
            // skipping obsID which is unknown, not stored in RINEX
         TUASSERTE(CommonTime, toeCT, eph.ctToe);
         TUASSERTE(CommonTime, tocCT, eph.ctToc);
         TUASSERTFE( .342056155205e-04, eph.af0);
         TUASSERTFE( .193267624127e-11, eph.af1);
         TUASSERTFE( .000000000000e+00, eph.af2);
         TUASSERTFE( .231892822330e+00, eph.M0);
         TUASSERTFE( .415874465698e-08, eph.dn);
         TUASSERTFE( .632588984445e-02, eph.ecc);
         TUASSERTFE( A, eph.A);
         TUASSERTFE(-.171556435925e+01, eph.OMEGA0);
         TUASSERTFE( .986372320378e+00, eph.i0);
         TUASSERTFE(-.171070282354e+01, eph.w);
         TUASSERTFE(-.827605901679e-08, eph.OMEGAdot);
         TUASSERTFE( .533236497155e-09, eph.idot);
         TUASSERTFE( .578165054321e-05, eph.Cuc);
         TUASSERTFE( .529363751411e-05, eph.Cus);
         TUASSERTFE( .290062500000e+03, eph.Crc);
         TUASSERTFE( .113406250000e+03, eph.Crs);
         TUASSERTFE(-.745058059692e-07, eph.Cic);
         TUASSERTFE(-.279396772385e-07, eph.Cis);
      }
      catch (Exception& e)
      {
            // cout << e;
      }


         // Removed because formatting inconsistencies.  If you want
         // to do regression tests on formats, create an independent
         // test.
         //TUCSM("findEphemeris Output");
         //-------------------------------------------------------
         // Check findEphemeris output with pre-determined standard
         //-------------------------------------------------------
         //testFramework.assert(testFramework.fileEqualTest(findEphTestOutput, findEphTestInput, 0), "Output file did not match regressive standard.", __LINE__);

      TURETURN();
   }


//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member getXvt()
//      This test makes sure that exceptions are thrown if there is no
//      ephemeris data for the given PRN and also that an exception is
//      thrown if there is no data for the PRN at the given
//      time. Furthermore, this test finds an Xvt for a given
//      CivilTime Time and PRN.
//      To see the Xvt information for the selected Time and PRN
//      please see the files getXvt#.txt
//=============================================================================

   unsigned getXvtTest()
   {
      TUDEF("OrbitEphStore", "getXvt");

      stringstream fPRN1;
      stringstream fPRN15;
      stringstream fPRN32;

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      const short PRN0 = 0; // Zero PRN (Border test case)
      const short PRN1 = 1;
      const short PRN15 = 15;
      const short PRN32 = 32;
      const short PRN33 = 33;  // Top PRN (33) (Border test case);
      SatID sid0(PRN0,SatelliteSystem::GPS);
      SatID sid1(PRN1,SatelliteSystem::GPS);
      SatID sid15(PRN15,SatelliteSystem::GPS);
      SatID sid32(PRN32,SatelliteSystem::GPS);
      SatID sid33(PRN33,SatelliteSystem::GPS);

      CivilTime Time(2006,1,31,11,45,0,TimeSystem::Any);
         // Border Time (Time of Border test cases)
      CivilTime bTime(2006,1,31,2,0,0,TimeSystem::Any);
      const CommonTime ComTime = (CommonTime)Time;
      const CommonTime CombTime = (CommonTime)bTime;

      Xvt xvt1;
      Xvt xvt15;
      Xvt xvt32;
      try
      {
            //-------------------------------------------------------
            // Does getXvt work in ideal settings?
            //-------------------------------------------------------
         try
         {
            rinEphStore.getXvt(sid1,ComTime);
            TUPASS("sedIdentifier");
         }
         catch (Exception& e)
         {
            cout << "Exception thrown is " << e << endl;
            TUFAIL("getXvt threw an exception when it should not");
         }

         xvt1 = rinEphStore.getXvt(sid1,ComTime);
         xvt15 = rinEphStore.getXvt(sid15,ComTime);
         xvt32 = rinEphStore.getXvt(sid32,ComTime);

         fPRN1 << xvt1;
         fPRN15 << xvt15;
         fPRN32 << xvt32;

            //-------------------------------------------------------
            // Can I get an xvt for a unlisted (too small) SV?
            //-------------------------------------------------------
         try
         {
            rinEphStore.getXvt(sid0,CombTime);
            TUFAIL("getXvt was successful when it shouldn't be");
         }
         catch (InvalidRequest& e)
         {
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("getXvt threw an unexpected exception");
         }

            //-------------------------------------------------------
            // Can I get an xvt for a unlisted (too large) SV?
            //-------------------------------------------------------
         try
         {
            rinEphStore.getXvt(sid33,CombTime);
            TUFAIL("getXvt was successful when it shouldn't be");
         }
         catch (InvalidRequest& e)
         {
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("getXvt threw an unexpected exception");
         }
      }
      catch (Exception& e)
      {
            // cout << e;
      }



      std::string comparisonOutput1  =
         "x:(1.43293e+07, -2.70658e+06, -2.19986e+07),"
         " v:(354.7, 2812.28, -117.977),"
         " clk bias:3.42039e-05,"
         " clk drift:1.93268e-12,"
         " relcorr:-1.49802e-09,"
         " health:Healthy";
      std::string comparisonOutput15 =
         "x:(1.46708e+07, 7.54272e+06, 2.07205e+07),"
         " v:(-2147.84, 1575.62, 902.864),"
         " clk bias:0.000558473,"
         " clk drift:5.91172e-12,"
         " relcorr:2.04148e-08,"
         " health:Healthy";
      std::string comparisonOutput32 =
         "x:(8.40859e+06, 1.71989e+07, -1.87307e+07),"
         " v:(-2248.19, -606.201, -1577.98),"
         " clk bias:2.12814e-05,"
         " clk drift:3.41061e-12,"
         " relcorr:-5.04954e-09,"
         " health:Healthy";
      TUCSM("getXvt Output");

         //-------------------------------------------------------
         // Compare data for SatID 1 with pre-determined standard
         //-------------------------------------------------------
      TUASSERTE(std::string, comparisonOutput1, fPRN1.str());

         //-------------------------------------------------------
         // Compare data for SatID 15 with pre-determined standard
         //-------------------------------------------------------
      TUASSERTE(std::string, comparisonOutput15, fPRN15.str());

         //-------------------------------------------------------
         // Compare data for SatID 32 with pre-determined standard
         //-------------------------------------------------------
      TUASSERTE(std::string, comparisonOutput32, fPRN32.str());

      TURETURN();
   }

   unsigned computeXvtTest()
   {
      TUDEF("OrbitEphStore", "computeXvt");

      try
      {
         RinexEphemerisStore rinEphStore;
         rinEphStore.loadFile(inputRinexNavData.c_str());

         stringstream fPRN1;
         stringstream fPRN15;
         stringstream fPRN32;
         Xvt rv;
         const short PRN0 = 0; // Zero PRN (Border test case)
         const short PRN1 = 1;
         const short PRN15 = 15;
         const short PRN32 = 32;
         const short PRN33 = 33;  // Top PRN (33) (Border test case);

         SatID sid0(PRN0,SatelliteSystem::GPS);
         SatID sid1(PRN1,SatelliteSystem::GPS);
         SatID sid15(PRN15,SatelliteSystem::GPS);
         SatID sid32(PRN32,SatelliteSystem::GPS);
         SatID sid33(PRN33,SatelliteSystem::GPS);

         CivilTime tTime(2006,1,31,11,45,0,TimeSystem::Any);
            // Border Time (Time of Border test cases)
         CivilTime bTime(2006,1,31,2,0,0,TimeSystem::Any);
         const CommonTime comTime = (CommonTime)tTime;
         const CommonTime combTime = (CommonTime)bTime;

         TUCATCH(rv = rinEphStore.computeXvt(sid1,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy,rv.health);
         fPRN1 << rv;
         TUCATCH(rv = rinEphStore.computeXvt(sid15,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy,rv.health);
         fPRN15 << rv;
         TUCATCH(rv = rinEphStore.computeXvt(sid32,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy,rv.health);
         fPRN32 << rv;
         TUCATCH(rv = rinEphStore.computeXvt(sid0,combTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable,rv.health);
         TUCATCH(rv = rinEphStore.computeXvt(sid33,combTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable,rv.health);

         TUASSERTE(std::string, comparisonOutput1, fPRN1.str());
         TUASSERTE(std::string, comparisonOutput15, fPRN15.str());
         TUASSERTE(std::string, comparisonOutput32, fPRN32.str());
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }


   unsigned getSVHealthTest()
   {
      TUDEF("OrbitEphStore", "getSVHealth");

      try
      {
         RinexEphemerisStore rinEphStore;
         rinEphStore.loadFile(inputRinexNavData.c_str());

         Xvt::HealthStatus rv;
         const short PRN0 = 0; // Zero PRN (Border test case)
         const short PRN1 = 1;
         const short PRN15 = 15;
         const short PRN32 = 32;
         const short PRN33 = 33;  // Top PRN (33) (Border test case);

         SatID sid0(PRN0,SatelliteSystem::GPS);
         SatID sid1(PRN1,SatelliteSystem::GPS);
         SatID sid15(PRN15,SatelliteSystem::GPS);
         SatID sid32(PRN32,SatelliteSystem::GPS);
         SatID sid33(PRN33,SatelliteSystem::GPS);

         CivilTime Time(2006,1,31,11,45,0,TimeSystem::Any);
            // Border Time (Time of Border test cases)
         CivilTime bTime(2006,1,31,2,0,0,TimeSystem::Any);
         const CommonTime comTime = (CommonTime)Time;
         const CommonTime combTime = (CommonTime)bTime;

         TUCATCH(rv = rinEphStore.getSVHealth(sid1,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy, rv);
         TUCATCH(rv = rinEphStore.getSVHealth(sid15,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy, rv);
         TUCATCH(rv = rinEphStore.getSVHealth(sid32,comTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Healthy, rv);
         TUCATCH(rv = rinEphStore.getSVHealth(sid0,combTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable, rv);
         TUCATCH(rv = rinEphStore.getSVHealth(sid33,combTime));
         TUASSERTE(Xvt::HealthStatus, Xvt::HealthStatus::Unavailable, rv);
      }
      catch (...)
      {
         TUFAIL("Unexpected exception");
      }
      TURETURN();
   }


//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      getXvt() This test differs from the previous in that this
//      getXvt() has another parameter for the IODC
//
//      This test makes sure that exceptions are thrown if there is no
//      ephemeris data for the given PRN and also that an exception is
//      thrown if there is no data for the PRN at the given
//      time. Furthermore, this test finds an Xvt for a given
//      CivilTime Time and PRN and IODC.
//
//      To see the Xvt information for the selected Time and PRN please see
//      getXvt2.txt
// NOTE: getXvt with an IODC option is now deprecated. Test is no
//      longer necessary, but is being left here in case the
//      functionality returns.
// NOTE: This test will need to be brought up to the newest standard
// should it be used again.
//=============================================================================
      /*
        unsigned getXvt2Test()
        {
        TUDEF("OrbitEphStore", "getXvt with IODC");


        ofstream fPRN1;
        ofstream fPRN15;
        ofstream fPRN32;

        string filename;
        filename = __FILE__;
        filename = filename.substr(0, filename.find_last_of("\\/"));

        fPRN1.open ((filename+"/Logs/getXvt2_1.txt").c_str());
        fPRN15.open ((filename+"/Logs/getXvt2_15.txt").c_str());
        fPRN32.open ((filename+"/Logs/getXvt2_32.txt").c_str());

        RinexEphemerisStore rinEphStore;
        rinEphStore.loadFile(inputRinexNavData.c_str());

        std::list<RinexNavData> R3NList;
        GPSEphemerisStore gpsEphStore;
        list<RinexNavData>::const_iterator it;
        rinEphStore.addToList(R3NList);
        for (it = R3NList.begin(); it != R3NList.end(); ++it)
        gpsEphStore.addEphemeris(EngEphemeris(*it));

        const short PRN0 = 0; // Zero PRN (Border test case)
        const short PRN1 = 1;
        const short PRN15 = 15;
        const short PRN32 = 32;
        const short PRN33 = 33;  // Top PRN (33) (Border test case);
        SatID sid0(PRN0,SatelliteSystem::GPS);
        SatID sid1(PRN1,SatelliteSystem::GPS);
        SatID sid15(PRN15,SatelliteSystem::GPS);
        SatID sid32(PRN32,SatelliteSystem::GPS);
        SatID sid33(PRN33,SatelliteSystem::GPS);

        short IODC0 = 89;
        short IODC1 = 372;
        short IODC15 = 455;
        short IODC32 = 441;
        short IODC33 = 392;

        CivilTime Time(2006,1,31,11,45,0,TimeSystem::GPS);
        CivilTime bTime(2006,1,31,2,0,0,TimeSystem::GPS); // Border Time (Time of Border test cases)
        const CommonTime ComTime = (CommonTime)Time;
        const CommonTime CombTime = (CommonTime)bTime;

        try
        {
      //--------------RinexEphemerisStore_getXvt2Test_1 - Does getXvt work in ideal settings?
      try
      {
      rinEphStore.getXvt(sid1,ComTime,IODC1);
      TUPASS("sedIdentifier");
      }
      catch (...) {
      testFramework.failTest();
      }

      fPRN1 << rinEphStore.getXvt(sid1,ComTime,IODC1) << endl;
      fPRN15 << rinEphStore.getXvt(sid15,ComTime,IODC15) << endl;
      fPRN32 << rinEphStore.getXvt(sid32,ComTime,IODC32) << endl;


      //--------------RinexEphemerisStore_getXvt2Test_2 - Is an error thrown when SatID is too small?
      try
      {
      rinEphStore.getXvt(sid0,CombTime,IODC0);
      testFramework.failTest();
      }
      catch (InvalidRequest& e) {
      TUPASS("sedIdentifier");
      }
      catch (...) {
      testFramework.failTest();
      }

      //--------------RinexEphemerisStore_getXvt2Test_3 - Is an error thrown when SatID is too large?
      try
      {
      rinEphStore.getXvt(sid33,CombTime,IODC33);
      testFramework.failTest();
      }
      catch (InvalidRequest& e) {
      TUPASS("sedIdentifier");
      }
      catch (...) {
      testFramework.failTest();
      }
      }
      catch (Exception& e)
      {
         // cout << e;
         }

         fPRN1.close();
         fPRN15.close();
         fPRN32.close();

         TUCSM("getXvt with IODC Output");
      //--------------RinexEphemerisStore_getXvt2Test_4 - Compare data for SatID 1 with pre-determined standard
      testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_1.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt1.chk").c_str()));
      testFramework.next();
      //--------------RinexEphemerisStore_getXvt2Test_5 - Compare data for SatID 15 with pre-determined standard
      testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_15.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt15.chk").c_str()));
      testFramework.next();
      //--------------RinexEphemerisStore_getXvt2Test_6 - Compare data for SatID 32 with pre-determined standard
      testFramework.assert(fileEqualTest((char*)(filename+"/Logs/getXvt2_32.txt").c_str(),(char*)(filename+"/Checks/getPrnXvt32.chk").c_str()));
      testFramework.next();

      TURETURN();
      }
      */


//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member dump()
//
//      This test makes sure that dump() behaves as expected.  With
//      parameters from 1-3 with each giving more and more respective
//      information, this information is then put into txt files.
//
//      To see the dump with parameter 1, please view DumpData1.txt
//      To see the dump with parameter 2, pleave view DumpData2.txt
//      To see the dump with parameter 3, please view DumpData3.txt
//=============================================================================
   unsigned dumpTest()
   {
      TUDEF("GPSEphemerisStore", "dump");


      ofstream dumpTestOutputStreamForDetail0;
      ofstream dumpTestOutputStreamForDetail1;
      ofstream dumpTestOutputStreamForDetail2;

      dumpTestOutputStreamForDetail0.open(dumpTestOutputForDetail0.c_str());
      dumpTestOutputStreamForDetail1.open(dumpTestOutputForDetail1.c_str());
      dumpTestOutputStreamForDetail2.open(dumpTestOutputForDetail2.c_str());


      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      try
      {
            //-----------------------------------------------------------
            // Check that dump(, detail = 1) will work with no exceptions
            //-----------------------------------------------------------
         try
         {
            rinEphStore.dump(dumpTestOutputStreamForDetail0,1);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("Dump with detail=1 threw an exception when it should not");
         }

            //-----------------------------------------------------------
            // Check that dump(, detail = 2) will work with no exceptions
            //-----------------------------------------------------------
         try
         {
            rinEphStore.dump(dumpTestOutputStreamForDetail1,2);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("Dump with detail=2 threw an exception when it should not");
         }

            //-----------------------------------------------------------
            // Check that dump(, detail = 3) will work with no exceptions
            //-----------------------------------------------------------
         try
         {
            rinEphStore.dump(dumpTestOutputStreamForDetail2,3);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("Dump with detail=3 threw an exception when it should not");
         }


      }
      catch (Exception& e)
      {
            // cout << e;
      }

      dumpTestOutputStreamForDetail0.close();
      dumpTestOutputStreamForDetail1.close();
      dumpTestOutputStreamForDetail2.close();

         //-----------------------------------------------------------
         // Check dump(, detail = 1) output against its
         // pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(dumpTestOutputForDetail0,
                            dumpTestInputForDetail0, 2),
                            "Dump(*,detail=1) did not match its regressive output", __LINE__);

         //-----------------------------------------------------------
         // Check dump(, detail = 2) output against its
         // pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(dumpTestOutputForDetail1,
                            dumpTestInputForDetail1, 2),
                            "Dump(*,detail=2) did not match its regressive output", __LINE__);

         //-----------------------------------------------------------
         // Check dump(, detail = 3) output against its
         // pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(dumpTestOutputForDetail2,
                            dumpTestInputForDetail2, 2),
                            "Dump(*,detail=3) did not match its regressive output", __LINE__);

      TURETURN();
   }

//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      addEphemeris()
//
//      This test assures that no exceptions are thrown when an
//      ephemeris, taken from Store is added to a blank BCES Object.
//      Then the test makes sure that only that Ephemeris is in the
//      object by checking the start and end times of the Object
//=============================================================================

   unsigned addEphemerisTest()
   {
      TUDEF("GPSEphemerisStore", "addEphemeris");


      GPSEphemerisStore Blank;

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      std::list<GPSEphemeris> R3NList;
      GPSEphemerisStore gpsEphStore;
      list<GPSEphemeris>::const_iterator it;
      rinEphStore.addToList(R3NList);
      for (it = R3NList.begin(); it != R3NList.end(); ++it)
      {
         gpsEphStore.addEphemeris(GPSEphemeris(*it));
      }

      short PRN = 1;
      SatID sid(PRN,SatelliteSystem::GPS);

      CivilTime Time(2006,1,31,11,45,0,TimeSystem::GPS);
         // CivilTime TimeB(2006,1,31,9,59,44,TimeSystem::GPS);
         // CivilTime TimeE(2006,1,31,13,59,44,TimeSystem::GPS);
      CivilTime TimeBeginning(2006,1,31,10,0,0,TimeSystem::GPS);
      CivilTime TimeEnd(2006,1,31,14,0,0,TimeSystem::GPS);
      CivilTime defaultBeginning(4713, 1, 1, 0, 0, 0, TimeSystem::GPS);
      CivilTime defaultEnd(-4713, 1, 1, 0, 0, 0, TimeSystem::GPS);
      CivilTime check;
      const CommonTime ComTime  = Time.convertToCommonTime();
      const CommonTime ComTimeB = TimeBeginning.convertToCommonTime();
      const CommonTime ComTimeE = TimeEnd.convertToCommonTime();
      const CommonTime ComDefB  = defaultBeginning.convertToCommonTime();
      const CommonTime ComDefE  = defaultEnd.convertToCommonTime();
      const GPSEphemeris eph = gpsEphStore.findEphemeris(sid,ComTime);

      try
      {
            //-----------------------------------------------------------
            // Verify that addEphemeris runs with no errors
            //-----------------------------------------------------------
         try
         {
            Blank.addEphemeris(eph);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("addEphemeris threw an exception when all necessary data has been provided");
         }

            //-----------------------------------------------------------
            // Verify that addEphemeris added by checking the initial
            // time of the GPSEphemerisStore
            //-----------------------------------------------------------
         testFramework.assert(ComTimeB == Blank.getInitialTime(),
                               "addEphemeris may not have added the ephemeris or updated the initial time",
                               __LINE__);

            //-----------------------------------------------------------
            // Verify that addEphemeris added by checking the final
            // time of the GPSEphemerisStore
            //-----------------------------------------------------------
         testFramework.assert(ComTimeE == Blank.getFinalTime(),
                               "addEphemeris may not have added the ephemeris or updated the final time",
                               __LINE__);


         Blank.clear();
            //-----------------------------------------------------------
            // Verify that clear() worked by checking the initial time
            // of the GPSEphemerisStore
            //-----------------------------------------------------------
         testFramework.assert(ComDefB == Blank.getInitialTime(),
                               "clear may not have functioned or reset the initial time", __LINE__);

            //-----------------------------------------------------------
            // Verify that clear() worked by checking the initial time
            // of the GPSEphemerisStore
            //-----------------------------------------------------------
         testFramework.assert(ComDefE == Blank.getFinalTime(),
                               "clear may not have functioned or reset the final time", __LINE__);
      }
      catch (Exception& e)
      {
         cout << e;
      }

      TURETURN();
   }


//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      edit()
//
//      This test assures that no exceptions are thrown when we edit a
//      RES object then after we edit the RES Object, we test to make
//      sure that our edit time parameters are now the time endpoints
//      of the object.
//
//      For further inspection of the edit, please view editTest.txt
//=============================================================================
   unsigned editTest()
   {

      TUDEF("OrbitEphStore", "edit");


      ofstream editTestOutputStream;
      editTestOutputStream.open(editTestOutput.c_str());

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      CivilTime TimeMax(2006,1,31,15,45,0,TimeSystem::GPS);
      CivilTime TimeMin(2006,1,31,3,0,0,TimeSystem::GPS);

         // cout << "TimeMax: " << TimeMax << "\n" << "TimeMin: " << TimeMin << "\n";

      const CommonTime ComTMax = (CommonTime)TimeMax;
      const CommonTime ComTMin = (CommonTime)TimeMin;

         // cout << "ComTMax: " << ComTMax << "\n" << "ComTMin: " << ComTMin << "\n";

      try
      {
            //-----------------------------------------------------------
            // Verify that the edit method runs
            //-----------------------------------------------------------
         try
         {
            rinEphStore.edit(ComTMin, ComTMax);
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("edit threw an error when it should have functioned");
         }

         rinEphStore.edit(ComTMin, ComTMax);
            //-----------------------------------------------------------
            // Verify that the edit method changed the initial time
            //-----------------------------------------------------------
         testFramework.assert(ComTMin == rinEphStore.getInitialTime(),
                              "Edit did not change the initial time", __LINE__);

            //-----------------------------------------------------------
            // Verify that the edit method changed the final time
            //-----------------------------------------------------------
         testFramework.assert(ComTMax == rinEphStore.getFinalTime(),
                              "Edit did not change the initial time", __LINE__);


         rinEphStore.dump(editTestOutputStream,2);

      }
      catch (Exception& e)
      {
            // cout << e;
      }

      editTestOutputStream.close();
         //-----------------------------------------------------------
         // Check edited output against its pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(editTestOutput,
                            editTestInput, 2), "Output from edit does not match regressive standard",
                            __LINE__);

      TURETURN();
   }

//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      wiper()
//
//      This test assures that no exceptions are thrown when we wiper
//      a RES object then after we wiper the RES Object, we test to
//      make sure that our wiper time parameter in now the time
//      endpoint of the object.
//
//      For further inspection of the edit, please view wiperTest.txt
//
//      Please note that this test also indirectly tests size
//=============================================================================
      /*              unsigned wiperTest()
                      {
                      TUDEF("RinexEphemerisStore", "wiper");


                      ofstream dumpData1;
                      ofstream dumpData2;
                      outputTestOutput1 = outputTestOutput + "wiperTest.txt";
                      outputTestOutput15 = outputTestOutput + "wiperTest2.txt";
                      dumpData1.open(outputTestOutput1.c_str());
                      dumpData2.open(outputTestOutput15.c_str());


                      RinexEphemerisStore rinEphStore;
                      rinEphStore.loadFile(inputRinexNavData.c_str());

                      std::list<GPSEphemeris> R3NList;
                      GPSEphemerisStore gpsEphStore;
                      list<GPSEphemeris>::const_iterator it;
                      rinEphStore.addToList(R3NList);
                      for (it = R3NList.begin(); it != R3NList.end(); ++it)
                      {
                      gpsEphStore.addEphemeris(GPSEphemeris(*it));
                      }

                      CivilTime Time(2006,1,31,11,45,0,TimeSystem::GPS);
                      const CommonTime ComTime = (CommonTime)Time;

                      try
                      {
      //--------------RinexEphemerisStore_wiperTest_1 - Verify that the wiper method runs (but shouldn't wipe anything this time)
      try {
      gpsEphStore.wiper(CommonTime::BEGINNING_OF_TIME); TUPASS("sedIdentifier");
      }
      catch (...) {
      testFramework.failTest();
      }

         // Wipe everything outside interval and make sure that we did wipe all the data
            // up to the provided time.
            gpsEphStore.wiper(ComTime);

            gpsEphStore.dump(dumpData1,1);

      //--------------RinexEphemerisStore_wiperTest_2 - Verify that the new initial time is the time provided (partial wipe)
      testFramework.assert(ComTime == gpsEphStore.getInitialTime());
      testFramework.next();

         // Wipe everything, return size (should be zero)
         gpsEphStore.wiper(CommonTime::END_OF_TIME);
         unsigned int Num = gpsEphStore.gpstk::OrbitEphStore::size(); // Get the size of the GPSEphemerisStore

      //--------------RinexEphemerisStore_wiperTest_3 - Verify that the store is empty (total wipe)
      testFramework.assert((unsigned int) 0 == Num);
      testFramework.next();

      //--------------RinexEphemerisStore_wiperTest_4 - Verify that the initial time is the default END_OF_TIME (indicates empty GPSEphemerisStore)
      testFramework.assert(CommonTime::END_OF_TIME == gpsEphStore.getInitialTime());
      testFramework.next();

      gpsEphStore.dump(dumpData2,1);
      }
      catch (Exception& e)
      {
         // cout << e;
         }
         dumpData1.close();
         dumpData2.close();
         inputComparisonOutput1  = inputComparisonOutput + "wiperTest.chk";
         inputComparisonOutput15 = inputComparisonOutput + "wiperTest2.chk";

      //--------------RinexEphemerisStore_dumpTest_5 - Check partially wiped output against its pre-determined standard
      testFramework.assert(testFramework.fileEqualTest(outputTestOutput1, inputComparisonOutput1, 0));
      testFramework.next();

      //--------------RinexEphemerisStore_dumpTest_6 - Check totally wiped output against its pre-determined standard
      testFramework.assert(testFramework.fileEqualTest(outputTestOutput15, inputComparisonOutput15, 0));
      testFramework.next();

      TURETURN();
      }
      */

//=============================================================================
//      Test to assure the quality of OrbitEphStore class member
//      clear()
//
//      This test assures that no exceptions are thrown when we clear
//      a RES object then after we clear the RES Object, we test to
//      make sure that END_OF_TIME is our initial time and
//      BEGINNING_OF_TIME is our final time
//
//      For further inspection of the edit, please view clearTest.txt
//=============================================================================
   unsigned clearTest()
   {
      TUDEF("OrbitEphStore", "clear");


      ofstream clearTestOutputStream;
      clearTestOutputStream.open(clearTestOutput.c_str());

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      try
      {
            //-----------------------------------------------------------
            // Verify the gpstk::OrbitEphStore::clear() method runs
            //-----------------------------------------------------------
         try
         {
            rinEphStore.gpstk::OrbitEphStore::clear();
            TUPASS("sedIdentifier");
         }
         catch (...)
         {
            TUFAIL("OrbitEphStore::clear() threw an exception when it should not have");
         }

            //-----------------------------------------------------------
            // Verify that clear set the initial time to END_OF_TIME
            //-----------------------------------------------------------
         testFramework.assert(CommonTime::END_OF_TIME == rinEphStore.getInitialTime(),
                              "clear may not have cleared or may not have reset the initial time", __LINE__);

            //-----------------------------------------------------------
            // Verify that clear set the final time to BEGINNING_OF_TIME
            //-----------------------------------------------------------
         testFramework.assert(CommonTime::BEGINNING_OF_TIME ==
                              rinEphStore.getFinalTime(),
                              "clear may not have cleared or may not have reset the final time", __LINE__);


         rinEphStore.dump(clearTestOutputStream,1);
      }
      catch (Exception& e)
      {
            // cout << e;
      }

      clearTestOutputStream.close();
         //-----------------------------------------------------------
         // Check partially wiped output against its pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(clearTestOutput,
                            clearTestInput, 2),
                            "Output from clear does not match its regressive standard.", __LINE__);
      TURETURN();
   }


//=============================================================================
//      Test to assure the quality of OrbitEphStore class member
//      findUserOrbitEph()
//
//      This test will be performed using OrbitEphStore's grand-child
//      class GPSEphemerisStore
//
//      findUserOrbitEph find the ephemeris which a) is within the fit
//      tinterval for the given time of interest and 2) is the last
//      ephemeris transmitted before the time of interest
//      (i.e. min(toi-HOW time))
//
//      This test makes sure that exceptions are thrown if there is no
//      ephemeris data for the given PRN and also that an exception is
//      thrown if there is no data for the PRN at the given
//      time. Store is then cleared and the ephemeris data is readded
//      for output purposes.
//
//      For further inspection of the find, please view findUserTest.txt
//=============================================================================
   unsigned findUserOrbEphTest()
   {
      TUDEF("OrbitEphStore", "findUserOrbitEph");


      ofstream findUserTestOutputStream;
      findUserTestOutputStream.open(findUserTestOutput.c_str());

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      OrbitEphStore orbEphStore; // Store for found ephemerides

      CivilTime Time(2006,1,31,13,0,1,TimeSystem::GPS);
      const CommonTime ComTime = (CommonTime)Time;

      short PRN0 = 0;
      short PRN1 = 1;
      short PRN15 = 15;
      short PRN32 = 32;
      short PRN33 = 33;
      SatID sid0(PRN0,SatelliteSystem::GPS);
      SatID sid1(PRN1,SatelliteSystem::GPS);
      SatID sid15(PRN15,SatelliteSystem::GPS);
      SatID sid32(PRN32,SatelliteSystem::GPS);
      SatID sid33(PRN33,SatelliteSystem::GPS);

      try
      {
            //-----------------------------------------------------------
            // Check that a missing satID (too small) yields a thrown error
            //-----------------------------------------------------------
         const OrbitEph* oe = rinEphStore.findUserOrbitEph(sid0,ComTime);
         if (oe==NULL)
         {
            TUPASS("sedIdentifier");
         }
         else
         {
            TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
         }

            //-----------------------------------------------------------
            // Check that a missing satID (too big) yields a thrown error
            //-----------------------------------------------------------
         oe = rinEphStore.findUserOrbitEph(sid33,ComTime);
         if (oe==NULL)
         {
            TUPASS("sedIdentifier");
         }
         else
         {
            TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
         }

            //-----------------------------------------------------------
            // Check that an invalid time yields a thrown error
            //-----------------------------------------------------------
         oe = rinEphStore.findUserOrbitEph(sid1,CommonTime::END_OF_TIME);
         if (oe==NULL)
         {
            TUPASS("sedIdentifier");
         }
         else
         {
            TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
         }

            //-----------------------------------------------------------
            // Verify that for ideal conditions findUserOrbitEph runs
            //-----------------------------------------------------------
         oe = rinEphStore.findUserOrbitEph(sid1, ComTime);
         if (oe==NULL)
         {
            TUFAIL("findUserOrbitEph failed to find orbit elements when it should have succeeded");
         }
         else
         {
            TUPASS("sedIdentifier");
         }

         const OrbitEph* Eph1 = rinEphStore.findUserOrbitEph(sid1, ComTime);
         const OrbitEph* Eph15 = rinEphStore.findUserOrbitEph(sid15, ComTime);
         const OrbitEph* Eph32 = rinEphStore.findUserOrbitEph(sid32, ComTime);

         orbEphStore.addEphemeris(Eph1);
         orbEphStore.addEphemeris(Eph15);
         orbEphStore.addEphemeris(Eph32);

         orbEphStore.dump(findUserTestOutputStream,2);

      }
      catch (Exception& e)
      {
            // cout << e;
      }

      findUserTestOutputStream.close();
         //-----------------------------------------------------------
         // Check findUserOrbitEph output against its pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(findUserTestOutput,
                            findUserTestInput, 0),
                            "findUserOrbitEph output does not match its regressive standard", __LINE__);
      TURETURN();
   }

//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      findNearOrbitEph() findNearOrbitEph finds the ephemeris with
//      the HOW time closest to the time t, i.e with the smalles
//      fabs(t-HOW), but still within the fit interval
//
//      This test makes sure that exceptions are thrown if there is no
//      ephemeris data for the given PRN and also that an exception is
//      thrown if there is no data for the PRN at the given
//      time. Store is then cleared and the epeheris data is readded
//      for output purposes.
//
//      For further inspection of the find, please view findNearTest.txt
//=============================================================================



   unsigned findNearOrbEphTest()
   {
      TUDEF("OrbitEphStore", "findNearOrbitEph");


      ofstream findNearTestOutputStream;
      findNearTestOutputStream.open(findNearTestOutput.c_str());

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      OrbitEphStore orbEphStore; // Store for found ephemerides

      CivilTime Time(2006,1,31,13,0,1,TimeSystem::GPS);
      const CommonTime ComTime = (CommonTime)Time;

      short PRN0 = 0;
      short PRN1 = 1;
      short PRN15 = 15;
      short PRN32 = 32;
      short PRN33 = 33;
      SatID sid0(PRN0,SatelliteSystem::GPS);
      SatID sid1(PRN1,SatelliteSystem::GPS);
      SatID sid15(PRN15,SatelliteSystem::GPS);
      SatID sid32(PRN32,SatelliteSystem::GPS);
      SatID sid33(PRN33,SatelliteSystem::GPS);

      try
      {
            //-----------------------------------------------------------
            // Check that a missing satID (too small) yields a thrown error
            //-----------------------------------------------------------
         const OrbitEph* oe = rinEphStore.findNearOrbitEph(sid0,ComTime);
         if (oe==NULL)
         {
            TUPASS("sedIdentifier");
         }
         else
         {
            TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
         }

            //-----------------------------------------------------------
            // Check that a missing satID (too big) yields a thrown error
            //-----------------------------------------------------------
         oe = rinEphStore.findNearOrbitEph(sid33,ComTime);
         if (oe==NULL)
         {
            TUPASS("sedIdentifier");
         }
         else
         {
            TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
         }

            //-----------------------------------------------------------
            // Check that an invalid time yields a thrown error
            //
            // TEST REMOVED.  findNearOrbitEph() does not check
            // that the elements are within their fit interval.  Therefore
            // there is no concept of an "invalid time".
            //-----------------------------------------------------------
            /*
            oe = rinEphStore.findNearOrbitEph(sid1,CommonTime::END_OF_TIME);
            if (oe==NULL)
            {
               TUPASS("sedIdentifier");
            }
            else
            {
               TUFAIL("findUserOrbitEph returned a valid pointer when it should not");
            }
            */

            //-----------------------------------------------------------
            // Verify that for ideal conditions findUserOrbitEph runs
            //-----------------------------------------------------------
         oe = rinEphStore.findNearOrbitEph(sid1, ComTime);
         if (oe==NULL)
         {
            TUFAIL("findUserOrbitEph failed to find orbit elements when it should have succeeded");
         }
         else
         {
            TUPASS("sedIdentifier");
         }

         const OrbitEph* Eph1 = rinEphStore.findUserOrbitEph(sid1, ComTime);
         const OrbitEph* Eph15 = rinEphStore.findUserOrbitEph(sid15, ComTime);
         const OrbitEph* Eph32 = rinEphStore.findUserOrbitEph(sid32, ComTime);

         orbEphStore.addEphemeris(Eph1);
         orbEphStore.addEphemeris(Eph15);
         orbEphStore.addEphemeris(Eph32);

         orbEphStore.dump(findNearTestOutputStream,2);

      }
      catch (Exception& e)
      {
         e.addLocation(FILE_LOCATION);
         cout << e;
      }

      findNearTestOutputStream.close();

         //-----------------------------------------------------------
         // Check findNearOrbitEph output against its pre-determined standard
         //-----------------------------------------------------------
      testFramework.assert(testFramework.fileEqualTest(findNearTestOutput,
                            findNearTestInput, 0),
                            "findNearOrbitEph output does not match its regressive standard", __LINE__);
      TURETURN();
   }



//=============================================================================
//      Test to assure the quality of GPSEphemerisStore class member
//      addToList()

//      This test creats a list of GPSEphemeris and then adds all of
//      the ephemeris members to that list.  After that of the List
//      and Store are checked to be equal.
//=============================================================================

   unsigned addToListTest()
   {
      TUDEF("OrbitEphStore", "addToList");


      const short PRN1 = 1;
      const short PRN15 = 15;
      const short PRN32 = 32;

      unsigned numberOfEntries = 41;
      unsigned numberOfEntries1 = 15;
      unsigned numberOfEntries15 = 13;
      unsigned numberOfEntries32 = 13;

      SatID sid1(PRN1,SatelliteSystem::GPS);
      SatID sid15(PRN15,SatelliteSystem::GPS);
      SatID sid32(PRN32,SatelliteSystem::GPS);

      RinexEphemerisStore rinEphStore;
      rinEphStore.loadFile(inputRinexNavData.c_str());

      std::list<GPSEphemeris> R3NList;
      GPSEphemerisStore gpsEphStore;
      list<GPSEphemeris>::const_iterator it;
      rinEphStore.addToList(R3NList);
      for (it = R3NList.begin(); it != R3NList.end(); ++it)
      {
         gpsEphStore.addEphemeris(GPSEphemeris(*it));
//                        GPSEphemeris(*it).dumpBody(cout);
      }
      try
      {
            //-----------------------------------------------------------
            // Assert that the number of added members equals the size
            // of Store (all members added)
            //-----------------------------------------------------------
         testFramework.assert(rinEphStore.gpstk::OrbitEphStore::size() ==
                              gpsEphStore.gpstk::OrbitEphStore::size(),
                              "The added entries are not reflected in the GPSEphemerisStore", __LINE__);

         testFramework.assert(rinEphStore.gpstk::OrbitEphStore::size() ==
                              numberOfEntries, "The total number of entries is not what is expected",
                              __LINE__);

         testFramework.assert(rinEphStore.gpstk::OrbitEphStore::size(
                                 sid1) == numberOfEntries1,
                              "The total number of entries for SatID 1 is not what is expected", __LINE__);

         testFramework.assert(rinEphStore.gpstk::OrbitEphStore::size(
                                 sid15) == numberOfEntries15,
                              "The total number of entries for SatID 15 is not what is expected", __LINE__);

         testFramework.assert(rinEphStore.gpstk::OrbitEphStore::size(
                                 sid32) == numberOfEntries32,
                              "The total number of entries for SatID 32 is not what is expected", __LINE__);
      }
      catch (Exception& e)
      {
            // cout << e;
      }
      TURETURN();
   }

//=============================================================================
//      Initialize Test Data Filenames
//=============================================================================

   void init()
   {
      std::string dataFilePath = gpstk::getPathData();
      std::string tempFilePath = gpstk::getPathTestTemp();
      std::string file_sep = "/";

      inputRinexNavData             = dataFilePath + file_sep +
                                      "test_input_rinex_nav_ephemerisData.031";
      inputNotaFile                 = dataFilePath + file_sep + "NotaFILE";

      findEphTestOutput             = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_findEph.txt";
      findEphTestInput              = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_findEph.txt";
      gpsEphemerisStoreDumpOutput   = tempFilePath + file_sep + "DataDump.txt";

      dumpTestOutputForDetail0      = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_detail0.txt";
      dumpTestOutputForDetail1      = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_detail1.txt";
      dumpTestOutputForDetail2      = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_detail2.txt";
      dumpTestInputForDetail0       = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_detail0.txt";
      dumpTestInputForDetail1       = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_detail1.txt";
      dumpTestInputForDetail2       = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_detail2.txt";

      editTestOutput                = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_edit.txt";
      editTestInput                 = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_edit.txt";

      clearTestOutput               = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_clear.txt";
      clearTestInput                = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_clear.txt";

      findUserTestOutput            = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_findUser.txt";
      findUserTestInput             = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_findUser.txt";

      findNearTestOutput            = tempFilePath + file_sep +
                                      "test_output_ephemeris_dump_findNear.txt";
      findNearTestInput             = dataFilePath + file_sep +
                                      "test_input_ephemeris_dump_findNear.txt";
      comparisonOutput1  =
         "x:(1.43293e+07, -2.70658e+06, -2.19986e+07),"
         " v:(354.7, 2812.28, -117.977),"
         " clk bias:3.42039e-05,"
         " clk drift:1.93268e-12,"
         " relcorr:-1.49802e-09,"
         " health:Healthy";
      comparisonOutput15 =
         "x:(1.46708e+07, 7.54272e+06, 2.07205e+07),"
         " v:(-2147.84, 1575.62, 902.864),"
         " clk bias:0.000558473,"
         " clk drift:5.91172e-12,"
         " relcorr:2.04148e-08,"
         " health:Healthy";
      comparisonOutput32 =
         "x:(8.40859e+06, 1.71989e+07, -1.87307e+07),"
         " v:(-2248.19, -606.201, -1577.98),"
         " clk bias:2.12814e-05,"
         " clk drift:3.41061e-12,"
         " relcorr:-5.04954e-09,"
         " health:Healthy";
   }

private:
   std::string inputRinexNavData;
   std::string inputNotaFile;

   std::string findEphTestOutput;
   std::string findEphTestInput;
   std::string gpsEphemerisStoreDumpOutput;

   std::string dumpTestOutputForDetail0;
   std::string dumpTestOutputForDetail1;
   std::string dumpTestOutputForDetail2;
   std::string dumpTestInputForDetail0;
   std::string dumpTestInputForDetail1;
   std::string dumpTestInputForDetail2;

   std::string editTestOutput;
   std::string editTestInput;

   std::string clearTestOutput;
   std::string clearTestInput;

   std::string findUserTestOutput;
   std::string findUserTestInput;

   std::string findNearTestOutput;
   std::string findNearTestInput;

   std::string comparisonOutput1;
   std::string comparisonOutput15;
   std::string comparisonOutput32;
};

int main() // Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   RinexEphemerisStore_T testClass;
   testClass.init();

   errorTotal += testClass.RESTest();
   errorTotal += testClass.findEphTest();
   errorTotal += testClass.getXvtTest();
   errorTotal += testClass.computeXvtTest();
   errorTotal += testClass.getSVHealthTest();
   errorTotal += testClass.dumpTest();
   errorTotal += testClass.addToListTest();
   errorTotal += testClass.addEphemerisTest();
   errorTotal += testClass.editTest();
   errorTotal += testClass.clearTest();
   errorTotal += testClass.findUserOrbEphTest();
   errorTotal += testClass.findNearOrbEphTest();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal; // Return the total number of errors
}
