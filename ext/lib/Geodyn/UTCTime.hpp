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
 * @file UTCTime.hpp
 * class to convert among different time systems
 */

#ifndef GPSTK_UTC_TIME_HPP
#define GPSTK_UTC_TIME_HPP

#include "CommonTime.hpp"
#include "YDSTime.hpp"
#include "CivilTime.hpp"
#include "Epoch.hpp"
#include "TimeSystem.hpp"
namespace gpstk
{
      /// @ingroup GeoDynamics 
      //@{
      
      /** Class to convert UTC Time to all other different time systems.
       *
       * 
       *  The Chart of Different Time System                               
       *-------------------------------------------------------------------
       *
       *          -14s
       *    -----------------> BDT(Compass Time)
       *    |                                                                    
       *    |         +19s             +32.184s           +rel.effects       
       *   GPST ------------> TAI ----------------> TT -----------------> TDB   
       *                      T |                                         
       *           -(UT1-TAI) | |    -leap seconds                          
       *   UT1 ---------------| |--------------------> UTC                
       * 
       *    |   earth rotation                                            
       *    ---------------------> GAST                                   
       *                                                                    
       */
   class UTCTime : public CommonTime
   {
   public:

         /// Default constructor
      UTCTime(){TimeSystem(UTC);}

      UTCTime(CommonTime& utc) : CommonTime(utc)
      {TimeSystem(UTC); }

      UTCTime(int year,int month,int day,int hour,int minute,double second)
          {CivilTime(year, month, day, hour, minute, second);}
      

      UTCTime(int year,int doy,double sod)
          {YDSTime(year, doy, sod);}
      

      UTCTime(double mjdUTC)
         {set(mjdUTC, 0,TimeSystem::UTC);}
           

         /// Default deconstructor
      virtual ~UTCTime()
	throw()
	 { };


         // MJD formats

      double mjdUT1(){ return this->asUT1().MJD(); }

      double mjdUTC(){ return this->asUTC().MJD(); };

      double mjdTT() { return this->asTT().MJD(); };
      
      double mjdTDB(){ return this->asTDB().MJD(); };
       
      double mjdTAI(){ return this->asTAI().MJD(); };
      
      double mjdGPST(){ return this->asGPST().MJD(); };
      

         // JD formats

      double jdUT1(){ return this->asUT1().JD(); };
      
      double jdUTC(){ return this->asUTC().JD(); };

      double jdTT() { return this->asTT().JD(); };
      
      double jdTDB(){ return this->asTDB().JD(); };
      
      double jdTAI(){ return this->asTAI().JD(); };
      
      double jdGPST(){ return this->asGPST().JD(); };


         // convert to different time system

         /// Return UT1 Time
      Epoch asUT1();
         
         /// Return UTC Time
      Epoch asUTC();
         
	/// Return TT Time
	Epoch asTT();      

        /// Return TDB Time
      Epoch asTDB();
         
         /// Return TAI Time
      Epoch asTAI();
         
         /// Return GPS Time
      Epoch asGPST();
         
         /// Return BD(Compass) Time
      Epoch asBDT();
      
         // EOP data

      double xPole();

      double yPole();
      
      double UT1mUTC();
      

      double TAImUTC();
      
      double GPSTmUTC();
         
         /// some test
      void test();

   protected:

         /**
          * Computes the cumulative relativistic time correction to
          * earth-based clocks, TDB-TDT, for a given time. Routine
          * furnished by the Bureau des Longitudes, modified by
          * removal of terms much smaller than 0.1 microsecond.
          * @param jdno.  Julian day number of lookup
          * @param fjdno. Fractional part of Julian day number
          * @return       Time difference TDB-TDT (seconds)
          */
      double ctatv(long jdno, double fjdno);

         /// Handy struct
      struct MJDTime 
      {
         long      MJDint;
         double   MJDfr;
      };

   public:

         /// Seconds per day.
      static const double DAY_TO_SECOND;      // = 86400; 
         
         /// Constant used for conversion to Terrestrial Time.
      static const double TT_TAI;           // = 32.184;  // constant
         
         /// Constant used for conversion to GPS time.
      static const double TAI_GPS;           // = 19.0;  // constant 

   }; // End of class 'UTCTime'

      
      /// GPS time to UTC time
      /// @param gpst    GPST as input 
      /// @param utc     UTC as output 
   void GPST2UTC(const CommonTime& gpst, UTCTime& utc);
      
      /// UTC time to GPS time 
      /// @param utc    UTC as input 
      /// @param gpst   GPST as output 
   void UTC2GPST(const UTCTime& utc, CommonTime& gpst);

      /// UT1 time to UTC time
      /// @param ut1     UT1 as input 
      /// @param utc     UTC as output 
   void UT12UTC(const UTCTime& ut1, UTCTime& utc);

      /// UTC time to UT1 time
      /// @param utc     UTC as input 
      /// @param ut1     UT1 as output 
   void UTC2UT1(const UTCTime& utc, UTCTime& ut1);

      /// TT time to UTC time
      /// @param tt      TT as input 
      /// @param utc     UTC as output 
   void TT2UTC(const CommonTime& tt, UTCTime& utc);

      /// UTC time to TT time
      /// @param UTC     UTC as input 
      /// @param tt      TT as output 
   void UTC2TT(const UTCTime& utc, CommonTime& tt);

      /// TAI time to UTC time
      /// @param tai     TAI as input 
      /// @param utc     UTC as output 
   void TAI2UTC(const CommonTime& tai, UTCTime& utc);

      /// UTC time to TAI time
      /// @param utc     UTC as input 
      /// @param tai     TAI as output 
   void UTC2TAI(const UTCTime& utc, CommonTime& tai);


      /// BDT time to UTC time
      /// @param bdt     BDT as input 
      /// @param utc     UTC as output 
   void BDT2UTC(const CommonTime& bdt, UTCTime& utc);

      /// UTC time to BDT time
      /// @param utc     UTC as input 
      /// @param bdt     BDT as output 
   void UTC2BDT(const UTCTime& utc, CommonTime& bdt);


   // @}

}  // End of namespace 'gpstk'

#endif  // GPSTK_UTC_TIME_HPP
