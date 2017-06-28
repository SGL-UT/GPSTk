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

#ifndef GPSTK_JULIANDATE_HPP
#define GPSTK_JULIANDATE_HPP

#include "TimeTag.hpp"
#include <string>
#include <sstream>
#include "StringUtils.hpp"
#include "gpstkplatform.h"

namespace gpstk
{
      /// @ingroup TimeHandling
      //@{

      /**
       * This class encapsulates the "Julian Date" time representation.
       */
   class JulianDate : public TimeTag
   {
   public:
         /**
          * @name JulianDate Basic Operations
          */

         //@{
         /**
          * Default constructor from Julian day (not JD) and seconds-of-day
          * All elements are initialized to zero.
          */
      JulianDate(void)
      {
         jday = 0L;
         dday = JDHALFDAY;       // so JD() = 0
         fday = static_cast<uint64_t>(0);
         timeSystem = TimeSystem::Unknown;
      }

         /**
          * Constructor from Julian day (not JD) and optional seconds-of-day
          * NB do not write JulianDate(long, double, TimeSystem) since this creates
          *  ambiguity with JulianData(long double, TimeSystem) b/c TimeSystem is int
          */
      void fromJDaySOD(long jd, double sod, TimeSystem ts = TimeSystem::Unknown)
      {
         jday = jd;
         timeSystem = ts;
         dday = static_cast<uint64_t>(0);
         fday = static_cast<uint64_t>(0);
         if(sod > 0.0) {
            dday = static_cast<uint64_t>((sod/86400.)/JDFACT);
            sod -= 86400.*dday*JDFACT;
            fday = static_cast<uint64_t>((sod/86400.)/(JDFACT*JDFACT));
         }
      }

         /**
          * Constructor from long double JD
          * Warning - precision lost on systems where long double == double (WIN)
          * All elements are initialized to zero.
          */
      JulianDate(long double jd, TimeSystem ts = TimeSystem::Unknown)
      {
         //std::cout << " ld ctor " << std::fixed << std::setprecision(10) << jd;
         jday = static_cast<long>(jd+0.5);
         jd -= static_cast<long double>(jday);
         if(jd >= 0.5) { dday = static_cast<uint64_t>((jd-0.5)/JDFACT); jday += 1L; }
         else            dday = static_cast<uint64_t>((jd+0.5)/JDFACT);
         fday = static_cast<uint64_t>((jd/JDFACT-dday)/JDFACT);
         timeSystem = ts;
         //std::cout << " " << jday << " " << dday << " " << fday << std::endl;
      }

         /**
          * Constructor (except for system ) from string
          */
      void fromString(std::string instr)
      {
         // parse the string
         int sign,exp,index;
         //TEMP std::cout << "instr is " << instr << std::endl;
         std::string str = StringUtils::parseScientific(instr, sign, exp, index);
         //TEMP std::cout << "parse is " << str << std::endl;

         // cannot have negative
         if(sign < 0) GPSTK_THROW(Exception("Negative JD"));

         // mod the string to make exp=0
         if(exp != 0) { index += exp; exp = 0; }
         // pad the string to put index within the string
         if(index < 0)                    // left leading zeros
            str = std::string(-index,'0') + str;
         else if(index >= str.size())     // trailing zeros
            str = str + std::string(str.size()-index,'0');
         //TEMP std::cout << "str is " << str << std::endl;

         // break into 3 strings  int (.) 17-dig 17-dig
         std::string istr("0");
         if(index > 0) {
            istr = str.substr(0,index);
            StringUtils::stripLeading(str,istr);
         }
         //TEMP std::cout << "istr is " << istr << std::endl;
         // 64 bit long max value is 9223372036854775807, 19 digits
         std::string dstr = (str.length() > 0 ? str.substr(0,JDLEN) : "0");
         //TEMP std::cout << "dstr is " << dstr << " " << dstr.size() << std::endl;
         StringUtils::leftJustify(dstr,JDLEN,'0');
         // truncate string after 17 digits, 17+17=34 digits past index
         std::string fstr = (str.length() > JDLEN ? str.substr(JDLEN,JDLEN) : "0");
         //TEMP std::cout << "fstr is " << fstr << " " << fstr.size() << std::endl;
         StringUtils::leftJustify(fstr,JDLEN,'0');

         bool rnd(dstr[0] >= '5');
         jday = std::strtol(istr.c_str(),0,10) + (rnd ? 1 : 0);
         dday = std::strtol(dstr.c_str(),0,10);
         if(rnd) dday -= JDHALFDAY;
         else    dday += JDHALFDAY;         // this accnts for 0.5d JD-jday
         fday = std::strtol(fstr.c_str(),0,10);
         //TEMP std::cout<< "finally "<< jday <<" "<< dday <<" "<< fday << std::endl;
      }

