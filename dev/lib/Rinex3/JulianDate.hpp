#pragma ident "$Id: JulianDate.hpp 1162 2008-03-27 21:18:13Z snelsen $"



#ifndef GPSTK_JULIANDATE_HPP
#define GPSTK_JULIANDATE_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

#include "TimeTag.hpp"

using namespace gpstk;

namespace Rinex3
{
      /**
       * This class encapsulates the "Julian Date" time representation.
       */
   class JulianDate : public TimeTag
   {
   public:
         /**
          * @defgroup jdbo JulianDate Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      JulianDate( long double j = 0. )
         throw()
            : jd( j )
      {}
      
         /** 
          * Copy Constructor.
          * @param right a reference to the JulianDate object to copy
          */
      JulianDate( const JulianDate& right )
         throw()
            : jd( right.jd )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      JulianDate( const TimeTag& right )
         throw( gpstk::InvalidRequest )
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
         throw( )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the JulianDate to copy
          * @return a reference to this JulianDate
          */
      JulianDate& operator=( const JulianDate& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~JulianDate()
         throw()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const
         throw( InvalidRequest );

      virtual void convertFromCommonTime( const CommonTime& ct )
         throw();

         /// This function formats this time to a string.  The exceptions 
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const
         throw( gpstk::StringUtils::StringException );

         /// This function works similarly to printf.  Instead of filling
         /// the format with data, it fills with error messages.
      virtual std::string printError( const std::string& fmt ) const
         throw( gpstk::StringUtils::StringException );
         
         /**
          * Set this object using the information provided in \a info.
          * @param info the IdToValue object to which this object shall be set.
          * @return true if this object was successfully set using the 
          *  data in \a info, false if not.
          */
      virtual bool setFromInfo( const IdToValue& info )
         throw();
      
         /// Return a string containing the characters that this class
         /// understands when printing times.
      virtual std::string getPrintChars() const
         throw()
      { 
         return "J";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
         throw()
      {
         return "%J";
      }

      void setTimeSytem( const TimeSystem& timeSystem )
         throw()
      {
         m_timeSystem = timeSystem;
      }

      virtual bool isValid() const
         throw();

      virtual void reset()
         throw();
      
         /**
          * @defgroup jdco JulianDate Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the JulianDate object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const JulianDate& right ) const
         throw();
      bool operator!=( const JulianDate& right ) const
         throw();
      bool operator<( const JulianDate& right ) const
         throw();
      bool operator>( const JulianDate& right ) const
         throw();
      bool operator<=( const JulianDate& right ) const
         throw();
      bool operator>=( const JulianDate& right ) const
         throw();
         //@}

      long double jd;
   };

} // namespace

#endif // GPSTK_JULIANDATE_HPP
