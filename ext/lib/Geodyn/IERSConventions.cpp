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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
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

/**
 * @file IERSConventions2003.cpp
 * IERS Conventions 2003
 */

#include "IERSConventions.hpp"
//#include "Logger.hpp"
#include "CommonTime.hpp"
#include "YDSTime.hpp"
#include "MJD.hpp"
#include "CivilTime.hpp"

namespace gpstk
{
   using namespace std;
   
      // Reference epoch (J2000)
   const CommonTime J2000(CivilTime(2000,1,1,12,0,0.0,TimeSystem::UTC));

   const double PI = std::atan(1.0)*4.0;    // 3.1415926535897932; 
      // 2PI
   const double D2PI = PI+PI;               // 6.283185307179586476925287;

      // Days per Julian century 
   const double DJC = 36525.0;

      // Arcseconds to radians 
   const double DAS2R = PI/180.0/3600.0;    // 4.848136811095359935899141e-6;

      // seconds to radians
   const double DS2R = PI/43200.0;          // 7.272205216643039903848712e-5;


   // IERS Data Handling
   //--------------------------------------------------------------------------
   
   class TAImUTCData : public map<CommonTime,int>
   {     
   public:
      void leapHistory(int year, int month, int day,int leap)
      { (*this)[ CivilTime(year,month,day,0,0,0.0,TimeSystem::UTC) ] = leap; }  
      
      TAImUTCData()
      {         
         leapHistory( 1972, 1, 1, 10 );
         leapHistory( 1972, 7, 1, 11 );
         leapHistory( 1973, 1, 1, 12 );
         leapHistory( 1974, 1, 1, 13 );
         leapHistory( 1975, 1, 1, 14 );
         leapHistory( 1976, 1, 1, 15 );
         leapHistory( 1977, 1, 1, 16 );
         leapHistory( 1978, 1, 1, 17 );
         leapHistory( 1979, 1, 1, 18 );
         leapHistory( 1980, 1, 1, 19 );
         leapHistory( 1981, 7, 1, 20 );
         leapHistory( 1982, 7, 1, 21 );
         leapHistory( 1983, 7, 1, 22 );
         leapHistory( 1985, 7, 1, 23 );
         leapHistory( 1988, 1, 1, 24 );
         leapHistory( 1990, 1, 1, 25 );
         leapHistory( 1991, 1, 1, 26 );
         leapHistory( 1992, 7, 1, 27 );
         leapHistory( 1993, 7, 1, 28 );
         leapHistory( 1994, 7, 1, 29 );
         leapHistory( 1996, 1, 1, 30 );
         leapHistory( 1997, 7, 1, 31 );
         leapHistory( 1999, 1, 1, 32 );
         leapHistory( 2006, 1, 1, 33 );
         leapHistory( 2009, 1, 1, 34 );
         leapHistory( 2012, 7, 1, 35 );

         // more leap seconds should be appended here
         // ...
      }
   };
      // Leap seconds data table
   static const TAImUTCData  lsDataTable;

   int TAImUTC(const CommonTime& UTC)
      throw(InvalidRequest)
   {
      if( UTC < CivilTime(1972,1,1,0,0,0.0,TimeSystem::UTC) )
      {  
         GPSTK_THROW(
            InvalidRequest( "There are no leap second data for the epoch"
                           +UTC.asString()) );
      }
      
      // it point to the iterator >= utc
      TAImUTCData::const_iterator it = lsDataTable.lower_bound(UTC);

      if( (it == lsDataTable.end()) || (it->first != UTC) )
      {
         it--;
         return it->second;
      }
      else if( it->first == UTC )
      {
         return it->second;
      }
      
      // it should  never go here
      GPSTK_THROW(Exception("My God, it should never go here!"));
      
   }

   double TTmTAI(){ return 32.184; }

   double TAImGPST(){ return 19.0; }


   static EOPDataStore eopDataTable;

      // 'finals.data' from http://maia.usno.navy.mil/
   void LoadIERSFile(const std::string& fileName)
   {
      eopDataTable.clear();
      try { eopDataTable.loadIERSFile(fileName); }
      catch(...)
      {
         GPSTK_THROW(Exception("Failed to load the IERS ERP File "+fileName));
      }
   }

      // ERP data file from IGS
   void LoadIGSFile(const std::string& fileName)
   {
      eopDataTable.clear();
      try { eopDataTable.loadIGSFile(fileName); }
      catch(...)
      {
         GPSTK_THROW(Exception("Failed to load the IGS ERP File "+fileName));
      }
   }

      // ERP data file from STK 
   void LoadSTKFile(const std::string& fileName)
   {
      eopDataTable.clear();
      try { eopDataTable.loadSTKFile(fileName); }
      catch(...)
      {
         GPSTK_THROW(Exception("Failed to load the STK ERP File "+fileName));
      }
   }


      // Request EOP Data
   EOPDataStore::EOPData EOPData(const CommonTime& UTC)
      throw(InvalidRequest)
   {
      return eopDataTable.getEOPData(UTC);
   }

      // in arcsecond
   double PolarMotionX(const CommonTime& UTC)
   {
      try { return EOPData(UTC).xp; }
      catch(...) 
      { 
         GPSTK_THROW(Exception("Failed to get EOP data on" + CivilTime(UTC).asString()));

         return 0.0;
      }
   }

      // in arcsecond
   double PolarMotionY(const CommonTime& UTC)
   {
      try { return EOPData(UTC).yp; }
      catch(...) 
      {
         GPSTK_THROW(Exception("Failed to get EOP data on " + CivilTime(UTC).asString()));

         return 0.0;
      }
   }

      // in second
   double UT1mUTC(const CommonTime& UTC)
   {
      try { return EOPData(UTC).UT1mUTC; }
      catch(...) 
      { 
         GPSTK_THROW(Exception("Failed to get EOP data on " + CivilTime(UTC).asString()));

         return 0.0;
      }
   }

      // in arcsecond
   double NutationDPsi(const CommonTime& UTC)
   {
      try { return EOPData(UTC).dPsi; }
      catch(...) 
      {
         GPSTK_THROW(Exception("Failed to get EOP data on "+CivilTime(UTC).asString()));

         return 0.0;
      }
   }

      // in arcsecond
   double NutationDEps(const CommonTime& UTC)
   {
      try { return EOPData(UTC).dEps; }
      catch(...) 
      {
         GPSTK_THROW(Exception("Failed to get EOP data on "+CivilTime(UTC).asString()));

         return 0.0;
      }
   }

   // Time System Handling
   //--------------------------------------------------------------------------
   
