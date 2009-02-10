#pragma ident "$Id: SP3Data.hpp 1651 2009-01-29 20:45:29Z ehagen $"

/**
 * @file SP3Data.hpp
 * Encapsulate SP3 file data, including I/O
 */

#ifndef GPSTK_SP3DATA_HPP
#define GPSTK_SP3DATA_HPP

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

#include "SatID.hpp"
#include "SP3Base.hpp"
#include <iomanip>



namespace gpstk
{
   /** @addtogroup SP3ephem */
   //@{

      /**
       * This class models Satellite Position in Three Dimensions.
       * When using this class it's version member needs to be set correctly.
       * This is best done using the header. For example: 
       *
       * @code
       * SP3Stream ss("igr14080.sp3");
       * SP3Header sh;
       * SP3Data sd;
       *
       * ss >> sh;
       * sd.version = sh.version;
       *
       * while (ss >> sd)
       * {
       *    // Interesting stuff...
       * }    
       * @endcode
       *
       * @sa gpstk::SP3Header and gpstk::SP3Stream for more information.
       * @sa petest.cpp for an example.
       */
   class SP3Data : public SP3Base
   {
   public:
         /// Constructor.
      SP3Data() : version('a'),flag('\000'), time(CommonTime::BEGINNING_OF_TIME),
                  clockEventFlag(false),clockPredFlag(false),orbitManeuverFlag(false),
                  orbitPredFlag(false),correlationFlag(false)
         {}
     
         /// Destructor
      virtual ~SP3Data() {}
     
         // The next four lines is our common interface
         /// SP3Data is "data" so this function always returns true.
      virtual bool isData() const {return true;}

         /// Debug output function.
      virtual void dump(std::ostream& s) const;

         ///@name data members
         //@{
      char version; ///< Version of SP3, 'a' or 'c' ONLY
      char flag;    ///< Data type indicator. P for position or V for velocity ONLY
      SatID sat;    ///< Satellite ID
      CommonTime time; ///< Time of epoch for this record
      double x[3];  ///< The three-vector for position | velocity (m | dm/s).
      double clk;   ///< The clock bias or drift for P|V (microsec|1).
      int sig[4];   ///< (c) Four-vector of integer exponents for estimated sigma of
                    ///< position,clock | velocity,clock rate; sigma = base**n
                    ///< (mm,psec | 10^-4 mm/sec,psec/sec); base in header
      bool clockEventFlag; ///< clock event flag, 'E' in file, version c only
      bool clockPredFlag;  ///< clock prediction flag, 'P' in file, version c only
      bool orbitManeuverFlag; ///< orbit maneuver flag, 'M' in file, version c only
      bool orbitPredFlag;  ///< orbit prediction flag, 'P' in file, version c only
      /// data for optional P|V Correlation record, version c only
      bool correlationFlag; ///< If true, on input: a correlation record was read;
                            ///< on output: stream should output correlation.
      unsigned sdev[4];  ///< std dev of 3 positions (XYZ,mm) and clock (psec)
                         ///< or velocities(10^-4 mm/sec) and clock rate (10^-4 ps/s)
      int correlation[6];///< elements of correlation matrix: xy,xz,xc,yz,yc,zc
         //@}
      
   protected:

         /// Writes the formatted record to the FFStream \a s.
         /// @warning This function is currently unimplemented
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);

         /**
          * This function reads a record from the given FFStream.
          * If an error is encountered in retrieving the record, the 
          * stream is reset to its original position and its fail-bit is set.
          * @throws StringException when a StringUtils function fails
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s) 
         throw(std::exception, FFStreamError,
               gpstk::StringUtils::StringException);
   };

   //@}

}  // namespace

#endif
