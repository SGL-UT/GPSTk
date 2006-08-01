#pragma ident "$Id$"


#ifndef GPSTK_COMMONTIME_HPP
#define GPSTK_COMMONTIME_HPP

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

#include "Exception.hpp"

namespace gpstk
{
      /** 
       * This is the common time format that all time formats convert to when
       * converting between themselves.  This allows the decoupling of 
       * inter-format conversions.
       *
       * The interface is based on three quantites: days, seconds of day, and
       * fractional seconds of day.  The internal representation, however, is
       * slightly different.  It consists of a day, milliseconds of day, and
       * fractional seconds of day.  Their valid ranges are shown below:
       * 
       *  Quantity   >=     <
       *  --------   ---   ---
       *   day        0    2^31
       *   msod       0    86400000
       *   fsod       0    0.001
       * 
       * The above is somewhat difficult to grasp at first, but the reason for 
       * keeping the fractional part of time in units of seconds is due to the
       * fact that the time formats usually break at seconds and partial seconds
       * not at milliseconds and partial milliseconds.  By keeping the value in
       * seconds, we save ourselves additional work and loss of precision
       * through conversion of fractional seconds to fractional milliseconds.
       */
   class CommonTime
   {
     public:
         /**
          * @defgroup ctc CommonTime Constants
          */
         //@{
         /// 'julian day' of earliest epoch expressible by CommonTime:
         /// 1/1/4713 B.C.
      static const long BEGIN_LIMIT_JDAY;
         /// 'julian day' of latest epoch expressible by CommonTime:
         /// 1/1/4713 A.D.
      static const long END_LIMIT_JDAY;
         /// earliest representable CommonTime
      static const CommonTime BEGINNING_OF_TIME;
         /// latest representable CommonTime
      static const CommonTime END_OF_TIME;
  
         //@}

         /**
          * @defgroup ctbo CommonTime Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements default to zero.
          * @see CommonTime::set()
          */
      CommonTime( long day = 0, 
                  long sod = 0,
                  double fsod = 0.0 )
         throw( gpstk::InvalidParameter )
      { set( day, sod, fsod ); }

         /**
          * Constructor that accepts days and second of day.
          * All elements default to zero.
          */
      CommonTime( long day,
                  double sod )
         throw( gpstk::InvalidParameter )
      { set( day, sod ); }
      
         /** 
          * Constructor that accepts days only.
          * All elements default to zero.
          */
      CommonTime( double day )
         throw( gpstk::InvalidParameter )
      { set( day ); }
      
         /**
          * Copy Constructor.
          * @param right a const reference to the CommonTime object to copy.
          */
      CommonTime( const CommonTime& right )
         throw();

         /**
          * Assignment Operator.
          * @param right a const reference to the CommonTime object to copy.
          * @return a reference to this CommonTime object.
          */
      CommonTime& operator=( const CommonTime& right )
         throw();
      
         /// Destructor.
      ~CommonTime()
         throw()
      {}
         //@}

         /** 
          * @defgroup ctam CommonTime Accessor Methods
          */
         //@{
         /** 
          * Set method that accepts values for day, seconds of day and
          * fractional seconds of day.  It also checks the validity of the 
          * arguments and throws an exception if any values are out of bounds.
          */
      CommonTime& set( long day, 
                       long sod,
                       double fsod = 0.0 )
         throw( gpstk::InvalidParameter );

         /**
          * Set method that accepts values for day and seconds of day.  It also
          * checks the validity of the arguments and throws an exception if any
          * values are out of bounds.
          */
      CommonTime& set( long day,
                       double sod = 0.0 )
         throw( gpstk::InvalidParameter );
      
         /**
          * Set method that accepts a value for day.  It also checks the
          * validity of the argument and throws an exception if its value is
          * out of bounds.
          */
      CommonTime& set( double day )
         throw( gpstk::InvalidParameter );

         /**
          * Set internal values method.  Set the data members of this object
          * directly.  Checks the validity of the given time representation
          * and throws an exception if any values are out of bounds.
          */
      CommonTime& setInternal( long day = 0,
                               long msod = 0,
                               double fsod = 0.0 )
         throw( gpstk::InvalidParameter );

         /**
          * Get method.  Obtain values in days, second of day and fractional
          * second of day.
          */
      void get( long& day, 
                long& sod,
                double& fsod ) const
         throw();

