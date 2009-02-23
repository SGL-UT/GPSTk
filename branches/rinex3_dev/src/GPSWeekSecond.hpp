#pragma ident "$Id$"



#ifndef GPSTK_GPSWEEKSECOND_HPP
#define GPSTK_GPSWEEKSECOND_HPP

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

#include "GPSWeek.hpp"
#include "TimeConstants.hpp"

namespace gpstk
{
      /** 
       * This class encapsulates the "Full GPS Week and GPS 
       * Seconds-of-week" time representation.
       */
   class GPSWeekSecond : public GPSWeek
   {
   public:
         /** 
          * @defgroup gwsbo GPSWeekSecond Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      GPSWeekSecond( unsigned int w = 0,
                     double s = 0. )
         throw()
            : GPSWeek(w), sow(s)
      {}
      
         /**
          * Copy Constructor.
          * @param right a reference to the GPSWeekSecond object to copy
          */
      GPSWeekSecond( const GPSWeekSecond& right )
         throw()
            : GPSWeek( right ), sow( right.sow )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSWeekSecond( const TimeTag& right )
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
      GPSWeekSecond( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the GPSWeekSecond to copy
          * @return a reference to this GPSWeekSecond
          */
      GPSWeekSecond& operator=( const GPSWeekSecond& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~GPSWeekSecond()
         throw()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const
         throw(InvalidRequest);

      virtual void convertFromCommonTime( const CommonTime& ct )
         throw(InvalidRequest);

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
         return GPSWeek::getPrintChars() + "wg";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
         throw()
      {
         return GPSWeek::getDefaultFormat() + " %010.3g";
      }

      virtual bool isValid() const
         throw();

      virtual void reset()
         throw();

      inline virtual unsigned int getDayOfWeek() const
         throw()
      {
         return static_cast<unsigned int>(sow) / SEC_PER_DAY;
      }

         /**
          * @defgroup gwsco GPSWeekSecond Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the GPSWeekSecond object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const GPSWeekSecond& right ) const
         throw();
      bool operator!=( const GPSWeekSecond& right ) const
         throw();
      bool operator<( const GPSWeekSecond& right ) const
         throw();
      bool operator>( const GPSWeekSecond& right ) const
         throw();
      bool operator<=( const GPSWeekSecond& right ) const
         throw();
      bool operator>=( const GPSWeekSecond& right ) const
         throw();
         //@}

      double sow;
   };

}

#endif // GPSTK_GPSWEEKSECOND_HPP
