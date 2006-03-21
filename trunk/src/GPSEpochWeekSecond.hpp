#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSEpochWeekSecond.hpp#4 $"

#ifndef GPSTK_GPSEPOCHWEEKSECOND_HPP
#define GPSTK_GPSEPOCHWEEKSECOND_HPP

#include "TimeTag.hpp"

namespace gpstk
{
      /** 
       * This class encapsulates the "GPS Epoch, GPS Week and GPS 
       * Seconds-of-week" time representation.
       */
   class GPSEpochWeekSecond : public TimeTag
   {
   public:
      static const std::string printChars;
      static const std::string defaultFormat;

         /** 
          * @defgroup gewsbo GPSEpochWeekSecond Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      GPSEpochWeekSecond( int e = 0,
                          int w = 0,
                          double s = 0. )
         throw()
            : epoch(e), week(w), sow(s)
      {}
      
         /**
          * Copy Constructor.
          * @param right a reference to the GPSEpochWeekSecond object to copy
          */
      GPSEpochWeekSecond( const GPSEpochWeekSecond& right )
         throw()
            : epoch( right.epoch ), week( right.week ), sow( right.sow )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSEpochWeekSecond( const TimeTag& right )
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
      GPSEpochWeekSecond( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the GPSEpochWeekSecond to copy
          * @return a reference to this GPSEpochWeekSecond
          */
      GPSEpochWeekSecond& operator=( const GPSEpochWeekSecond& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~GPSEpochWeekSecond()
         throw()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const;

      virtual void convertFromCommonTime( const CommonTime& ct ) ;

         /// This function formats this time to a string.  The exceptions 
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf(const std::string& fmt) const
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
         return printChars;
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
         throw()
      {
         return defaultFormat;
      }

      virtual bool isValid() const
         throw();

         /**
          * @defgroup gewsco GPSEpochWeekSecond Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the GPSEpochWeekSecond object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const GPSEpochWeekSecond& right ) const
         throw();
      bool operator!=( const GPSEpochWeekSecond& right ) const
         throw();
      bool operator<( const GPSEpochWeekSecond& right ) const
         throw();
      bool operator>( const GPSEpochWeekSecond& right ) const
         throw();
      bool operator<=( const GPSEpochWeekSecond& right ) const
         throw();
      bool operator>=( const GPSEpochWeekSecond& right ) const
         throw();
         //@}

      int epoch;
      int week;
      double sow;
   };

}

#endif // GPSTK_GPSEPOCHWEEKSECOND_HPP
