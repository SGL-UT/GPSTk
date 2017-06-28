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

#ifndef GPSTK_MJD_HPP
#define GPSTK_MJD_HPP

#include "TimeTag.hpp"
#include "TimeSystem.hpp"
#include "gpstkplatform.h"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /**
       * This class encapsulates the "Modified Julian Date" time
       * representation.
       */
   class MJD : public TimeTag
   {
   public:
         /**
          * @name MJD Basic Operations
          */
         //@{

         /** Default empty constructor
          * All elements are initialized to zero.
          */
      MJD(void)
      {
         imjd = 0L;
         dday = fday = static_cast<uint64_t>(0);
         timeSystem = TimeSystem::Unknown;
      }

         /** Constructor from long double MJD
          * Warning - precision lost on systems where long double == double (WIN)
          */
      MJD(long double mjd, TimeSystem ts = TimeSystem::Unknown);

         /// Constructor from long int(MJD) and double seconds-of-day
      void fromMJDsod(long mjd, double sod, TimeSystem ts = TimeSystem::Unknown);

         /// Constructor from long int(MJD) and double frac(MJD)
      void fromIntFrac(long mjd, double frac, TimeSystem ts = TimeSystem::Unknown);

         /// Constructor (except for system) from string
      void fromString(std::string instr);

         /// Write full MJD to a string.
      std::string asString(const int prec=-1) const;

         /**
          * Copy Constructor.
          * @param right a reference to the MJD object to copy
          */
      MJD( const MJD& right )
            : imjd(right.imjd), dday(right.dday), fday(right.fday)
      { timeSystem = right.timeSystem; }

         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      MJD( const TimeTag& right )
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
      MJD( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the MJD to copy
          * @return a reference to this MJD
          */
      MJD& operator=( const MJD& right );

         /// Virtual Destructor.
      virtual ~MJD()
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
         return "QP";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
      {
         return "%.9Q %P";
      }

      virtual bool isValid() const;

      virtual void reset();

         /**
          * @name MJD Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the MJD object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const MJD& right ) const;
      bool operator!=( const MJD& right ) const;
      bool operator<( const MJD& right ) const;
      bool operator>( const MJD& right ) const;
      bool operator<=( const MJD& right ) const;
      bool operator>=( const MJD& right ) const;
         //@}

      /// Compute long double MJD
      /// Warning - precision lost when cast to double, and on systems where
      ///   long double is implemented as double (WIN)
      /// @return long double MJD
      long double asLongDouble(void) const;

      /// Compute and @return long MJD
      long asLong(void) const
         { return imjd; }

      /// Compute and @return seconds of day
      double SecOfDay(void) const
         { return static_cast<double>((dday+fday*MJDFACT)*MJDFACT) * SEC_PER_DAY; }

      // constants
      static const unsigned int MJDLEN;
      static const double MJDFACT;

      // member data
      long imjd;     ///< integer MJD ("MJ day")
      // fraction of day = (dday+fday*MJDFACT)*MJDFACT
      uint64_t dday; ///< fraction of day/MJDFACT
      uint64_t fday; ///< (fraction of day/MJDFACT-dday)/MJDFACT
   };

      //@}

} // namespace

#endif // GPSTK_MJD_HPP
