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

/// @file MJD.hpp

#include <cmath>
#include <sstream>
#include "MJD.hpp"
#include "TimeConstants.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
   const unsigned int MJD::MJDLEN(17);          // < # dec digits uint64_t
   const double MJD::MJDFACT(1.0e-17);          // 1.0e-MJDLEN

   // Constructor from long double MJD
   // Warning - precision lost on systems where long double == double (WIN)
   MJD::MJD(long double mjd)
   {
      imjd = static_cast<long>(mjd);
      mjd -= static_cast<long double>(imjd);          // now fraction of day
      if(mjd < 0.0) { mjd = 1.0L+mjd; imjd += 1L; }
      dday = static_cast<uint64_t>(mjd/MJDFACT);
      fday = static_cast<uint64_t>((mjd/MJDFACT-dday)/MJDFACT);
   }

   // Constructor from long int(MJD) and double seconds-of-day
   MJD::MJD(long lmjd, double sod, TimeSystem ts)
   {
      imjd = lmjd;
      if(sod < 0.0 || sod > SEC_PER_DAY)
         GPSTK_THROW(InvalidParameter("Sec-of-day out of range"));

      sod /= SEC_PER_DAY;     // now its fraction of a day
      dday = static_cast<uint64_t>(sod/MJDFACT);
      fday = static_cast<uint64_t>((sod/MJDFACT-dday)/MJDFACT);
      timeSystem = ts;
   }

   // Constructor from long int(MJD) and double frac(MJD)
   void MJD::fromIntFrac(long lmjd, double frac, TimeSystem ts)
   {
      imjd = lmjd;
      if(frac < 0.0 || frac >= 1.0)
         GPSTK_THROW(InvalidParameter("Fraction-of-day out of range"));

      dday = static_cast<uint64_t>(frac/MJDFACT);
      fday = static_cast<uint64_t>((frac/MJDFACT-dday)/MJDFACT);
      timeSystem = ts;
   }
      
   // Constructor (except for system) from string
   void MJD::fromString(std::string instr)
   {
      static const char ten('9'+1);
      // parse the string
      int sign,exp,index;
      std::string str = StringUtils::parseScientific(instr, sign, exp, index);

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
      imjd = std::strtol(istr.c_str(),0,10);

      // str is now fractional part with (.) implied at position -1
      // if MJD is negative, replace str with 1-str
      if(sign < 0) {
         imjd = -imjd;
         int i=str.length()-1;
         str[i] = '0'+(ten-str[i]);
         while(--i >= 0) str[i] = '0'+('9'-str[i]);
      }

      // 64 bit long max value is 9223372036854775807, 19 digits
      std::string dstr = (str.length() > 0 ? str.substr(0,MJDLEN) : "0");
      StringUtils::leftJustify(dstr,MJDLEN,'0');
      // truncate string after 17 digits, 17+17=34 digits past index
      std::string fstr = (str.length() > MJDLEN ? str.substr(MJDLEN,MJDLEN) : "0");
      StringUtils::leftJustify(fstr,MJDLEN,'0');
      dday = strtoull(dstr.c_str(),0,10);
      fday = strtoull(fstr.c_str(),0,10);
   }

   // Write full MJD to a string.
   std::string MJD::asString(const int prec) const
   {
      long lmjd(imjd);
      uint64_t d(dday),f(fday);
      static const char ten('9'+1);
      std::string str;
      std::ostringstream oss;

      // first write part to the right of '.'
      oss << std::setfill('0') << std::setw(MJDLEN) << d;
      oss << std::setw(MJDLEN) << f;
      oss << std::setfill(' ');
      str = oss.str();

      // trim to prec digits
      if(prec > -1) {
         if(prec < str.length()) {                       // too long
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
               if(rnd) lmjd += 1L;
            }
            str = str.substr(0,prec);                    // truncate at prec-1
         }
         else                                            // too short
            // 0-pad on the right
            str = str + std::string(prec-str.length(),'0');
      }
      
      // now write the integer part and '.'
      oss.str("");
      oss << lmjd << "." << str;
      return oss.str();
   }

   MJD& MJD::operator=( const MJD& right )
   {
      imjd = right.imjd;
      dday = right.dday;
      fday = right.fday;
      timeSystem = right.timeSystem;
      return *this;
   }

   CommonTime MJD::convertToCommonTime() const
   {
      try
      {
         // fraction of day
         double frod = static_cast<double>(dday)*MJDFACT;
         frod += static_cast<double>(fday)*MJDFACT*MJDFACT;
         long sod = static_cast<long>(frod*SEC_PER_DAY);       // truncate
         frod -= static_cast<double>(sod)/SEC_PER_DAY;
         // fractional seconds of day
         double frsod = frod*SEC_PER_DAY;
         
         CommonTime ct;
         return ct.set(imjd+MJD_JDAY, sod, frsod, timeSystem);
      }
      catch (InvalidParameter& ip)
      {
         InvalidRequest ir(ip);
         GPSTK_THROW(ip);
      }
   }

   void MJD::convertFromCommonTime( const CommonTime& ct )
   {
      long jday, isod;
      double fsod;
      ct.get(jday, isod, fsod, timeSystem);

      imjd = jday - MJD_JDAY;
      // fraction of day
      double frac((static_cast<double>(isod)+fsod) / SEC_PER_DAY);
      dday = static_cast<uint64_t>(frac/MJDFACT);
      fday = static_cast<uint64_t>((frac/MJDFACT-static_cast<double>(dday))/MJDFACT);
   }

   std::string MJD::printf( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "Q",
                              "QLf", asLongDouble() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", timeSystem.asString().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   std::string MJD::printError( const std::string& fmt ) const
   {
      try
      {
         using gpstk::StringUtils::formattedPrint;
         std::string rv( fmt );

         rv = formattedPrint( rv, getFormatPrefixFloat() + "Q",
                              "Qs", getError().c_str() );
         rv = formattedPrint( rv, getFormatPrefixInt() + "P",
                              "Ps", getError().c_str() );
         return rv;
      }
      catch( gpstk::StringUtils::StringException& se )
      {
         GPSTK_RETHROW( se );
      }
   }

   bool MJD::setFromInfo( const IdToValue& info )
   {
      using namespace gpstk::StringUtils;

      for( IdToValue::const_iterator i = info.begin(); i != info.end(); i++ )
      {
         switch( i->first )
         {
            case 'Q':
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

   bool MJD::isValid() const
   {
      MJD temp;
      temp.convertFromCommonTime( convertToCommonTime() );
      if( *this == temp )
      {
         return true;
      }
      return false;
   }

   void MJD::reset()
   {
      imjd = 0L;
      dday = fday = static_cast<uint64_t>(0);
      timeSystem = TimeSystem::Unknown;
   }

   bool MJD::operator==( const MJD& right ) const
   {
      /// Any (wildcard) type exception allowed, otherwise must be same time systems
      if ((timeSystem != TimeSystem::Any &&
                  right.timeSystem != TimeSystem::Any)
                           && timeSystem != right.timeSystem)
         return false;

      if(imjd == right.imjd
            && std::abs(((dday-right.dday) + (fday-right.fday)*MJDFACT) * MJDFACT)
                        < CommonTime::eps)
      {
         return true;
      }
      return false;
   }

   bool MJD::operator!=( const MJD& right ) const
   {
      return ( !operator==( right ) );
   }

   bool MJD::operator<( const MJD& right ) const
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

      if(imjd < right.imjd || dday < right.dday || fday < right.fday) return true;
      return false;
   }

   bool MJD::operator>( const MJD& right ) const
   {
      return ( !operator<=( right ) );
   }

   bool MJD::operator<=( const MJD& right ) const
   {
      return ( operator<(  right ) ||
               operator==( right )   );
   }

   bool MJD::operator>=( const MJD& right ) const
   {
      return ( !operator<( right ) );
   }

   // Compute long double MJD
   // Warning - precision lost when cast to double, and on systems where
   //   long double is implemented as double (WIN)
   // @return long double MJD
   long double MJD::asLongDouble(void) const
   {
      long double mjd = static_cast<long double>(imjd);
      mjd += (static_cast<long double>(dday)
               + static_cast<long double>(fday)*MJDFACT) * MJDFACT;
      return mjd;
   }

} // namespace
