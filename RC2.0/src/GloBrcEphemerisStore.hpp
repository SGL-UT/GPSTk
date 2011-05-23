#pragma ident "$Id"

/**
 * @file GloBrcEphemerisStore.hpp
 * Get GLONASS broadcast ephemeris data information
 */

#ifndef GPSTK_GLOBRCEPHEMERISSTORE_HPP
#define GPSTK_GLOBRCEPHEMERISSTORE_HPP

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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2011
//
//============================================================================


#include <iostream>
#include "TabularEphemerisStore.hpp"
#include "GloRecord.hpp"
#include "RinexGloNavData.hpp"
#include "GlonassSatelliteModel.hpp"


namespace gpstk
{

      /** @addtogroup ephemstore */

      //@{

      /**
       * This adds the interface to get GLONASS broadcast ephemeris information
       */
   class GloBrcEphemerisStore : public TabularEphemerisStore<GloRecord>
   {
   public:

         /// Default constructor
      GloBrcEphemerisStore()
         throw()
         : step(1.0), checkHealthFlag(false)
      { TabularEphemerisStore<GloRecord>(); };


         /** Common constructor
          *
          * @param rkStep     Runge-Kutta integration step in seconds.
          * @param checkHealth   Enable or disable the use of the health bit.
          */
      GloBrcEphemerisStore( double rkStep,
                            double checkHealth )
         throw()
         : step(rkStep), checkHealthFlag(checkHealth)
      { TabularEphemerisStore<GloRecord>(); };


         /// Destructor
      virtual ~GloBrcEphemerisStore() {};


         /// Add ephemeris information from a RinexGloNavData object.
      void addEphemeris(const RinexGloNavData& data)
         throw();


         /** Returns the position and clock offset of the indicated
          *  satellite in ECEF coordinates (meters) at the indicated time,
          *  in the PZ-90 ellipsoid.
          * 
          *  @param[in] sat   Satellite's identifier
          *  @param[in] epoch Time to look up
          * 
          *  @return the Xt of the object at the indicated time
          * 
          *  @throw InvalidRequest If the request can not be completed for any
          *  reason, this is thrown. The text may have additional information
          *  as to why the request failed.
          */
      Xt getXt( const SatID& sat,
                const CommonTime& epoch ) const
         throw( InvalidRequest );


         /** Returns the position, velocity and clock offset of the indicated
          *  satellite in ECEF coordinates (meters) at the indicated time,
          *  in the PZ-90 ellipsoid.
          * 
          *  @param[in] sat   Satellite's identifier
          *  @param[in] epoch Time to look up
          * 
          *  @return the Xvt of the object at the indicated time
          * 
          *  @throw InvalidRequest If the request can not be completed for any
          *  reason, this is thrown. The text may have additional information
          *  as to why the request failed.
          */
      Xvt getXvt( const SatID& sat,
                  const CommonTime& epoch ) const
         throw( InvalidRequest );


         /// Get integration step for Runge-Kutta algorithm.
      double getIntegrationStep() const
      { return step; };


         /** Set integration step for Runge-Kutta algorithm.
          *
          * @param rkStep  Runge-Kutta integration step in seconds.
          */
      GloBrcEphemerisStore& setIntegrationStep( double rkStep )
      { step = rkStep; return (*this); };


         /// Get whether satellite health bit will be used or not.
      bool getCheckHealthFlag() const
      { return checkHealthFlag; };


         /** Set whether satellite health bit will be used or not.
          *
          * @param checkHealth   Enable or disable the use of the health bit.
          */
      GloBrcEphemerisStore& setCheckHealthFlag( bool checkHealth )
      { checkHealthFlag = checkHealth; return (*this); };


   private:


         /// Integration step for Runge-Kutta algorithm (10 seconds by default)
      double step;


         /// Flag signaling if satellites will be screened out according to
         /// their health bit (by default it is false)
      bool checkHealthFlag;


   };  // End of class 'GloBrcEphemerisStore'

}  // End of namespace gpstk

#endif   // GPSTK_GLOBRCEPHEMERISSTORE_HPP
