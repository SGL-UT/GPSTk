#pragma ident "$Id: //depot/sgl/gpstk/dev/apps/reszilla/ElevationRange.hpp#1 $"

#ifndef ELEVATIONRANGE_HPP
#define ELEVATIONRANGE_HPP

#include <iostream>
#include <list>

typedef std::pair<float, float> ElevationRange;
typedef std::list< ElevationRange > ElevationRangeList;

inline void dump(std::ostream& s, const ElevationRange& er)
{
   s << er.first << "-" << er.second;
};

inline std::ostream& operator<<(std::ostream& s, const ElevationRange& er)
{
   dump(s, er);
   return s;
};

inline void dump(std::ostream& s, const ElevationRangeList& erl)
{
   for (ElevationRangeList::const_iterator i=erl.begin(); i != erl.end(); i++)
      s << *i << " ";
   s << std::endl;
};

inline std::ostream& operator<<(std::ostream& s, const ElevationRangeList& erl)
{
   dump(s, erl);
   return s;
};

#endif
