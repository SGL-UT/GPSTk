#pragma ident "$Id$"

/**
* @file String.hpp
* Modified from Poco, Original copyright by Applied Informatics.
*/

#ifndef GPSTK_STRING_HPP
#define GPSTK_STRING_HPP

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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Wei Yan - Chinese Academy of Sciences . 2011
//
//============================================================================

#include <stdio.h>
#include <string>
#include <cstring>
#include "Ascii.hpp"
#include "StringUtils.hpp"

namespace gpstk
{
      /// Returns a copy of str with all leading whitespace removed.
   template <class S>
   S trimLeft(const S& str)
   {
      typename S::const_iterator it  = str.begin();
      typename S::const_iterator end = str.end();

      while (it != end && Ascii::isSpace(*it)) ++it;
      return S(it, end);
   }

      /// Removes all leading whitespace in str.
   template <class S>
   S& trimLeftInPlace(S& str)  
   {
      typename S::iterator it  = str.begin();
      typename S::iterator end = str.end();

      while (it != end && Ascii::isSpace(*it)) ++it;
      str.erase(str.begin(), it);
      return str;
   }

      /// Returns a copy of str with all trailing whitespace removed.
   template <class S>
   S trimRight(const S& str)
   {
      int pos = int(str.size()) - 1;

      while (pos >= 0 && Ascii::isSpace(str[pos])) --pos;
      return S(str, 0, pos + 1);
   }

      /// Removes all trailing whitespace in str.
   template <class S>
   S& trimRightInPlace(S& str)
   {
      int pos = int(str.size()) - 1;

      while (pos >= 0 && Ascii::isSpace(str[pos])) --pos;
      str.resize(pos + 1);

      return str;
   }

      /// Returns a copy of str with all leading and trailing whitespace removed.
   template <class S>
   S trim(const S& str)
   {
      int first = 0;
      int last  = int(str.size()) - 1;

      while (first <= last && Ascii::isSpace(str[first])) ++first;
      while (last >= first && Ascii::isSpace(str[last])) --last;

      return S(str, first, last - first + 1);
   }

      /// Removes all leading and trailing whitespace in str.
   template <class S>
   S& trimInPlace(S& str)
   {
      int first = 0;
      int last  = int(str.size()) - 1;

      while (first <= last && Ascii::isSpace(str[first])) ++first;
      while (last >= first && Ascii::isSpace(str[last])) --last;

      str.resize(last + 1);
      str.erase(0, first);

      return str;
   }

      /// Returns a copy of str containing all upper-case characters.
   template <class S>
   S toUpper(const S& str)
   {
      typename S::const_iterator it  = str.begin();
      typename S::const_iterator end = str.end();

      S result;
      result.reserve(str.size());
      while (it != end) result += Ascii::toUpper(*it++);
      return result;
   }

      /// Replaces all characters in str with their upper-case counterparts.
   template <class S>
   S& toUpperInPlace(S& str)
   {
      typename S::iterator it  = str.begin();
      typename S::iterator end = str.end();

      while (it != end) { *it = Ascii::toUpper(*it); ++it; }
      return str;
   }

      /// Returns a copy of str containing all lower-case characters.
   template <class S>
   S toLower(const S& str)
   {
      typename S::const_iterator it  = str.begin();
      typename S::const_iterator end = str.end();

      S result;
      result.reserve(str.size());
      while (it != end) result += Ascii::toLower(*it++);
      return result;
   }

      /// Replaces all characters in str with their lower-case counterparts.
   template <class S>
   S& toLowerInPlace(S& str)
   {
      typename S::iterator it  = str.begin();
      typename S::iterator end = str.end();

      while (it != end) { *it = Ascii::toLower(*it); ++it; }
      return str;
   }

      /// Returns a copy of str with all characters in
      /// from replaced by the corresponding (by position)
      /// characters in to. If there is no corresponding
      /// character in to, the character is removed from
      /// the copy.
   template <class S>
   S translate(const S& str, const S& from, const S& to) 
   {
      S result;
      result.reserve(str.size());
      typename S::const_iterator it  = str.begin();
      typename S::const_iterator end = str.end();
      typename S::size_type toSize = to.size();
      while (it != end)
      {
         typename S::size_type pos = from.find(*it);
         if (pos == S::npos)
         {
            result += *it;
         }
         else
         {
            if (pos < toSize) result += to[pos];
         }
         ++it;
      }
      return result;
   }


   template <class S>
   S translate(const S& str, const typename S::value_type* from, const typename S::value_type* to)
   {
      GPSTK_CHECK_PTR (from);
      GPSTK_CHECK_PTR (to);
      return translate(str, S(from), S(to));
   }

