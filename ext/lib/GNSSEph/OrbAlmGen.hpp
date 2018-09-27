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

/**
  *  @file OrbAlmGen.hpp
  *  Contains the "engineering units" contents of a almanac that uses 
  *  pseudo-Keplerian elements as its parameters.  This include GPS LNAV,
  *  GPS CNAV (Midi), BeiDou (both D1 and D2), and IRNSS
  *  The same algorithm is used across all these systems. therefore, it
  *  is practical to roll them all into one class.  Several of the members
  *  in OrbElem are not used by the almanac formats.  These are all set
  *  to zero by convention. 
  */ 

#ifndef GPSTK_ORBALMGEN_HPP
#define GPSTK_ORBALMGEN_HPP

#include <string>
#include <iostream>

#include "OrbAlm.hpp"

namespace gpstk
{
   class OrbAlmGen : public OrbAlm
   {
   public:
         /// Default constructor
      OrbAlmGen();

      OrbAlmGen( const PackedNavBits& pnb,
                    const unsigned short hArg = 0 )
         throw( InvalidParameter );

         /// Destructor
      virtual ~OrbAlmGen() {}

        /// Clone method
      virtual OrbAlmGen* clone() const;

         /**  Load an existing object from a PackedNavBits object.
           *  @throw InvalidParameter if the data are not consistent.
           *  For GPS, the health argument (hArg) will be ignored
           *  and the health in the msg will be used.   However, 
           *  this is the only source of health information
           *  for BDS.    */ 
      void loadData(const gpstk::PackedNavBits& msg,
                    const unsigned short hArg = 0)
         throw(gpstk::InvalidParameter);

       //------------------------------------------------------------
      //  When the calling program receives the following pages, 
      //  this should be called to update the almanac time parameters.   
      //    LNAV SF4/Pg 25  
      //    BDS D1 SF5/Pg 8
      //    BDS D2 SF5/Pg 36
      //  This "maintenance" is not required for GPS CNAV
      static void loadWeekNumber(const CommonTime& ct);
      static void loadWeekNumber(const unsigned int WNa, const double toa);

      virtual std::string getName() const
      {
         return "OrbAlmGen";
      }
 
      virtual std::string getNameLong() const
      {
         return "SV Almanac Orbit";
      }

        /** Compute the satellite clock bias (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockBias(const gpstk::CommonTime& t) const
                     throw(gpstk::InvalidRequest);

         /** Compute the satellite clock bias (meters) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockBiasM(const gpstk::CommonTime& t) const
                     throw(gpstk::InvalidRequest);

         /** Compute the satellite clock drift (sec/sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svClockDrift(const gpstk::CommonTime& t) const 
                     throw(gpstk::InvalidRequest);


         /** Compute satellite position at the given time
          * using this orbit data.
          * @throw Invalid Request if the required data has not been stored.
          */
      virtual gpstk::Xvt svXvt(const gpstk::CommonTime& t) const
                        throw(gpstk::InvalidRequest);

         /** Compute satellite relativity correction (sec) at the given time
          *  @throw Invalid Request if the required data has not been stored.
          */
      virtual double svRelativity(const gpstk::CommonTime& t) const
                     throw(gpstk::InvalidRequest);

      virtual bool isSameData(const OrbElemBase* right) const;      

      virtual std::string listDifferences(const OrbElemBase* right) const;      

      virtual void dumpBody(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest );

      virtual void dumpFooter(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest ) {}

         /** Generate a formatted human-readable output of the entire contents of
          *  this object and send it to the designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */
      virtual void dump(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest );  

         /** Generate a formatted human-readable one-line output that summarizes
          *  the critical times associated with this object and send it to the
          *  designated output stream (default to cout).
          *  @throw Invalid Parameter if the object has been instantiated, but not loaded.
          */   
      virtual void dumpTerse(std::ostream& s = std::cout) const
         throw( gpstk::InvalidRequest );     
         //@}
 
         // GPS LNAV, GPS CNAV, BDS D1/D2, and IRNSS all have the following
         // almanac members in common. 
      double          AHalf;       // sqrt A
      double              A;       // semi-major axis
      double            af1;
      double            af0;
      double         OMEGA0;
      double            ecc;
      double         deltai;
      double             i0;       // Inclination (radians)
      double       OMEGAdot;
      double              w;
      double             M0;
      unsigned long     toa; 

         // For GPS, health is provided in the page containing the orbit data.
         // For BDs, health is supplied from Sf5 pg 7 or pg 8
         // For IRNSS, TBD
      unsigned short health;

   protected:
      static const unsigned long ALMANAC_PERIOD_LNAV;
      static const unsigned long FRAME_PERIOD_LNAV;

         // The following variables record the most recently seen
         // WNa/toa from either GPS LNAV SF5/p25 or the BDS equivalent.
         // Since there is no guarantee that this information will appear
         // BEFORE the first almanac data page, there is also provision
         // for estimating these values from the first available data. 
      static bool WN_set; 
      static unsigned int WNa_full;
      static double t_oa;

      void loadDataGpsLNAV(const gpstk::PackedNavBits& msg)
                throw(gpstk::InvalidParameter);
      void loadDataGpsCNAV(const gpstk::PackedNavBits& msg)
                throw(gpstk::InvalidParameter);
      void loadDataBDS(const gpstk::PackedNavBits& msg,
                    const unsigned short hArg = 0)
                throw(gpstk::InvalidParameter);
      void loadDataIRN(const gpstk::PackedNavBits& msg,
		       const unsigned short hArg = 0)
                throw(gpstk::InvalidParameter);

         // Used internally during startup if WNa has not been received
         // prior to first almanac data page.         
      void estimateWeekNumber(const CommonTime& currTime);
      void estimateWeekNumberBDS(const CommonTime& currTime);


         // Only used for BDS
      unsigned short translateToSubjectPRN(const bool isD1,
                                           const unsigned short subframe,
                                           const unsigned short page) const;

   }; // end class OrbAlmGen

   //@}

   std::ostream& operator<<(std::ostream& s, 
                                     const OrbAlmGen& eph);

} // end namespace 

#endif // GPSTK_ORBALMGEN_HPP
