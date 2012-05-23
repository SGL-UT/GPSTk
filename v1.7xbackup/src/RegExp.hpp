#pragma ident "$Id$"

/**
 * @file RegExp.hpp
 * 
 */

#ifndef GPSTK_REGEXP_HPP
#define GPSTK_REGEXP_HPP

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <string>
#include <vector>
#include <regex.h>

// it appears that some systems don't define this...
#if !defined(REG_NOERROR)
#define REG_NOERROR 0
#endif

namespace gpstk
{
      /** This class ...
      * RegExp reg("([0-9]{4})([0-9]{2})([0-9]{2})([0-9]{2})([0-9]{2})"); 
      * cout << reg.match("20100331115531Z") << endl;;
      * for(int i=0;i<reg.count();i++)
      * {
      *    cout << reg[i] <<endl;
      * }
      *
      * cout << RegExp::replace("20100331115531Z",
      *                "([0-9]{4})([0-9]{2})([0-9]{2})([0-9]{2})([0-9]{2}).*",
      *                "Time \\& date: \\4:\\5 \\2/\\3/\\1")<<endl;
      */
   class RegExp   
   {
   public:
      typedef std::vector<std::string>::const_iterator Iterator;

   public:
      RegExp(const std::string& pattern = "*", 
             const int& options = REG_EXTENDED );      
      
      RegExp(const RegExp& right);

      virtual ~RegExp();

      int setPattern(const std::string& pattern);
   
      std::string pattern();
      
      bool match(const std::string& str);

      size_t size() const;

      size_t count() const;

      size_t matchedLength() const;

      std::vector<std::string> matchedStrings() const;

      std::string operator[] (size_t nth) const;

      Iterator begin() const;

      Iterator end() const;

      RegExp& operator=(const RegExp& right);

      bool operator!=(const RegExp& right);

      bool operator==(const RegExp& right);

      void set_eflags( int mask )   { _eflags |= mask; }

      void reset_eflags( int mask ) { _eflags &= ~mask;}

      //
      // static methods
      //

      static std::string replace(const std::string& str,
                                 const std::string& pattern,
                                 const std::string& newstr,
                                 const int& flag = 0);

   protected:

      void handleError(int rc, regex_t& re);

   protected:
   	
      std::string _pattern;
      std::string _subject;
      std::vector<std::string> _matched;

      regex_t _regex;
      int _cflags;
      int _eflags;
      int _regerr; 

   };   // End of class 'RegExp'
   
}   // End of namespace gpstk


#endif  //GPSTK_REGEXP_HPP