   CommonTime ConvertTimeSystem(const CommonTime& time, TimeSystemEnum from, TimeSystemEnum to)
   {
      if(from==to) return time;

      static std::map<TimeSystemEnum,std::string> mapTSName;
      if(mapTSName.size()==0)
      {
         mapTSName[TS_UTC] = "UTC";
         mapTSName[TS_UT1] = "UT1";
         mapTSName[TS_GPST]= "GPST";
         mapTSName[TS_TAI] = "TAI";
         mapTSName[TS_TT]  = "TT";
      }     
      
      std::map<TimeSystemEnum,std::string>::const_iterator itf,itt,ite;
      itf = mapTSName.find(from);
      itt = mapTSName.find(to);
      ite = mapTSName.end();
      
      if( (itf==ite) || (itt==ite) )
      {
         Exception e("Can't convert the Time System from "
            + ((itf==ite)?"Unknown":itf->second) + std::string(" to")
            + ((itt==ite)?"Unknown":itt->second) + std::string("."));

         GPSTK_THROW(e);
      }

      typedef CommonTime (*ConvertFunPtr)(const CommonTime&);
      
      ConvertFunPtr funPtr1(0);
      if( itf->first == TS_UT1) funPtr1 = UT12UTC;
      if( itf->first == TS_GPST)funPtr1 = GPST2UTC;
      if( itf->first == TS_TAI) funPtr1 = TAI2UTC;
      if( itf->first == TS_TT)  funPtr1 = TT2UTC;

      CommonTime utc = funPtr1 ? funPtr1(time) : time;
      
      ConvertFunPtr funPtr2(0);
      if( itt->first == TS_UT1) funPtr2 = UTC2UT1;
      if( itt->first == TS_GPST)funPtr2 = UTC2GPST;
      if( itt->first == TS_TAI) funPtr2 = UTC2TAI;
      if( itt->first == TS_TT)  funPtr2 = UTC2TT;

      CommonTime dest = funPtr2 ? funPtr2(utc) : utc;

      return dest;
   }

   CommonTime GPST2UTC(const CommonTime& GPST)
   {
      // the input should be UTC
      int leapSec = TAImUTC(GPST);   
      CommonTime UTC = GPST;
      UTC += (19.0 - double(leapSec));

      leapSec = TAImUTC(UTC);
      UTC = GPST;
      UTC += (19.0 - double(leapSec));

      return UTC;
   }
   CommonTime UTC2GPST(const CommonTime& UTC)
   {
      CommonTime GPST(UTC);
      GPST += TAImUTC(UTC);   // TAI
      GPST +=-TAImGPST();     // GPST
      return GPST;
   }

   CommonTime UT12UTC(const CommonTime& UT1)
   {
      CommonTime UTC(UT1);
      UTC -= UT1mUTC(UT1);   // input should be utc
      
      CommonTime T(UT1);
      T -= UT1mUTC(UTC);

      UTC = UT1;
      UTC -= UT1mUTC(T);

      return UTC;
   }
   CommonTime UTC2UT1(const CommonTime& UTC)
   {
      CommonTime UT1(UTC);
      UT1 += UT1mUTC(UTC);

      return UT1;
   }

   CommonTime UT12UTC(const CommonTime& UT1,double ut1mutc)
   {
      CommonTime UTC(UT1);
      UTC -= ut1mutc;

      return UTC;
   }
   CommonTime UTC2UT1(const CommonTime& UTC,double ut1mutc)
   {
      CommonTime UT1(UTC);
      UT1 += ut1mutc;

      return UT1;
   }

   CommonTime TT2UTC(const CommonTime& TT)
   {
      CommonTime TAI  = TT;          // TT
      TAI -= TTmTAI();       // TAI

      CommonTime UTC = TAI;
      UTC -= TAImUTC(TAI); // input should be UTC     

      CommonTime UTC2(UTC);

      UTC = TAI;
      UTC -= TAImUTC(UTC2);

      UTC2 = UTC;

      UTC = TAI;
      UTC -= TAImUTC(UTC2);

      return UTC;
   }
   CommonTime UTC2TT(const CommonTime& UTC)
   {
      CommonTime TAI(UTC);
      TAI += TAImUTC(UTC);

      CommonTime TT(TAI);
      TT += TTmTAI();
      
      return TT;
   }

   CommonTime TAI2UTC(const CommonTime& TAI)
   {
      CommonTime UTC(TAI);
      UTC -= TAImUTC(TAI); // input should be UTC     

      CommonTime UTC2 = TAI;
      UTC2 -= TAImUTC(UTC);

      UTC = TAI;
      UTC -= TAImUTC(UTC2);

      return UTC;
   }
   CommonTime UTC2TAI(const CommonTime& UTC)
   {
      CommonTime TAI(UTC);
      TAI += TAImUTC(UTC);      // TAI

      return TAI;
   }

   CommonTime BDT2UTC(const CommonTime& BDT)
   {
      CommonTime GPST(BDT);
      GPST += 14.0;

      return GPST2UTC(GPST);
   }
   CommonTime UTC2BDT(const CommonTime& UTC)
   {
      CommonTime BDT = UTC2GPST(UTC);  
      BDT -= 14.0;

      return BDT;
   }

   // Reference System Handling
   //--------------------------------------------------------------------------

   Triple J2kPosToECEF(const Triple& j2kPos, const CommonTime& time, TimeSystemEnum sys)
   {
      Vector<double> j2kR(3,0.0);
      j2kR[0] = j2kPos[0];
      j2kR[1] = j2kPos[1];
      j2kR[2] = j2kPos[2];
      
      CommonTime UTC = ConvertTimeSystem(time, sys, TS_UTC);
      Vector<double> ecefR = J2kPosToECEF(UTC,j2kR);

      return Triple(ecefR[0], ecefR[1], ecefR[2]);
   }

   Triple ECEFPosToJ2k(const Triple& ecefPos, const CommonTime& time, TimeSystemEnum sys)
   {
      Vector<double> ecefR(3,0.0);
      ecefR[0] = ecefPos[0];
      ecefR[1] = ecefPos[1];
      ecefR[2] = ecefPos[2];

      CommonTime UTC = ConvertTimeSystem(time, sys, TS_UTC);
      Vector<double> j2kR = ECEFPosToJ2k(UTC,ecefR);

      return Triple(j2kR[0], j2kR[1], j2kR[2]);
   }


