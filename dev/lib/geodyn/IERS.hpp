#pragma ident "$Id: $"

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
//  Wei Yan - Chinese Academy of Sciences . 2009, 2010
//
//============================================================================

#include <string>
#include "EarthOrientation.hpp"

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
         /// Arcseconds to radius convention 
      static const double ARCSEC2RAD;

         /** Add EOPs to the store via a flat IERS file. 
          *  get finals.data from http://maia.usno.navy.mil/
          *
          *  @param iersFile  Name of file to read, including path.
          */
      static void loadIERSFile(std::string iersFile)
         throw(FileMissingException);
      
         /** Add EOPs to the store via a flat STK file. 
          *  EOP-v1.1.txt
          *  http://celestrak.com/SpaceData/EOP-format.asp
          *
          *  @param stkFile  Name of file to read, including path.
          */
      static void loadSTKFile(std::string stkFile)
         throw(FileMissingException);

         /// UT1-UTC time difference [seconds]
         /// @param  Modified Julidate in UTC
         /// @return UT1-UTC time difference in seconds
      static double UT1mUTC(double mjdUTC)
         throw (InvalidRequest); 

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

      void test();

   protected:
      
         /// Default constructor
      IERS() { }
         
         /// Default destructor
      ~IERS() { }
      
         /// Object to hold EOP data
      static EOPStore eopStore;

   }; // End of class 'IERS'

      // @}

}  // End of namespace 'gpstk'

#endif   // GPSTK_IERS_HPP


