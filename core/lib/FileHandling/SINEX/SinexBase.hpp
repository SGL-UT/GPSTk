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

/**
 * @file SinexBase.hpp
 * Base constants, types, and functions for SINEX files
 */

#ifndef GPSTK_SINEXBASE_HPP
#define GPSTK_SINEXBASE_HPP

#include "CommonTime.hpp"
#include "Exception.hpp"
#include <stdint.h>

namespace gpstk
{
   namespace Sinex
   {
         /// @ingroup FileHandling
         //@{

         /// @name SINEX Constants
         //@{
      const char         HEAD_TAIL_START = '%';
      const char         BLOCK_START     = '+';
      const char         BLOCK_END       = '-';
      const char         COMMENT_START   = '*';
      const char         DATA_START      = ' ';
      const char         FIELD_DIV       = ' ';
      const size_t       MIN_LINE_LEN    = 1;
      const size_t       MAX_LINE_LEN    = 80;
      const float        VERSION         = 2.02f;
      const std::string  FILE_BEGIN("%=SNX");
      const std::string  FILE_END("%ENDSNX");
      const std::string  OBS_CODES("CDLMPR");
      const std::string  CONSTRAINT_CODES("012");
      const std::string  SOLUTION_TYPES("SOETCA ");
         //@}

         /**
          * Return whether c is a valid Observation Code.  If it is not,
          * and if toss is true, the function will throw and exception.
          */
      extern bool isValidObsCode(char c, bool toss = true);

         /**
          * Return whether c is a valid Constraint Code.  If it is not,
          * and if toss is true, the function will throw and exception.
          */
      extern bool isValidConstraintCode(char c, bool toss = true);

         /**
          * Return whether c is a valid Solution Type.  If it is not,
          * and if toss is true, the function will throw and exception.
          */
      extern bool isValidSolutionType(char c, bool toss = true);

         /**
          * Returns whether the supplied data line has a valid structure,
          * i.e. whether its length is correct and whether its field dividers
          * are in expected locations.  If the line structure is not valid,
          * and if toss is true, the function will throw an exception.
          */
      extern bool isValidLineStructure(const std::string& line,
                                       size_t minLength,
                                       size_t maxLength,
                                       int divs[] = NULL,  /// Array terminated by element < 0
                                       bool toss = true);

         /**
          * Format a string such that it fills but does not exceed
          * the specified width.  The value is justified to the left within
          * the returned string.  If the value cannot fit into the specified
          * width, it is truncated.  If the value is empty and allowBlank
          * is false, the returned string will be filled with '-'s as
          * dictated by the SINEX specification.
          */
      extern std::string formatStr(const std::string& value,
                                   size_t width,
                                   bool allowBlank = false);

         /**
          * Format a signed integer such that it's string representation
          * fills but does not exceed the specified width.  The value
          * is justified to the right within the string.  If the value
          * cannot fit into the specified width, the function will throw
          * an exception.
          */
      extern std::string formatInt(long value, size_t width);

         /**
          * Format an unsigned integer such that it's string representation
          * fills but does not exceed the specified width.  The value
          * is justified to the right within the string.  If the value
          * cannot fit into the specified width, the function will throw
          * an exception.
          */
      extern std::string formatUint(unsigned long value, size_t width);

         /**
          * Format a fixed-point value in fortran like scientific notation
          * with a specified width and exponent length.
          */
      extern std::string formatFor(double value,
                                   size_t width,
                                   size_t expLen);
   

         /**
          * Format a fixed-point value such that it has a specified width
          * and precision.  If the value is close to 0, the function will
          * attempt to squeeze it into the specified width by removing
          * the 0 before the decimal (0.123 becomes .123).  If the value
          * cannot be represented in the specified width, the function
          * will throw an exception.
          */
      extern std::string formatFixed(double value,
                                     size_t width,
                                     size_t precision);


         /**
          * SINEX time type data and methods; 00:000:00000 has special meaning
          */
      struct Time
      {
            /// Constructors
         Time() : year(0), doy(0), sod(0) {};
         Time(const gpstk::CommonTime& other) { *this = other; };
         Time(const std::string& str) { *this = str; };

            /// Destructor
         virtual ~Time() {};

            /// Converters
         operator gpstk::CommonTime() const;
         operator std::string() const;
         void operator=(const gpstk::CommonTime& other);
         void operator=(const std::string& other);

         uint8_t   year;  /// YY <= 50 implies 21st century, YY > 50 implies 20th
         uint16_t  doy;   /// Day of year
         uint32_t  sod;   /// Second of day
      }; // struct Time

         //@}

   }  // namespace Sinex

}  // namespace gpstk

#endif // GPSTK_SINEXBASE_HPP
