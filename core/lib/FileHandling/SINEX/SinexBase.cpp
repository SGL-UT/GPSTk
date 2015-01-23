#pragma ident "$Id: //depot/msn/main/code/shared/gpstk/SinexBase.cpp#6 $"

/**
 * @file SinexBase.cpp
 * Base constants, types, and functions for SINEX files
 */

//lgpl-license START
//lgpl-license END

//dod-release-statement START
//dod-release-statement END

#include "SinexBase.hpp"
#include "YDSTime.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
namespace Sinex
{

   bool
   isValidObsCode(char c, bool toss)
   {
      if (OBS_CODES.find(c) == string::npos)
      {
         if (toss)
         {
            string  errMsg("Invalid Observation Code: ");
            errMsg += c;
            Exception  err(errMsg);
            GPSTK_THROW(err);
         }
         return false;
      }
      return true;
   }


   bool
   isValidConstraintCode(char c, bool toss)
   {
      if (CONSTRAINT_CODES.find(c) == string::npos)
      {
         if (toss)
         {
            string  errMsg("Invalid Constraint Code: ");
            errMsg += c;
            Exception  err(errMsg);
            GPSTK_THROW(err);
         }
         return false;
      }
      return true;
   }


   bool
   isValidSolutionType(char c, bool toss)
   {
      if (SOLUTION_TYPES.find(c) == string::npos)
      {
         if (toss)
         {
            string  errMsg("Invalid Solution Type: ");
            errMsg += c;
            Exception  err(errMsg);
            GPSTK_THROW(err);
         }
         return false;
      }
      return true;
   }


   bool
   isValidLineStructure(const std::string& line,
                                   size_t minLen,
                                   size_t maxLen,
                                   int divs[],
                                   bool toss)
   {
      size_t  sz = line.size();

         /// Check Minimum line length
      if (minLen < MIN_LINE_LEN) minLen = MIN_LINE_LEN;
      if (sz < minLen)
      {
         if (toss)
         {
            ostringstream  ss;
            ss << "Missing data; inadequate line length ("
               << sz << " < " << minLen << ")";
            Exception  err(ss.str() );
            GPSTK_THROW(err);
         }
         return false;
      }

         /// Check maximum line length
      if (maxLen > MAX_LINE_LEN) maxLen = MAX_LINE_LEN;
      if (sz > maxLen)
      {
         if (toss)
         {
            ostringstream  ss;
            ss << "Excessive line length ("
               << sz << " > " << maxLen << ")";
            Exception  err(ss.str() );
            GPSTK_THROW(err);
         }
         return false;
      }
         /// Check for valid field dividers
      if (divs != NULL)
      {
         int  pos;
         for (size_t i = 0; (pos = divs[i]) >= 0; ++i)
         {
            if ( (pos >= sz) || (line[pos] != FIELD_DIV) )
            {
               if (toss)
               {
                  ostringstream  ss;
                  ss << "Field divider '" << FIELD_DIV
                     << "' expected in column " << pos;
                  Exception  err(ss.str() );
                  GPSTK_THROW(err);
               }
               return false;
            }
         }
      }
      return true;
   }


   std::string
   formatStr(const std::string& value, size_t width, bool allowBlank)
   {
      if ( (value.size() == 0) && !allowBlank)
      {
         return string(width, '-');
      }
      else
      {
         ostringstream  ss;
         ss << left << setw(width) << value.substr(0, width);
         return ss.str();
      }
   }


   std::string
   formatInt(long value, size_t width)
   {
      ostringstream  ss;
      ss << right << setw(width) << value;
      string  s = ss.str();
      if (s.size() > width)
      {
         ostringstream  msg;
         msg << "Cannot represent value " << value;
         msg << " within width " << width;
         Exception  err(msg.str() );
         GPSTK_THROW(err);
      }
      return s;
   }


   std::string
   formatUint(unsigned long value, size_t width)
   {
      ostringstream  ss;
      ss << right << setw(width) << value;
      string  s = ss.str();
      if (s.size() > width)
      {
         ostringstream  msg;
         msg << "Cannot represent value " << value;
         msg << " within width " << width;
         Exception  err(msg.str() );
         GPSTK_THROW(err);
      }
      return s;
   }


   std::string
   formatFor(double value, size_t width, size_t expLen)
   {
      string s = gpstk::StringUtils::doub2for(value, width, expLen);
      std::string::size_type idx = s.find('D', 0);
      s[idx] = 'E';
      return s;
   }

   std::string
   formatFixed(double value, size_t width, size_t precision)
   {
      ostringstream  ss;
      ss << fixed << right << setw(width) << setprecision(precision) << value;
      string  s = ss.str();

      if (s.size() > width)
      {
            /// If value is close to 0, try squeezing to stay within width
         if ( (s.size() == width+1) && (value < 1.0) && (value > -1.0) )
         {
            if (value < 0.0)
            {
               s.erase(1, 1);  /// Squeeze -0.123 to -.123
            }
            else
            {
               s.erase(0, 1);  /// Squeeze 0.1234 to .1234
            }
         }
         else
         {
            ostringstream  msg;
            msg << "Cannot represent value " << value;
            msg << " within width " << width;
            msg << " with precision " << precision;
            Exception  err(msg.str() );
            GPSTK_THROW(err);
         }
      }
      return s;
   }


   Time::operator gpstk::CommonTime() const
   {
      long  y = (year > 50) ? (year + 1900) : (year + 2000);
      gpstk::YDSTime  ydsTime(y, doy, (double)sod);
      return ydsTime.convertToCommonTime();
   }


   Time::operator std::string() const
   {
      std::ostringstream  ss;
      ss << setfill('0');
      ss << setw(2) << (uint16_t)year << ":";
      ss << setw(3) << doy << ":";
      ss << setw(5) << sod;
      return ss.str();
   }


   void
   Time::operator=(const gpstk::CommonTime& other)
   {
      gpstk::YDSTime  ydsTime(other);
      if (ydsTime.year > 2050)
      {
         Exception  err("SINEX does not support years past 2050.");
         GPSTK_THROW(err);
      }
      year = ydsTime.year % 100;  // Get last 2 digits of time
      doy  = ydsTime.doy;
      sod  = ydsTime.sod;
   }


   void
   Time::operator=(const std::string& other)
   {
      if ( (other.size() < 12) || (other[2] != ':') || (other[6] != ':') )
      {
         Exception  err("Invalid time syntax: " + other);
         GPSTK_THROW(err);
      }
      year = asInt(other.substr(0, 2) );
      doy  = asInt(other.substr(3, 3) );
      sod  = asInt(other.substr(7, 5) );
   }

}  // namespace Sinex

}  // namespace gpstk
