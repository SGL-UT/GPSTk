//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/// @file JulianDate.cpp

#include <cmath>
#include "JulianDate.hpp"
#include "TimeConstants.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   const unsigned int JulianDate::JDLEN(17);                // < # dec digits uint64_t
   const double JulianDate::JDFACT(1.0e-17);                // 1.0e-JDLEN
   //const uint64_t JulianDate::JDHALFDAY(500000000L);        // JDLEN=9 digits
   //const uint64_t JulianDate::JDHALFDAY(500000000000L);     // JDLEN=12 digits
   const uint64_t JulianDate::JDHALFDAY(50000000000000000L);// JDLEN=17 digits

   // Constructor from long double JD
   // Warning - precision lost on systems where long double == double (WIN)
   JulianDate::JulianDate(long double jd)
   {
      if(jd < 0.0L)
         GPSTK_THROW(InvalidParameter("Invalid input"));
      jday = static_cast<long>(jd+0.5);
      jd -= static_cast<long double>(jday);
      if(jd >= 0.5) { dday = static_cast<uint64_t>((jd-0.5L)/JDFACT); jday += 1L; }
      else            dday = static_cast<uint64_t>((jd+0.5L)/JDFACT);
      fday = static_cast<uint64_t>((jd/JDFACT-dday)/JDFACT);
   }

   // Constructor from Julian day (not JD) and seconds-of-day
   JulianDate::JulianDate(long jd, int isod, double fsod, TimeSystem ts)
   {
      if(jd < 0)
         GPSTK_THROW(InvalidParameter("Invalid jday input"));
      if(isod < 0 || isod >= SEC_PER_DAY)
         GPSTK_THROW(InvalidParameter("Invalid sec-of-day input"));
      if(fsod < 0.0 || fsod >= 1.0)
         GPSTK_THROW(InvalidParameter("Invalid frac-sec-of-day input"));

      jday = jd;
      dday = static_cast<uint64_t>(0);
      fday = static_cast<uint64_t>(0);

      double fracday;
      if(isod > 0) {
         fracday = static_cast<double>(isod)/SEC_PER_DAY; // NB SEC_PER_DAY=int
         dday = static_cast<uint64_t>(fracday/JDFACT);
         fracday -= static_cast<double>(dday)*JDFACT;
         fday = static_cast<uint64_t>(fracday/(JDFACT*JDFACT));
      }
      if(fsod > 0.0) {
         fracday = fsod/SEC_PER_DAY;
         uint64_t fdday = static_cast<uint64_t>(fracday/JDFACT);
         fracday -= static_cast<double>(fdday)*JDFACT;
         uint64_t ffday = static_cast<uint64_t>(fracday/(JDFACT*JDFACT));

         if(ffday > 0) { fday += ffday; }
         if(fday >= 2*JDHALFDAY) { fday -= 2*JDHALFDAY; dday += 1L; }
         if(fdday > 0) { dday += fdday; }
         if(dday >= 2*JDHALFDAY) { dday -= 2*JDHALFDAY; jday += 1L; }
      }
      timeSystem = ts;
   }

   // Constructor from long int(JD) and frac(JD)
   void JulianDate::fromIntFrac(long ijd, double fjd, TimeSystem ts)
   {
      if(ijd < 0 || fjd < 0.0 || fjd >= 1.0)
         GPSTK_THROW(InvalidParameter("Invalid input"));

      bool rnd(fjd >= 0.5);
      jday = ijd + (rnd ? 1L : 0L);
      if(rnd) dday = static_cast<uint64_t>((fjd-0.5)/JDFACT);
      else    dday = static_cast<uint64_t>((fjd+0.5)/JDFACT);
      fday = static_cast<uint64_t>((fjd - dday*JDFACT)/(JDFACT*JDFACT));
      timeSystem = ts;
   }

   // Constructor (except for system ) from string
   void JulianDate::fromString(std::string instr)
   {
      // parse the string
      int sign,exp,index;
      std::string str = StringUtils::parseScientific(instr, sign, exp, index);

      // cannot have negative
      if(sign < 0) GPSTK_THROW(Exception("Negative JD"));

      // mod the string to make exp=0
      if(exp != 0) { index += exp; exp = 0; }
      // pad the string to put index within the string
      if(index < 0)                    // left leading zeros
         str = std::string(-index,'0') + str;
      else if(index >= str.size())     // trailing zeros
         str = str + std::string(str.size()-index,'0');

      // break into 3 strings  int (.) 17-dig 17-dig
      std::string istr("0");
      if(index > 0) {
         istr = str.substr(0,index);
         StringUtils::stripLeading(str,istr);
      }
      // 64 bit long max value is 9223372036854775807, 19 digits
      std::string dstr = (str.length() > 0 ? str.substr(0,JDLEN) : "0");
      StringUtils::leftJustify(dstr,JDLEN,'0');
      // truncate string after 17 digits, 17+17=34 digits past index
      std::string fstr = (str.length() > JDLEN ? str.substr(JDLEN,JDLEN) : "0");
      StringUtils::leftJustify(fstr,JDLEN,'0');

      bool rnd(dstr[0] >= '5');
      jday = std::strtol(istr.c_str(),0,10) + (rnd ? 1 : 0);
      dday = strtoull(dstr.c_str(),0,10);
      if(rnd) dday -= JDHALFDAY;
      else    dday += JDHALFDAY;         // this accnts for 0.5d JD-jday
      fday = strtoull(fstr.c_str(),0,10);
   }

   // write full JD to string.
   std::string JulianDate::asString(const int prec) const
   {
      long j(jday);
      uint64_t d(dday),f(fday);
      static const char ten('9'+1);

      if(dday < JDHALFDAY) { d += JDHALFDAY; j -= 1L; }
      else                 { d -= JDHALFDAY; }

      // write the part to right of '.' first
      std::string str;
      std::ostringstream oss;
      oss << std::setfill('0') << std::setw(JDLEN) << d;
      oss << std::setw(JDLEN) << f;
      oss << std::setfill(' ');
      str = oss.str();

      // trim to prec digits
      if(prec > -1) {
         if(prec < str.length()) {
            if(str[prec] >= '5' && str[prec] <= '9') {
               // round the string at prec
               int k(prec-1);
               bool rnd(str[k]=='9');
               str[k] = (rnd ? '0' : str[k]+1);

               while(rnd && --k >= 0) {
                  str[k] += 1;
                  rnd = (str[k]==ten);
                  if(rnd) str[k] = '0';
               }
               if(rnd) j += 1L;
            }
            // truncate at prec-1
            str = str.substr(0,prec);
         }
         else
            // 0-pad on the right
            str = str + std::string(prec-str.length(),'0');
      }

      oss.str("");
      oss << j << "." << str;
      return oss.str();
   }

   JulianDate& JulianDate::operator=( const JulianDate& right )
   {
      jday = right.jday;
      dday = right.dday;
      fday = right.fday;
      timeSystem = right.timeSystem;
      return *this;
   }

   CommonTime JulianDate::convertToCommonTime() const
   {
      try
      {
         // fraction of day
         double frod = static_cast<double>(dday)*JDFACT;
         frod += static_cast<double>(fday)*JDFACT*JDFACT;
         long sod = static_cast<long>(frod*SEC_PER_DAY);       // truncate
         frod -= static_cast<double>(sod)/SEC_PER_DAY;
         // fractional seconds of day
         double frsod = frod*SEC_PER_DAY;
         
         CommonTime ct;
         return ct.set( jday, sod, frsod, timeSystem );
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ir);
      }
   }

   void JulianDate::convertFromCommonTime( const CommonTime& ct )
   {
      long isod;
      double fsod;
      ct.get( jday, isod, fsod, timeSystem );

      double frac((static_cast<double>(isod)+fsod)/SEC_PER_DAY);
      dday = static_cast<uint64_t>(frac/JDFACT);
      fday = static_cast<uint64_t>((frac/JDFACT-static_cast<double>(dday))/JDFACT);
   }

   std::string JulianDate::printf( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "JLf", JD() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", timeSystem.asString().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string JulianDate::printError( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "J",
                              "Js", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   bool JulianDate::setFromInfo( const IdToValue& info )
   {
      using namespace gpstk::StringUtils;

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'J':
               this->fromString(i->second);
               break;

            case 'P':
               timeSystem.fromString(i->second);
               break;

            default:
                  // do nothing
               break;
         };
      }

      return true;
   }

   bool JulianDate::isValid() const
   {
      JulianDate temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   void JulianDate::reset()
   {
      jday = 0L;
      dday = fday = static_cast<uint64_t>(0);
      timeSystem = TimeSystem::Unknown;
   }

   bool JulianDate::operator==( const JulianDate& right ) const
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if((timeSystem != TimeSystem::Any && right.timeSystem != TimeSystem::Any)
            && timeSystem != right.timeSystem)
      {
         return false;
      }
      if(jday == right.jday
         && std::abs(((dday-right.dday)*JDFACT + (fday-right.fday)*JDFACT) * JDFACT)
            < CommonTime::eps)
      {
         return true;
      }

      return false;
   }

   bool JulianDate::operator!=( const JulianDate& right ) const
   {
      return ( !operator==( right ) );
   }

   bool JulianDate::operator<( const JulianDate& right ) const
   {
     /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != TimeSystem::Any &&
           right.timeSystem != TimeSystem::Any) &&
          timeSystem != right.timeSystem)
      {
         gpstk::InvalidRequest ir("CommonTime objects not in same time system,"
                                       " cannot be compared");
         GPSTK_THROW(ir);
      }

      if( jday < right.jday || dday < right.dday || fday < right.fday ) return true;
      return false;
   }

   bool JulianDate::operator>( const JulianDate& right ) const
   {
      return ( !operator<=( right ) );
   }

   bool JulianDate::operator<=( const JulianDate& right ) const
   {
      return ( operator<( right ) ||
               operator==( right ) );
   }

   bool JulianDate::operator>=( const JulianDate& right ) const
   {
      return ( !operator<( right ) );
   }

   // Compute long double JD
   // Warning - precision lost when cast to double,
   //    and on systems where long double is implemented as double (eg. WIN)
   // return long double Julian Date JD (not Julian day jday)
   long double JulianDate::JD(void) const
   {
      long double jd = static_cast<long double>(jday);
      // NB uint64_t can't do negative
      if(dday < JDHALFDAY) {
         jd += (static_cast<long double>(dday+JDHALFDAY)
                + static_cast<long double>(fday)*JDFACT)*JDFACT-1.0L;
      }
      else {
         jd += (static_cast<long double>(dday-JDHALFDAY)
                + static_cast<long double>(fday)*JDFACT)*JDFACT;
      }
      return jd;
   }

} // namespace
