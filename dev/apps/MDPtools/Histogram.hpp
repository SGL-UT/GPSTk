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
//============================================================================

#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <map>
#include <list>
#include <ostream>

namespace gpstk
{

   //-----------------------------------------------------------------------------
   class Histogram
   {
   public:
      typedef std::pair<double, double> BinRange;
      typedef std::list<BinRange> BinRangeList;
      typedef std::map<BinRange, unsigned> BinMap;

      BinMap bins;
      unsigned total;

      void resetBins(const BinRangeList& brl)
      {
         bins.clear();
         total=0;
         for (BinRangeList::const_iterator i=brl.begin(); i != brl.end(); i++)
            bins[*i] = 0;
      }

      inline void addValue(double v)
      {
         BinMap::iterator bm_itr;
         for (bm_itr=bins.begin(); bm_itr != bins.end(); bm_itr++)
         {
            const BinRange& range = bm_itr->first;
            if (range.first < v && v <= range.second)
            {
               bm_itr->second++;
               total++;
               break;
            }
         }
      }

      inline void dump(std::ostream& s) const
      {
         BinMap::const_iterator bmi;
         for (bmi = bins.begin(); bmi != bins.end(); bmi++)
         {
            const BinRange& br = bmi->first;
            s << std::right << std::setw(3) << br.first
              << "-" << std::left  << std::setw(3) << br.second
              << ":   " << std::right <<  bmi->second
              << std::endl;
         }

         s << std::right << std::setw(3) << bins.begin()->first.first
           << "-" << std::left  << std::setw(3) << bins.rbegin()->first.second
           << ":   " << std::right <<  total
           << std::endl;
      };

   };

   inline std::ostream& operator<<(std::ostream& s, const Histogram& hist)
   {
      hist.dump(s);
      return s;
   };

}
#endif
