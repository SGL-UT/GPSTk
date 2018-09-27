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

/// @file RationalizeRinexNav.hpp
/// Reads RinexNavData or Rinex3Navdata file (or files).  Stores the records 
/// internally as Rinex3NavData objects.   Once reading is complete, the 
/// stored objects are sanity-checked to resolve common problems in IGS brdc 
/// aggregated navigation message data files.   The results can then be loaded
/// into a OrbEphStore or OrbElemStore object.  The rationalization process creates
/// a log of all changes made in the process of rationalizing the data set. 
///
///
#ifndef GPSTK_RATIONALIZERINEXNAV_HPP
#define GPSTK_RATIONALIZERINEXNAV_HPP

#include <iostream>
#include <list>
#include <map>
#include <string>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "OrbitEphStore.hpp"
#include "OrbElemStore.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3NavHeader.hpp"
#include "SatID.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /**
       * This class models a RINEX 3 Nav record.
       *
       * \sa FunctionalTests/Rinex3NavText for example.
       * \sa Rinex3NavHeader and Rinex3NavStream classes.
       */
   class RationalizeRinexNav
   {
   public:
         /// Constructor
      RationalizeRinexNav(void) {}

         /// Destructor
      virtual ~RationalizeRinexNav() {}

         // Attempt to read a file (assumed ot the Rinex or Rinex 3 navgation data)
         // and load the data into memory.   This method may be called multiple times.
      bool inputFile(const std::string inFileName); 

         // Generate a new Rinex 3 Nav file containing the same header 
         // and all the items in the original, but corrected.
      bool writeOutputFile(const std::string inFileName,
                           const std::string outFileName,
                           const std::string progName,
                           const std::string agencyName="") const;

         // Examine the stored data, attempt to find any data sets
         // that are labeled with the incorrect PRN.  Remove
         // such data sets.
      void removeMisTaggedDataSets()
         throw(InvalidRequest);

         // Return a bit-encoded count of the number of 
         // parameters that are NOT equal.
      unsigned long countUnequal(const Rinex3NavData& left,
                                 const Rinex3NavData& right);

         // Examine and stored data, fix the transmit times.
         // Throws an error if it encounters data that cannot be interpreted.
      void rationalize()
         throw(InvalidRequest);

         // Dump summary of contents of store
      void dump(std::ostream& out=std::cout) const;

         // Dump a summary of the actions taken by the rationalize( ) method.
      void outputActionLog(std::ostream& out=std::cout) const;

         // Claer all data and all intermediate results from this object
      void clear(); 

      bool loadStore(OrbitEphStore& oes);
      bool loadStore(OrbElemStore& oes);
      std::string getLoadErrorList() const;
      unsigned long getNumLoaded() const;

   protected:
      std::string strSumm(const Rinex3NavData& r3nd) const;
      void addLog(const SatID& sidr, const CommonTime& ct, const std::string& s);

      static bool compXmitTimes(const Rinex3NavData& left, const Rinex3NavData& right); 
      static CommonTime formXmitTime(const Rinex3NavData& r3nd);

         // List of all Rinex Nav Data objects for a single SV
         // When first created, the list is in the order in which the data were
         // present in the file.  It is re-sorted as the process executes
         // and some transmit times are adjusted. 
      typedef std::list<Rinex3NavData> NAV_DATA_LIST;

         // Key is the satellite that transmitted the data.
         // The value is the list of objects transmitted by that SV.
      typedef std::map<SatID, NAV_DATA_LIST> SAT_NAV_DATA_LIST;
      SAT_NAV_DATA_LIST sndl;

         // Map of all actions in which data were modified.  The 
         // map is for a single SV.
         // Key is the Toe of the subject nav data set.  The string is
         // a description of the action taken wrt to that object.
      typedef std::map<CommonTime, std::string> LOG_DATA_LIST;

         // Key is the satellite that transmitted the data.
         // The value is a map of the actions associated with 
         // changes to the data set for that satellite.
      typedef std::map<SatID, LOG_DATA_LIST> SAT_LOG_DATA_LIST;
      SAT_LOG_DATA_LIST sldl;

      unsigned long mostRecentLoadCount;
      std::string mostRecentLoadErrors;

         // Variables that track file-specific information
      std::map<std::string,Rinex3NavHeader> rnhMap;

      typedef std::pair<SatID, CommonTime> NAV_ID_PAIR;
      typedef std::list<NAV_ID_PAIR> TOC_LIST;
      std::map<std::string,TOC_LIST> listOfTocsByFile;

   }; // End of class 'RationalizeRinexNav'

}  // End of namespace gpstk

#endif   // GPSTK_RATIONALIZERINEXNAV_HPP
