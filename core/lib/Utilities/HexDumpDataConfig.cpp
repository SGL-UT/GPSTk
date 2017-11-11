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
 * @file StringUtils.cpp
 * Implementation of GPSTK string utility functions.
 */

#include "StringUtils.hpp"

namespace gpstk
{
   namespace StringUtils
   {
      HexDumpDataConfig ::
      HexDumpDataConfig()
            : showIndex(true), hexIndex(true), upperHex(false),
              idxDigits(4), indexSep(": "), groupBy(1), groupSep(" "),
              group2By(8), group2Sep("  "), bytesPerLine(16),
              showText(true), preText("    "),
              showBaseData(false), showBaseIndex(false)
      {}


      HexDumpDataConfig ::
      HexDumpDataConfig(bool ashowIndex, bool ahexIndex, bool aupperHex,
                        unsigned aidxDigits, unsigned aindexWS,
                        unsigned agroupBy, unsigned agroupWS,
                        unsigned agroup2By, unsigned agroup2WS,
                        unsigned abytesPerLine, bool ashowText,
                        char aseparator, unsigned atextWS,
                        bool aShowBaseData, bool aShowBaseIndex)
            : showIndex(ashowIndex), hexIndex(ahexIndex),
         upperHex(aupperHex), idxDigits(aidxDigits),
         groupBy(agroupBy), group2By(agroup2By),
         bytesPerLine(abytesPerLine), showText(ashowText),
         showBaseData(aShowBaseData), showBaseIndex(aShowBaseIndex)
      {
         indexSep = ":" + std::string(aindexWS, ' ');
         groupSep = std::string(agroupWS, ' ');
         group2Sep = std::string(agroup2WS, ' ');
         preText = std::string(atextWS, ' ');
         if (aseparator != 0)
         {
            preText += aseparator;
            postText = std::string(1, aseparator);
         }
      }


      HexDumpDataConfig ::
      HexDumpDataConfig(bool ashowIndex, bool ahexIndex, bool aupperHex,
                        unsigned aidxDigits, const std::string& aindexSep,
                        unsigned agroupBy, const std::string& agroupSep,
                        unsigned agroup2By, const std::string& agroup2Sep,
                        unsigned abytesPerLine, bool ashowText,
                        char aseparator, const std::string& atextSep,
                        bool aShowBaseData, bool aShowBaseIndex,
                        const std::string& adataEndSep,
                        const std::string& adataFinal)
      : showIndex(ashowIndex), hexIndex(ahexIndex),
         upperHex(aupperHex), idxDigits(aidxDigits),
         groupBy(agroupBy), group2By(agroup2By),
         bytesPerLine(abytesPerLine), showText(ashowText),
         indexSep(aindexSep), groupSep(agroupSep), group2Sep(agroup2Sep),
         showBaseData(aShowBaseData), showBaseIndex(aShowBaseIndex),
         dataEndSep(adataEndSep), dataFinal(adataFinal)
      {
         preText = atextSep;
         if (aseparator != 0)
         {
            preText += aseparator;
            postText = std::string(1, aseparator);
         }
      }


      HexDumpDataConfig ::
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
                        const std::string& aprefix)
      : showIndex(ashowIndex), hexIndex(ahexIndex),
         upperHex(aupperHex), idxDigits(aidxDigits),
         groupBy(agroupBy), group2By(agroup2By),
         bytesPerLine(abytesPerLine), showText(ashowText),
         indexSep(aindexSep), groupSep(agroupSep), group2Sep(agroup2Sep),
         preText(apreText), postText(apostText),
         showBaseData(aShowBaseData), showBaseIndex(aShowBaseIndex),
         dataEndSep(adataEndSep), dataFinal(adataFinal), prefix(aprefix)
      {
      }


      unsigned HexDumpDataConfig ::
      computeLineSize(unsigned bytesThisLine,
                      bool lastLine)
         const
      {
         unsigned linesize = prefix.length();
         unsigned w2 = 0;
         unsigned w1 = 0;
         if (this->showIndex)
         {
               // number of characters used by index
            linesize += this->idxDigits + this->indexSep.length();
            if (this->showBaseIndex)
               linesize += 2; // "0x"
         }
            // 2 characters per byte for data
         linesize += bytesThisLine * 2;
         if (this->showBaseData)
         {
               // 2 characters per group 1 for base/radix "0x"
            linesize += (bytesThisLine / this->groupBy) * 2;
               // extra radix and groupSep for incomplete group
            if (bytesThisLine % this->groupBy)
            {
               linesize += 2;
               w1++;
            }
         }
         if (this->group2By)
         {
            w2 += ((bytesThisLine / this->group2By) -
                      // no group 2 separator at the end of the line
                   ((bytesThisLine % this->group2By) == 0 ? 1 : 0));
         }
         if (this->groupBy)
         {
               // number of group 1's minus the number of group 2
               // separators already computed and -1 for no separator
               // at the end of the line
            w1 += (bytesThisLine / this->groupBy) - w2 - 1;
         }
         if (this->groupBy > 0)
         {
               // characters of white space between level 1 grouped data
            linesize += this->groupSep.length() * w1;
         }
         if (this->group2By > 0)
         {
               // characters of white space between level 2 grouped data
            linesize += this->group2Sep.length() * w2;
         }
         if (lastLine)
            linesize += this->dataFinal.length();
         else
            linesize += this->dataEndSep.length();
         return linesize;
      }


      std::string HexDumpDataConfig ::
      baseIndex()
         const
      {
         if (showBaseIndex && hexIndex)
         {
            return (upperHex ? "0X" : "0x");
         }
         return "";
      }


      std::string HexDumpDataConfig ::
      baseData()
         const
      {
         if (showBaseData)
         {
            return (upperHex ? "0X" : "0x");
         }
         return "";
      }
   } // namespace StringUtils
} // namespace gpstk
