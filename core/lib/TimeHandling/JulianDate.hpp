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

#ifndef GPSTK_JULIANDATE_HPP
#define GPSTK_JULIANDATE_HPP

#include "TimeTag.hpp"
#include <string>
#include <sstream>
#include "gpstkplatform.h"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /// This class encapsulates the "Julian Date" time representation.
      ///
      /// The implementation is in terms of "jday", which is int(JD+0.5), plus two
      /// scaled 64-bit integers (dday,fday) to represent the fraction of the day.
      /// So fraction of day = (dday+fday*JDFACT)*JDFACT where JDFACT = 1.0e-17;
      /// this yields precision up to 1e-34. Implementation of class MJD is similar.
      ///
      /// There are a few subtle implementation issues here:
      /// (0) JD is integer at noon, which is awkward; MJD is integer at midnight.
      /// Thus the representation of "integer day" plus "fractional part of day"
      /// or similar is straightforward for MJD but for JD there is that pesky 0.5.
      /// 
      /// (1) Some compliers, notably MSVC on Windows implement long double as double.
      /// This causes a loss of precision when attempting to write JD as a single
      /// floating number (double or even long double). Therefore, point 2:
      /// 
      /// (2) Here, long double is eliminated; the long double c'tor is deprecated.
      /// There is a "long double JD()" provided, for convenience when high precision
      /// is not needed (e.g. Solar system ephemeris), but with a warning of less
      /// precision. NEVER try to store timetags for reuse as long double JD().
      ///
      /// (3) TimeSystem is left out of the long double constructor on purpose,
      /// otherwise compilers see an ambiguity between the two c'tors
      /// (GPS here could be any system)
      ///   MJD(long double jd, TimeSystem::GPS) and
      ///   MJD(long ijd, double sod)
      /// because TimeSystem::GPS is an int and can be implicitly cast to double.
      /// [However note that TimeSystem(2) will not be implicitly cast.]
      /// Use setTimeSystem(TimeSystem::GPS).
      ///
      /// (4) On constructors for JulianDate jd. The following give the same value:
      ///    jd.fromString("1350000");                // full JD
      ///    jd = JulianDate(1350000,43200,0.0);      // jday not int(JD), sod, fsod
      ///    jd.fromIntFrac(135000,0.0);              // int(JD) and frac(JD)
      ///
      /// (5) fromString() and asString() provide I/O which is repeatable and
      /// the most precise, with up to 34 decimal digits (prec ~ 1e-34 = JDFACT^2).
      /// fromIntFrac() is the worst b/c of the double fraction of the day.
      ///  
   class JulianDate : public TimeTag
   {
   public:
         /**
          * @name JulianDate Basic Operations
          */

         //@{
         /**
          * Default empty constructor
          * Initialized to JD() = zero.
          */
      JulianDate(void)
      {
         jday = 0L;
         dday = JDHALFDAY;       // so JD() = 0
         fday = static_cast<uint64_t>(0);
         timeSystem = TimeSystem::Unknown;
      }

         /// \deprecated
         /// Constructor from long double JD - deprecated
         /// Warning - precision lost on systems where long double == double (e.g.WIN)
         /// NB TimeSystem is left out of this constructor on purpose, otherwise
         /// compilers see an ambiguity between the two c'tors (GPS could be any sys)
         ///   JD(long double jd, TimeSystem::GPS) and
         ///   JD(long ijd, double sod)
         /// because TimeSystem::GPS is an int and can be implicitly cast to double.
         /// [However note that TimeSystem(2) cannot be implicitly cast.]
         ///
         /// @param JD long double JD (non-negative)
      JulianDate(long double JD);

         /// Constructor from Julian day (not JD) and seconds-of-day
         /// NB see note for fromString().
         /// @param jd long Julian day (not JD : jday=int(JD+0.5)), non-negative
         /// @param isod int seconds of day 0 <= sod < 86400
         /// @param fsod double seconds of day 0 <= sod < 1.
         /// @param ts TimeSystem, defaults to Unknown
         /// @throw InvalidParameter if jd or isod or fsod out of range
      JulianDate(long jd, int isod, double fsod, TimeSystem ts = TimeSystem::Unknown);

         /// Constructor from long int(JD) and frac(JD)
         /// NB precision here is limited by double fjd; prefer other forms.
         /// @param ijd long integer part of JD (not jday), non-negative
         /// @param fjd double fractional part of JD 0<= fjd < 1.
         /// @param ts TimeSystem, defaults to Unknown
      void fromIntFrac(long ijd, double fjd, TimeSystem ts=TimeSystem::Unknown);

         /// Constructor (except for system ) from string representation of full JD.
         /// fromString provides the most precise input, up to 1.e-34 (JDFACT^2)
         /// NB on constructors for JulianDate jd - these give the same value:
         ///    jd.fromString("1350000");                // full JD
         ///    jd = JulianDate(1350000,43200,0.0);      // jday not int(JD), sod
         ///    jd.fromIntFrac(135000,0.0);              // int(JD) and frac(JD)
         /// @param instr string rep of full JD e.g. "2457222.50114583333333333333"
      void fromString(std::string instr);

         /// Dump member data as string of comma-separated long integers.
      std::string dumpString(void) const
      {
         std::ostringstream oss;
         oss << jday << "," << dday << "," << fday;
         return oss.str();
      }

         /**
          * write JD to string.
          * @param precision (number of digits beyond decimal)
          */
      std::string asString(const int prec=-1) const;

         /**
          * Copy Constructor.
          * @param right a reference to the JulianDate object to copy
          */
      JulianDate( const JulianDate& right )
            : jday(right.jday), dday(right.dday), fday(right.fday)
      { timeSystem = right.timeSystem; }

         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      JulianDate( const TimeTag& right )
      {
         convertFromCommonTime( right.convertToCommonTime() );
      }

         /**
          * Alternate Copy Constructor.
          * Takes a const CommonTime reference and copies its contents via
          * the convertFromCommonTime method.
          * @param right a const reference to the CommonTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      JulianDate( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the JulianDate to copy
          * @return a reference to this JulianDate
          */
      JulianDate& operator=( const JulianDate& right );

         /// Virtual Destructor.
      virtual ~JulianDate()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const;

         // Convert CommonTime to this
      virtual void convertFromCommonTime( const CommonTime& ct );

         /// This function formats this time to a string.  The exceptions
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const;

         /// This function works similarly to printf.  Instead of filling
         /// the format with data, it fills with error messages.
      virtual std::string printError( const std::string& fmt ) const;

         /**
          * Set this object using the information provided in \a info.
          * @param info the IdToValue object to which this object shall be set.
          * @return true if this object was successfully set using the
          *  data in \a info, false if not.
          */
      virtual bool setFromInfo( const IdToValue& info );

         /// Return a string containing the characters that this class
         /// understands when printing times.
      virtual std::string getPrintChars() const
      {
         return "JP";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
      {
         return "%J %P";
      }

      virtual bool isValid() const;

      virtual void reset();

         /**
          * @name JulianDate Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the JulianDate object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const JulianDate& right ) const;
      bool operator!=( const JulianDate& right ) const;
      bool operator<( const JulianDate& right ) const;
      bool operator>( const JulianDate& right ) const;
      bool operator<=( const JulianDate& right ) const;
      bool operator>=( const JulianDate& right ) const;
         //@}

         /// Compute long double JD
         /// Warning - precision lost when cast to double,
         ///    and on systems where long double is implemented as double (eg. WIN)
         /// @return long double Julian Date JD (not Julian day jday)
      long double JD(void) const;

      /// Compute and @return seconds of day
      double SecOfDay(void) const
         { return ((dday+fday*JDFACT)*JDFACT*SEC_PER_DAY); }

      // constants
      static const unsigned int JDLEN;    ///< decimal digits in dday,fday
      static const double JDFACT;         ///< 1.0e-(JDLEN)
      static const uint64_t JDHALFDAY;    ///< one-half/JDFACT; dday at noon

      // member data
      long jday;     ///< long(JD+0.5) integer (Julian Date before noon)
      // fraction of day = (dday+fday*JDFACT)*JDFACT
      uint64_t dday; ///< fraction of day/JDFACT
      uint64_t fday; ///< (fraction of day/JDFACT - dday)/JDFACT
   };

      //@}

} // namespace

#endif // GPSTK_JULIANDATE_HPP
