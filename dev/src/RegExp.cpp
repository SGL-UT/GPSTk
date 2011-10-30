#pragma ident "$Id$"

/**
 * @file RegExp.cpp
 * 
 */
 
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

#include "RegExp.hpp"
#include "Exception.hpp"
#include "MemoryUtils.hpp"
#include "DebugUtils.hpp"
#include "Logger.hpp"

namespace gpstk
{
   using namespace std;


   RegExp::RegExp(const std::string& pattern, const int& options )
   {
      _eflags = REG_NOTBOL|REG_NOTEOL;
      
      _cflags = options;
      _pattern = pattern;
      _regerr = regcomp(&_regex,_pattern.c_str(),_cflags);

      handleError(_regerr, _regex);
   }      

   RegExp::RegExp(const RegExp& right)
   {
      _pattern = right._pattern;
      _subject = right._subject;
      _matched = right._matched;

      _cflags = right._cflags;
      _eflags = right._eflags;
      _regerr = right._regerr;

      match(_subject);
   }

   // Destractor
   RegExp::~RegExp()
   {
      regfree(&_regex);
   }
   

   void RegExp::handleError(int rc, regex_t& re)
   {
      if( (rc!=REG_NOERROR) && (rc!=REG_NOMATCH) )
      {
         char msg[1024+1];
         regerror(rc,&re,msg,1024);
         
         Exception e("Regexp error: "+std::string(msg));
         GPSTK_THROW(e);
      }
   }


   int RegExp::setPattern(const std::string& pattern)
   {
      if(_pattern!=pattern)
      {  
         regfree(&_regex);
         _pattern = pattern;
         _regerr = regcomp(&_regex,_pattern.c_str(),_cflags);

         handleError(_regerr,_regex);
      }

      return _regerr;
   }


   std::string RegExp::pattern()
   {
      return _pattern;
   }


   bool RegExp::match(const std::string& str)
   {
      _subject = str;
      _matched.clear();

      const size_t n = _regex.re_nsub + 1;

      regmatch_t* match = new regmatch_t[n];
      AutoPtr< regmatch_t,ReleaseArrayPolicy<regmatch_t> > pmatch = match;
      
      _regerr = regexec(&_regex,_subject.c_str(),n,match,_eflags);
      handleError(_regerr,_regex); 
      
      if(_regerr==REG_NOERROR)
      {
         for(int i=0;i<n;i++)
         {
            std::string s = _subject.substr( match[ i ].rm_so, match[ i ].rm_eo - match[ i ].rm_so );
            _matched.push_back( s );
         }

         return true;
      }

      return false;
   }

   size_t RegExp::size() const
   { 
      return _matched.size(); 
   }

   size_t RegExp::count() const
   {
      return _matched.size(); 
   }

   size_t RegExp::matchedLength() const
   {
      return _matched.size(); 
   }

   std::vector<std::string> RegExp::matchedStrings() const
   {
      return _matched;
   }

   std::string RegExp::operator[] (size_t nth) const
   {
      GPSTK_ASSERT( nth < _matched.size() );
      return _matched[nth];
   }
   

   RegExp::Iterator RegExp::begin() const
   {
      return _matched.begin();
   }

   RegExp::Iterator RegExp::end() const
   {
      return _matched.end();
   }

   RegExp& RegExp::operator=(const RegExp& right)
   {
      _pattern = right._pattern;
      _subject = right._subject;
      _matched = right._matched;

      _cflags = right._cflags;
      _eflags = right._eflags;
      _regerr = right._regerr;

      match(_subject);

      return (*this);
   }

   bool RegExp::operator!=(const RegExp& right)
   {
      return !( *this == right);
   }

   bool RegExp::operator==(const RegExp& right)
   {
      return (_pattern == right._pattern) && 
             (_subject == right._subject);
   }


//////////////////////////////////////////////////////////////////////////

   std::string RegExp::replace(const std::string& str,
                               const std::string& pattern,
                               const std::string& pattern2,
                               const int& flag)
   {
      string result = str;

      string newstr = pattern2;

      RegExp regex(pattern);
     
      if( regex.match(str) )
      {
         vector<string> sv = regex.matchedStrings();
         
         int pos = 0;
         // replace all & with sv[0]. Do not replace "\&"
         while ( ( pos = newstr.find( '&', pos ) ) != string::npos )
         {
            if ( pos > 0 )
            {
               if ( newstr[ pos - 1 ] == '\\' )
               {
                  newstr.erase( pos - 1, 1 );
                  continue;
               }
               newstr.replace( pos, 1, sv[0] );
               pos += sv[0].size();
            }
         }

         string substr( "\\0" );
         // replace all \1..9 with res[ 1..9 ]. Do not replace "\\N"
         for ( int i = 1; i < sv.size(); i++ )
         {
            substr[ 1 ] = '0' + i;
            pos = 0;
            while ( ( pos = newstr.find( substr, pos )  ) != string::npos )
            {
               if ( pos > 0 )
                  if ( newstr[pos-1] == '\\' )
                  {
                     newstr.erase( pos, 1 );
                     continue;
                  }
                  newstr.replace( pos, 2, sv[i] );
                  pos += sv[i].size();
            }
         }
         // replace matched string in str with newstr
         
         pos = result.find( sv[0] );
         result.replace( pos, sv[ 0 ].size(), newstr );

         return result;
      }

      return "";
   }

}   // End of namespace gpstk