   double iauNut80Args(const CommonTime& TT,double& eps, double& dpsi,double& deps)
      throw(Exception)
   {
      static const double nut[106][10]={
         {   0,   0,   0,   0,   1, -6798.4, -171996, -174.2, 92025,   8.9},
         {   0,   0,   2,  -2,   2,   182.6,  -13187,   -1.6,  5736,  -3.1},
         {   0,   0,   2,   0,   2,    13.7,   -2274,   -0.2,   977,  -0.5},
         {   0,   0,   0,   0,   2, -3399.2,    2062,    0.2,  -895,   0.5},
         {   0,  -1,   0,   0,   0,  -365.3,   -1426,    3.4,    54,  -0.1},
         {   1,   0,   0,   0,   0,    27.6,     712,    0.1,    -7,   0.0},
         {   0,   1,   2,  -2,   2,   121.7,    -517,    1.2,   224,  -0.6},
         {   0,   0,   2,   0,   1,    13.6,    -386,   -0.4,   200,   0.0},
         {   1,   0,   2,   0,   2,     9.1,    -301,    0.0,   129,  -0.1},
         {   0,  -1,   2,  -2,   2,   365.2,     217,   -0.5,   -95,   0.3},
         {  -1,   0,   0,   2,   0,    31.8,     158,    0.0,    -1,   0.0},
         {   0,   0,   2,  -2,   1,   177.8,     129,    0.1,   -70,   0.0},
         {  -1,   0,   2,   0,   2,    27.1,     123,    0.0,   -53,   0.0},
         {   1,   0,   0,   0,   1,    27.7,      63,    0.1,   -33,   0.0},
         {   0,   0,   0,   2,   0,    14.8,      63,    0.0,    -2,   0.0},
         {  -1,   0,   2,   2,   2,     9.6,     -59,    0.0,    26,   0.0},
         {  -1,   0,   0,   0,   1,   -27.4,     -58,   -0.1,    32,   0.0},
         {   1,   0,   2,   0,   1,     9.1,     -51,    0.0,    27,   0.0},
         {  -2,   0,   0,   2,   0,  -205.9,     -48,    0.0,     1,   0.0},
         {  -2,   0,   2,   0,   1,  1305.5,      46,    0.0,   -24,   0.0},
         {   0,   0,   2,   2,   2,     7.1,     -38,    0.0,    16,   0.0},
         {   2,   0,   2,   0,   2,     6.9,     -31,    0.0,    13,   0.0},
         {   2,   0,   0,   0,   0,    13.8,      29,    0.0,    -1,   0.0},
         {   1,   0,   2,  -2,   2,    23.9,      29,    0.0,   -12,   0.0},
         {   0,   0,   2,   0,   0,    13.6,      26,    0.0,    -1,   0.0},
         {   0,   0,   2,  -2,   0,   173.3,     -22,    0.0,     0,   0.0},
         {  -1,   0,   2,   0,   1,    27.0,      21,    0.0,   -10,   0.0},
         {   0,   2,   0,   0,   0,   182.6,      17,   -0.1,     0,   0.0},
         {   0,   2,   2,  -2,   2,    91.3,     -16,    0.1,     7,   0.0},
         {  -1,   0,   0,   2,   1,    32.0,      16,    0.0,    -8,   0.0},
         {   0,   1,   0,   0,   1,   386.0,     -15,    0.0,     9,   0.0},
         {   1,   0,   0,  -2,   1,   -31.7,     -13,    0.0,     7,   0.0},
         {   0,  -1,   0,   0,   1,  -346.6,     -12,    0.0,     6,   0.0},
         {   2,   0,  -2,   0,   0, -1095.2,      11,    0.0,     0,   0.0},
         {  -1,   0,   2,   2,   1,     9.5,     -10,    0.0,     5,   0.0},
         {   1,   0,   2,   2,   2,     5.6,      -8,    0.0,     3,   0.0},
         {   0,  -1,   2,   0,   2,    14.2,      -7,    0.0,     3,   0.0},
         {   0,   0,   2,   2,   1,     7.1,      -7,    0.0,     3,   0.0},
         {   1,   1,   0,  -2,   0,   -34.8,      -7,    0.0,     0,   0.0},
         {   0,   1,   2,   0,   2,    13.2,       7,    0.0,    -3,   0.0},
         {  -2,   0,   0,   2,   1,  -199.8,      -6,    0.0,     3,   0.0},
         {   0,   0,   0,   2,   1,    14.8,      -6,    0.0,     3,   0.0},
         {   2,   0,   2,  -2,   2,    12.8,       6,    0.0,    -3,   0.0},
         {   1,   0,   0,   2,   0,     9.6,       6,    0.0,     0,   0.0},
         {   1,   0,   2,  -2,   1,    23.9,       6,    0.0,    -3,   0.0},
         {   0,   0,   0,  -2,   1,   -14.7,      -5,    0.0,     3,   0.0},
         {   0,  -1,   2,  -2,   1,   346.6,      -5,    0.0,     3,   0.0},
         {   2,   0,   2,   0,   1,     6.9,      -5,    0.0,     3,   0.0},
         {   1,  -1,   0,   0,   0,    29.8,       5,    0.0,     0,   0.0},
         {   1,   0,   0,  -1,   0,   411.8,      -4,    0.0,     0,   0.0},
         {   0,   0,   0,   1,   0,    29.5,      -4,    0.0,     0,   0.0},
         {   0,   1,   0,  -2,   0,   -15.4,      -4,    0.0,     0,   0.0},
         {   1,   0,  -2,   0,   0,   -26.9,       4,    0.0,     0,   0.0},
         {   2,   0,   0,  -2,   1,   212.3,       4,    0.0,    -2,   0.0},
         {   0,   1,   2,  -2,   1,   119.6,       4,    0.0,    -2,   0.0},
         {   1,   1,   0,   0,   0,    25.6,      -3,    0.0,     0,   0.0},
         {   1,  -1,   0,  -1,   0, -3232.9,      -3,    0.0,     0,   0.0},
         {  -1,  -1,   2,   2,   2,     9.8,      -3,    0.0,     1,   0.0},
         {   0,  -1,   2,   2,   2,     7.2,      -3,    0.0,     1,   0.0},
         {   1,  -1,   2,   0,   2,     9.4,      -3,    0.0,     1,   0.0},
         {   3,   0,   2,   0,   2,     5.5,      -3,    0.0,     1,   0.0},
         {  -2,   0,   2,   0,   2,  1615.7,      -3,    0.0,     1,   0.0},
         {   1,   0,   2,   0,   0,     9.1,       3,    0.0,     0,   0.0},
         {  -1,   0,   2,   4,   2,     5.8,      -2,    0.0,     1,   0.0},
         {   1,   0,   0,   0,   2,    27.8,      -2,    0.0,     1,   0.0},
         {  -1,   0,   2,  -2,   1,   -32.6,      -2,    0.0,     1,   0.0},
         {   0,  -2,   2,  -2,   1,  6786.3,      -2,    0.0,     1,   0.0},
         {  -2,   0,   0,   0,   1,   -13.7,      -2,    0.0,     1,   0.0},
         {   2,   0,   0,   0,   1,    13.8,       2,    0.0,    -1,   0.0},
         {   3,   0,   0,   0,   0,     9.2,       2,    0.0,     0,   0.0},
         {   1,   1,   2,   0,   2,     8.9,       2,    0.0,    -1,   0.0},
         {   0,   0,   2,   1,   2,     9.3,       2,    0.0,    -1,   0.0},
         {   1,   0,   0,   2,   1,     9.6,      -1,    0.0,     0,   0.0},
         {   1,   0,   2,   2,   1,     5.6,      -1,    0.0,     1,   0.0},
         {   1,   1,   0,  -2,   1,   -34.7,      -1,    0.0,     0,   0.0},
         {   0,   1,   0,   2,   0,    14.2,      -1,    0.0,     0,   0.0},
         {   0,   1,   2,  -2,   0,   117.5,      -1,    0.0,     0,   0.0},
         {   0,   1,  -2,   2,   0,  -329.8,      -1,    0.0,     0,   0.0},
         {   1,   0,  -2,   2,   0,    23.8,      -1,    0.0,     0,   0.0},
         {   1,   0,  -2,  -2,   0,    -9.5,      -1,    0.0,     0,   0.0},
         {   1,   0,   2,  -2,   0,    32.8,      -1,    0.0,     0,   0.0},
         {   1,   0,   0,  -4,   0,   -10.1,      -1,    0.0,     0,   0.0},
         {   2,   0,   0,  -4,   0,   -15.9,      -1,    0.0,     0,   0.0},
         {   0,   0,   2,   4,   2,     4.8,      -1,    0.0,     0,   0.0},
         {   0,   0,   2,  -1,   2,    25.4,      -1,    0.0,     0,   0.0},
         {  -2,   0,   2,   4,   2,     7.3,      -1,    0.0,     1,   0.0},
         {   2,   0,   2,   2,   2,     4.7,      -1,    0.0,     0,   0.0},
         {   0,  -1,   2,   0,   1,    14.2,      -1,    0.0,     0,   0.0},
         {   0,   0,  -2,   0,   1,   -13.6,      -1,    0.0,     0,   0.0},
         {   0,   0,   4,  -2,   2,    12.7,       1,    0.0,     0,   0.0},
         {   0,   1,   0,   0,   2,   409.2,       1,    0.0,     0,   0.0},
         {   1,   1,   2,  -2,   2,    22.5,       1,    0.0,    -1,   0.0},
         {   3,   0,   2,  -2,   2,     8.7,       1,    0.0,     0,   0.0},
         {  -2,   0,   2,   2,   2,    14.6,       1,    0.0,    -1,   0.0},
         {  -1,   0,   0,   0,   2,   -27.3,       1,    0.0,    -1,   0.0},
         {   0,   0,  -2,   2,   1,  -169.0,       1,    0.0,     0,   0.0},
         {   0,   1,   2,   0,   1,    13.1,       1,    0.0,     0,   0.0},
         {  -1,   0,   4,   0,   2,     9.1,       1,    0.0,     0,   0.0},
         {   2,   1,   0,  -2,   0,   131.7,       1,    0.0,     0,   0.0},
         {   2,   0,   0,   2,   0,     7.1,       1,    0.0,     0,   0.0},
         {   2,   0,   2,  -2,   1,    12.8,       1,    0.0,    -1,   0.0},
         {   2,   0,  -2,   0,   1,  -943.2,       1,    0.0,     0,   0.0},
         {   1,  -1,   0,  -2,   0,   -29.3,       1,    0.0,     0,   0.0},
         {  -1,   0,   0,   1,   1,  -388.3,       1,    0.0,     0,   0.0},
         {  -1,  -1,   0,   2,   1,    35.0,       1,    0.0,     0,   0.0},
         {   0,   1,   0,   1,   0,    27.3,       1,    0.0,     0,   0.0}
      };

      static const double fc[][5]={ /* coefficients for iau 1980 nutation */
         { 134.96340251, 1717915923.2178,  31.8792,  0.051635, -0.00024470},
         { 357.52910918,  129596581.0481,  -0.5532,  0.000136, -0.00001149},
         {  93.27209062, 1739527262.8478, -12.7512, -0.001037,  0.00000417},
         { 297.85019547, 1602961601.2090,  -6.3706,  0.006593, -0.00003169},
         { 125.04455501,   -6962890.2665,   7.4722,  0.007702  -0.00005939}
      };

      eps = 0.0;
      dpsi = 0.0; 
      deps = 0.0;
      
      // Julian cent. since J2000
      const double T = (TT-J2000)/86400.0/36525.0;
      
      eps = (84381.448-46.8150*T-0.00059*T*T+0.001813*T*T*T)*DAS2R;  // eps

      double f[5]={0.0};
      {
         double tt[4]={0.0}; tt[0] = T;
         for ( int i=1; i<4; i++) tt[i]=tt[i-1]*T;
         for (int i=0; i<5; i++) 
         {
            f[i]=fc[i][0]*3600.0;
            for (int j=0; j<4; j++) f[i]+=fc[i][j+1]*tt[j];
            f[i]=fmod(f[i]*DAS2R, 2.0*PI);
         }
      }
      
      for(int i = 0; i < 106; i++) 
      {
         double ang(0.0);
         for(int j=0; j<5; j++) ang+=nut[i][j]*f[j];
         
         dpsi+=(nut[i][6]+nut[i][7]*T)*std::sin(ang);
         deps+=(nut[i][8]+nut[i][9]*T)*std::cos(ang);
      }

      dpsi *= 1E-4*DAS2R; /* 0.1 mas -> rad */
      deps *= 1E-4*DAS2R;

      return f[4];

   }  // End of method 'iauNut80Args()'


