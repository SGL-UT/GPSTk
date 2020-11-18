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

#include "NavID.hpp"

#include "TestUtil.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <set>

using namespace std;
using namespace gpstk;

namespace gpstk
{
      // define some stream operators so that test failures involving
      // enums are a bit more readable.

   std::ostream& operator<<(std::ostream& s, gpstk::NavType e)
   {
      s << (long)e << " (" << gpstk::StringUtils::asString(e) << ")";
      return s;
   }
}

class NavID_T
{
public:
   NavID_T();
      /** Verifies that the NavID objects constructed in NavID_T()
       * meet expectations */
   unsigned constructorTest();
      /** Outputs the different NavID objects to their own
       * stringstreams and makes sure the results are as expected */
   unsigned streamOutputTest();
      /** Makes sure that turning the string names of NavTypes convert
       * back into their expected values.
       * @pre streamOutput test must have been executed.
       */
   unsigned stringConstructorTest();
   unsigned inequalityTest();
   unsigned asStringEnumTest();

   NavID testIDLNAV, testIDCNAVL2, testIDCNAVL5, testIDCNAV2, testIDMNAV,
      testIDBD1, testIDBD2, testIDGloF, testIDGloC, testIDGalOS, testIDGalOS_2,
      testIDGalOS_F, testID_IRNSS_SPS, testIDUnkwn;
   stringstream ln, l2, l5, cnav2, mn, d1, d2, gf, gc, ginv, ginv_2, gfnv, is,
      un;
};


NavID_T ::
NavID_T()
      : testIDLNAV(SatID(1, SatelliteSystem::GPS),
                   ObsID(ObservationType::NavMsg, CarrierBand::L1,
                         TrackingCode::CA)),
        testIDCNAVL2(SatID(1, SatelliteSystem::GPS),
                     ObsID(ObservationType::NavMsg, CarrierBand::L2,
                           TrackingCode::L2CML)),
        testIDCNAVL5(SatID(1, SatelliteSystem::GPS),
                     ObsID(ObservationType::NavMsg, CarrierBand::L5,
                           TrackingCode::L5I)),
        testIDCNAV2(SatID(1, SatelliteSystem::GPS),
                    ObsID(ObservationType::NavMsg, CarrierBand::L1,
                          TrackingCode::L1CDP)),
        testIDMNAV(SatID(1, SatelliteSystem::GPS),
                   ObsID(ObservationType::NavMsg, CarrierBand::L2,
                         TrackingCode::MDP)),
        testIDBD1(SatID(6, SatelliteSystem::BeiDou),
                  ObsID(ObservationType::NavMsg, CarrierBand::B1,
                        TrackingCode::B1I)),
        testIDBD2(SatID(5, SatelliteSystem::BeiDou),
                  ObsID(ObservationType::NavMsg, CarrierBand::B2,
                        TrackingCode::B1I)),
        testIDGloF(SatID(2, SatelliteSystem::Glonass),
                   ObsID(ObservationType::NavMsg, CarrierBand::G1,
                         TrackingCode::Standard)),
        testIDGloC(SatID(2, SatelliteSystem::Glonass),
                   ObsID(ObservationType::NavMsg, CarrierBand::G3,
                         TrackingCode::L3OCDP)),
        testIDGalOS(SatID(2, SatelliteSystem::Galileo),
                    ObsID(ObservationType::NavMsg, CarrierBand::L1,
                          TrackingCode::E1B)),
        testIDGalOS_2(SatID(2, SatelliteSystem::Galileo),
                      ObsID(ObservationType::NavMsg, CarrierBand::E5b,
                            TrackingCode::E5bI)),
        testIDGalOS_F(SatID(2, SatelliteSystem::Galileo),
                      ObsID(ObservationType::NavMsg, CarrierBand::L5,
                            TrackingCode::E5aI)),
        testID_IRNSS_SPS(SatID(2, SatelliteSystem::IRNSS),
                         ObsID(ObservationType::NavMsg, CarrierBand::L5,
                               TrackingCode::SPSL5)),
        testIDUnkwn(SatID(1, SatelliteSystem::GPS),
                    ObsID(ObservationType::NavMsg, CarrierBand::L5,
                          TrackingCode::MDP))
{
}


