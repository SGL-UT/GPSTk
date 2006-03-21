#pragma ident "$Id: //depot/sgl/gpstk/dev/src/GPSZcount29.hpp#3 $"

#ifndef GPSTK_GPSZCOUNT29_HPP
#define GPSTK_GPSZCOUNT29_HPP

#include "TimeTag.hpp"

namespace gpstk
{
      /**
       * This class encapsulates the "GPS Epoch and 29-bit full Zcount" time 
       * representation.
       */
   class GPSZcount29 : public TimeTag
   {
   public:
      static const std::string printChars;
      static const std::string defaultFormat;
      
         /**
          * @defgroup gz29bo GPSZcount29 Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      GPSZcount29( int e = 0,
                   int z = 0 )
         throw()
            : epoch( e ), zcount( z )
      {}
      
         /** 
          * Copy Constructor.
          * @param right a reference to the GPSZcount29 object to copy
          */
      GPSZcount29( const GPSZcount29& right )
         throw()
            : epoch( right.epoch ), zcount( right.zcount )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      GPSZcount29( const TimeTag& right )
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
      GPSZcount29( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the GPSZcount29 to copy
          * @return a reference to this GPSZcount29
          */
      GPSZcount29& operator=( const GPSZcount29& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~GPSZcount29()
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
          * @defgroup gz29co GPSZcount29 Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the GPSZcount29 object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const GPSZcount29& right ) const
         throw();
      bool operator!=( const GPSZcount29& right ) const
         throw();
      bool operator<( const GPSZcount29& right ) const
         throw();
      bool operator>( const GPSZcount29& right ) const
         throw();
      bool operator<=( const GPSZcount29& right ) const
         throw();
      bool operator>=( const GPSZcount29& right ) const
         throw();
         //@}

      int epoch;
      int zcount;
   };

} // namespace

#endif // GPSTK_GPSZCOUNT32_HPP
