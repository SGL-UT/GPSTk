#pragma ident "$Id: miscdefs.hpp 71 2006-08-01 18:46:39Z ehagen $"


/**
 * @file miscdefs.hpp
 * This file contains miscellaneous definitions that potentially have
 * global significance (as opposed to being relevant only to one class).
 */

#ifndef MISCDEFS_HPP
#define MISCDEFS_HPP

#include <map>
#include "miscenum.hpp"

namespace gpstk
{
   typedef std::pair<gpstk::RangeCode, gpstk::CarrierCode> RangeCarrierPair;
   typedef std::pair<RangeCarrierPair, short> NavIndex;
} // namespace gpstk

#endif // MISCDEFS_HPP