         /**
          * Get method through which one may obtain values for day and
          * second of day which includes the fractional second of day.
          */
      void get( long& day,
                double& sod ) const
         throw();
      
         /**
          * Get method through which one may obtain a value for day which
          * includes the fraction of a day.
          */
      void get( double& day ) const
         throw();
      
         /** 
          * Get internal values method.  Obtain the values stored within this
          * object.
          */
      void getInternal( long& day,
                        long& msod,
                        double& fsod ) const
         throw()
      { day = m_day; msod = m_msod; fsod = m_fsod; }

         /// Obtain the time, in days, including the fraction of a day.
      double getDays() const 
         throw();

         /// Obtain the seconds of day ( ignoring the day ).
      double getSecondOfDay() const
         throw();

         //@}
      
         /** 
          * @defgroup ctao CommonTime Arithmetic Operations
          */
         //@{
         /** 
          * Difference two Common Time objects.
          * @param right CommonTime to subtract from this one
          * @param reutrn difference in seconds
          */
      double operator-( const CommonTime& right ) const
         throw();
      
         /** 
          * Add seconds to a copy of this CommonTime.
          * @param sec the number of seconds to add to a copy of this CommonTime
          * @return the new CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime operator+( double seconds ) const
         throw( InvalidRequest );
      
         /**
          * Subtract seconds from a copy of this CommonTime.
          * @param sec the number of seconds to subtract from a copy of this
          *  CommonTime
          * @return the new CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime operator-( double seconds ) const
         throw( InvalidRequest );
      
         /** 
          * Add seconds to this CommonTime.
          * @param sec the number of seconds to add to this CommonTime.
          * @return a reference to this CommonTime
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& operator+=( double seconds )
         throw( InvalidRequest );
      
         /**
          * Subtract seconds from this CommonTime.
          * @param sec the number of seconds to subtract from this CommonTime
          * @return a reference to this CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& operator-=( double seconds )
         throw( InvalidRequest );
      
         /** 
          * Add seconds to this CommonTime object.
          * This is the workhorse for the addition/subtraction operators.
          * @param sec the number of seconds to add to this CommonTime
          * @return a reference to this CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& addSeconds( double seconds )
         throw( InvalidRequest );

         /** 
          * Add integer days to this CommonTime object.
          * param days the number of days to add to this CommonTime
          * @return a reference to this CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& addDays( long days )
         throw( InvalidRequest );
      
         /** 
          * Add integer seconds to this CommonTime object.
          * @param sec the number of seconds to add to this CommonTime
          * @return a reference to this CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& addSeconds( long seconds )
         throw( InvalidRequest );
      
         /** 
          * Add integer milliseconds to this CommonTime object.
          * @param ms the number of milliseconds to add to this CommonTime
          * @return a reference to this CommonTime object
          * @throws InvalidRequest on over-/under-flow
          */
      CommonTime& addMilliseconds( long ms )
         throw( InvalidRequest );
         //@}

         /**
          * @defgroup ctco CommonTime Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the CommonTime object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const CommonTime& right ) const
         throw();
      bool operator!=( const CommonTime& right ) const
         throw();
      bool operator<( const CommonTime& right ) const
         throw();
      bool operator>( const CommonTime& right ) const
         throw();
      bool operator<=( const CommonTime& right ) const
         throw();
      bool operator>=( const CommonTime& right ) const
         throw();
         //@}

   protected:
      
         /**
          * Add the following quantities to this CommonTime object.
          * @param days the number of days to add
          * @param msod the number of milliseconds to add
          * @param fsod the number of fractional seconds to add
          * @return the result of calling the normalize() function
          */
      bool add( long days,
                long msod,
                double fsod )
         throw();
      
         /// Normalize the values.  This takes out of bounds values and rolls
         /// other values appropriately.
         /// @return true if m_day is valid, false otherwise
      bool normalize()
         throw();
      
      long m_day;     //< days (as a Julian Day)     0 <= val < 2^31
      long m_msod;    //< milliseconds-of-day        0 <= val < 86400000
      double m_fsod;  //< fractional seconds-of-day  0 <= val < 0.001
   };

} // namespace

#endif // GPSTK_COMMONTIME_HPP
