#pragma ident "$Id$"

/**
 * @file RinexGloNavData.hpp
 * Encapsulate Rinex GLONASS navigation data
 */

#ifndef GPSTK_RINEXGLONAVDATA_HPP
#define GPSTK_RINEXGLONAVDATA_HPP

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


#include <string>

#include "StringUtils.hpp"
#include "CommonTime.hpp"
#include "FFStream.hpp"
#include "RinexNavBase.hpp"
#include "SatID.hpp"

namespace gpstk
{
   /** @addtogroup RinexNav */

   //@{

      /**
       * This class models a RINEX Nav GLONASS data record.
       *
       * \sa RinexGloNavHeader and RinexGloNavStream classes.
       */

   class RinexGloNavData : public RinexNavBase
   {
   public:

         /**
          * Constructor
          * @warning CHECK THE PRNID TO SEE IF THIS DATA IS VALID BEFORE USING!!
          */
      RinexGloNavData(void)
        : time(CommonTime::BEGINNING_OF_TIME), PRNID(-1)
      {}


         /// Destructor
      virtual ~RinexGloNavData() {}


         /// RinexGloNavData is "data" so this function always returns true.
      virtual bool isData(void) const
      { return true; }


         /** A debug output function.
          *  Prints the PRN id and the IODC for this record.
          */ 
      virtual void dump( std::ostream& s ) const;


         /// @name EpochDataGLO

         //@{
            
      CommonTime time;     ///< Time according to the record
      short PRNID;         ///< SV PRN ID
      SatID sat;           ///< SatID (from PRNID & satSys)
      short health;        ///< SV health
      double  TauN;        ///< SV clock bias (sec)
      double  GammaN;      ///< SV relative frequency bias
      short   MFtime;      ///< Message frame time (sec of UTC day, tk)
      short   freqNum;     ///< Frequency number (R2.1=1..24; R2.11=-7..+13)
      double  ageOfInfo;   ///< Age of oper. information (days)

         //@}

         /// @name TabularEphemerisParameters

         //@{

      double  px, py, pz;  ///< SV position
      double  vx, vy, vz;  ///< SV velocity
      double  ax, ay, az;  ///< SV acceleration

         //@}


   private:


         /// Parses string \a currentLine to obtain PRN id and epoch.
      void getPRNEpoch( const std::string& currentLine )
         throw( StringUtils::StringException, FFStreamError );


         /** @name OrbitParameters
          * Obtain orbit parameters from strint \a currentLine.
          */

         //@{

         /// Reads line 1 of the Nav Data record
      void getBroadcastOrbit1( const std::string& currentLine )
         throw( StringUtils::StringException, FFStreamError );

         /// Reads line 2 of the Nav Data record
      void getBroadcastOrbit2( const std::string& currentLine )
         throw( StringUtils::StringException, FFStreamError );

         /// Reads line 3 of the Nav Data record
      void getBroadcastOrbit3( const std::string& currentLine )
         throw( StringUtils::StringException, FFStreamError );

         //@}

         /// Generates a line to be output to a file for the PRN/epoch line
      std::string putPRNEpoch(void) const
         throw( StringUtils::StringException );


         /** @name OrbitParameters
          * Generate orbit parameter lines from data to be output to a file
          */

         //@{

         /// Writes line 1 of the Nav Data record
      std::string putBroadcastOrbit1(void) const
         throw( StringUtils::StringException );

         /// Writes line 2 of the Nav Data record
      std::string putBroadcastOrbit2(void) const
         throw( StringUtils::StringException );

         /// Writes line 3 of the Nav Data record
      std::string putBroadcastOrbit3(void) const
         throw( StringUtils::StringException );

         //@}


   protected:


         /// Outputs the record to the FFStream \a s.
      virtual void reallyPutRecord( FFStream& s ) const 
         throw( std::exception, FFStreamError, StringUtils::StringException );


         /** This function retrieves a RINEX 3 NAV record from the given
          *  FFStream. If an error is encountered in reading from the stream,
          *  the stream is returned to its original position and its fail-bit
          *  is set.
          *
          * @throws StringException when a StringUtils function fails.
          * @throws FFStreamError when exceptions(failbit) is set and
          *    a read or formatting error occurs.  This also resets the
          *    stream to its pre-read position.
          */
      virtual void reallyGetRecord( FFStream& s )
         throw( std::exception, FFStreamError, StringUtils::StringException );


   }; // End of class 'RinexGloNavData'

   //@}

}  // End of namespace gpstk

#endif   // GPSTK_RINEXGLONAVDATA_HPP
