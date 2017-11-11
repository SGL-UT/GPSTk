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
          * Default Constructor.
          * All elements are initialized to zero.
          */
      JulianDate( long double j = 0., TimeSystem ts = TimeSystem::Unknown )
            : jd( j )
      { timeSystem = ts; }

         /**
          * Copy Constructor.
          * @param right a reference to the JulianDate object to copy
          */
      JulianDate( const JulianDate& right )
            : jd( right.jd )
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

      long double jd;
   };

      //@}

} // namespace

#endif // GPSTK_JULIANDATE_HPP
