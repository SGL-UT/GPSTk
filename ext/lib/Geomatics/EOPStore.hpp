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

/// @file EOPStore.hpp
/// class gpstk::EOPStore encapsulates input, storage and retreval of
/// Earth Orientation Parameters (EOPs - cf. class EarthOrientation).

#ifndef CLASS_EARTHORIENTSTORE_INCLUDE
#define CLASS_EARTHORIENTSTORE_INCLUDE

//------------------------------------------------------------------------------------
// system includes
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
// GPSTk
#include "Exception.hpp"
#include "IERSConvention.hpp"
#include "EOPPrediction.hpp"
#include "EarthOrientation.hpp"

//------------------------------------------------------------------------------------
namespace gpstk {

   /// Earth orientation parameter store. Store EarthOrientation objects in a map
   /// with key = integer MJD at which the EOPs are computed. Access the store
   /// with any MJD(UTC), interpolating the stored EOPs to the given epoch using the
   /// algorithm in class EarthOrientation.
   class EOPStore
   {
      /// key is MJD at which the Earth orientation parameters apply
      std::map<int, EarthOrientation> mapMJD_EOP;

      /// first and last times in the store, -1 if store is empty.
      int begMJD,endMJD;
   
   public:
      /// Constructor
      EOPStore() : begMJD(-1), endMJD(-1) { }

      /// Add to the store directly
      void addEOP(int MJD, EarthOrientation& eop)
         throw();

      /// Add to the store by computing using an EOPPrediction
      /// @param MJD integer MJD at which to add EOPs
      /// @return non-0 if MJD is outside range
      /// @throw if MJD is invalid
      int addEOP(int MJD, EOPPrediction& eopp)
         throw(Exception);

      /// Add EOPs to the store via an input file: either an EOPP file
      /// or a flat file produced by the IERS and available at USNO
      /// (see http://maia.usno.navy.mil/ and get either file
      /// 'finals.data' or 'finals2000A.data').
      /// @param filename Name of file to read, including path.
      /// @throw if file is not found.
      void addFile(const std::string& filename)
         throw(FileMissingException);

      /// Add EOPs to the store via an EOPP file using class EOPPrediction.
      /// Read the EOPP file and compute EOPs for all days within the valid range.
      /// @param filename Name of file to read, including path.
      /// @throw if file is not found.
      void addEOPPFile(const std::string& filename)
         throw(FileMissingException);

      /// Add EOPs to the store via a flat IERS file; e.g. finals2000A.data from USNO.
      /// @param filename Name of file to read, including path.
      /// @throw if file is not found.
      void addIERSFile(const std::string& filename)
         throw(FileMissingException);

      /// Edit the store by deleting all entries before(after)
      /// the given min(max) MJDs (TimeSystem UTC).
      /// If mjdmin is later than mjdmax, the two are switched.
      /// @param mjdmin integer MJD desired earliest store time.
      /// @param mjdmax integer MJD desired latest store time.
      void edit(int mjdmin, int mjdmax)
         throw();

      /// return the number of entries in the store
      int size(void) throw()
         { return mapMJD_EOP.size(); }

      /// clear the store
      void clear(void) throw()
         { mapMJD_EOP.clear(); begMJD = endMJD = -1; }

      /// Dump the store to cout.
      /// @param detail determines how much detail to include in the output
      ///  0 start and stop times (MJD), and number of EOPs.
      ///  1 list of all times and EOPs.
      void dump(short detail=0, std::ostream& s=std::cout) const
         throw();

      /// Return first time (MJD) in the store.
      int getFirstTimeMJD(void) throw()
         { return begMJD; }

      /// Return last time (MJD) in the store.
      int getLastTimeMJD(void) throw()
         { return endMJD; }

      /// Get the EOP at the given epoch. This involves interpolation and corrections
      /// as prescribed by the appropriate IERS convention, using code in class
      /// EarthOrientation. This routine pulls data from the map for 4 entries
      /// surrounding the input time; this array of data is passed to class
      /// EarthOrientation to perform the interpolation and corrections.
      /// @param mjd MJD(UTC) time of interest
      /// @param conv IERSConvention to be used.
      /// @throw InvalidRequest if the integer MJD falls outside the store,
      ///   or if the store contains fewer than 4 entries
      /// @return EarthOrientation EOPs at mjd.
      EarthOrientation getEOP(const double& mjd, const IERSConvention& conv)
         throw(InvalidRequest);

   };    // end class EOPStore

}  // end namespace gpstk

#endif   // CLASS_EARTHORIENTSTORE_INCLUDE