   void J2kToECEFMatrix(const CommonTime& UTC, 
                        const EOPDataStore::EOPData& ERP,
                        Matrix<double>& POM, 
                        Matrix<double>& Theta, 
                        Matrix<double>& NP)
      throw(Exception)
   {
      double xp = ERP.xp * DAS2R;
      double yp = ERP.yp * DAS2R;
      double ut1_utc = ERP.UT1mUTC;
      double ddeps = ERP.dEps * DAS2R;
      double ddpsi = ERP.dPsi * DAS2R;
      
      CommonTime TT = UTC2TT(UTC);
      CommonTime UT1 = UTC2UT1(UTC,ut1_utc);
      
      // IAU 1976 precession matrix 
      Matrix<double> P = iauPmat76(TT);

      // IAU 1980 nutation matrix 
      double eps(0.0),dpsi(0.0),deps(0.0);
      double f = iauNut80Args(TT,eps,dpsi,deps);

      Matrix<double> N = iauNmat(eps, dpsi + ddpsi, deps + ddeps);

      NP = N * P;                     // output NP

      YDSTime ut1_yds(UT1);
      double  ut1_sec = ut1_yds.sod;
      ut1_yds.sod = 0.0;
      CommonTime ut1_day(ut1_yds);
      const double t = (ut1_day-J2000) / 86400.0 / DJC;
   
      double temp = 24110.54841+8640184.812866*t+0.093104*(t*t)-6.2E-6*(t*t*t)
                   +1.002737909350795*ut1_sec;
      double gmst = fmod(temp,86400.0)*DS2R;
      double ee = dpsi * std::cos(eps) 
                + (0.00264 * std::sin(f) + 0.000063 * std::sin(f+f))*DAS2R;
      double gast = normalizeAngle(gmst + ee);
     
      Theta = Rz(gast);               // output Theta

      // Polar motion matrix
      POM = Ry(-xp) * Rx(-yp);        // output POM

      //GPSTK_DEBUG_MAT("",Theta,20,12,"Theta");
   }

