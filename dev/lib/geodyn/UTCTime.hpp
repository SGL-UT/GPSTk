#pragma ident "$Id: $"

/**
 * @file UTCTime.hpp
 * class to convert among different time systems
 */

#ifndef GPSTK_UTC_TIME_HPP
#define GPSTK_UTC_TIME_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================


#include "DayTime.hpp"

namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{
      
      /** Class to convert UTC Time to all other different time systems.
       *
       * 
       *  The Chart of Different Time System                               
       *-------------------------------------------------------------------
       *
       *          -14s
       *   -----------------> BDT(Compass Time)
       *   |                                                                    
       *   |         +19s             +32.184s           +rel.effects       
       *  GPST ------------> TAI ----------------> TT -----------------> TDB   
       *                        T |                                         
       *           -(UT1-TAI) | |    -leap seconds                          
       *   UT1 ---------------| |--------------------> UTC                
       *    |                                                              
       *    |   earth rotation                                            
       *      ---------------------> GAST                                   
       *                                                                    
       */
   class UTCTime : public DayTime
   {
   public:

         /// Default constructor
      UTCTime(){ setTimeFrame(UTC);};

      UTCTime(DayTime& utc) : DayTime(utc)
      { setTimeFrame(DayTime::UTC); }

      UTCTime(int year,int month,int day,int hour,int minute,double second)
         : DayTime(year, month, day, hour, minute, second, DayTime::UTC)
      {}

      UTCTime(int year,int doy,double sod)
         : DayTime(year, doy, sod, DayTime::UTC)
      {}

      UTCTime(double mjdUTC)
         :DayTime(mjdUTC, DayTime::UTC)
      {}     

         /// Default deconstructor
      virtual ~UTCTime() { };


         // MJD formats

      double mjdUT1(){ return this->asUT1().MJD(); };

      double mjdUTC(){ return this->MJD(); };
      
      double mjdTT(){ return this->asTT().MJD(); };
      
      double mjdTDB(){ return this->asTDB().MJD(); };
       
      double mjdTAI(){ return this->asTAI().MJD(); };
      
      double mjdGPST(){ return this->asGPST().MJD(); };
      

         // JD formats

      double jdUT1(){ return this->asUT1().JD(); };
      
      double jdUTC(){ return this->asUTC().JD(); };
      
      double jdTT(){ return this->asTT().JD(); };
      
      double jdTDB(){ return asTDB().JD(); };
      
      double jdTAI(){ return asTAI().JD(); };
      
      double jdGPST(){ return asGPST().JD(); };


         // convert to different time system

         /// Return UT1 Time
      DayTime asUT1();
         
         /// Return UTC Time
      DayTime asUTC();
         
         /// Return TT Time
      DayTime asTT();
      
         /// Return TDB Time
      DayTime asTDB();
         
         /// Return TAI Time
      DayTime asTAI();
         
         /// Return GPS Time
      DayTime asGPST();
         
         /// Return BD(Compass) Time
      DayTime asBDT();
      
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
   void GPST2UTC(const DayTime& gpst, DayTime& utc);
      
      /// UTC time to GPS time 
      /// @param utc    UTC as input 
      /// @param gpst   GPST as output 
   void UTC2GPST(const DayTime& utc, DayTime& gpst);

      /// UT1 time to UTC time
      /// @param ut1     UT1 as input 
      /// @param utc     UTC as output 
   void UT12UTC(const DayTime& ut1, DayTime& utc);

      /// UTC time to UT1 time
      /// @param utc     UTC as input 
      /// @param ut1     UT1 as output 
   void UTC2UT1(const DayTime& utc, DayTime& ut1);

      /// TT time to UTC time
      /// @param tt      TT as input 
      /// @param utc     UTC as output 
   void TT2UTC(const DayTime& tt, DayTime& utc);

      /// UTC time to TT time
      /// @param UTC     UTC as input 
      /// @param tt      TT as output 
   void UTC2TT(const DayTime& utc, DayTime& tt);

      /// TAI time to UTC time
      /// @param tai     TAI as input 
      /// @param utc     UTC as output 
   void TAI2UTC(const DayTime& tai, DayTime& utc);

      /// UTC time to TAI time
      /// @param utc     UTC as input 
      /// @param tai     TAI as output 
   void UTC2TAI(const DayTime& utc, DayTime& tai);


      /// BDT time to UTC time
      /// @param bdt     BDT as input 
      /// @param utc     UTC as output 
   void BDT2UTC(const DayTime& bdt, DayTime& utc);

      /// UTC time to BDT time
      /// @param utc     UTC as input 
      /// @param bdt     BDT as output 
   void UTC2BDT(const DayTime& utc, DayTime& bdt);


   // @}

}  // End of 'namespace gpstk'

#endif  // GPSTK_UTC_TIME_HPP