      /// Replaces in str all occurences of characters in from
      /// with the corresponding (by position) characters in to.
      /// If there is no corresponding character, the character
      /// is removed.
   template <class S>
   S& translateInPlace(S& str, const S& from, const S& to)
   {
      str = translate(str, from, to);
      return str;
   }


   template <class S>
   S translateInPlace(S& str, const typename S::value_type* from, const typename S::value_type* to)
   {
      GPSTK_CHECK_PTR (from);
      GPSTK_CHECK_PTR (to);
      str = translate(str, S(from), S(to));
      return str;
   }

   template <class S>
   S& replaceInPlace(S& str, const S& from, const S& to, typename S::size_type start = 0)
   {
      poco_assert (from.size() > 0);

      S result;
      typename S::size_type pos = 0;
      result.append(str, 0, start);
      do
      {
         pos = str.find(from, start);
         if (pos != S::npos)
         {
            result.append(str, start, pos - start);
            result.append(to);
            start = pos + from.length();
         }
         else result.append(str, start, str.size() - start);
      }
      while (pos != S::npos);
      str.swap(result);
      return str;
   }


   template <class S>
   S& replaceInPlace(S& str, const typename S::value_type* from, const typename S::value_type* to, typename S::size_type start = 0)
   {
      poco_assert (*from);

      S result;
      typename S::size_type pos = 0;
      typename S::size_type fromLen = std::strlen(from);
      result.append(str, 0, start);
      do
      {
         pos = str.find(from, start);
         if (pos != S::npos)
         {
            result.append(str, start, pos - start);
            result.append(to);
            start = pos + fromLen;
         }
         else result.append(str, start, str.size() - start);
      }
      while (pos != S::npos);
      str.swap(result);
      return str;
   }


   template <class S>
   S replace(const S& str, const S& from, const S& to, typename S::size_type start = 0)
      /// Replace all occurences of from (which must not be the empty string)
      /// in str with to, starting at position start.
   {
      S result(str);
      replaceInPlace(result, from, to, start);
      return result;
   }


   template <class S>
   S replace(const S& str, const typename S::value_type* from, const typename S::value_type* to, typename S::size_type start = 0)
   {
      S result(str);
      replaceInPlace(result, from, to, start);
      return result;
   }


   template <class S>
   S cat(const S& s1, const S& s2)
   {
      S result = s1;
      result.reserve(s1.size() + s2.size());
      result.append(s2);
      return result;
   }


   template <class S>
   S cat(const S& s1, const S& s2, const S& s3)
   {
      S result = s1;
      result.reserve(s1.size() + s2.size() + s3.size());
      result.append(s2);
      result.append(s3);
      return result;
   }


   template <class S>
   S cat(const S& s1, const S& s2, const S& s3, const S& s4)
   {
      S result = s1;
      result.reserve(s1.size() + s2.size() + s3.size() + s4.size());
      result.append(s2);
      result.append(s3);
      result.append(s4);
      return result;
   }


   template <class S>
   S cat(const S& s1, const S& s2, const S& s3, const S& s4, const S& s5)
   {
      S result = s1;
      result.reserve(s1.size() + s2.size() + s3.size() + s4.size() + s5.size());
      result.append(s2);
      result.append(s3);
      result.append(s4);
      result.append(s5);
      return result;
   }


   template <class S>
   S cat(const S& s1, const S& s2, const S& s3, const S& s4, const S& s5, const S& s6)
   {
      S result = s1;
      result.reserve(s1.size() + s2.size() + s3.size() + s4.size() + s5.size() + s6.size());
      result.append(s2);
      result.append(s3);
      result.append(s4);
      result.append(s5);
      result.append(s6);
      return result;
   }


   template <class S, class It>
   S cat(const S& delim, const It& begin, const It& end)
   {
      S result;
      for (It it = begin; it != end; ++it)
      {
         if (!result.empty()) result.append(delim);
         result += *it;
      }
      return result;
   }

 
      /** Split a string by some delimiters
       * @param  aStr           the string to be splitted
       * @param  theDelimiters  the delimiters to split the string
       * @param  trimWhitespace will trim the token string, default is false
       * @param  ignoreEmpty    will ignore the empty tokens, default is true
       */
   inline std::vector<std::string> split(const std::string& aStr,
                                         const std::string& theDelimiters=" ",
                                         bool trimWhitespace = false, 
                                         bool ignoreEmpty = true)
   {
      std::vector<std::string> toReturn;

      std::string::size_type lastPos = aStr.find_first_not_of(theDelimiters, 0);         
      std::string::size_type pos     = aStr.find_first_of(theDelimiters, lastPos);      

      while (std::string::npos != pos || std::string::npos != lastPos)     
      {              
         std::string token = aStr.substr(lastPos, pos - lastPos);

         if(trimWhitespace) token = trim(token);

         if(!token.empty() || !ignoreEmpty) toReturn.push_back(token);  

         lastPos = aStr.find_first_not_of(theDelimiters, pos);              
         pos = aStr.find_first_of(theDelimiters, lastPos);     
      } 

      return toReturn;
   }

