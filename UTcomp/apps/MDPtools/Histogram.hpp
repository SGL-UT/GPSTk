#pragma ident "$Id: Histogram.hpp 71 2006-08-01 18:46:39Z ehagen $"

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
