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
//  Copyright 2017, The University of Texas at Austin
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

#ifndef GPSTK_POSIXTIME_HPP
#define GPSTK_POSIXTIME_HPP

#include <time.h>
#include "TimeTag.hpp"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /**
       * This class encapsulates the struct timespec time
       * representation defined by POSIX as a replacement (?) for
       * timeval.
       *
       * @note struct timeval used by gettimeofday() is expected to
       * represent a real time, but there is no such expectation for
       * struct timespec, which may be used to represent a variety of
       * clocks, including but not limited to real-time.  It is
       * assumed that the timespec is a real-time clock value when
       * converting to and from CommonTime.
       */
   class PosixTime : public TimeTag
   {
   public:
         /**
          * @name PosixTime Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{

         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      PosixTime( int sec = 0,
                 int nsec = 0,
                 TimeSystem tsys = TimeSystem::Unknown )
      { ts.tv_sec = sec;  ts.tv_nsec = nsec;  timeSystem = tsys; }

         /** struct timespec Constructor.
          * Sets time according to the given struct timespec.
          */
      PosixTime( struct timespec t,
                 TimeSystem tsys = TimeSystem::Unknown )
      {
         ts.tv_sec = t.tv_sec;  ts.tv_nsec = t.tv_nsec;  timeSystem = tsys;
      }

         /**
          * Copy Constructor.
          * @param right a reference to the PosixTime object to copy
          */
      PosixTime( const PosixTime& right )
            : ts( right.ts )
      { timeSystem = right.timeSystem; }

         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      PosixTime( const TimeTag& right )
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
      PosixTime( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the PosixTime to copy
          * @return a reference to this PosixTime
          */
      PosixTime& operator=( const PosixTime& right );

         /// Virtual Destructor.
      virtual ~PosixTime()
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
         return "WNP";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
      {
         return "%W %N %P";
      }

      virtual bool isValid() const;

      virtual void reset();

         /**
          * @name PosixTime Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the PosixTime object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      virtual bool operator==( const PosixTime& right ) const;
      virtual bool operator!=( const PosixTime& right ) const;
      virtual bool operator<( const PosixTime& right ) const;
      virtual bool operator>( const PosixTime& right ) const;
      virtual bool operator<=( const PosixTime& right ) const;
      virtual bool operator>=( const PosixTime& right ) const;
         //@}

      struct timespec ts;
   };

      //@}

} // namespace

#endif // GPSTK_POSIXTIME_HPP
