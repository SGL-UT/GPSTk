#pragma ident "$Id$"
#ifndef GPSTK_CIVILTIME_HPP
#define GPSTK_CIVILTIME_HPP
#include "TimeTag.hpp"
#include "TimeSystem.hpp"

namespace gpstk
{
   class CivilTime : public TimeTag
   {
   public:
      CivilTime( int yr = 0,
                 int mo = 0,
                 int dy = 0,
                 int hr = 0,
                 int mn = 0,
                 double s = 0.0,
                 TimeSystem ts = TimeSystem::Unknown )
         throw()
	    : year(yr), month(mo), day(dy), hour(hr), minute(mn), second(s)
      { timeSystem = ts; }

      CivilTime( const CivilTime& right )
         throw()
            : year( right.year ), month( right.month )  , day( right.day ),
              hour( right.hour ), minute( right.minute ), second( right.second )
      { timeSystem = right.timeSystem; }
      
      CivilTime( const TimeTag& right )
         throw()
      {
         convertFromCommonTime( right.convertToCommonTime() ); 
      }

      CivilTime( const CommonTime& right )
         throw()
      {
         convertFromCommonTime( right );
      }

      CivilTime& operator=( const CivilTime& right )
         throw();

      virtual ~CivilTime() 
         throw()
      {}

      // static const char *MonthNames[];
      
      // static const char *MonthAbbrevNames[];
      
      virtual CommonTime convertToCommonTime() const
         throw( gpstk::InvalidRequest );

      virtual void convertFromCommonTime( const CommonTime& ct )
         throw();

      virtual std::string printf(const std::string& fmt) const
         throw( gpstk::StringUtils::StringException );

      virtual std::string printError( const std::string& fmt) const
         throw( gpstk::StringUtils::StringException );

      virtual bool setFromInfo( const IdToValue& info )
         throw();

      virtual std::string getPrintChars() const
         throw()
      { 
         return "YymbBdHMSfP";
      }

      virtual std::string getDefaultFormat() const
         throw()
      {
         return "%02m/%02d/%04Y %02H:%02M:%02S %P";
      }

      virtual bool isValid() const
         throw();

      virtual void reset() 
         throw();

      bool operator==( const CivilTime& right ) const
         throw();
      bool operator!=( const CivilTime& right ) const
         throw();
      bool operator<( const CivilTime& right ) const
         throw( gpstk::InvalidRequest );
      bool operator>( const CivilTime& right ) const
         throw( gpstk::InvalidRequest );
      bool operator<=( const CivilTime& right ) const
         throw( gpstk::InvalidRequest );
      bool operator>=( const CivilTime& right ) const
         throw( gpstk::InvalidRequest );

      int year;
      int month;
      int day;
      int hour;
      int minute;
      double second;

   };
   
   std::ostream& operator<<( std::ostream& s,
                             const gpstk::CivilTime& cit );

} // namespace

#endif // GPSTK_CIVILTIME_HPP
