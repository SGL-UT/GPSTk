#pragma ident "$Id: //depot/sgl/gpstk/dev/src/ANSITime.hpp#2 $"

#ifndef GPSTK_ANSITIME_HPP
#define GPSTK_ANSITIME_HPP

#include "TimeTag.hpp"
#include <time.h>     // for time_t

namespace gpstk
{
      /**
       * This class encapsulates the "ANSITime (seconds since Unix epoch)" time
       * representation.
       */
   class ANSITime : public TimeTag
   {
   public:
      static const std::string printChars;
      static const std::string defaultFormat;
      
         /**
          * @defgroup utbo ANSITime Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{

         /**
          * Default Constructor.
          * All elements are initialized to zero.
          */
      ANSITime( time_t t = 0 )
         throw()
      {
         time = t;
      }

         /** 
          * Copy Constructor.
          * @param right a reference to the ANSITime object to copy
          */
      ANSITime( const ANSITime& right )
         throw()
            : time( right.time )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      ANSITime( const TimeTag& right )
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
      ANSITime( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the ANSITime to copy
          * @return a reference to this ANSITime
          */
      ANSITime& operator=( const ANSITime& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~ANSITime()
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
          * @defgroup utco ANSITime Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the ANSITime object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const ANSITime& right ) const
         throw();
      bool operator!=( const ANSITime& right ) const
         throw();
      bool operator<( const ANSITime& right ) const
         throw();
      bool operator>( const ANSITime& right ) const
         throw();
      bool operator<=( const ANSITime& right ) const
         throw();
      bool operator>=( const ANSITime& right ) const
         throw();
         //@}

      time_t time;
   };

} // namespace

#endif // GPSTK_ANSITIME_HPP