   template <class S, class It>
   int icompare(
      const S& str,
      typename S::size_type pos, 
      typename S::size_type n,
      It it2, 
      It end2)
      /// Case-insensitive string comparison
   {
      typename S::size_type sz = str.size();
      if (pos > sz) pos = sz;
      if (pos + n > sz) n = sz - pos;
      It it1  = str.begin() + pos; 
      It end1 = str.begin() + pos + n;
      while (it1 != end1 && it2 != end2)
      {
         typename S::value_type c1(Ascii::toLower(*it1));
         typename S::value_type c2(Ascii::toLower(*it2));
         if (c1 < c2)
            return -1;
         else if (c1 > c2)
            return 1;
         ++it1; ++it2;
      }

      if (it1 == end1)
         return it2 == end2 ? 0 : -1;
      else
         return 1;
   }


   template <class S>
   int icompare(const S& str1, const S& str2)
      // A special optimization for an often used case.
   {
      typename S::const_iterator it1(str1.begin());
      typename S::const_iterator end1(str1.end());
      typename S::const_iterator it2(str2.begin());
      typename S::const_iterator end2(str2.end());
      while (it1 != end1 && it2 != end2)
      {
         typename S::value_type c1(Ascii::toLower(*it1));
         typename S::value_type c2(Ascii::toLower(*it2));
         if (c1 < c2)
            return -1;
         else if (c1 > c2)
            return 1;
         ++it1; ++it2;
      }

      if (it1 == end1)
         return it2 == end2 ? 0 : -1;
      else
         return 1;
   }


   template <class S>
   int icompare(const S& str1, typename S::size_type n1, const S& str2, typename S::size_type n2)
   {
      if (n2 > str2.size()) n2 = str2.size();
      return icompare(str1, 0, n1, str2.begin(), str2.begin() + n2);
   }


   template <class S>
   int icompare(const S& str1, typename S::size_type n, const S& str2)
   {
      if (n > str2.size()) n = str2.size();
      return icompare(str1, 0, n, str2.begin(), str2.begin() + n);
   }


   template <class S>
   int icompare(const S& str1, typename S::size_type pos, typename S::size_type n, const S& str2)
   {
      return icompare(str1, pos, n, str2.begin(), str2.end());
   }


   template <class S>
   int icompare(
      const S& str1, 
      typename S::size_type pos1, 
      typename S::size_type n1, 
      const S& str2,
      typename S::size_type pos2,
      typename S::size_type n2)
   {
      typename S::size_type sz2 = str2.size();
      if (pos2 > sz2) pos2 = sz2;
      if (pos2 + n2 > sz2) n2 = sz2 - pos2;
      return icompare(str1, pos1, n1, str2.begin() + pos2, str2.begin() + pos2 + n2);
   }


   template <class S>
   int icompare(
      const S& str1, 
      typename S::size_type pos1, 
      typename S::size_type n, 
      const S& str2,
      typename S::size_type pos2)
   {
      typename S::size_type sz2 = str2.size();
      if (pos2 > sz2) pos2 = sz2;
      if (pos2 + n > sz2) n = sz2 - pos2;
      return icompare(str1, pos1, n, str2.begin() + pos2, str2.begin() + pos2 + n);
   }


   template <class S>
   int icompare(
      const S& str,
      typename S::size_type pos,
      typename S::size_type n,
      const typename S::value_type* ptr)
   {
      GPSTK_CHECK_PTR (ptr);
      typename S::size_type sz = str.size();
      if (pos > sz) pos = sz;
      if (pos + n > sz) n = sz - pos;
      typename S::const_iterator it  = str.begin() + pos; 
      typename S::const_iterator end = str.begin() + pos + n;
      while (it != end && *ptr)
      {
         typename S::value_type c1(Ascii::toLower(*it));
         typename S::value_type c2(Ascii::toLower(*ptr));
         if (c1 < c2)
            return -1;
         else if (c1 > c2)
            return 1;
         ++it; ++ptr;
      }

      if (it == end)
         return *ptr == 0 ? 0 : -1;
      else
         return 1;
   }


   template <class S>
   int icompare(
      const S& str,
      typename S::size_type pos,
      const typename S::value_type* ptr)
   {
      return icompare(str, pos, str.size() - pos, ptr);
   }


   template <class S>
   int icompare(
      const S& str,
      const typename S::value_type* ptr)
   {
      return icompare(str, 0, str.size(), ptr);
   }


}   // End of namespace gpstk


#endif  //GPSTK_STRING_HPP

