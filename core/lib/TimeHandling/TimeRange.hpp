//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024 
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public 
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 *   A utility class that contains a pair of CommonTime objects that
 *   specify the beginning and end of a time range.  
 */
#ifndef TIMERANGE_HPP
#define TIMERANGE_HPP

#include <map>
#include <string>

#include "CommonTime.hpp"
#include "Exception.hpp"
#include "StringUtils.hpp"


namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

   class TimeRange
   {
   public:

      NEW_EXCEPTION_CLASS(TimeRangeException, Exception);
      
      typedef std::pair<CommonTime, CommonTime> DTPair;
   
      TimeRange();
      
         /**
          * @throw @TimeRangeException
          */
      TimeRange(const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true);

         /** To cover potential use with RiseSetTimeList               
          * @throw @TimeRangeException
          */
      TimeRange(DTPair dtPair,
                const bool startInclusive=true, 
                const bool endInclusive=true);

         /// Copy construtor       
      TimeRange(const TimeRange& tr); 
     
      ~TimeRange( ) {};

         // Accessors
      CommonTime getStart() const { return start; }
      CommonTime getEnd() const { return end; }

         /**
          * @throw @TimeRangeException
          */
      void set( const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true );

         /** Return true is testDT is within the TimeRange.  Whether
          * the boundaries are included is in accordance with the
          * indications provided in includeStartTime and
          * includeEndTime. */
      bool inRange( const CommonTime& testDT ) const; 

         /// Equivalence means all members are identical.
      bool operator==(const TimeRange& right) const;

         /**
          * Included to enable use in maps/sets.  This
          * operator returns true if the start time of
          * left is less than start time of right, regardless
          * of end time. 
          */
      bool operator<(const TimeRange& right) const;

         /** True if start/end of this object are both prior
          * to start of "right" */
      bool isPriorTo( const TimeRange& right ) const;

         /** @return true if this.start < right.end and 
          * this.end > right.start */
      bool overlaps( const TimeRange& right ) const;

         /** @return true if this.start >= right.start and 
          * this.end <= right.end */
      bool isSubsetOf( const TimeRange& right ) const;
      
         /** True if start/end of this object are both after the end
          * of "right" */
      bool isAfter( const TimeRange& right ) const;

         /** Formatted string input.
          * Assume string has 
          *  \li possible white space followed by 
          *  \li optional '[' or '(' (assume '['), 
          *  \li followed by a valid CommonTime string corresponding to fmt, 
          *  \li followed by a ','
          *  \li followed by a valid CommonTime string corresponding to fmt,
          *  \li followed by an optional ']' or ')' (assume ']').
          * @throw TimeRangeException
          * @throw StringUtils::StringException */
      TimeRange& setToString( const std::string& str, 
                              const std::string& fmt);

         /** Formatted print
          * @throw StringUtils::StringException
          */
      std::string printf(const std::string formatArg="%02m/%02d/%02y %02H:%02M:%02S" ) const;

         /// Dump method.   
      std::string dump(const std::string formatArg="%02m/%02d/%02y %02H:%02M:%02S" ) const;
        
   protected: 
      CommonTime start;
      CommonTime end;
      bool includeStartTime;
      bool includeEndTime; 

         /**
          * @throw TimeRangeException
          */
      void init(const CommonTime& startDT, 
                const CommonTime& endDT,
                const bool startInclusive=true, 
                const bool endInclusive=true);

   };

      //@}

}   // end namespace

#endif