unsigned NavID_T ::
constructorTest()
{
   TUDEF("NavID", "NavID()");

   NavID dfltTest;
   TUASSERTE(gpstk::NavType, NavType::Unknown, dfltTest.navType);

      //GPS LNAV
   TUCSM("NavID(SatID,ObsID)");
   TUASSERTE(gpstk::NavType, NavType::GPSLNAV, testIDLNAV.navType);

      //GPS CNAV L2
   TUASSERTE(gpstk::NavType, NavType::GPSCNAVL2, testIDCNAVL2.navType);

      //GPS CNAV L5
   TUASSERTE(gpstk::NavType, NavType::GPSCNAVL5, testIDCNAVL5.navType);

      //GPS CNAV2
   TUASSERTE(gpstk::NavType, NavType::GPSCNAV2, testIDCNAV2.navType);

      //GPS MNAV
   TUASSERTE(gpstk::NavType, NavType::GPSMNAV, testIDMNAV.navType);

      //Beidou D1
   TUASSERTE(gpstk::NavType, NavType::BeiDou_D1, testIDBD1.navType);

      //Beidou D2
   TUASSERTE(gpstk::NavType, NavType::BeiDou_D2, testIDBD2.navType);
   
      //Glonass Civil F
   TUASSERTE(gpstk::NavType, NavType::GloCivilF, testIDGloF.navType);
   
      //Glonass Civil C
   TUASSERTE(gpstk::NavType, NavType::GloCivilC, testIDGloC.navType);
   
      //Galileo Open Sys
   TUASSERTE(gpstk::NavType, NavType::GalINAV, testIDGalOS.navType);

   TUASSERTE(gpstk::NavType, NavType::GalINAV, testIDGalOS_2.navType);

   TUASSERTE(gpstk::NavType, NavType::GalFNAV, testIDGalOS_F.navType);

      //IRNSS L5 SPS
   TUASSERTE(gpstk::NavType, NavType::IRNSS_SPS, testID_IRNSS_SPS.navType);
   
      //Unknown
   TUASSERTE(gpstk::NavType, NavType::Unknown, testIDUnkwn.navType);

   TURETURN();
}


unsigned NavID_T ::
streamOutputTest()
{
   TUDEF("NavID", "operator<<");

   ln << testIDLNAV;
   TUASSERTE(std::string, "GPS_LNAV", ln.str());
   
   l2 << testIDCNAVL2;
   TUASSERTE(std::string, "GPS_CNAV_L2", l2.str());
   
   l5 << testIDCNAVL5;
   TUASSERTE(std::string, "GPS_CNAV_L5", l5.str());

   cnav2 << testIDCNAV2;
   TUASSERTE(std::string, "GPS_CNAV2", cnav2.str());
   
   mn << testIDMNAV;
   TUASSERTE(std::string, "GPS_MNAV", mn.str());
   
   d1 << testIDBD1;
   TUASSERTE(std::string, "Beidou_D1", d1.str());
   
   d2 << testIDBD2;
   TUASSERTE(std::string, "Beidou_D2", d2.str());
   
   gf << testIDGloF;
   TUASSERTE(std::string, "GloCivilF", gf.str());
   
   gc << testIDGloC;
   TUASSERTE(std::string, "GloCivilC", gc.str());
   
   ginv << testIDGalOS;
   TUASSERTE(std::string, "GalINAV", ginv.str());
   
   ginv_2 << testIDGalOS_2;
   TUASSERTE(std::string, "GalINAV", ginv_2.str());
   
   gfnv << testIDGalOS_F;
   TUASSERTE(std::string, "GalFNAV", gfnv.str());

   is << testID_IRNSS_SPS;
   TUASSERTE(std::string, "IRNSS_SPS", is.str());
   
   un << testIDUnkwn;
   TUASSERTE(std::string, "Unknown", un.str());

   TURETURN();
}