   // ECI to ECF transform matrix, POM * Theta * NP 
   Matrix<double> J2kToECEFMatrix(const CommonTime& UTC,const EOPDataStore::EOPData& ERP)
   {
      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,ERP,POM,Theta,NP);
      
      return (POM * Theta * NP);
   }

      // Convert position from J2000 to ECEF.
   Vector<double> J2kPosToECEF(const CommonTime& UTC, const Vector<double>& j2kPos)
      throw(Exception)
   {
      EOPDataStore::EOPData ERP = EOPData(UTC);
      Matrix<double> c2tMat = J2kToECEFMatrix(UTC,ERP);
      return c2tMat * j2kPos;
   }


      // Convert position from ECEF to J2000.
   Vector<double> ECEFPosToJ2k(const CommonTime& UTC, const Vector<double>& ecefPos)
      throw(Exception)
   {
      EOPDataStore::EOPData ERP = EOPData(UTC);
      Matrix<double> c2tMat = J2kToECEFMatrix(UTC,ERP);
      return transpose(c2tMat) * ecefPos;
   }

   // Convert position and velocity from J2000 to ECEF.
   Vector<double> J2kPosVelToECEF(const CommonTime& UTC, const Vector<double>& j2kPosVel)
      throw(Exception)
   {
      EOPDataStore::EOPData ERP = EOPData(UTC);

      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,ERP,POM,Theta,NP);

      const double dera = earthRotationAngleRate1( UTC2TT(UTC) );

      // Derivative of Earth rotation 
      Matrix<double> S(3,3,0.0);
      S(0,1) = 1.0; S(1,0) = -1.0;      

      Matrix<double> dTheta = dera * S * Theta;

      Matrix<double> c2t = POM * Theta * NP;
      Matrix<double> dc2t = POM * dTheta * NP;

      Vector<double> j2kPos(3, 0.0), j2kVel(3, 0.0);
      for(int i=0; i<3; i++)
      {
         j2kPos(i) = j2kPosVel(i);
         j2kVel(i) = j2kPosVel(i+3);
      }

      Vector<double> ecefPos = c2t * j2kPos;
      Vector<double> ecefVel = c2t * j2kVel + dc2t * j2kPos;

      Vector<double> ecefPosVel(6,0.0);
      for(int i=0; i<3; i++)
      {
         ecefPosVel(i) = ecefPos(i);
         ecefPosVel(i+3) = ecefVel(i);
      }

      return ecefPosVel;
   }

   // Convert position and velocity from ECEF to J2000.
   Vector<double> ECEFPosVelToJ2k(const CommonTime& UTC, const Vector<double>& ecefPosVel)
      throw(Exception)
   {
      EOPDataStore::EOPData ERP = EOPData(UTC);

      Matrix<double> POM, Theta, NP;
      J2kToECEFMatrix(UTC,ERP,POM,Theta,NP);

      const double dera = earthRotationAngleRate1( UTC2TT(UTC) );

      // Derivative of Earth rotation 
      Matrix<double> S(3,3,0.0);
      S(0,1) = 1.0; S(1,0) = -1.0;      

      Matrix<double> dTheta = dera * S * Theta;

      Matrix<double> c2t = POM * Theta * NP;
      Matrix<double> dc2t = POM * dTheta * NP;

      Vector<double> ecefPos(3, 0.0), ecefVel(3, 0.0);
      for(int i=0; i<3; i++)
      {
         ecefPos(i) = ecefPosVel(i);
         ecefVel(i) = ecefPosVel(i+3);
      }

      Vector<double> j2kPos = transpose(c2t) * ecefPos;
      Vector<double> j2kVel = transpose(c2t) * ecefVel 
         +transpose(dc2t)* ecefPos;

      Vector<double> j2kPosVel(6,0.0);
      for(int i=0; i<3; i++)
      {
         j2kPosVel(i) = j2kPos(i);
         j2kPosVel(i+3) = j2kVel(i);
      }

      return j2kPosVel;
   }


   Vector<double> sunJ2kPosition(const CommonTime& TT)
   {
      // P70~P73

      // Obliquity of J2000 ecliptic
      const double eps = 23.43929111 * PI / 180.0;

      // Julian cent. since J2000
      const double T = (TT-J2000)/86400.0/36525.0;  

      // [rad] Eq 3.43
      double M = std::fmod(0.9931267 + 99.9973583*T,1.0)*D2PI;   

      // Ecliptic longitude [rad]
      double L = std::fmod( 0.7859444 + M/D2PI 
         +(6892.0*std::sin(M)+72.0*std::sin(2.0*M))/1296.0e3
         ,1.0)*D2PI; 

      // Distance [m] Eq 3.44
      double r = 149.619e9-2.499e9*std::cos(M)-0.021e9*std::cos(2.0*M);    


      return Triple(r*std::cos(L),r*std::sin(L),0.0).R1(-eps*180.0/PI).toVector();
   }

   Vector<double> moonJ2kPosition(const CommonTime& TT)
   {
      // Obliquity of J2000 ecliptic
      const double eps = 23.43929111 * PI / 180.0;
      const double Arcs = 3600.0*180.0/PI;

      // Julian cent. since J2000
      const double T = (TT-J2000)/86400.0/36525.0;  

      // Mean elements of lunar orbit

      // Eq 3.47
      double L0 = std::fmod(0.606433 + 1336.851344*T, 1.0);
      double l  = std::fmod( 0.374897 + 1325.552410*T,1.0)*D2PI;
      double lp = std::fmod( 0.993133 +   99.997361*T,1.0)*D2PI;
      double F  = std::fmod( 0.259086 + 1342.227825*T,1.0)*D2PI;
      double D  = std::fmod( 0.827361 + 1236.853086*T,1.0)*D2PI;

      // Ecliptic longitude (w.r.t. equinox of J2000)

      // Eq 3.48
      double dL = +22640.0*std::sin(l)-4586.0*std::sin(l-2*D)+2370.0*std::sin(2*D)
         +769.0*std::sin(2.0*l)-668.0*std::sin(lp)-412.0*std::sin(2.0*F)
         -212.0*std::sin(2.0*l-2.0*D)-206.0*std::sin(l+lp-2.0*D)
         +192.0*std::sin(l+2.0*D)-165.0*std::sin(lp-2.0*D)-125.0*std::sin(D) 
         -110.0*std::sin(l+lp)+148.0*std::sin(l-lp)-55.0*std::sin(2.0*F-2.0*D);

      double L = std::fmod( L0 + dL/1296.0e3, 1.0)*D2PI;  // [rad]

      // Ecliptic latitude

      // Eq 3.49
      double S = F + (dL+412.0*std::sin(2.0*F)+541.0*std::sin(lp)) / Arcs; 
      double h = F-2.0*D;
      double N =-526.0*std::sin(h) + 44.0*std::sin(l+h)-31.0*std::sin(-l+h)-23.0*std::sin(lp+h) 
         +11.0*std::sin(-lp+h)-25.0*std::sin(-2.0*l+F)+21.0*std::sin(-l+F);

      double B = (18520.0*std::sin(S)+N) / Arcs;   // [rad]

      // Distance [m] Eq 3.50

      double R = 385000e3-20905e3*std::cos(l)-3699e3*std::cos(2.0*D-l)
         -2956e3*std::cos(2.0*D)-570e3*std::cos(2.0*l)+246e3*std::cos(2.0*l-2.0*D) 
         -205e3*std::cos(lp-2.0*D)-171e3*std::cos(l+2.0*D)-152e3*std::cos(l+lp-2.0*D);   

      // Eq 3.51
      Triple rMoon(R*std::cos(L)*std::cos(B),
         R*std::sin(L)*std::cos(B),
         R*std::sin(B));

      return rMoon.R1(-eps*180.0/PI).toVector();
   }


   //////////////////////////////////////////////////////////////////////////
   
      // Normalize angle into the range -pi <= a < +pi.
   double normalizeAngle(double a)
   {
      double w = fmod(a, D2PI);
      if (fabs(w) >= (D2PI*0.5)) 
      {
         w-= ((a<0.0)?-D2PI:D2PI);
      }

      return w;
   }


   // Rotate an r-matrix about the x-axis.
   Matrix<double> Rx(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { 1, 0, 0, 0, c, s, 0,-s, c };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

   // Rotate an r-matrix about the y-axis.
   Matrix<double> Ry(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { c, 0,-s, 0, 1, 0, s, 0, c };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

   // Rotate an r-matrix about the z-axis.
   Matrix<double> Rz(const double& angle)
   {
      const double s = std::sin(angle);
      const double c = std::cos(angle);

      const double a[9] = { c, s, 0,-s, c, 0, 0, 0, 1 };

      Matrix<double> r(3,3,0.0);
      r = a;

      return r;
   }

   Matrix<double> iauPmat76(const CommonTime& TT)
   {
      // Interval between fundamental epoch J2000.0 and start epoch (JC). 
      const double t0 = 0.0;

      // Interval over which precession required (JC). 
      const double t = ( TT - J2000 ) / 86400.0 / DJC;

      // Euler angles. 
      const double tas2r = t * DAS2R;
      const double w = 2306.2181 + (1.39656 - 0.000139 * t0) * t0;

      double zeta = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;

      double z = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;

      double theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)
         + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

      return ( Rz(-z) * Ry(theta) * Rz(-zeta) );

   }  // End of method 'iauPmat76()'


   void nutationAngles(const CommonTime& TT, double& dpsi, double& deps)
   {
      // Units of 0.1 milliarcsecond to radians 
      const double U2R = DAS2R / 1e4;

      // Table of multiples of arguments and coefficients 
      // ------------------------------------------------ 

      // The units for the sine and cosine coefficients are 0.1 mas and 
      // the same per Julian century 

      static const struct 
      {
         int nl,nlp,nf,nd,nom; // coefficients of l,l',F,D,Om 
         double sp,spt;        // longitude sine, 1 and t coefficients 
         double ce,cet;        // obliquity cosine, 1 and t coefficients 
      } x[] = {

         /* 1-10 */
         {  0,  0,  0,  0,  1, -171996.0, -174.2,  92025.0,    8.9 },
         {  0,  0,  0,  0,  2,    2062.0,    0.2,   -895.0,    0.5 },
         { -2,  0,  2,  0,  1,      46.0,    0.0,    -24.0,    0.0 },
         {  2,  0, -2,  0,  0,      11.0,    0.0,      0.0,    0.0 },
         { -2,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  1, -1,  0, -1,  0,      -3.0,    0.0,      0.0,    0.0 },
         {  0, -2,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0, -2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2, -2,  2,  -13187.0,   -1.6,   5736.0,   -3.1 },
         {  0,  1,  0,  0,  0,    1426.0,   -3.4,     54.0,   -0.1 },

         /* 11-20 */
         {  0,  1,  2, -2,  2,    -517.0,    1.2,    224.0,   -0.6 },
         {  0, -1,  2, -2,  2,     217.0,   -0.5,    -95.0,    0.3 },
         {  0,  0,  2, -2,  1,     129.0,    0.1,    -70.0,    0.0 },
         {  2,  0,  0, -2,  0,      48.0,    0.0,      1.0,    0.0 },
         {  0,  0,  2, -2,  0,     -22.0,    0.0,      0.0,    0.0 },
         {  0,  2,  0,  0,  0,      17.0,   -0.1,      0.0,    0.0 },
         {  0,  1,  0,  0,  1,     -15.0,    0.0,      9.0,    0.0 },
         {  0,  2,  2, -2,  2,     -16.0,    0.1,      7.0,    0.0 },
         {  0, -1,  0,  0,  1,     -12.0,    0.0,      6.0,    0.0 },
         { -2,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },

         /* 21-30 */
         {  0, -1,  2, -2,  1,      -5.0,    0.0,      3.0,    0.0 },
         {  2,  0,  0, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
         {  0,  1,  2, -2,  1,       4.0,    0.0,     -2.0,    0.0 },
         {  1,  0,  0, -1,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  2,  1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0, -2,  2,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  1, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  0,  2,       1.0,    0.0,      0.0,    0.0 },
         { -1,  0,  0,  1,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 31-40 */
         {  0,  0,  2,  0,  2,   -2274.0,   -0.2,    977.0,   -0.5 },
         {  1,  0,  0,  0,  0,     712.0,    0.1,     -7.0,    0.0 },
         {  0,  0,  2,  0,  1,    -386.0,   -0.4,    200.0,    0.0 },
         {  1,  0,  2,  0,  2,    -301.0,    0.0,    129.0,   -0.1 },
         {  1,  0,  0, -2,  0,    -158.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  0,  2,     123.0,    0.0,    -53.0,    0.0 },
         {  0,  0,  0,  2,  0,      63.0,    0.0,     -2.0,    0.0 },
         {  1,  0,  0,  0,  1,      63.0,    0.1,    -33.0,    0.0 },
         { -1,  0,  0,  0,  1,     -58.0,   -0.1,     32.0,    0.0 },
         { -1,  0,  2,  2,  2,     -59.0,    0.0,     26.0,    0.0 },

         /* 41-50 */
         {  1,  0,  2,  0,  1,     -51.0,    0.0,     27.0,    0.0 },
         {  0,  0,  2,  2,  2,     -38.0,    0.0,     16.0,    0.0 },
         {  2,  0,  0,  0,  0,      29.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  2, -2,  2,      29.0,    0.0,    -12.0,    0.0 },
         {  2,  0,  2,  0,  2,     -31.0,    0.0,     13.0,    0.0 },
         {  0,  0,  2,  0,  0,      26.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  0,  1,      21.0,    0.0,    -10.0,    0.0 },
         { -1,  0,  0,  2,  1,      16.0,    0.0,     -8.0,    0.0 },
         {  1,  0,  0, -2,  1,     -13.0,    0.0,      7.0,    0.0 },
         { -1,  0,  2,  2,  1,     -10.0,    0.0,      5.0,    0.0 },

         /* 51-60 */
         {  1,  1,  0, -2,  0,      -7.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2,  0,  2,       7.0,    0.0,     -3.0,    0.0 },
         {  0, -1,  2,  0,  2,      -7.0,    0.0,      3.0,    0.0 },
         {  1,  0,  2,  2,  2,      -8.0,    0.0,      3.0,    0.0 },
         {  1,  0,  0,  2,  0,       6.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2, -2,  2,       6.0,    0.0,     -3.0,    0.0 },
         {  0,  0,  0,  2,  1,      -6.0,    0.0,      3.0,    0.0 },
         {  0,  0,  2,  2,  1,      -7.0,    0.0,      3.0,    0.0 },
         {  1,  0,  2, -2,  1,       6.0,    0.0,     -3.0,    0.0 },
         {  0,  0,  0, -2,  1,      -5.0,    0.0,      3.0,    0.0 },

         /* 61-70 */
         {  1, -1,  0,  0,  0,       5.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2,  0,  1,      -5.0,    0.0,      3.0,    0.0 },
         {  0,  1,  0, -2,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2,  0,  0,       4.0,    0.0,      0.0,    0.0 },
         {  0,  0,  0,  1,  0,      -4.0,    0.0,      0.0,    0.0 },
         {  1,  1,  0,  0,  0,      -3.0,    0.0,      0.0,    0.0 },
         {  1,  0,  2,  0,  0,       3.0,    0.0,      0.0,    0.0 },
         {  1, -1,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         { -1, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
         { -2,  0,  0,  0,  1,      -2.0,    0.0,      1.0,    0.0 },

         /* 71-80 */
         {  3,  0,  2,  0,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  0, -1,  2,  2,  2,      -3.0,    0.0,      1.0,    0.0 },
         {  1,  1,  2,  0,  2,       2.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2, -2,  1,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0,  0,  0,  1,       2.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  0,  0,  2,      -2.0,    0.0,      1.0,    0.0 },
         {  3,  0,  0,  0,  0,       2.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2,  1,  2,       2.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  0,  0,  2,       1.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 81-90 */
         { -2,  0,  2,  2,  2,       1.0,    0.0,     -1.0,    0.0 },
         { -1,  0,  2,  4,  2,      -2.0,    0.0,      1.0,    0.0 },
         {  2,  0,  0, -4,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  1,  2, -2,  2,       1.0,    0.0,     -1.0,    0.0 },
         {  1,  0,  2,  2,  1,      -1.0,    0.0,      1.0,    0.0 },
         { -2,  0,  2,  4,  2,      -1.0,    0.0,      1.0,    0.0 },
         { -1,  0,  4,  0,  2,       1.0,    0.0,      0.0,    0.0 },
         {  1, -1,  0, -2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  2,  0,  2, -2,  1,       1.0,    0.0,     -1.0,    0.0 },
         {  2,  0,  2,  2,  2,      -1.0,    0.0,      0.0,    0.0 },

         /* 91-100 */
         {  1,  0,  0,  2,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  4, -2,  2,       1.0,    0.0,      0.0,    0.0 },
         {  3,  0,  2, -2,  2,       1.0,    0.0,      0.0,    0.0 },
         {  1,  0,  2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  2,  0,  1,       1.0,    0.0,      0.0,    0.0 },
         { -1, -1,  0,  2,  1,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0, -2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2, -1,  2,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2, -2,  0,      -1.0,    0.0,      0.0,    0.0 },

         /* 101-106 */
         {  0, -1,  2,  0,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  1,  0, -2,  1,      -1.0,    0.0,      0.0,    0.0 },
         {  1,  0, -2,  2,  0,      -1.0,    0.0,      0.0,    0.0 },
         {  2,  0,  0,  2,  0,       1.0,    0.0,      0.0,    0.0 },
         {  0,  0,  2,  4,  2,      -1.0,    0.0,      0.0,    0.0 },
         {  0,  1,  0,  1,  0,       1.0,    0.0,      0.0,    0.0 }
      };

      // Number of terms in the series 
      const int NT = (int) (sizeof x / sizeof x[0]);


      // Interval between fundamental epoch J2000.0 and given date (JC). 
      const double t = ( TT - J2000 ) / 86400.0 / DJC;

      // Fundamental arguments 
      // --------------------- 

      // Mean longitude of Moon minus mean longitude of Moon's perigee. 
      double el = normalizeAngle(
         (485866.733 + (715922.633 + (31.310 + 0.064 * t) * t) * t)
         * DAS2R + fmod(1325.0 * t, 1.0) * D2PI);

      // Mean longitude of Sun minus mean longitude of Sun's perigee. 
      double elp = normalizeAngle(
         (1287099.804 + (1292581.224 + (-0.577 - 0.012 * t) * t) * t)
         * DAS2R + fmod(99.0 * t, 1.0) * D2PI);

      // Mean longitude of Moon minus mean longitude of Moon's node. 
      double f = normalizeAngle(
         (335778.877 + (295263.137 + (-13.257 + 0.011 * t) * t) * t)
         * DAS2R + fmod(1342.0 * t, 1.0) * D2PI);

      // Mean elongation of Moon from Sun. 
      double d = normalizeAngle(
         (1072261.307 + (1105601.328 + (-6.891 + 0.019 * t) * t) * t)
         * DAS2R + fmod(1236.0 * t, 1.0) * D2PI);

      // Longitude of the mean ascending node of the lunar orbit on the 
      // ecliptic, measured from the mean equinox of date. 
      double om = normalizeAngle(
         (450160.280 + (-482890.539 + (7.455 + 0.008 * t) * t) * t)
         * DAS2R + fmod(-5.0 * t, 1.0) * D2PI);


      // Nutation series 
      // --------------- 

      // Initialize nutation components. 
      double dp = 0.0;
      double de = 0.0;

      // Sum the nutation terms, ending with the biggest. 
      for (int j = NT-1; j >= 0; j--) 
      {

         // Form argument for current term. 
         double arg = (double)x[j].nl  * el
            + (double)x[j].nlp * elp
            + (double)x[j].nf  * f
            + (double)x[j].nd  * d
            + (double)x[j].nom * om;

         // Accumulate current nutation term. 
         double s = x[j].sp + x[j].spt * t;
         double c = x[j].ce + x[j].cet * t;
         if (s != 0.0) dp += s * std::sin(arg);
         if (c != 0.0) de += c * std::cos(arg);
      }

      // Convert results from 0.1 mas units to radians. 
      dpsi = dp * U2R;
      deps = de * U2R;

   }  // End of 'nutationAngles()'


   double meanObliquity(const CommonTime& TT)
   {
      // Interval between fundamental epoch J2000.0 and given date (JC)
      const double t = ( TT - J2000 ) / 86400.0 / DJC;
      const double t2 = t*t;
      const double t3 = t2*t;

      return (84381.448-46.8150*t-0.00059*t2+0.001813*t3)*DAS2R;
   }

   double iauEqeq94(const CommonTime& TT,double eps,double dPsi)
   {
      // Interval between fundamental epoch J2000.0 and given date (JC). 
      double t = ( TT - J2000 ) / 86400.0 / DJC;

      // Longitude of the mean ascending node of the lunar orbit on the 
      // ecliptic, measured from the mean equinox of date. 
      double om = normalizeAngle((450160.280 + (-482890.539
         + (7.455 + 0.008 * t) * t) * t) * DAS2R
         + fmod(-5.0 * t, 1.0) * D2PI);

      // Nutation components and mean obliquity. 
      //double dpsi(0.0), deps(0.0);
      //nutationAngles(TT, dpsi, deps);

      //double eps0 = meanObliquity(TT);

      // Equation of the equinoxes. 
      double ee = dPsi * std::cos(eps) 
         + DAS2R*(0.00264 * std::sin(om) + 0.000063 * std::sin(om + om));

      return ee;
   }

   double iauGmst82(const CommonTime& UT1)
   {
      // Coefficients of IAU 1982 GMST-UT1 model 
      const double A = 24110.54841  -  86400.0 / 2.0;
      const double B = 8640184.812866;
      const double C = 0.093104;
      const double D =  -6.2e-6;

      // Note: the first constant, A, has to be adjusted by 12 hours 
      // because the UT1 is supplied as a Julian date, which begins  
      // at noon.                                                    

      // Julian centuries since fundamental epoch. 
      double d2 = MJD_TO_JD;
      double d1 = MJD(UT1).mjd;
      double t = ( MJD(UT1).mjd - MJD(J2000).mjd ) / DJC;

      // Fractional part of JD(UT1), in seconds. 
      double f = 86400.0 * (fmod(d1, 1.0) + fmod(d2, 1.0));

      // GMST at this UT1. 
      double gmst = normalizeAngle(
         DS2R * ((A + (B + (C + D * t) * t) * t) + f));

      return gmst;

   }  // End of method 'iauGmst82()'

      // Greenwich mean sidereal time by IAU 2000 model
   double iauGmst00(const CommonTime& UT1,CommonTime TT)
   {

      // TT Julian centuries since J2000.0. 
      double t = ( TT - J2000 ) / 86400.0 / DJC;

      /* Greenwich Mean Sidereal Time, IAU 2000. */
      double gmst = normalizeAngle(earthRotationAngle(UT1) +
         (     0.014506   +
         (  4612.15739966 +
         (     1.39667721 +
         (    -0.00009344 +
         (     0.00001882 )
         * t) * t) * t) * t) * DAS2R);

      return gmst;

   }  // End of method 'ReferenceFrames::iauGmst00()'


   // Nutation matrix from nutation angles
   Matrix<double> iauNmat(const double& eps, const double& dpsi, const double& deps)
   {
      return ( Rx(-(eps+deps)) * Rz(-dpsi) * Rx(eps) );
   }


      // Get earth rotation angle
   double earthRotationAngle(const CommonTime& UT1)
   {
      // IAU 2000 model
      double t = (UT1 - J2000)/86400.0;
         double f = ( fmod(static_cast<double>(MJD(UT1).mjd), 1.0) +
                      fmod(static_cast<double>(MJD_TO_JD), 1.0) );

      double era = normalizeAngle(D2PI*(f+0.7790572732640+0.00273781191135448*t));

      return era;
   }

      /*Earth rotation angle first order rate.
       *  @param mjdTT         Modified Julian Date in TT
       *  @return              d(GAST)/d(t) in [rad]
       */
   double earthRotationAngleRate1(const CommonTime& TT)
   {
      double T = ( TT - J2000 )/86400.0/36525.0;
      double dera = (1.002737909350795 + 5.9006e-11 * T - 5.9e-15 * T * T ) 
         * D2PI / 86400.0;

      return dera;
   }


   
 
} // end namespace gpstk
