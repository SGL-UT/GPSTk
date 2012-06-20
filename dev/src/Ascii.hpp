#pragma ident "$Id$"

/**
 * @file Ascii.hpp
 * 
 */

#ifndef GPSTK_ASCII_HPP
#define GPSTK_ASCII_HPP

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

#include "gpstkplatform.h"

namespace gpstk
{
      /// This class contains enumerations and static
      /// utility functions for dealing with ASCII characters
      /// and their properties.
      ///
      /// The classification functions will also work if
      /// non-ASCII character codes are passed to them,
      /// but classification will only check for
      /// ASCII characters.
      ///
      /// This allows the classification methods to be used
      /// on the single bytes of a UTF-8 string, without
      /// causing assertions or inconsistent results (depending
      /// upon the current locale) on bytes outside the ASCII range,
      /// as may be produced by Ascii::isSpace(), etc.
   class Ascii    
   {
   public:
      enum CharacterProperties
         /// ASCII character properties.
      {
         ACP_CONTROL  = 0x0001,
         ACP_SPACE    = 0x0002,
         ACP_PUNCT    = 0x0004,
         ACP_DIGIT    = 0x0008,
         ACP_HEXDIGIT = 0x0010,
         ACP_ALPHA    = 0x0020,
         ACP_LOWER    = 0x0040,
         ACP_UPPER    = 0x0080,
         ACP_GRAPH    = 0x0100,
         ACP_PRINT    = 0x0200
      };

      static int properties(int ch);

      static bool hasSomeProperties(int ch, int properties);

      static bool hasProperties(int ch, int properties);

      static bool isAscii(int ch);

      static bool isSpace(int ch);

      static bool isDigit(int ch);

      static bool isHexDigit(int ch);

      static bool isPunct(int ch);

      static bool isAlpha(int ch);

      static bool isAlphaNumeric(int ch);

      static bool isLower(int ch);

      static bool isUpper(int ch);

      static int toLower(int ch);

      static int toUpper(int ch);

   private:
      static const int CHARACTER_PROPERTIES[128];

   }; // End of class 'Ascii'


   //
   // inlines
   //
   inline int Ascii::properties(int ch)
   {
      if (isAscii(ch)) 
         return CHARACTER_PROPERTIES[ch];
      else
         return 0;
   }


   inline bool Ascii::isAscii(int ch)
   {
      return (static_cast<uint32_t>(ch) & 0xFFFFFF80) == 0;
   }


   inline bool Ascii::hasProperties(int ch, int props)
   {
      return (properties(ch) & props) == props;
   }


   inline bool Ascii::hasSomeProperties(int ch, int props)
   {
      return (properties(ch) & props) != 0;
   }


   inline bool Ascii::isSpace(int ch)
   {
      return hasProperties(ch, ACP_SPACE);
   }


   inline bool Ascii::isDigit(int ch)
   {
      return hasProperties(ch, ACP_DIGIT);
   }


   inline bool Ascii::isHexDigit(int ch)
   {
      return hasProperties(ch, ACP_HEXDIGIT);
   }


   inline bool Ascii::isPunct(int ch)
   {
      return hasProperties(ch, ACP_PUNCT);
   }


   inline bool Ascii::isAlpha(int ch)
   {
      return hasProperties(ch, ACP_ALPHA);
   }


   inline bool Ascii::isAlphaNumeric(int ch)
   {
      return hasSomeProperties(ch, ACP_ALPHA | ACP_DIGIT);
   }


   inline bool Ascii::isLower(int ch)
   {
      return hasProperties(ch, ACP_LOWER);
   }


   inline bool Ascii::isUpper(int ch)
   {
      return hasProperties(ch, ACP_UPPER);
   }


   inline int Ascii::toLower(int ch)
   {
      if (isUpper(ch)) return ch + 32;
      else return ch;
   }


   inline int Ascii::toUpper(int ch)
   {
      if (isLower(ch)) return ch - 32;
      else return ch;
   }

}   // End of namespace gpstk


#endif  //GPSTK_ASCII_HPP

