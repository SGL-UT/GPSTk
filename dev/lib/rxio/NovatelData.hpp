#pragma ident "$Id$"


/**
 * @file NovatelData.hpp
 * gpstk::NovatelData - container for Novatel data, with methods to convert to Rinex
 */

#ifndef GPSTK_NOVATEL_DATA_HPP
#define GPSTK_NOVATEL_DATA_HPP

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






#include <ostream>
#include "Exception.hpp"
#include "FFStream.hpp"
#include "RinexNavData.hpp"
#include "RinexObsData.hpp"
#include "NovatelStream.hpp"

namespace gpstk
{
   /** @addtogroup NovatelGroup */
   //@{

   /// Read Novatel binary records and parse into Rinex. Currently supports
   /// the basic data records of OEM2 and OEM4 files only; others are yet to be
   /// implemented.
   /// Ref. (OEM2) 'GPSCard Command Descriptions' Manual (Rev 3)
   /// Ref. 'OEM4 Family of Receivers,' Users Manual Volume 2 (OM-20000047 Rev 12)
   class NovatelData : public FFData
   {
   public:
         /// block types implemented here
      enum RecType
      {
         Unknown=0,  ///< unknown block type
            // OEM2 records
         RGEB,       ///< observations -- not implemented
         RGEC,       ///< observations
         POSB,       ///< position solution -- not implemented
         REPB,       ///< ephemeris
         RCSB,       ///< receiver stats -- not implemented
            // OEM4 records
         RANGE,      ///< range and phase data (synchronous)
         RANGECMP,   ///< range and phase data, compressed (synchronous)
         RAWEPHEM    ///< ephemeris data (asynchronous)
      };

         /// Names of the record types : RecNames[rectype]
      static const std::string RecNames[];

         /// Default constructor
      NovatelData(void) : rectype(Unknown), datasize(0), headersize(0), gpsWeek(-1)
         {}

         /// Destructor
      virtual ~NovatelData(void) {}

         /// Return true if this is a valid Novatel record.
         /// Test the validity of the record with this before further processing.
      bool isValid(void) const;

         /// This class is not header
      virtual bool isHeader(void) const { return false; }

         /// This class is data
      virtual bool isData(void) const { return true; }

         /// True if this record is an ephemeris record
         /// Test the identity of the record with this before casting into Rinex.
      bool isNav(void) const;

         /// True if this record is observation data
         /// Test the identity of the record with this before casting into Rinex.
      bool isObs(void) const;

         /// True if this record is auxiliary data (not Nav, not Obs)
         /// (only one of isNav(), isObs() and isAux() is true
      bool isAux(void) const;

         /// True if this record belongs to OEM2 receivers
      bool isOEM2(void) const;

         /// True if this record belongs to OEM4 receivers
      bool isOEM4(void) const;

         /// Dump the contents of the record to the ostream \c str.
      virtual void dump(std::ostream& str) const;

         /// set the week number of the data, this is required for
         /// OEM2 nav records that are processed before any obs records
      void setWeek(long& gpsweek) { gpsWeek = gpsweek; }

         /// cast *this into an gpstk::RinexNavData.
         /// @throw if the record is invalid or not an ephemeris (isNav()==false)
      operator RinexNavData() throw(gpstk::Exception);

         /// cast *this into a gpstk::RinexObsData
         /// @throw if the record is invalid or not an observation (isObs()==false)
      operator RinexObsData() throw(gpstk::Exception);

         /// public data members
      RecType rectype;              ///< record type (cf. enum RecType)
      int recnum;                   ///< record number (byte 4 of record)
      long datasize;                ///< size of data in bytes
      int headersize;               ///< size of header in bytes (=3 for OEM2)

   protected:
         /// Write this record to the stream \a s.
      virtual void reallyPutRecord(FFStream& s) const 
         throw(std::exception, gpstk::StringUtils::StringException, 
               gpstk::FFStreamError);

         /**
          * Read a NovatelData record from the FFStream \c s. 
          * If an error is encountered, the function will 
          * return the stream to its original state and mark its fail-bit.
          * @throws FFStreamError when exceptions(failbit) is set and
          *  a read or formatting error occurs.  This also resets the
          *  stream to its pre-read position.
          */
      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, gpstk::StringUtils::StringException, 
               gpstk::FFStreamError);

   private:
         /// private data members
         /// header length(28) + data length(max 65536) = 65564
      unsigned char buffer[65564];   ///< buffer for raw data

         /// Reference GPS week, for OEM2, where the nav records require a GPS week,
         /// but only the obs records have one, and then it is 10-bit.
         /// This epoch will be used to remove the ambiguity in the 10-bit week
         /// number of the obs records, and then will provide a week number for
         /// for the nav records. If not set by the user, it will be set by the
         /// system time, and then by the first obs record.
      long gpsWeek;
      
   }; // end class NovatelData

   //@}

}  // end namespace gpstk

#endif