      std::string dumpString(void) const
      {
         std::ostringstream oss;
         oss << jday << "," << dday << "," << fday;
         return oss.str();
      }

      std::string asString(const int prec=-1) const
      {
         long j(jday);
         uint64_t d(dday),f(fday);
         static const char ten('9'+1);

         if(dday < JDHALFDAY) { d += JDHALFDAY; j -= 1L; }
         else                 { d -= JDHALFDAY; }

         std::string str;
         std::ostringstream oss;
         oss << std::setfill('0') << std::setw(JDLEN) << d;
         oss << std::setw(JDLEN) << f;
         oss << std::setfill(' ');
         str = oss.str();

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

         /**
          * Copy Constructor.
          * @param right a reference to the JulianDate object to copy
          */
      JulianDate( const JulianDate& right )
            : jday(right.jday), dday(right.dday), fday(right.fday)
      { timeSystem = right.timeSystem; }

         /**
          * Alternate Copy Constructor.
          * Takes a const TimeTag reference and copies its contents via
          * conversion to CommonTime.
          * @param right a const reference to the BasicTime object to copy
          * @throw InvalidRequest on over-/under-flow
          */
      JulianDate( const TimeTag& right )
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
      JulianDate( const CommonTime& right )
      {
         convertFromCommonTime( right );
      }

         /**
          * Assignment Operator.
          * @param right a const reference to the JulianDate to copy
          * @return a reference to this JulianDate
          */
      JulianDate& operator=( const JulianDate& right );

         /// Virtual Destructor.
      virtual ~JulianDate()
      {}
         //@}

         // The following functions are required by TimeTag.
      virtual CommonTime convertToCommonTime() const;

      virtual void convertFromCommonTime( const CommonTime& ct );

         /// This function formats this time to a string.  The exceptions
         /// thrown would only be due to problems parsing the fmt string.
      virtual std::string printf( const std::string& fmt ) const;

         /// This function works similarly to printf.  Instead of filling
         /// the format with data, it fills with error messages.
      virtual std::string printError( const std::string& fmt ) const;

         /**
          * Set this object using the information provided in \a info.
          * @param info the IdToValue object to which this object shall be set.
          * @return true if this object was successfully set using the
          *  data in \a info, false if not.
          */
      virtual bool setFromInfo( const IdToValue& info );

         /// Return a string containing the characters that this class
         /// understands when printing times.
      virtual std::string getPrintChars() const
      {
         return "JP";
      }

         /// Return a string containing the default format to use in printing.
      virtual std::string getDefaultFormat() const
      {
         return "%J %P";
      }

      virtual bool isValid() const;

      virtual void reset();

         /**
          * @name JulianDate Comparison Operators
          * All comparison operators have a parameter "right" which corresponds
          *  to the JulianDate object to the right of the symbol.
          * All comparison operators are const and return true on success
          *  and false on failure.
          */
         //@{
      bool operator==( const JulianDate& right ) const;
      bool operator!=( const JulianDate& right ) const;
      bool operator<( const JulianDate& right ) const;
      bool operator>( const JulianDate& right ) const;
      bool operator<=( const JulianDate& right ) const;
      bool operator>=( const JulianDate& right ) const;
         //@}

         /**
          * Compute long double JD
          * Warning - precision lost when cast to double,
          *    and on systems where long double is implemented as double (eg. WIN)
          * @return long double Julian Date JD (not Julian day jday)
          */
      long double JD(void) const
      {
         long double jd = static_cast<long double>(jday);
         // NB uint64_t can't do negative
//std::cout << "JD()1 jday is " << std::fixed << std::setprecision(19) << jd << " and dday is " << dday << " and fday is " << fday << std::endl;
         if(dday < JDHALFDAY) {
            jd += (static_cast<long double>(dday+JDHALFDAY)
                   + static_cast<long double>(fday)*JDFACT)*JDFACT-1.0L;
         }
         else {
            jd += (static_cast<long double>(dday-JDHALFDAY)
                   + static_cast<long double>(fday)*JDFACT)*JDFACT;
//std::cout << "JD()2 jday is " << std::fixed << std::setprecision(19) << jd << " and fday is " << fday << std::endl;
         }
         return jd;
      }

      // constants
      static const unsigned int JDLEN;
      static const double JDFACT;
      static const uint64_t JDHALFDAY;

      // member data
      long jday;     ///< long(JD+0.5) integer (Julian Date before noon)
      // fraction of day = (dday+fday*JDFACT)*JDFACT
      uint64_t dday; ///< fraction of day/JDFACT
      uint64_t fday; ///< (fraction of day/JDFACT - dday)/JDFACT
   };

      //@}

} // namespace

#endif // GPSTK_JULIANDATE_HPP
