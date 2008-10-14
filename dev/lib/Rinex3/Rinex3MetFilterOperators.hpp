#pragma ident "$Id$"



/**
 * @file Rinex3MetFilterOperators.hpp
 * Operators for FileFilter using Rinex3 meteorological data
 */

#ifndef GPSTK_RINEX3METFILTEROPERATORS_HPP
#define GPSTK_RINEX3METFILTEROPERATORS_HPP

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






#include "CommonTime.hpp"
#include "FileFilter.hpp"
#include "Rinex3MetData.hpp"
#include "Rinex3MetHeader.hpp"

#include <set>

namespace gpstk
{
   /** @addtogroup Rinex3Met */
   //@{

   typedef std::unary_function<gpstk::Rinex3MetHeader, bool> Rinex3MetDataUnaryOperator;
   typedef std::binary_function<gpstk::Rinex3MetData, gpstk::Rinex3MetData, bool> Rinex3MetDataBinaryOperator;

      /// This compares all elements of the Rinex3MetData with less than
      /// (only for those fields which the two obs data share).
   struct Rinex3MetDataOperatorLessThanFull : 
      public Rinex3MetDataBinaryOperator
   {
   public:
         /// The set is a set of Rinex3MetType that the two files have in 
         /// common.  This is easily generated with the set_intersection
         /// STL function.  See difftools/rmwdiff.cpp for an example.
      Rinex3MetDataOperatorLessThanFull
      (const std::set<gpstk::Rinex3MetHeader::Rinex3MetType>& rmhset)
            : obsSet(rmhset)
         {}

      bool operator()(const gpstk::Rinex3MetData& l,
                      const gpstk::Rinex3MetData& r) const
         {
               // compare the times, offsets, then only those elements
               // that are common to both.  this ignores the flags
               // that are set to 0
            if (l.time < r.time)
               return true;
            else if (l.time != r.time)
               return false;
            
               // then check that each observation has the same data
               // for each item in the set of common observations
            gpstk::Rinex3MetData::Rinex3MetMap::const_iterator 
               lItr, rItr;
            std::set<gpstk::Rinex3MetHeader::Rinex3MetType>::const_iterator
               obsItr = obsSet.begin();
         
            while (obsItr != obsSet.end())
            {
               rItr = r.data.find(*obsItr);
               if (rItr == r.data.end())
                  return false;

               lItr = l.data.find(*obsItr);
               if (lItr == l.data.end())
                  return false;

               if ((*lItr).second < (*rItr).second)
                  return true;
               if ((*lItr).second > (*rItr).second)
                  return false;

               obsItr++;
            }

               // the data is either == or > at this point
            return false;
         }

   private:
      std::set<gpstk::Rinex3MetHeader::Rinex3MetType> obsSet;
   };

      /// Only compares times
   struct Rinex3MetDataOperatorLessThanSimple : 
      public Rinex3MetDataBinaryOperator
   {
   public:
      bool operator()(const gpstk::Rinex3MetData& l,
                      const gpstk::Rinex3MetData& r) const
         {
            if (l.time < r.time)
               return true;
            return false;
         }
   };

      /// Only compares times
   struct Rinex3MetDataOperatorEqualsSimple : 
      public Rinex3MetDataBinaryOperator
   {
   public:
      bool operator()(const gpstk::Rinex3MetData& l,
                      const gpstk::Rinex3MetData& r) const
         {
            if (l.time == r.time)
               return true;
            return false;
         }
   };

      /// Combines Rinex3MetHeaders into a single header, combining comments
      /// and adding the appropriate Rinex3MetTypes.  This assumes that
      /// all the headers come from the same station for setting the other
      /// header fields. After running touch() on a list of Rinex3MetHeader,
      /// the internal theHeader will be the merged header data for
      /// those files and obsSet will be the set of Rinex3MetTypes that
      /// will be printed to the file.
   struct Rinex3MetHeaderTouchHeaderMerge :
      public Rinex3MetDataUnaryOperator
   {
   public:
      Rinex3MetHeaderTouchHeaderMerge()
            : firstHeader(true)
         {}

      bool operator()(const gpstk::Rinex3MetHeader& l)
         {
            if (firstHeader)
            {
               theHeader = l;
               firstHeader = false;
            }
            else
            {
               std::set<gpstk::Rinex3MetHeader::Rinex3MetType> thisMetSet, 
                  tempMetSet;
               std::set<std::string> commentSet;
               obsSet.clear();

                  // insert the comments to the set
                  // and let the set take care of uniqueness
               copy(theHeader.commentList.begin(),
                    theHeader.commentList.end(),
                    inserter(commentSet, commentSet.begin()));
               copy(l.commentList.begin(),
                    l.commentList.end(),
                    inserter(commentSet, commentSet.begin()));
                  // then copy the comments back into theHeader
               theHeader.commentList.clear();
               copy(commentSet.begin(), commentSet.end(),
                    inserter(theHeader.commentList,
                             theHeader.commentList.begin()));

                  // find the set intersection of the obs types
               copy(theHeader.obsTypeList.begin(),
                    theHeader.obsTypeList.end(),
                    inserter(thisMetSet, thisMetSet.begin()));
               copy(l.obsTypeList.begin(),
                    l.obsTypeList.end(),
                    inserter(tempMetSet, tempMetSet.begin()));
               set_intersection(thisMetSet.begin(), thisMetSet.end(),
                                tempMetSet.begin(), tempMetSet.end(),
                                inserter(obsSet, obsSet.begin()));
                  // then copy the obsTypes back into theHeader
               theHeader.obsTypeList.clear();
               copy(obsSet.begin(), obsSet.end(),
                    inserter(theHeader.obsTypeList, 
                             theHeader.obsTypeList.begin()));
            }
            return true;
         }

      bool firstHeader;
      gpstk::Rinex3MetHeader theHeader;
      std::set<gpstk::Rinex3MetHeader::Rinex3MetType> obsSet;
   };


      /// This filter will remove any data not within the specified time range.
   struct Rinex3MetDataFilterTime : public Rinex3MetDataUnaryOperator
   {
   public:
      Rinex3MetDataFilterTime(const gpstk::CommonTime& startTime,
                             const gpstk::CommonTime& endTime)
            : start(startTime), end(endTime)
      {}
      
      bool operator() (const gpstk::Rinex3MetData& l) const
      {
         if ( (l.time < start) ||
              (l.time >= end))
            return true;
         return false;
      }
      
   private:
      gpstk::CommonTime start, end;
   };

   //@}

}


#endif
