#pragma ident "$Id$"
#ifndef GPSTK_TIMESYSTEM_HPP
#define GPSTK_TIMESYSTEM_HPP
#include <iostream>
#include <string>

namespace gpstk
{
   class TimeSystem
   {
   public:

      enum Systems
      {
         Unknown = 0, ///< unknown time frame; for legacy code compatibility
         Any,         ///< wildcard; allows comparison with any other type
         GPS,         ///< GPS system time
         GLO,         ///< GLONASS system time
         GAL,         ///< Galileo system time
         COM,         ///< Compass system time(BeiDou Time, BDT)
         UTC,         ///< Coordinated Universal Time (e.g., from NTP)
         UT1,         ///< UT0 corrected for polar wandering
         TAI,         ///< International Atomic Time
         TT,          ///< Terrestial Time
         count        ///< the number of systems - not a system
      };

      // TimeSystem(Systems sys = Unknown) throw()
      // {
      //    if(sys < 0 || sys >= count)
      //       system = Unknown;
      //    else
      //       system = sys;
      // }

      TimeSystem(int i) throw()
      {
         if(i < 0 || i >= count)
            system = Unknown;
         else
            system = static_cast<Systems>(i);
      }

      void setTimeSystem(const Systems& sys) throw();

      // Systems getTimeSystem() const throw()
      // { return system; }

      std::string asString() const throw()
      { return Strings[system]; }

      void fromString(const std::string str) throw();

      bool operator==(const TimeSystem& right) const throw()
      { return system == right.system; }

      bool operator<(const TimeSystem& right) const throw()
      { return system < right.system; }

      bool operator!=(const TimeSystem& right) const throw()
      { return !operator==(right); }

      bool operator>=(const TimeSystem& right) const throw()
      { return !operator<(right); }

      bool operator<=(const TimeSystem& right) const throw()
      { return (operator<(right) || operator==(right)); }

      bool operator>(const TimeSystem& right) const throw()
      { return (!operator<(right) && !operator==(right)); }

   // private:
   //    Systems system;
   //    static const std::string Strings[];

   };
   // std::ostream& operator<<(std::ostream& os, const TimeSystem& ts);
}
#endif // GPSTK_TIMESYSTEM_HPP



%extend gpstk::TimeSystem {
   TimeSystem(std::string sys) throw() {
      TimeSystem* t = new TimeSystem(0);
      t->fromString(sys);
      return t;
   }

   std::string getTimeSystem() throw() {
      return $self->asString();
   }

   std::string __str__() throw() {
       return $self->asString();
   }
};
