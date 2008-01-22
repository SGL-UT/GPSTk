#pragma ident "$Id$"



#ifndef GPSTK_GPSWEEKZCOUNT_HPP
#define GPSTK_GPSWEEKZCOUNT_HPP

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

namespace gpstk
{
      /**
       * This class encapsulates the "Full GPS Week and GPS Z-count" time
       * representation.
       */
   class GPSWeekZcount : public TimeTag
   {
   public:
         /**
          * @defgroup gwzbo GPSWeekZcount Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      GPSWeekZcount( int w = 0,
                     int z = 0 )
         throw()
            : week( w ), zcount( z )
      {}
      
         /** 
          * Copy Constructor.
          * @param right a reference to the GPSWeekZcount object to copy
          */
      GPSWeekZcount( const GPSWeekZcount& right )
         throw()
            : week( right.week ), zcount( right.zcount )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSWeekZcount( const TimeTag& right )
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
      GPSWeekZcount( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the GPSWeekZcount to copy
          * @return a reference to this GPSWeekZcount
          */
      GPSWeekZcount& operator=( const GPSWeekZcount& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~GPSWeekZcount()
         throw()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const;

      virtual void convertFromCommonTime( const CommonTime& ct ) ;

         /// This function formats this time to a string.  The exceptions 
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const
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
         return "FwzZ";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
         throw()
      {
         return "%04F %06Z";
      }

      virtual bool isValid() const
         throw();
      
      virtual void reset()
         throw();

         /**
          * @defgroup gwzco GPSWeekZcount Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the GPSWeekZcount object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const GPSWeekZcount& right ) const
         throw();
      bool operator!=( const GPSWeekZcount& right ) const
         throw();
      bool operator<( const GPSWeekZcount& right ) const
         throw();
      bool operator>( const GPSWeekZcount& right ) const
         throw();
      bool operator<=( const GPSWeekZcount& right ) const
         throw();
      bool operator>=( const GPSWeekZcount& right ) const
         throw();
         //@}

      int week;
      int zcount;
   };   
   
} // namespace

#endif // GPSTK_GPSWEEKZCOUNT_HPP
