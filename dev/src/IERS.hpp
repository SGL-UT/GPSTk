#pragma ident "$Id$"

/**
 * @file IERS.hpp
 * This class ease handling IERS earth orientation data.
 */

#ifndef GPSTK_IERS_HPP
#define GPSTK_IERS_HPP

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
//  Wei Yan - Chinese Academy of Sciences . 2010
//
//============================================================================

#include <string>
#include "IERSConventions.hpp"


namespace gpstk
{
      /** @addtogroup GeoDynamics */
      //@{

      /**
       * Class to easy handle IERS Earth Orientation Data globally.
       */
   class IERS
   {
   public:
         /// PI
      static const double PI;

         /// Arcseconds to radius convention 
      static const double ARCSEC2RAD;

         /// 'finals.data' from http://maia.usno.navy.mil/
      static void loadIERSFile(const std::string& iersFile)
         throw(FileMissingException);

         /// ERP data file from IGS
      static void loadIGSFile(const std::string& igsFile)
         throw(FileMissingException);

      
         /// ERP data file from STK
      static void loadSTKFile(const std::string& stkFile)
         throw(FileMissingException);

        
         /// Pole coordinate [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return Pole coordinate x in arcseconds
      static double xPole(double mjdUTC)
         throw (InvalidRequest);
      
         /// Pole coordinate [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return Pole coordinate x in arcseconds
      static double yPole(double mjdUTC)
         throw (InvalidRequest);

         /// UT1-UTC time difference [seconds]
         /// @param  Modified Julidate in UTC
         /// @return UT1-UTC time difference in seconds
      static double UT1mUTC(double mjdUTC)
         throw (InvalidRequest); 

      
         /// Nutation dPsi [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return dPsi in arcseconds
      static double dPsi(double mjdUTC)
         throw (InvalidRequest);


         /// Nutation dEps [arcseconds]
         /// @param  Modified Julidate in UTC
         /// @return dEps in arcseconds
      static double dEps(double mjdUTC)
         throw (InvalidRequest);


         /** Return the difference between TAI and UTC (known as leap seconds).
          * Values from the USNO website: ftp://maia.usno.navy.mil/ser7/leapsec.dat
          * As of July 19, 2002, no leap second in Dec 2002 so next opportunity for
          * adding a leap second is July 2003. Check IERS Bulletin C.
          * http://hpiers.obspm.fr/eoppc/bul/bulc/UTC-TAI.history
          *
          * @param mjd   Modified Julian Date in UTC
          * @return      number of leaps seconds.
         */
      static int TAImUTC(const double& mjdUTC);

         /// Transform GPS Time to UTC Time
      static DayTime GPST2UTC(DayTime gpst);


   protected:
      IERS() {}
      ~IERS() {}
      
   }; // End of class 'IERS'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_IERS_HPP


