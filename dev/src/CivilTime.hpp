#pragma ident "$Id: //depot/sgl/gpstk/dev/src/CivilTime.hpp#4 $"

#ifndef GPSTK_CIVILTIME_HPP
#define GPSTK_CIVILTIME_HPP

#include "TimeTag.hpp"

namespace gpstk
{
      /**
       * This class encapsulates the representation of time consisting of 
       * year, month, day (of month), hour (of day), minute (of hour), and
       * second (of minute).
       */
   class CivilTime : public TimeTag
   {
   public:
      
      static const std::string printChars;
      static const std::string defaultFormat;
      
         /**
          * @defgroup caltbo CivilTime Basic Operations
          * Default and Copy Constructors, Assignment Operator and Destructor.
          */
         //@{
         /**
          * Default Constructor.
          * All elements default to zero.
          */
      CivilTime( int yr = 0,
                 int mo = 0,
                 int dy = 0,
                 int hr = 0,
                 int mn = 0,
                 double s = 0. )
         throw()
            : year(yr), month(mo), day(dy), hour(hr), minute(mn), second(s) 
      {}
      
         /**
          * Copy Constructor.
          * @param right a const reference to the CivilTime object to copy
          */
      CivilTime( const CivilTime& right )
         throw()
            : year( right.year ), month( right.month ), day( right.day ),
              hour( right.hour ), minute( right.minute ), 
              second( right.second )
      {}
      
         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via 
          * conversion to CommonTime.
          * @param right a const reference to the TimeTag-based object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      CivilTime( const TimeTag& right )
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
      CivilTime( const CommonTime& right )
         throw( InvalidRequest )
      {
         convertFromCommonTime( right );
      }

         /** 
          * Assignment Operator.
          * @param right a const reference to the CivilTime object to copy
          * @return a reference to this CivilTime object
          */
      CivilTime& operator=( CivilTime& right )
         throw();

         /// Virtual Destructor.
      virtual ~CivilTime() 
         throw()
      {}
         //@}

         /// Long month names for converstion from numbers to strings
      static const char *MonthNames[];
      
         /// Short month names for converstion from numbers to strings
      static const char *MonthAbbrevNames[];
      
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
          * @defgroup ctco CivilTime Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the CivilTime object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const CivilTime& right ) const
         throw();
      bool operator!=( const CivilTime& right ) const
         throw();
      bool operator<( const CivilTime& right ) const
         throw();
      bool operator>( const CivilTime& right ) const
         throw();
      bool operator<=( const CivilTime& right ) const
         throw();
      bool operator>=( const CivilTime& right ) const
         throw();
         //@}

      int year;
      int month;
      int day;
      int hour;
      int minute;
      double second;

   };

} // namespace

#endif // GPSTK_CIVILTIME_HPP
