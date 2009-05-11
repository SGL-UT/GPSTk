#pragma ident "$Id$"

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================
//
//   Helper class for compSatVis and compStaVis.  This class provideds a 
//   convenient means of accumulaing the discrete counts of how many SVs/stations 
//   are visible to a station/SV at each evaluation epoch.
//
//   At the end of the computation, these values are used to compute the
//   percentage of time a given number of SVs/stations are visible to a 
//   given station/SV and the percentage of time a "at least a given number
//   of SVs/stations are visible to a given station/SV.   
//
//
#ifndef GPSTK_DISCRETEVISIBLECOUNTS_HPP
#define GPSTK_DISCRETEVISIBLECOUNTS_HPP

#include <map>
#include <string>
// gpstk

namespace gpstk
{

      /// Storage for statistics assocaited with SV/station visibility   
   class DiscreteVisibleCounts
   {
      public:
            // Constructors
          /**
          */
        DiscreteVisibleCounts( );
         
            // Methods
         /**
          *  Add a new count.  We don't care "when" it is, we only 
          *  want to record the count. 
          */
         void addCount( const int newCount );
         
         /**
          *  Return the maximum visibility count that has been recorded.  
          */
         int getMaxCount( ) const;
         
         /**
          *  Return the total samples over all visibility counts
          */
         long getSumOfAllCounts( ) const;
         
         /**
          *  Dump the counts from 0 to max.  Prefix the string with the
          *  SV/station identifier.
          */
         std::string dumpCounts( const int max, const int width=5 ) const; 
         
         /**
          *  Dump the counts from min to max expressed as percentages of the
          *  total (one digit to the right of the decimal.  Prefix the string
          *  with the SV/station identifier.
          */
         std::string dumpCountsAsPercentages( const int max, 
                                              const int width=5  ) const; 

         /**
          *  Sum the counts starting at max and working down.  In other words
          *  form the "visible to at least" count.  Dump these counts from 0
          *  to max.  NOTE: "0" is a special case.  The count for "0" will
          *  be the exact count received for "0".  Put another way, the 
          *  first column will be "count = 0 visible" the remainder will be 
          *  "count >= X visible"
          *  Prefix the string with the SV/station identifier.
          */
         std::string dumpCumulativeCounts( const int max,
                                           const int width=5  ) const; 
         
         /**
          *  Sum the counts starting at max and working down.  In other words
          *  form the "visible to at least" count.  Dump the counts from min
          *  to max expressed as percentages of the total (one digit to the
          *  right of the decimal.  Prefix the string with the 
          *  SV/station identifier.
          */
         std::string dumpCumulativeCountsAsPercentages( const int max, 
                                                        const int width=5  ) const; 

            // Data
      protected:
         std::map<int,long> countMap;              
   };

}   
#endif      
