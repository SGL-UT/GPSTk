#pragma ident "$Id: //depot/sgl/gpstk/dev/src/YDSTime.hpp#4 $"

#ifndef GPSTK_YDSTIME_HPP
#define GPSTK_YDSTIME_HPP

#include "TimeTag.hpp"

namespace gpstk
{
      /**
       * This class encapsulates the "year, day-of-year, and seconds-of-day"
       * time format.
       */
   class YDSTime : public TimeTag
   {
   public:
      
      static const std::string printChars;
      static const std::string defaultFormat;

         /**
          * @defgroup ydstbo YDSTime Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /** 
          * Default Constructor.
          * All elements are set to zero by default.
          */
      YDSTime( long y = 0, 
               long d = 0, 
               double s = 0.)
         throw()
            : year(y), doy(d), sod(s) 
      {}
      
         /** Copy Constructor.
          * @param right a const reference to the YDSTime object to copy
          */
      YDSTime( const YDSTime& right )
         throw()
            : year( right.year ), doy( right.doy ), sod( right.sod )
      {}
      
         /** 
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the TimeTag-based object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      YDSTime( const TimeTag& right )
         throw( InvalidRequest )
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
      YDSTime( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the YDSTime object to copy
          * @return a reference to this YDSTime 
          */
      YDSTime& operator=( const YDSTime& right )
         throw();
      
         /// Virtual Destructor.
      virtual ~YDSTime()
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
          * @defgroup ydstco YDSTime Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the YDSTime object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const YDSTime& right ) const
         throw();
      bool operator!=( const YDSTime& right ) const
         throw();
      bool operator<( const YDSTime& right ) const
         throw();
      bool operator>( const YDSTime& right ) const
         throw();
      bool operator<=( const YDSTime& right ) const
         throw();
      bool operator>=( const YDSTime& right ) const
         throw();
         //@}

      int year;
      int doy; 
      double sod;
   };
   
} // namespace

#endif // GPSTK_YDSTIME_HPP