unsigned NavID_T ::
stringConstructorTest()
{
   TUDEF("NavID", "NavID(string)");

   NavID testIDLNAVString(ln.str());
   TUASSERTE(gpstk::NavType, NavType::GPSLNAV, testIDLNAVString.navType);
   
   NavID testIDCNAVL2String(l2.str());
   TUASSERTE(gpstk::NavType, NavType::GPSCNAVL2, testIDCNAVL2String.navType);
   
   NavID testIDCNAVL5String(l5.str());
   TUASSERTE(gpstk::NavType, NavType::GPSCNAVL5, testIDCNAVL5String.navType);
   
   NavID testIDCNAV2String(cnav2.str());
   TUASSERTE(gpstk::NavType, NavType::GPSCNAV2, testIDCNAV2String.navType);
   
   NavID testIDMNAVString(mn.str());
   TUASSERTE(gpstk::NavType, NavType::GPSMNAV, testIDMNAVString.navType);
   
   NavID testIDBD1String(d1.str());
   TUASSERTE(gpstk::NavType, NavType::BeiDou_D1, testIDBD1String.navType);
   
   NavID testIDBD2String(d2.str());
   TUASSERTE(gpstk::NavType, NavType::BeiDou_D2, testIDBD2String.navType);
   
   NavID testIDGloFString(gf.str());
   TUASSERTE(gpstk::NavType, NavType::GloCivilF, testIDGloFString.navType);
   
   NavID testIDGloCString(gc.str());
   TUASSERTE(gpstk::NavType, NavType::GloCivilC, testIDGloCString.navType);
   
   NavID testIDGalOSStringI(ginv.str());
   TUASSERTE(gpstk::NavType, NavType::GalINAV, testIDGalOSStringI.navType);
   
   NavID testIDGalOSStringF(gfnv.str());
   TUASSERTE(gpstk::NavType, NavType::GalFNAV, testIDGalOSStringF.navType);
   
   NavID testID_IRNSS_SPS_String(is.str());
   TUASSERTE(gpstk::NavType,NavType::IRNSS_SPS,testID_IRNSS_SPS_String.navType);
   
   NavID testIDUnkwnString(un.str());
   TUASSERTE(gpstk::NavType, NavType::Unknown, testIDUnkwnString.navType);

   TURETURN();
}


unsigned NavID_T ::
inequalityTest()
{
   TUDEF("NavID", "operator!=");

   set<NavID> testSet;
      //Insert NavTypes into set in backward order.
      // This implicitly tests operator<
   testSet.insert(testIDUnkwn);
   testSet.insert(testID_IRNSS_SPS);
   testSet.insert(testIDGalOS);
   testSet.insert(testIDGalOS_F);
   testSet.insert(testIDGloC);
   testSet.insert(testIDGloF);
   testSet.insert(testIDBD2);
   testSet.insert(testIDBD1);
   testSet.insert(testIDMNAV);
   testSet.insert(testIDCNAV2);
   testSet.insert(testIDCNAVL5);
   testSet.insert(testIDCNAVL2);
   testSet.insert(testIDLNAV);

      //Instantiate currTest as GPS_LNAV by 
      //instantiating navType as ntGPSLNAV.
   NavID currTest;
   currTest.navType = NavType::GPSLNAV;

      //Set nid equal to dereferenced NavID set iterator; should
      //initially correspond to first location in set (@ value GPS_LNAV).
      //Compare to currTest which initially corresponds to NavID for first
      //location in NavType enum (ntGPSLNAV).
   for (const auto& nid :  testSet)
   {
      TUASSERTE(gpstk::NavID, currTest, nid);
      currTest.navType = static_cast<gpstk::NavType>((int)currTest.navType + 1);
   }

   TURETURN();
}


unsigned NavID_T ::
asStringEnumTest()
{
   TUDEF("NavID", "asString");
      // These tests verify that every enum has a string
      // representation and every string has a corresponding enum.
      // It also implicitly verifies that the string
      // representations aren't duplicated, since if two enums
      // translated to string "XXX", the attempt to reverse the
      // translation would fail.
   for (gpstk::NavType nt : NavTypeIterator())
   {
      std::string s;
      TUCATCH(s = gpstk::StringUtils::asString(nt));
      TUASSERT(!s.empty());
      if (nt != gpstk::NavType::Unknown)
         TUASSERT(s != "Unknown");
      gpstk::NavType nt2;
      TUCATCH(nt2 = gpstk::StringUtils::asNavType(s));
      TUASSERTE(gpstk::NavType, nt, nt2);
   }
   TURETURN();
}

int main()
{
   unsigned errorTotal = 0;
   NavID_T testClass;

   errorTotal += testClass.constructorTest();
   errorTotal += testClass.streamOutputTest();
   errorTotal += testClass.stringConstructorTest();
   errorTotal += testClass.inequalityTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}

