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
 * @file HexDumpDataConfig.hpp
 * Define the configuration class used for the hexDumpData function.
 */

#ifndef GPSTK_HEXDUMPDATACONFIG_HPP
#define GPSTK_HEXDUMPDATACONFIG_HPP

namespace gpstk
{
   namespace StringUtils
   {
         // All the functionality here is inlined since they are
         // farily small functions.

         /// @ingroup stringutilsgroup
         //@{

         /// Class for configuring the appearance of hexDumpData() output
      class HexDumpDataConfig
      {
      public:
            /// Initialize to some sensible defaults.
         HexDumpDataConfig();
            /** Set most fields in a traditional manner, where
             * separators are a specified number of space
             * characters. 
             * @see data member documentation for an explanation of arguments.
             */
         HexDumpDataConfig(bool ashowIndex, bool ahexIndex, bool aupperHex,
                           unsigned aidxDigits, unsigned aindexWS,
                           unsigned agroupBy, unsigned agroupWS,
                           unsigned agroup2By, unsigned agroup2WS,
                           unsigned abytesPerLine, bool ashowText,
                           char aseparator, unsigned atextWS,
                           bool aShowBaseData = false,
                           bool aShowBaseIndex = false);
            /** Set fields using explicit strings for the separators.
             * @see data member documentation for an explanation of arguments.
             */
         HexDumpDataConfig(bool ashowIndex, bool ahexIndex, bool aupperHex,
                           unsigned aidxDigits, const std::string& aindexSep,
                           unsigned agroupBy, const std::string& agroupSep,
                           unsigned agroup2By, const std::string& agroup2Sep,
                           unsigned abytesPerLine, bool ashowText,
                           char aseparator, const std::string& atextSep,
                           bool aShowBaseData, bool aShowBaseIndex,
                           const std::string& adataEndSep,
                           const std::string& adataFinal);
            /** Set fields using explicit strings for the separators.
             * @see data member documentation for an explanation of arguments.
             */
         HexDumpDataConfig(bool ashowIndex, bool ahexIndex, bool aupperHex,
                           unsigned aidxDigits, const std::string& aindexSep,
                           unsigned agroupBy, const std::string& agroupSep,
                           unsigned agroup2By, const std::string& agroup2Sep,
                           unsigned abytesPerLine, bool ashowText,
                           const std::string& apreText,
                           const std::string& apostText,
                           bool aShowBaseData, bool aShowBaseIndex,
                           const std::string& adataEndSep,
                           const std::string& adataFinal,
                           const std::string& aprefix);
            /** Return the number of bytes on a line of hexDumpData
             * output without the ASCII representation length.  This
             * is used to line up the ASCII dump.
             * @param[in] indent The length of the "tag" argument to
             *   hexDumpData.
             * @param[in] bytesOnLine The number of bytes on the line
             *   of output (which may be different from bytesPerLine
             *   when this function is used for the last line of
             *   output).
             * @param[in] lastLine If true, the length of dataFinal
             *   will be added, otherwise the length of dataEndSep
             *   will be added. */
         unsigned computeLineSize(unsigned bytesOnLine,
                                  bool lastLine) const;
            /// Get the index radix ID
         std::string baseIndex() const;
            /// Get the data radix ID
         std::string baseData() const;

         bool showIndex;        ///< display index into string on each line.
         bool hexIndex;         ///< if true, use hex index numbers (else decimal).
         bool upperHex;         ///< if true, use upper-case hex digits.
         unsigned idxDigits;    ///< number of positions to use for index.
         std::string indexSep;  ///< text between index and data.
         unsigned groupBy;      ///< bytes of data to show between spaces.
         std::string groupSep;  ///< text put between groups of hex data.
         std::string group2Sep; ///< text put between 2nd layer groups.
         std::string prefix;    ///< text to put at the start of each line.
         std::string dataEndSep;///< text to put after last data on a line.
         std::string dataFinal; ///< text to put after last of data.
         std::string preText;   ///< text put between hex and ASCII.
         std::string postText;  ///< text to put after ASCII.
         bool showBaseData;     ///< Show number base indicator for data.
         bool showBaseIndex;    ///< Show number base indicator for indices.
            /** If true, show text of message (unprintable characters
             * become '.'. */
         bool showText;
            /** Number of groups to show per 2nd layer group (0=none,
             * must be multiple of groupBy). */
         unsigned group2By;
            /** Number of bytes to display on a line of output (must
             * be evenly divisible by both groupBy and group2By). */
         unsigned bytesPerLine;
      }; // class HexDumpDataConfig

         //@}

   } // namespace StringUtils
} // namespace gpstk

#endif // GPSTK_HEXDUMPDATACONFIG_HPP
