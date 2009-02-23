// Some operator<< are not wrapped.  Even some that are not nested.

#include <iostream>
#include <fstream>
#include "DayTime.hpp"

std::ostream& DayTime_streamRead(ostream& s, DayTime& t)
{
  s << t.printf("%02m/%02d/%04Y %02H:%02M:%02S");
  return s;
}
