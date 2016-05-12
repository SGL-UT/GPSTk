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
 * @file Rinex3ObsFilterOperators.hpp
 * Operators for FileFilter using Rinex observation data
 */

#ifndef GPSTK_RINEX3OBSFILTEROPERATORS_HPP
#define GPSTK_RINEX3OBSFILTEROPERATORS_HPP

#include <set>
#include <algorithm>

#include "FileFilter.hpp"
#include "Rinex3ObsData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "ObsID.hpp"

namespace gpstk
{
      /// @ingroup FileHandling
      //@{

      /// This compares all elements of the Rinex3ObsData with less than
      /// (only for those fields which the two obs data share).
      /// See code for details on how the comparison works for datasets
      /// that aren't exactly matchted.
      /// It appears this code was written to give a strict ordering
      /// to Rinex3ObsData objects, not to really imply any meaning about
      /// the data.
   struct Rinex3ObsDataOperatorLessThanFull :
      public std::binary_function<Rinex3ObsData, Rinex3ObsData, bool>
   {
   public:
         /// The set is a set of Rinex3ObsType that the two files have in
         /// common.  This is easily generated with the set_intersection
         /// STL function.  See difftools/rowdiff.cpp for an example.
      Rinex3ObsDataOperatorLessThanFull() {};

      bool operator()(const Rinex3ObsData& l, const Rinex3ObsData& r) const
      {
            // compare the times, offsets, then only those elements
            // that are common to both.  this ignores the flags
            // that are set to 0
         if (l.time < r.time)
            return true;
         else if (l.time == r.time)
         {
            if (l.epochFlag < r.epochFlag)
               return true;
            else if (l.epochFlag == r.epochFlag)
            {
               if (l.clockOffset < r.clockOffset)
                  return true;
               else if (l.clockOffset > r.clockOffset)
                  return false;
            }
            else
               return false;
         }
         else
            return false;

            // for the obs, first check that they're the same size
            // i.e. - contain the same number of PRNs
         if (l.obs.size() < r.obs.size())
            return true;

         if (l.obs.size() > r.obs.size())
            return false;

            // then check that each PRN has the same data for each of the
            // fields
         Rinex3ObsData::DataMap::const_iterator lItr;
         for (lItr = l.obs.begin(); lItr != l.obs.end(); lItr++)
         {
            SatID sat = lItr->first;
            Rinex3ObsData::DataMap::const_iterator rItr = r.obs.find(sat);
            if (rItr == r.obs.end())
            {
               std::cout << "not found" << std::endl;
               return false;
            }
            
            std::vector<RinexDatum> lObs = lItr->second,
               rObs = rItr->second;
            
            for(int i = 0; i < lObs.size(); ++i)
            {
               RinexDatum lData, rData;
               lData = lObs[i];
               rData = rObs[i];

               if (lData.data < rData.data)
                  return true;

               if ( lData.lli != 0 && rData.lli != 0 )
                  if (lData.lli < rData.lli)
                     return true;

               if ( lData.ssi != 0 && rData.ssi != 0 )
                  if (lData.ssi < rData.ssi)
                     return true;
            }
         }

            // the data is either == or > at this point
         
         return false;
      }
   };

      /// This is a much faster less than operator for Rinex3ObsData,
      /// only checking time
   struct Rinex3ObsDataOperatorLessThanSimple :
      public std::binary_function<Rinex3ObsData, Rinex3ObsData, bool>
   {
   public:
      bool operator()(const Rinex3ObsData& l, const Rinex3ObsData& r) const
      {
         if (l.time < r.time)
            return true;
         return false;
      }
   };

      /// This simply compares the times of the two records
      /// for equality
   struct Rinex3ObsDataOperatorEqualsSimple :
      public std::binary_function<Rinex3ObsData, Rinex3ObsData, bool>
   {
   public:
      bool operator()(const Rinex3ObsData& l, const Rinex3ObsData& r) const
      {
         if (l.time == r.time)
            return true;
         return false;
      }
   };

      /// Combines Rinex3ObsHeaders into a single header, combining comments
      /// and adding the appropriate Rinex3ObsTypes.  This assumes that
      /// all the headers come from the same station for setting the other
      /// header fields. After running touch() on a list of Rinex3ObsHeader,
      /// the internal theHeader will be the merged header data for
      /// those files and obsSet will be the set of Rinex3ObsTypes that
      /// will be printed to the file.
   struct Rinex3ObsHeaderTouchHeaderMerge :
      public std::unary_function<Rinex3ObsHeader, bool>
   {
   public:
      Rinex3ObsHeaderTouchHeaderMerge()
         : firstHeader(true)
      {}

      bool operator()(const Rinex3ObsHeader& l)
      {
         if (firstHeader)
         {
            theHeader = l;
            firstHeader = false;
         }
         else
         {
            std::set<std::string> commentSet;

               // insert the comments to the set
               // and let the set take care of uniqueness
            std::copy(theHeader.commentList.begin(),
                      theHeader.commentList.end(),
                      std::inserter(commentSet, commentSet.begin()));
            std::copy(l.commentList.begin(),
                      l.commentList.end(),
                      std::inserter(commentSet, commentSet.begin()));
               // then copy the comments back into theHeader
            theHeader.commentList.clear();
            std::copy(commentSet.begin(), commentSet.end(),
                      std::inserter(theHeader.commentList,
                                    theHeader.commentList.begin()));

            Rinex3ObsHeader::RinexObsMap::const_iterator i;
            Rinex3ObsHeader::RinexObsVec::const_iterator j, k;
            Rinex3ObsHeader::RinexObsMap& rom = theHeader.mapObsTypes;
            for( i = l.mapObsTypes.begin(); i != l.mapObsTypes.end(); i++)
            {
               const std::string& sys = i->first;
               const Rinex3ObsHeader::RinexObsVec& rov = i->second;
               if (rom.count(sys) ==0)
                  rom[sys] = Rinex3ObsHeader::RinexObsVec();
               for (j = rov.begin(); j != rov.end(); j++)
               {
                  k = std::find(rom[sys].begin(), rom[sys].end(), *j);
                  if (k == rom[sys].end())
                  {
                     std::cout << "Adding " << *j << std::endl;
                     rom[sys].push_back(*j);
                  }
               }
            }
         }
         return true;
      }

      bool firstHeader;
      Rinex3ObsHeader theHeader;
   };

      //@}

} // namespace gpstk

#endif // GPSTK_RINEX3OBSFILTEROPERATORS_HPP
