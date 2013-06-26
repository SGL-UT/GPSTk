#ifndef GPSTK_TIMESYSTEM_HPP
#define GPSTK_TIMESYSTEM_HPP
#include <iostream>
#include <string>
namespace gpstk
{
   class TimeSystem
   {
   public: 

      TimeSystem(int i) throw()
      {
         if(i < 0 || i >= count)
            system = Unknown;
         else
            system = static_cast<Systems>(i);
      }
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
   };   
   std::ostream& operator<<(std::ostream& os, const TimeSystem& ts);
}
#endif // GPSTK_TIMESYSTEM_HPP


%extend gpstk::TimeSystem {
    TimeSystem(std::string sys) throw() {
        TimeSystem* t = new TimeSystem(0);
        t->fromString(sys);
        return t;
    }
    void setTimeSystem(std::string sys) throw() {
        $self->fromString(sys);
    }
    std::string getTimeSystem() throw() {
        return $self->asString();
    }

    std::string __str__() throw() {
        return $self->asString();
    }
};

%pythoncode {
class TimeSystems:
  Unknown, Any, GPS, GLO, GAL, COM, UTC, UT1, TAI, TT = range(10)


TimeSystem.__eq__.__func__.__doc__ = """
Boolean equality operator.

Parameters
    ----------
    right : TimeSystem
        Another TimeSystem.       
"""

TimeSystem.__lt__ .__func__.__doc__= """
Boolean less than operator.
"""

TimeSystem.__gt__.__func__.__doc__ = """
Boolean greater than operator.
"""

TimeSystem.__le__.__func__.__doc__ = """
Boolean less than or equal to operator.
"""

TimeSystem.__ge__.__func__.__doc__ = """
Boolean greater than or equal to operator.
"""

TimeSystem.__ne__.__func__.__doc__ = """
Boolean not equal operator.
"""

TimeSystem.setTimeSystem.__func__.__doc__ = """
Sets the Time System to the given string.
"""

TimeSystem.getTimeSystem.__func__.__doc__ = """
Returns the current time system as a string.
"""

TimeSystem.__str__.__func__.__doc__ = """
Returns the current time system as a string.
"""
}