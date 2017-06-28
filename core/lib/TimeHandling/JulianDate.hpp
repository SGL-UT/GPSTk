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

      /**
       * This class encapsulates the "Julian Date" time representation.
       */
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

         /**
          * Constructor from long double JD
          * Warning - precision lost on systems where long double == double (WIN)
          */
      JulianDate(long double jd, TimeSystem ts = TimeSystem::Unknown);

         /**
          * Constructor from Julian day (not JD) and seconds-of-day
          * NB do not write JulianDate(long, double, TimeSystem) since this creates
          *  ambiguity with JulianDate(long double, TimeSystem) b/c TimeSystem is int
          */
      void fromJDaySOD(long jd, double sod, TimeSystem ts = TimeSystem::Unknown);

         /**
          * Constructor from long int(JD) and frac(JD)
          */
      void fromJDintfrac(long ijd, double fjd, TimeSystem ts=TimeSystem::Unknown);

         /**
          * Constructor (except for system ) from string
          */
      void fromString(std::string instr);

         /**
          * Dump member data
          */
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

         /**
          * Compute long double JD
          * Warning - precision lost when cast to double,
          *    and on systems where long double is implemented as double (eg. WIN)
          * @return long double Julian Date JD (not Julian day jday)
          */
      long double JD(void) const;

      // constants
      static const unsigned int JDLEN;
      static const double JDFACT;
      static const uint64_t JDHALFDAY;

      // member data
      long jday;     ///< long(JD+0.5) integer (Julian Date before noon)
      // fraction of day = (dday+fday*JDFACT)*JDFACT
      uint64_t dday; ///< fraction of day/JDFACT
      uint64_t fday; ///< (fraction of day/JDFACT - dday)/JDFACT
   };

      //@}

} // namespace

#endif // GPSTK_JULIANDATE_HPP
