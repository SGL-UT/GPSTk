#ifndef GPSTK_RNDOUBLE_HPP
#define GPSTK_RNDOUBLE_HPP

#include "FormattedDouble.hpp"

namespace gpstk
{
      /** This class specializes FormattedDouble to cover the data
       * that is present in RINEX NAV data files.  The formatting of
       * individual data is the same between RINEX 2 and 3 so we put
       * the shared implementation here. */
   class RNDouble : public FormattedDouble
   {
   public:
         /// Constructor for a value, with all defaults for format.
      RNDouble(double d = 0)
            : FormattedDouble(d, StringUtils::FFLead::Decimal, 12, 2, 19,
                              'D', StringUtils::FFSign::NegOnly,
                              StringUtils::FFAlign::Right)
      {}
         /// Decode a string.
      RNDouble(const std::string& str)
            : FormattedDouble(str, 19, 'D')
      {}

         /// Assign a value by decoding a string using existing formatting.
      RNDouble& operator=(const std::string& s)
      { FormattedDouble::operator=(s); return *this; }
   };
}

#endif // GPSTK_RNDOUBLE_HPP
