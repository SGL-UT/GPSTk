#ifndef GPSTK_NAVTYPE_HPP
#define GPSTK_NAVTYPE_HPP

#include <string>
#include "EnumIterator.hpp"

namespace gpstk
{
      /// @ingroup GNSSEph
      //@{
   
      /** Supported navigation types
       * @note If adding a new NavType enumerator, also add to string
       * input constructor and convertNavTypeToString constructor. */
      enum class NavType
      {
         GPSLNAV,
         GPSCNAVL2,
         GPSCNAVL5,
         GPSCNAV2,
         GPSMNAV,
         BeiDou_D1,
         BeiDou_D2,
         GloCivilF,
         GloCivilC,
         GalFNAV,
         GalINAV,
         IRNSS_SPS,
         Unknown,
         Last
      };

      /** Define an iterator so C++11 can do things like
       * for (NavType tc : NavTypeIterator()) */
   typedef EnumIterator<NavType, NavType::Unknown, NavType::Last> NavTypeIterator;

      /** Translate nav type enumeration to its string representation.
       * @note The string representation is being used in file
       *   formats, e.g. RawNavCSVHeader.  The string values should
       *   not be changed if at all possible, as that would break
       *   the ability to read older files.
       * @note Any new nav codes should not contain spaces in the
       *   string values.
       * @param[in] s The nav type to get the string name of.
       * @return A space-free string containing the name of the nav code.
       */
   std::string convertNavTypeToString(NavType e);

      /** Translate nav type names as strings into enumeration
       * equivalents.
       * @see convertNavTypeToString
       * @param[in] s The nav type name to convert to enumeration.
       * @return An enumeration equivalent of the given string.
       *   Unknown is returned for any names that do not
       *   exactly match known values.
       */
   NavType convertStringToNavType(const std::string& s);

      //@}

   namespace StringUtils
   {
         /// Convert a NavType to a whitespace-free string name.
      inline std::string asString(NavType e) throw()
      { return convertNavTypeToString(e); }
         /// Convert a string name to a NavType
      inline NavType asNavType(const std::string& s) throw()
      { return convertStringToNavType(s); }
   }
} // namespace gpstk

#endif // GPSTK_NAVTYPE_HPP
